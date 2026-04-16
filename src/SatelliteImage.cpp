#include "SatelliteImage.h"

// 构造函数
SatelliteImage:: SatelliteImage(const std::string& id, const std::string& name, const std::string& path,
                   int w, int h, int b, const std::string& sensor, time_t time)
    : DataObject(id, name, path), width(w), height(h), bands(b), sensorType(sensor),
     acquisitionTime(time) {}

// 拷贝构造函数
SatelliteImage::SatelliteImage(const SatelliteImage& other)
    : DataObject(other), width(other.width), height(other.height), bands(other.bands),
      sensorType(other.sensorType), cloudCover(other.cloudCover), acquisitionTime(other.acquisitionTime),
      data(other.data), bandStatistics(other.bandStatistics) {}


SatelliteImage& SatelliteImage::operator=(const SatelliteImage& other) {
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
    std::cout << "Created: " << ctime(&createTime);
    std::cout << "Dimensions: " << width << "x" << height << " Bands: " << bands << "\n";
    std::cout << "Sensor: " << sensorType << " Cloud Cover: " <<
                cloudCover << "% Acquisition Time: " << acquisitionTime << "\n";
    for(auto i : data){
        for(auto j : i){
            std::cout << j << " ";
        }
        std::cout << "\n";
    }
    std::cout << "Band Statistics: ";
    for (const auto& stat : bandStatistics) {
        std::cout << stat << " ";
    }
    std::cout << "\n";
}


DataObject* SatelliteImage::clone() const {
    return new SatelliteImage(*this);
}

// 实现数据导出逻辑
bool SatelliteImage::exportData(const std::string& format) const{

    // 这里可以根据format参数实现不同格式的导出逻辑
    std::cout << "Exporting SatelliteImage in format: " << format << "\n";
    std::ofstream outFile(name + "." + format);

    // 导出逻辑
    if (format == "TXT") {
        // 导出为TXT格式
        outFile << "SatelliteImage: " << name << " (" << id << ")\n";
        outFile << "Path: " << path << "\n";  
        outFile << "Size: " << size << " MB\n";
        outFile << "Created: " << ctime(&createTime);
        
        

    } else {
        std::cerr << "Unsupported export format: " << format << "\n";
        return false;
    }
    
}

double SatelliteImage::getQualityScore() const {
    return 100.0 - cloudCover;  // 简单示例：质量分数与云量成反比
}

bool SatelliteImage::isValid() const {
    return width > 0 && height > 0 && bands > 0 && !sensorType.empty() && !data.empty();
}   

// 算术运算符重载（影像运算)

SatelliteImage SatelliteImage::operator+(const SatelliteImage& other) const {
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

SatelliteImage SatelliteImage::operator-(const SatelliteImage& other) const {
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

SatelliteImage& SatelliteImage::operator+=(const SatelliteImage& other) {
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

SatelliteImage& SatelliteImage::operator-=(const SatelliteImage& other) {
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
bool SatelliteImage::operator==(const SatelliteImage& other) const {
    return id == other.id;  // 基于唯一标识符进行比较
}

bool SatelliteImage::operator!=(const SatelliteImage& other) const {
    return !(*this == other);
}

// 下标运算符（访问行）
std::vector<Pixel<double>>& SatelliteImage::operator[](int row) {
    if (row < 0 || row >= height) { 
        throw std::out_of_range("Row index out of range");
    }
    return data[row];
}   

const std::vector<Pixel<double>>& SatelliteImage::operator[](int row) const {
    if (row < 0 || row >= height) { 
        throw std::out_of_range("Row index out of range");
    }
    return data[row];
}

// 类型转换运算符
SatelliteImage::operator std::string() const {
    return DataObject::operator std::string() + " [" + sensorType + " " + std::to_string(width) + "x" + std::to_string(height) + "]";
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
            bandImage.data[i][j] = Pixel<double>(value, value, value, value, value); // 将单波段值复制到所有波段
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
            double red = data[i][j].getRed();
            double green = data[i][j].getGreen();
            double blue = data[i][j].getBlue();
            double nir = data[i][j].getNir();
            ndvi[i][j] = (nir - red) / (nir + red);
        }
    }
    return ndvi;
}

std::vector<std::vector<double>> SatelliteImage::calculateNDWI() const {
    if (bands != 4) {
        throw std::invalid_argument("NDWI calculation requires 4 bands (red, green, blue, nir)");
    }
    std::vector<std::vector<double>> ndwi(height, std::vector<double>(width));
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            double green = data[i][j].getGreen();
            double nir = data[i][j].getNir();
            ndwi[i][j] = (green - nir) / (green + nir);
        }
    }
    return ndwi;
}

