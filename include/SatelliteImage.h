#include"DataObject.h"
#include<vector>
#include "Pixel.h"
#include<iostream>
#include<fstream>


class SatelliteImage : public DataObject {
private:
    int width;                          // 宽度（像素）
    int height;                         // 高度（像素）
    int bands;                          // 波段数量
    std::string sensorType;                  // 传感器类型
    double cloudCover;                  // 云量百分比
    double acquisitionTime;             // 采集时间（儒略日）
    std::vector<std::vector<Pixel<double>>> data; // 影像数据矩阵
    std::vector<double> bandStatistics;      // 波段统计信息
    
    // 私有辅助函数
    void calculateStatistics();          // 计算统计信息
    void validateCoordinates(int x, int y) const;  // 坐标验证
    
public:
    // 构造函数
    SatelliteImage(const std::string& id, const std::string& name, const std::string& path,
                   int w, int h, int b, const std::string& sensor, double time = 0);
    
    // 拷贝构造函数
    SatelliteImage(const SatelliteImage& other);
    
    // 析构函数
    ~SatelliteImage();
    
    // 赋值运算符
    SatelliteImage& operator=(const SatelliteImage& other);
    
    // 实现基类虚函数
    void display() const override;
    DataObject* clone() const override;
    bool exportData(const std::string& format) const override;
    std::string getType() const override { return "SatelliteImage"; }
    double getQualityScore() const override;
    bool isValid() const override;
    
    // 算术运算符重载（影像运算）
    SatelliteImage operator+(const SatelliteImage& other) const;
    SatelliteImage operator-(const SatelliteImage& other) const;
    SatelliteImage operator*(double factor) const;
    SatelliteImage operator/(double divisor) const;
    SatelliteImage& operator+=(const SatelliteImage& other);
    SatelliteImage& operator-=(const SatelliteImage& other);
    
    // 比较运算符
    bool operator==(const SatelliteImage& other) const;
    bool operator!=(const SatelliteImage& other) const;
    
    // 下标运算符（访问行）
    std::vector<Pixel<double>>& operator[](int row);
    const std::vector<Pixel<double>>& operator[](int row) const;
    
    // 类型转换运算符
    operator double() const { return cloudCover; }  // 转换为云量
    operator std::string() const;                         // 转换为摘要字符串
    
    // 子图操作
    SatelliteImage getSubImage(int x, int y, int w, int h) const;
    
    // 波段操作
    std::vector<double> getBandValues(int band) const;  // 0:R,1:G,2:B,3:NIR,4:Thermal
    SatelliteImage extractBand(int band) const;
    
    // 遥感指数计算（返回二维数组）
    std::vector<std::vector<double>> calculateNDVI() const;
    std::vector<std::vector<double>> calculateNDWI() const;
    std::vector<std::vector<double>> calculateSAVI(double L = 0.5) const;
    std::vector<std::vector<double>> calculateTemperature() const;
    
    // 影像统计
    double getMeanValue() const;
    double getStdDev() const;
    double getMinValue() const;
    double getMaxValue() const;
    void printStatistics() const;
    
    // 影像处理
    void applyGaussianBlur(double sigma);
    void applyMedianFilter(int kernelSize);
    void normalize();
    void resample(int newWidth, int newHeight);
    
    // 获取器
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getBands() const { return bands; }
    std::string getSensorType() const { return sensorType; }
    double getCloudCover() const { return cloudCover; }
    double getAcquisitionTime() const { return acquisitionTime; }
    
    // 设置器（带验证）
    void setCloudCover(double cover);
    void setAcquisitionTime(double time);
    
    // 静态工厂方法
    static SatelliteImage createRandomImage(const std::string& id, int w, int h);
    static SatelliteImage createConstantImage(const std::string& id, int w, int h, 
                                               const Pixel<double>& value);
};