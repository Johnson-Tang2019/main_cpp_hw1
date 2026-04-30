#include <Algorithms.h>
#include <SatelliteImage.h>

// 构造函数
SatelliteImage::SatelliteImage(const std::string &id, const std::string &name,
                               const std::string &path, int w, int h, int b,
                               const std::string &sensor, time_t time)
    : DataObject(id, name, path), width(w), height(h), bands(b), sensorType(sensor),
      acquisitionTime(time) {}

// 从 OpenCV 矩阵创建图像
SatelliteImage::SatelliteImage(const std::string &id, const std::string &name,
                               const std::string &path, cv::Mat &mat)
    : DataObject(id, name, path), width(mat.cols), height(mat.rows), bands(mat.channels()),
      sensorType("Openellite"), acquisitionTime(0), mat(mat) {

    data.resize(height);
    spdlog::debug("初始化遥感影像: {}", path);

    for (int i = 0; i < height; ++i) {
        data[i].resize(width);
        for (int j = 0; j < width; ++j) {
            auto &p = data[i][j];
            p.setRed(mat.at<Vec5d>(i, j)[0]);
            p.setGreen(mat.at<Vec5d>(i, j)[1]);
            p.setBlue(mat.at<Vec5d>(i, j)[2]);
            if (bands > 3)
                p.setNir(mat.at<Vec5d>(i, j)[3]);
            else
                p.setNir(0);
            if (bands > 4)
                p.setThermal(mat.at<Vec5d>(i, j)[4]);
            else
                p.setThermal(0);
        }
    }

    // 初始化统计信息
    bandStatistics.resize(bands);
}

// 拷贝构造函数
SatelliteImage::SatelliteImage(const SatelliteImage &other)
    : DataObject(other), width(other.width), height(other.height), bands(other.bands),
      sensorType(other.sensorType), cloudCover(other.cloudCover),
      acquisitionTime(other.acquisitionTime), data(other.data),
      bandStatistics(other.bandStatistics) {}

// 赋值运算符
SatelliteImage &SatelliteImage::operator=(const SatelliteImage &other) {
    if (this != &other) {
        DataObject::operator=(other);
        width = other.width;
        height = other.height;
        bands = other.bands;
        sensorType = other.sensorType;
        cloudCover = other.cloudCover;
        acquisitionTime = other.acquisitionTime;
        data = other.data;
        bandStatistics = other.bandStatistics;
    }
    return *this;
}

// 实现基类虚函数
void SatelliteImage::display() const {
    // 实现显示逻辑
    std::cout << "SatelliteImage: " << name << " (" << id << ")\n";
    std::cout << "Path: " << path << "\n";
    std::cout << "Size: " << size << " MB\n";
    struct tm timeInfo;
    localtime_s(&timeInfo, &acquisitionTime);
    std::cout << "Created: " << std::put_time(&timeInfo, "%Y-%m-%d %H:%M:%S");
    std::cout << "Dimensions: " << width << "x" << height << " Bands: " << bands << "\n";
    std::cout << "Sensor: " << sensorType << " Cloud Cover: " << cloudCover
              << "% Acquisition Time: " << acquisitionTime << "\n";
    for (auto i : data) {
        for (auto j : i) {
            std::cout << j << " ";
        }
        std::cout << "\n";
    }
    std::cout << "Band Statistics: ";
    for (const auto &stat : bandStatistics) {
        std::cout << stat << " ";
    }
    std::cout << "\n";
}

DataObject *SatelliteImage::clone() const { return new SatelliteImage(*this); }

// 实现数据导出逻辑
bool SatelliteImage::exportData(const std::string &format) const {

    return exportDataInPath(format, "./");
}

bool SatelliteImage::exportDataInPath(const std::string &format, const std::string &path) const {
    // 这里可以根据format参数实现不同格式的导出逻辑
    std::cout << "Exporting SatelliteImage in format: " << format << "\n";
    std::ofstream outFile(name + "." + format);

    // 导出逻辑
    if (format == "TXT") {
        // 导出为TXT格式
        outFile << "SatelliteImage: " << name << " (" << id << ")\n";
        outFile << "Path: " << path << "\n";
        outFile << "Size: " << size << " MB\n";
        struct tm timeInfo;
        localtime_s(&timeInfo, &acquisitionTime);
        outFile << "Created: " << std::put_time(&timeInfo, "%Y-%m-%d %H:%M:%S");
        return true;

    } else if (format == "JPG") {
        // 导出为JPG格式
        cv::imwrite(name + ".jpg", get8UC3Mat());
        spdlog::debug("导出为JPG格式: {}", name + ".jpg");
        return true;
    }
    std::cerr << "Unsupported export format: " << format << "\n";
    return false;
}

