#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <algorithm>
#include <cmath>
#include <ctime>
#include <map>
#include <opencv2/opencv.hpp>
#include <random>
#include <vector>

/**
 * @namespace RemoteSensingAlgorithms
 * @brief 提供遥感影像处理与点云数据分析的核心算法
 */
namespace RemoteSensingAlgorithms {

// =================================================================
// 1. 遥感指数计算 (Remote Sensing Indices)
// =================================================================

/** @brief 归一化植被指数 (NDVI): 用于监测植被生长状态 */
double calculateNDVI(double red, double nir) { return (nir - red) / (nir + red); }

/** @brief 归一化水体指数 (NDWI): 用于提取水体边界 */
double calculateNDWI(double green, double nir) { return (green - nir) / (nir + green); }

/** @brief 土壤调节植被指数 (SAVI): 消除背景土壤影响，L通常取0.5 */
double calculateSAVI(double red, double nir, double L = 0.5) {
    return (nir - red) / (nir + red + L) * (1 + L);
}

/** @brief 增强植被指数 (EVI): 减弱大气影响，适用于高生物量区 */
double calculateEVI(double blue, double red, double nir) {
    return 2.5 * (nir - red) / (nir + 6 * red - 7.5 * blue + 1);
}

/** @brief 改进型归一化水体指数 (MNDWI): 抑制城镇建筑干扰 */
double calculateMNDWI(double green, double swir) { return (green - swir) / (green + swir); }

/** @brief 自动水体提取指数 (AWEI): 用于背景复杂的环境 */
double calculateAWEI(double blue, double green, double nir, double swir1, double swir2) {
    return blue + 2.5 * green - 1.5 * (swir1 + nir) - 0.25 * swir2;
}

// =================================================================
// 2. 聚类算法 (Clustering)
// =================================================================

/**
 * @brief K-Means 一维聚类算法模板
 * @param data 输入数据向量 (支持 int, float, double)
 * @param k 聚类中心数量
 * @param maxIter 最大迭代次数
 * @return std::vector<int> 对应每个数据点的类别标签 (0 到 k-1)
 */
template <typename T>
std::vector<int> kMeansClustering(const std::vector<T> &data, int k, int maxIter = 100) {
    if (data.empty() || k <= 0)
        return {};

    int n = static_cast<int>(data.size());
    std::vector<int> labels(n, 0);
    std::vector<double> centroids(k);

    // 1. 初始化质心：随机选择 k 个样本点作为初始质心
    std::default_random_engine generator(static_cast<unsigned int>(time(nullptr)));
    std::uniform_int_distribution<int> distribution(0, n - 1);

    for (int i = 0; i < k; ++i) {
        centroids[i] = static_cast<double>(data[distribution(generator)]);
    }

    // 开始迭代
    for (int iter = 0; iter < maxIter; ++iter) {
        bool changed = false;

        // A. 分配步骤：计算每个点到各质心的欧式距离（一维绝对值）
        for (int i = 0; i < n; ++i) {
            double minDist = std::abs(static_cast<double>(data[i]) - centroids[0]);
            int bestLabel = 0;
            for (int j = 1; j < k; ++j) {
                // BUG FIX: 此处原代码 data[j] 应为 data[i]，且 centroids 索引应匹配
                double dist = std::abs(static_cast<double>(data[i]) - centroids[j]);
                if (dist < minDist) {
                    minDist = dist;
                    bestLabel = j;
                }
            }
            if (labels[i] != bestLabel) {
                labels[i] = bestLabel;
                changed = true;
            }
        }

        // 如果本轮迭代没有点改变分类，说明已收敛
        if (!changed)
            break;

        // B. 更新步骤：重新计算每个簇的平均值作为新质心
        std::vector<double> newSum(k, 0.0);
        std::vector<int> counts(k, 0);
        for (int i = 0; i < n; ++i) {
            newSum[labels[i]] += static_cast<double>(data[i]);
            counts[labels[i]]++;
        }

        for (int j = 0; j < k; ++j) {
            if (counts[j] > 0) {
                centroids[j] = newSum[j] / counts[j];
            }
        }
    }
    return labels;
}

// =================================================================
// 3. 图像处理 (Image Processing)
// =================================================================

/**
 * @brief 离散卷积处理 (使用 OpenCV 加速)
 * @param image 二维 std::vector 表示的单通道影像
 * @param kernelData 卷积核矩阵
 * @return 处理后的二维影像
 */
template <typename T>
std::vector<std::vector<T>> convolve(const std::vector<std::vector<T>> &image,
                                     const std::vector<std::vector<double>> &kernelData) {
    if (image.empty() || image[0].empty())
        return {};

    int rows = image.size();
    int cols = image[0].size();

    // 1. 数据转换：std::vector -> cv::Mat
    cv::Mat matImage(rows, cols, CV_64F);
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            matImage.at<double>(i, j) = static_cast<double>(image[i][j]);
        }
    }

    // 2. 转换卷积核
    int kRows = kernelData.size();
    int kCols = kernelData[0].size();
    cv::Mat kernel(kRows, kCols, CV_64F);
    for (int i = 0; i < kRows; ++i) {
        for (int j = 0; j < kCols; ++j) {
            kernel.at<double>(i, j) = kernelData[i][j];
        }
    }

    // 3. 调用 OpenCV 卷积逻辑
    cv::Mat resultMat;
    // 参数 -1 表示结果图深度与原图一致，即 CV_64F
    cv::filter2D(matImage, resultMat, -1, kernel);

    // 4. 数据转回：cv::Mat -> std::vector
    std::vector<std::vector<T>> result(rows, std::vector<T>(cols));
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            // BUG FIX: resultMat 是 CV_64F (double)，应使用 .at<double>
            // 如果使用 .at<float> 会读取到错误的数据
            result[i][j] = static_cast<T>(resultMat.at<double>(i, j));
        }
    }
    return result;
}

