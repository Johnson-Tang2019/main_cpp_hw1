#ifndef PIXEL_H
#define PIXEL_H

#include <iostream>
#include <ctime>


template<typename T>
class Pixel {
private:
    T red, green, blue, nir;     // 多光谱波段
    T thermal;                    // 热红外波段
    time_t timestamp;             // 采集时间戳
    mutable int accessCount;      // 访问计数（mutable演示）
    
public:
    // 构造函数（支持默认参数）
    Pixel(T r = 0, T g = 0, T b = 0, T n = 0, T t = 0, time_t ts = time(nullptr)) 
        : red(r), green(g), blue(b), nir(n), thermal(t), timestamp(ts), accessCount(0) {};
    
    // 拷贝构造函数
    Pixel(const Pixel<T>& other) : red(other.red), green(other.green), blue(other.blue), 
    nir(other.nir), thermal(other.thermal), timestamp(other.timestamp), accessCount(0) {};
    
    // 析构函数
    ~Pixel() {};
    
    // 赋值运算符
    Pixel<T>& operator= (const Pixel<T>& other) {
        if (this != &other) {
            red = other.red;
            green = other.green;
            blue = other.blue;
            nir = other.nir;
            thermal = other.thermal;
            timestamp = other.timestamp;
            accessCount = 0; // 重置访问计数
        }
        return *this;
    }
    
    // 算术运算符重载
    Pixel<T> operator+(const Pixel<T>& other) const {
        return Pixel<T>(red + other.red, green + other.green, blue + other.blue, 
                        nir + other.nir, thermal + other.thermal, timestamp);
    }
    Pixel<T> operator-(const Pixel<T>& other) const {
        return Pixel<T>(red - other.red, green - other.green, blue - other.blue, 
                        nir - other.nir, thermal - other.thermal, timestamp);
    }
    Pixel<T> operator*(T factor) const {
        return Pixel<T>(red * factor, green * factor, blue * factor, 
                        nir * factor, thermal * factor, timestamp);
    }
    Pixel<T> operator/(T divisor) const {
        return Pixel<T>(red / divisor, green / divisor, blue / divisor, 
                        nir / divisor, thermal / divisor, timestamp);
    }
    Pixel<T>& operator+=(const Pixel<T>& other) {
        red += other.red;
        green += other.green;
        blue += other.blue;
        nir += other.nir;
        thermal += other.thermal;
        return *this;
    }
    Pixel<T>& operator-=(const Pixel<T>& other) {
        red -= other.red;
        green -= other.green;
        blue -= other.blue;
        nir -= other.nir;
        thermal -= other.thermal;
        return *this;
    }
    
    // 比较运算符
    bool operator==(const Pixel<T>& other) const {
        return red == other.red && green == other.green && blue == other.blue &&
               nir == other.nir && thermal == other.thermal;
    }
    bool operator!=(const Pixel<T>& other) const {
        return !(*this == other);
    }
    
    // 类型转换运算符
    operator T() const { return getBrightness(); }  // 转换为亮度值
    operator std::string() const { 
        return "Pixel" + std::to_string(red) + "," + std::to_string(green) + "," + std::to_string(blue) + 
               "," + std::to_string(nir) + "," + std::to_string(thermal); 
    }                         // 转换为字符串
    
    // 下标运算符（访问波段）
    T& operator[](int band) {
        switch (band) {
            case 0: return red;
            case 1: return green;
            case 2: return blue;
            case 3: return nir;
            case 4: return thermal;
            default: throw std::out_of_range("Band index must be between 0 and 4");
        }
    }

    const T& operator[](int band) const {
        switch (band) {
            case 0: return red;
            case 1: return green;
            case 2: return blue;
            case 3: return nir;
            case 4: return thermal;
            default: throw std::out_of_range("Band index must be between 0 and 4");
        }
    }
    
    // 友元函数（流输入输出）
    friend std::ostream& operator<<(std::ostream& os, const Pixel<T>& pixel) {
        os << "R:" << pixel.red << " G:" << pixel.green 
           << " B:" << pixel.blue << " NIR:" << pixel.nir
           << " Thermal:" << pixel.thermal;
        return os;
    }
    
    // 遥感指数计算
    double getNDVI() const {    // 归一化植被指数
        double numerator = nir - red;
        double denominator = nir + red;
        return (denominator == 0) ? 0 : (numerator / denominator);
    }      

    double getNDWI() const {  // 归一化水体指数
        double numerator = green - nir;
        double denominator = green + nir;
        return (denominator == 0) ? 0 : (numerator / denominator);
    }

    double getSAVI() const {  // 土壤调节植被指数
        double L = 0.5;
        double numerator = (nir - red) * (1 + L);
        double denominator = nir + red + L;
        return (denominator == 0) ? 0 : (numerator / denominator);
    }

    double getBrightness() const {  // 亮度值
        return (red + green + blue) / 3.0;
    }
    ///////////      未实现            ////////////////////////
    double getTemperature() const {  // 亮温（基于热红外）
        // 这里可以实现具体的亮温计算逻辑
        return thermal;  // 简化处理，实际应用中需要更复杂的计算
    }
    
    // 获取器
    T getRed() const { accessCount++; return red; }
    T getGreen() const { accessCount++; return green; }
    T getBlue() const { accessCount++; return blue; }
    T getNir() const { accessCount++; return nir; }
    T getThermal() const { accessCount++; return thermal; }
    int getAccessCount() const { return accessCount; }
    
    // 设置器
    void setRed(T r) { red = r; }
    void setGreen(T g) { green = g; }
    void setBlue(T b) { blue = b; }
    void setNir(T n) { nir = n; }
    void setThermal(T t) { thermal = t; }
};

#endif // PIXEL_H