double SatelliteImage::getQualityScore() const {
    return 100.0 - cloudCover; // 质量分数与云量成反比
}

bool SatelliteImage::isValid() const {
    return width > 0 && height > 0 && bands > 0 && !sensorType.empty() && !data.empty();
}

// 算术运算符重载（影像运算)

SatelliteImage SatelliteImage::operator+(const SatelliteImage &other) const {
    // 实现影像加法逻辑
    if (width != other.width || height != other.height || bands != other.bands) {
        throw std::invalid_argument("Image dimensions and bands must match for addition");
    }
    SatelliteImage result(*this);
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            result.data[i][j] = data[i][j] + other.data[i][j];
        }
    }
    return result;
}

SatelliteImage SatelliteImage::operator-(const SatelliteImage &other) const {
    // 实现影像减法逻辑
    if (width != other.width || height != other.height || bands != other.bands) {
        throw std::invalid_argument("Image dimensions and bands must match for subtraction");
    }
    SatelliteImage result(*this);
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            result.data[i][j] = data[i][j] - other.data[i][j];
        }
    }
    return result;
}

SatelliteImage SatelliteImage::operator*(double factor) const {
    // 实现影像乘法逻辑
    SatelliteImage result(*this);
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            result.data[i][j] = data[i][j] * factor;
        }
    }
    return result;
}

SatelliteImage SatelliteImage::operator/(double divisor) const {
    // 实现影像除法逻辑
    if (divisor == 0) {
        throw std::invalid_argument("Divisor cannot be zero");
    }
    SatelliteImage result(*this);
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            result.data[i][j] = data[i][j] / divisor;
        }
    }
    return result;
}

SatelliteImage &SatelliteImage::operator+=(const SatelliteImage &other) {
    // 实现影像加法赋值逻辑
    if (width != other.width || height != other.height || bands != other.bands) {
        throw std::invalid_argument("Image dimensions and bands must match for addition");
    }
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            data[i][j] += other.data[i][j];
        }
    }
    return *this;
}

SatelliteImage &SatelliteImage::operator-=(const SatelliteImage &other) {
    // 实现影像减法赋值逻辑
    if (width != other.width || height != other.height || bands != other.bands) {
        throw std::invalid_argument("Image dimensions and bands must match for subtraction");
    }
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            data[i][j] -= other.data[i][j];
        }
    }
    return *this;
}

// 比较运算符
bool SatelliteImage::operator==(const SatelliteImage &other) const {
    return id == other.id; // 基于唯一标识符进行比较
}

bool SatelliteImage::operator!=(const SatelliteImage &other) const { return !(*this == other); }

// 下标运算符（访问行）
std::vector<Pixel<double>> &SatelliteImage::operator[](int row) {
    if (row < 0 || row >= height) {
        throw std::out_of_range("Row index out of range");
    }
    return data[row];
}

// 下标运算符（访问行）（const版本）
const std::vector<Pixel<double>> &SatelliteImage::operator[](int row) const {
    if (row < 0 || row >= height) {
        throw std::out_of_range("Row index out of range");
    }
    return data[row];
}

// 类型转换运算符
SatelliteImage::operator std::string() const {
    return DataObject::operator std::string() + " [" + sensorType + " " + std::to_string(width) +
           "x" + std::to_string(height) + "]";
}

// 子图操作
SatelliteImage SatelliteImage::getSubImage(int x, int y, int w, int h) const {
    if (x < 0 || y < 0 || x + w > width || y + h > height) {
        throw std::invalid_argument("Sub-image coordinates are out of bounds");
    }
    SatelliteImage subImage(*this);
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            subImage.data[i][j] = data[y + i][x + j];
        }
    }
    return subImage;
}

// 波段操作
std::vector<double> SatelliteImage::getBandValues(int band) const {
    if (band < 0 || band >= bands) {
        throw std::out_of_range("Band index must be between 0 and " + std::to_string(bands - 1));
    }
    std::vector<double> bandValues;
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            bandValues.push_back(data[i][j][band]);
        }
    }
    return bandValues;
}