// =================================================================
// 4. 统计分析 (Statistical Analysis)
// =================================================================

/** @brief 计算信息熵：评估影像信息的丰富程度 */
template <typename T> double calculateEntropy(const std::vector<T> &data) {
    if (data.empty())
        return 0.0;

    // 1. 统计频次 (Histogram)
    std::map<T, int> counts;
    for (const T &value : data) {
        counts[value]++;
    }

    // 2. 计算香农熵: H = -sum(p * log2(p))
    double entropy = 0.0;
    double totalSize = static_cast<double>(data.size());

    for (auto const &[val, count] : counts) {
        double p = count / totalSize;
        if (p > 0) {
            entropy -= p * std::log2(p);
        }
    }
    return entropy;
}

/** @brief 计算直方图分布：返回各 bin 的概率分布 */
template <typename T> std::vector<double> calculateHistogram(const std::vector<T> &data, int bins) {
    if (data.empty() || bins <= 0)
        return std::vector<double>(bins, 0.0);

    // 1. 获取数据范围
    auto [minIt, maxIt] = std::minmax_element(data.begin(), data.end());
    double minVal = static_cast<double>(*minIt);
    double maxVal = static_cast<double>(*maxIt);
    double range = maxVal - minVal;

    std::vector<double> histogram(bins, 0.0);

    // 2. 特殊情况：全图数值一致
    if (range == 0) {
        histogram[0] = 1.0;
        return histogram;
    }

    // 3. 统计落入每个箱子 (bin) 的点数
    for (const T &value : data) {
        int binIdx = static_cast<int>((static_cast<double>(value) - minVal) / range * bins);

        // 处理边界值情况：当 value == maxVal 时 binIdx 会等于 bins
        if (binIdx >= bins)
            binIdx = bins - 1;
        if (binIdx < 0)
            binIdx = 0;

        histogram[binIdx]++;
    }

    // 4. 归一化：将频次转换为概率密度
    double total = static_cast<double>(data.size());
    for (double &count : histogram) {
        count /= total;
    }

    return histogram;
}

} // namespace RemoteSensingAlgorithms

#endif // ALGORITHMS_H