std::vector<std::vector<double>> SatelliteImage::calculateSAVI(double L = 0.5) const {
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
        throw std::invalid_argument("Temperature calculation requires 4 bands (red, green, blue, nir)");
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
    for (const auto& row : data) {
        for (const auto& pixel : row) {
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
    for (const auto& row : data) {
        for (const auto& pixel : row) {
            sumSqDiff += std::pow(pixel.getBrightness() - mean, 2);
            count += 5; // 每个像素有5个波段
        }
    }
    return (count > 1) ? std::sqrt(sumSqDiff / (count - 1)) : 0.0;
}

double SatelliteImage::getMinValue() const {
    double minVal = std::numeric_limits<double>::max();
    for (const auto& row : data) {
        for (const auto& pixel : row) {
            minVal = std::min({minVal, pixel.getRed(), pixel.getGreen(), pixel.getBlue(), pixel.getNir(), pixel.getThermal()});
        }
    }
    return minVal;
}

double SatelliteImage::getMaxValue() const {
    double maxVal = std::numeric_limits<double>::lowest();
    for (const auto& row : data) {
        for (const auto& pixel : row) {
            maxVal = std::max({maxVal, pixel.red, pixel.green, pixel.blue, pixel.nir, pixel.thermal});
        }
    }
    return maxVal;
}

void SatelliteImage::printStatistics() const {
    std::cout << "Mean Value: " << getMeanValue() << "\n";
    std::cout << "Standard Deviation: " << getStdDev() << "\n";
    std::cout << "Min Value: " << getMinValue() << "\n";
    std::cout << "Max Value: " << getMaxValue() << "\n";
}


// 影像处理   ***********未实现************

void SatelliteImage::applyGaussianBlur(double sigma) {
    // 这里可以实现高斯模糊的具体逻辑
    std::cout << "Applying Gaussian Blur with sigma: " << sigma << "\n";
    // 简化处理，实际应用中需要实现卷积操作
}

void SatelliteImage::applyMedianFilter(int kernelSize) {
    // 这里可以实现中值滤波的具体逻辑
    std::cout << "Applying Median Filter with kernel size: " << kernelSize << "\n";
    // 简化处理，实际应用中需要实现中值滤波
}

void SatelliteImage::normalize() {
    // 这里可以实现影像归一化的具体逻辑
    std::cout << "Normalizing SatelliteImage data\n";
    // 简化处理，实际应用中需要实现归一化算法
}

void SatelliteImage::resample(int newWidth, int newHeight) {
    // 这里可以实现影像重采样的具体逻辑
    std::cout << "Resampling SatelliteImage to " << newWidth << "x" << newHeight << "\n";
    // 简化处理，实际应用中需要实现重采样算法
}

// 设置器（带验证）

void SatelliteImage::setCloudCover(double cover) {
    if (cover < 0 || cover > 100) {
        throw std::invalid_argument("Cloud cover must be between 0 and 100%");
    }
    cloudCover = cover;
}

void SatelliteImage::setAcquisitionTime(double time) {
    if (time < 0) {
        throw std::invalid_argument("Acquisition time cannot be negative");
    }
    acquisitionTime = static_cast<time_t>(time);
}


 // 静态工厂方法  ******************未实现*****************
SatelliteImage SatelliteImage::createRandomImage
    (const std::string& id, int w, int h) {
    // 这里可以实现随机影像的创建逻辑
    return SatelliteImage(id, w, h, 5, "RandomSensor"
        , time(nullptr));
}