SatelliteImage SatelliteImage::extractBand(int band) const {
    if (band < 0 || band >= bands) {
        throw std::out_of_range("Band index must be between 0 and " + std::to_string(bands - 1));
    }
    SatelliteImage bandImage(*this);
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            double value = data[i][j][band];
            bandImage.data[i][j] =
                Pixel<double>(value, value, value, value, value); // 将单波段值复制到所有波段
        }
    }
    return bandImage;
}

// 遥感指数计算（返回二维数组）
std::vector<std::vector<double>> SatelliteImage::calculateNDVI() const {
    if (bands != 4) {
        throw std::invalid_argument("NDVI calculation requires 4 bands (red, green, blue, nir)");
    }
    std::vector<std::vector<double>> ndvi(height, std::vector<double>(width));
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            ndvi[i][j] =
                RemoteSensingAlgorithms::calculateNDVI(data[i][j].getRed(), data[i][j].getNir());
        }
    }
    return ndvi;
}

std::vector<std::vector<double>> SatelliteImage::calculateNDWI() const {
    if (bands != 4) {
        throw std::invalid_argument("NDWI calculation requires 4 bands (green, nir)");
    }
    std::vector<std::vector<double>> ndwi(height, std::vector<double>(width));
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            ndwi[i][j] =
                RemoteSensingAlgorithms::calculateNDWI(data[i][j].getGreen(), data[i][j].getNir());
        }
    }
    return ndwi;
}

std::vector<std::vector<double>> SatelliteImage::calculateSAVI(double L) const {
    if (bands != 4) {
        throw std::invalid_argument("SAVI calculation requires 4 bands (red, green, blue, nir)");
    }
    std::vector<std::vector<double>> savi(height, std::vector<double>(width));
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            double red = data[i][j].getRed();
            double nir = data[i][j].getNir();
            savi[i][j] = ((nir - red) / (nir + red + L)) * (1 + L);
        }
    }
    return savi;
}

std::vector<std::vector<double>> SatelliteImage::calculateTemperature() const {
    if (bands != 4) {
        throw std::invalid_argument(
            "Temperature calculation requires 4 bands (red, green, blue, nir)");
    }
    std::vector<std::vector<double>> temperature(height, std::vector<double>(width));
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            double nir = data[i][j].getNir();
            double thermal = data[i][j].getThermal();

            temperature[i][j] = (nir - thermal) / (nir + thermal);
        }
    }
    return temperature;
}

// 影像统计
double SatelliteImage::getMeanValue() const {
    double sum = 0.0;
    int count = 0;
    for (const auto &row : data) {
        for (const auto &pixel : row) {
            sum += pixel;
            count += 5; // 每个像素有5个波段
        }
    }
    return (count > 0) ? (sum / count) : 0.0;
}

double SatelliteImage::getStdDev() const {
    double mean = getMeanValue();
    double sumSqDiff = 0.0;
    int count = 0;
    for (const auto &row : data) {
        for (const auto &pixel : row) {
            sumSqDiff += std::pow(pixel.getBrightness() - mean, 2);
            count += 5; // 每个像素有5个波段
        }
    }
    return (count > 1) ? std::sqrt(sumSqDiff / (count - 1)) : 0.0;
}

double SatelliteImage::getMinValue() const {
    double minVal = std::numeric_limits<double>::max();
    for (const auto &row : data) {
        for (const auto &pixel : row) {
            minVal = std::min({minVal, pixel.getRed(), pixel.getGreen(), pixel.getBlue(),
                               pixel.getNir(), pixel.getThermal()});
        }
    }
    return minVal;
}

double SatelliteImage::getMaxValue() const {
    double maxVal = std::numeric_limits<double>::lowest();
    for (const auto &row : data) {
        for (const auto &pixel : row) {
            maxVal = std::max({maxVal, pixel.getRed(), pixel.getGreen(), pixel.getBlue(),
                               pixel.getNir(), pixel.getThermal()});
        }
    }
    return maxVal;
}

cv::Mat SatelliteImage::getMat() {
    updateMat();
    return mat;
}

void SatelliteImage::updateMat() {
    spdlog::debug("更新mat");
    cv::Mat newMat(height, width, CV_64FC(5));
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            const auto &p = data[i][j];
            newMat.at<Vec5d>(i, j) =
                Vec5d(p.getRed(), p.getGreen(), p.getBlue(), p.getNir(), p.getThermal());
        }
    }
    mat = newMat;
}

