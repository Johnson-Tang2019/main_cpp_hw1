#include "SatelliteImage.h"

// 构造函数
SatelliteImage:: SatelliteImage(const std::string& id, const std::string& name, const std::string& path,
                   int w, int h, int b, const std::string& sensor, double time = 0)
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