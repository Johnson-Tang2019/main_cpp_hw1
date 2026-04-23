#ifndef ALGORITHMS_H
#define ALGORITHMS_H
#include <vector>

namespace RemoteSensingAlgorithms {
// 植被指数
double calculateNDVI(double red, double nir) { return (nir - red) / (nir + red); };
double calculateNDWI(double green, double nir) { return (green - nir) / (nir + green); };
double calculateSAVI(double red, double nir, double L = 0.5) {
    return (nir - red) / (nir + red + L) * (1 + L);
};
double calculateEVI(double blue, double red, double nir) {
    return 2.5f * (nir - red) / (nir + 6 * red - 7.5 * blue + 1);
};

// 水体指数
double calculateMNDWI(double green, double swir) { return (green - swir) / (green + swir); };
double calculateAWEI(double blue, double green, double nir, double swir1, double swir2) {
    return blue + 2.5f * green - 1.5f * (swir1 + nir) - 0.25f * swir2;
};

// 分类算法
template <typename T>
std::vector<int> kMeansClustering(const std::vector<T> &data, int k, int maxIter = 100) {
    if (data.empty() || k <= 0) {
        return {};
    }

    int n = data.size();
    std::vector<int> labels(n, 0);
    std::vector<double> centroids(k);

    // 1. 初始化质心：随机从数据中挑选 k 个点
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(0, n - 1);
    for (int i = 0; i < k; ++i) {
        centroids[i] = static_cast<double>(data[distribution(generator)]);
    }

    // 2. 迭代聚类
    for (int iter = 0; iter < maxIter; ++iter) {
        bool changed = false;

        // A. 分配步骤：将每个点分配给最近的质心
        for (int i = 0; i < n; ++i) {
            double minDist = std::abs(data[i] - centroids[0]);
            int bestLabel = 0;
            for (int j = 1; j < k; ++j) {
                double dist = std::abs(data[i] - centroids[j]);
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

        // 如果本轮没有点改变分类，提前结束
        if (!changed)
            break;

        // B. 更新步骤：重新计算质心
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

// 图像处理
template <typename T>
std::vector<std::vector<T>> convolve(const std::vector<std::vector<T>> &image,
                                     const std::vector<std::vector<double>> &kernelData) {
    int rows = image.size();
    int cols = image[0].size();

    // 1. 将 std::vector 转换为 cv::Mat
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
            kernelData.at<double>(i, j) = kernelData[i][j];
        }
    }

    // 3. 调用 OpenCV 卷积
    cv::Mat resultMat;
    cv::filter2D(matImage, resultMat, -1, kernelData);

    // 4. 将结果转回 std::vector
    std::vector<std::vector<T>> result(rows, std::vector<T>(cols));
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            result[i][j] = static_cast<T>(resultMat.at<float>(i, j));
        }
    }

    return result;
}

// 统计分析
template <typename T> double calculateEntropy(const std::vector<T> &data) {
    if (data.empty()) {
        return 0.0;
    }

    // 1. 统计每个数值出现的频次
    std::map<T, int> counts;
    for (const T &value : data) {
        counts[value]++;
    }

    // 2. 根据频次计算概率并累加熵
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

template <typename T> std::vector<double> calculateHistogram(const std::vector<T> &data, int bins) {
    if (data.empty() || bins <= 0) {
        return std::vector<double>(bins, 0.0);
    }

    // 1. 获取数据的极值
    auto [minIt, maxIt] = std::minmax_element(data.begin(), data.end());
    double minVal = static_cast<double>(*minIt);
    double maxVal = static_cast<double>(*maxIt);
    double range = maxVal - minVal;

    std::vector<double> histogram(bins, 0.0);

    // 2. 处理特殊情况：所有数值都相等
    if (range == 0) {
        histogram[0] = static_cast<double>(data.size());
        return histogram;
    }

    // 3. 统计分布
    for (const T &value : data) {
        int binIdx = static_cast<int>((value - minVal) / range * bins);

        // 防止索引越界（当 value 等于 maxVal 时）
        if (binIdx >= bins) {
            binIdx = bins - 1;
        }
        histogram[binIdx]++;
    }

    // 4. (可选) 归一化为概率分布
    for (double &count : histogram) {
        count /= data.size();
    }

    return histogram;
}

} // namespace RemoteSensingAlgorithms

#endif // ALGORITHMS_H