void SatelliteImage::printStatistics() const {
    std::cout << "Mean Value: " << getMeanValue() << "\n";
    std::cout << "Standard Deviation: " << getStdDev() << "\n";
    std::cout << "Min Value: " << getMinValue() << "\n";
    std::cout << "Max Value: " << getMaxValue() << "\n";
}

// 影像处理

void SatelliteImage::applyGaussianBlur(double sigma) {
    if (mat.empty()) {
        mat = getMat();
    }
    spdlog::debug("高斯模糊开始，通道数为{}", mat.channels());
    cv::GaussianBlur(mat, mat, cv::Size(0, 0), sigma);
    spdlog::debug("高斯模糊完成");
    // 同步写回 data 向量 (如果你的业务逻辑必须依赖 data)
    setData();
}

void SatelliteImage::applyMedianFilter(int kernelSize) {

    if (mat.empty()) {
        mat = getMat();
    }

    cv::Mat medianFilterMat;
    std::vector<cv::Mat> channels;
    cv::split(mat, channels);
    spdlog::debug("拆分通道");
    spdlog::debug("通道数: {}", channels.size());
    // 只保留前三个通道 (B, G, R)
    std::vector<cv::Mat> bgrChannels = {channels[0], channels[1], channels[2]};
    cv::merge(bgrChannels, medianFilterMat);
    medianFilterMat.convertTo(medianFilterMat, CV_8UC3);
    spdlog::debug("中值滤波开始，通道数为{}", medianFilterMat.channels());
    // 3. 执行中值滤波
    if (kernelSize % 2 == 0)
        kernelSize++; // 确保 kernelSize 是奇数
    cv::medianBlur(medianFilterMat, medianFilterMat, kernelSize);
    // 合并通道
    spdlog::debug("中值滤波完成，通道数为{}", medianFilterMat.channels());
    medianFilterMat.convertTo(medianFilterMat, CV_64FC(3));
    cv::split(medianFilterMat, bgrChannels);
    channels[0] = bgrChannels[0];
    channels[1] = bgrChannels[1];
    channels[2] = bgrChannels[2];
    cv::merge(channels, this->mat);
    setData();
}

void SatelliteImage::normalize() {
    if (data.empty() || data[0].empty())
        return;

    double minVal = std::numeric_limits<double>::max();
    double maxVal = std::numeric_limits<double>::lowest();

    // 1. 寻找所有波段中的全局极值
    for (const auto &row : data) {
        for (const auto &pixel : row) {
            // 检查所有相关波段
            double localMin = pixel.getMin();
            double localMax = pixel.getMax();

            if (localMin < minVal)
                minVal = localMin;
            if (localMax > maxVal)
                maxVal = localMax;
        }
    }

    double range = maxVal - minVal;
    if (range <= 1e-9)
        return; // 防止除以零

    // 2. 执行线性归一化
    for (auto &row : data) {
        for (auto &pixel : row) {
            pixel.setRed((pixel.getRed() - minVal) / range);
            pixel.setGreen((pixel.getGreen() - minVal) / range);
            pixel.setBlue((pixel.getBlue() - minVal) / range);
            pixel.setNir((pixel.getNir() - minVal) / range);
            pixel.setThermal((pixel.getThermal() - minVal) / range);
            // 如果 thermal 也需要归一化，可以一并加入
        }
    }
}

void SatelliteImage::resample(int newWidth, int newHeight) {
    if (data.empty() || data[0].empty() || newWidth <= 0 || newHeight <= 0)
        return;

    int oldRows = data.size();
    int oldCols = data[0].size();
    using Vec5d = cv::Vec<double, 5>;

    // 1. 将原始数据装入 OpenCV 5通道矩阵 (CV_64FC5)
    cv::Mat oldMat(oldRows, oldCols, CV_64FC(5));
    for (int i = 0; i < oldRows; ++i) {
        Vec5d *rowPtr = oldMat.ptr<Vec5d>(i);
        for (int j = 0; j < oldCols; ++j) {
            const auto &p = data[i][j];
            rowPtr[j] = Vec5d(p.getRed(), p.getGreen(), p.getBlue(), p.getNir(), p.getThermal());
        }
    }

    // 2. 执行重采样
    // cv::INTER_LINEAR 为双线性插值，适合大多数遥感场景
    // 如果是缩小图片，推荐使用 cv::INTER_AREA 以减少锯齿
    cv::Mat newMat;
    cv::resize(oldMat, newMat, cv::Size(newWidth, newHeight), 0, 0, cv::INTER_LINEAR);

    // 3. 更新类成员 data 的尺寸并写回数据
    // 使用 assign 重新初始化外层和内层 vector
    data.assign(newHeight, std::vector<Pixel<double>>(newWidth));
    for (int i = 0; i < newHeight; ++i) {
        Vec5d *rowPtr = newMat.ptr<Vec5d>(i);
        for (int j = 0; j < newWidth; ++j) {
            auto &p = data[i][j];
            p.setRed(rowPtr[j][0]);
            p.setGreen(rowPtr[j][1]);
            p.setBlue(rowPtr[j][2]);
            p.setNir(rowPtr[j][3]);
            p.setThermal(rowPtr[j][4]);
        }
    }
}

// 设置器（带验证）

void SatelliteImage::setCloudCover(double cover) {
    if (cover < 0 || cover > 100) {
        throw std::invalid_argument("Cloud cover must be between 0 and 100%");
    }
    cloudCover = cover;
}

void SatelliteImage::setAcquisitionTime(time_t time) {
    if (time < 0) {
        throw std::invalid_argument("Acquisition time cannot be negative");
    }
    acquisitionTime = time;
}

void SatelliteImage::setData() {
    int rows = mat.rows;
    int cols = mat.cols;
    for (int i = 0; i < rows; ++i) {
        // 使用指针访问，比 .at 块 10 倍以上
        const Vec5d *ptr = mat.ptr<Vec5d>(i);
        for (int j = 0; j < cols; ++j) {
            auto &p = data[i][j];
            p.setRed(ptr[j][0]);
            p.setGreen(ptr[j][1]);
            p.setBlue(ptr[j][2]);
            p.setNir(ptr[j][3]);
            p.setThermal(ptr[j][4]);
        }
    }
    spdlog::debug("setData");
}

// 静态工厂方法
SatelliteImage SatelliteImage::createRandomImage(const std::string &id, int w, int h) {

    SatelliteImage img(id, "Name:", "Path:", w, h, 5, "RandomSensor", time(nullptr));
    // 1. 初始化二维 vector 容器

    img.data.assign(h, std::vector<Pixel<double>>(w));

    // 2. 配置随机数引擎 (使用 mt19937 性能较好)
    std::random_device rd;
    std::mt19937 gen(rd());

    // 假设归一化后的波段范围是 [0.0, 1.0]
    std::uniform_real_distribution<double> dis(0.0, 1.0);
    // 热红外波段通常范围不同，这里假设范围在 270.0 - 310.0 (开尔文)
    std::uniform_real_distribution<double> thermalDis(270.0, 310.0);

    // 3. 填充随机像素数据
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            auto &p = img.data[i][j];
            p.setRed(dis(gen));
            p.setGreen(dis(gen));
            p.setBlue(dis(gen));
            p.setNir(dis(gen));
            p.setThermal(thermalDis(gen));
        }
    }
    return img;
}

SatelliteImage SatelliteImage::createConstantImage(const std::string &id, int w, int h,
                                                   const Pixel<double> &value) {

    SatelliteImage img(id, "Name:", "Path:", w, h, 5, "ConstantSensor", time(nullptr));

    // 1. 检查输入参数的合法性
    if (w <= 0 || h <= 0) {
        return img;
    }

    // 2. 利用 vector 的构造函数直接填充数据
    // 这一步会创建一个包含 h 条线的 vector，每条线都由 w 个初始值为 value 的 Pixel 组成
    img.data.assign(h, std::vector<Pixel<double>>(w, value));

    return img;
}

cv::Mat SatelliteImage::get8UC3Mat() const {
    std::vector<cv::Mat> channels;
    cv::Mat result;
    cv::split(mat, channels);
    spdlog::debug("拆分通道");
    spdlog::debug("通道数: {}", channels.size());
    // 只保留前三个通道 (B, G, R)
    std::vector<cv::Mat> bgrChannels = {channels[0], channels[1], channels[2]};
    cv::merge(bgrChannels, result);
    result.convertTo(result, CV_8UC3);
    return result;
}