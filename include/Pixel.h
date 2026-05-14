#ifndef PIXEL_H
#define PIXEL_H

#include <ctime>
#include <iostream>

/**
 * @class Pixel
 * @brief 多光谱遥感像素模板类。
 * 支持常见的 RGB 波段、近红外 (NIR) 以及热红外 (Thermal) 波段的数据存储与指数运算。
 * @tparam T 波段数据类型（如 unsigned char, uint16_t, float, double）
 */

template <typename T> class Pixel {
  private:
    T red, green, blue, nir; // 多光谱波段
    T thermal;               // 热红外波段
    time_t timestamp;        // 采集时间戳
    mutable int accessCount; // 访问计数（mutable演示）

  public:
    // =================================================================
    // 1. 构造、析构与拷贝
    // =================================================================

    /** @brief 构造函数（支持默认参数） */
    Pixel(T r = 0, T g = 0, T b = 0, T n = 0, T t = 0, time_t ts = time(nullptr))
        : red(r), green(g), blue(b), nir(n), thermal(t), timestamp(ts), accessCount(0) {};

    /** @brief 拷贝构造函数 */
    Pixel(const Pixel<T> &other)
        : red(other.red), green(other.green), blue(other.blue), nir(other.nir),
          thermal(other.thermal), timestamp(other.timestamp), accessCount(0) {};

    /** @brief 析构函数 */
    ~Pixel() {};

    /** @brief 赋值运算符 */
    Pixel<T> &operator=(const Pixel<T> &other) {
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

    /** @brief 加法运算符 */
    Pixel<T> operator+(const Pixel<T> &other) const {
        return Pixel<T>(red + other.red, green + other.green, blue + other.blue, nir + other.nir,
                        thermal + other.thermal, timestamp);
    }

    /** @brief 减法运算符 */
    Pixel<T> operator-(const Pixel<T> &other) const {
        return Pixel<T>(red - other.red, green - other.green, blue - other.blue, nir - other.nir,
                        thermal - other.thermal, timestamp);
    }

    /** @brief 乘法运算符 */
    Pixel<T> operator*(T factor) const {
        return Pixel<T>(red * factor, green * factor, blue * factor, nir * factor, thermal * factor,
                        timestamp);
    }

    /** @brief 除法运算符 */
    Pixel<T> operator/(T divisor) const {
        return Pixel<T>(red / divisor, green / divisor, blue / divisor, nir / divisor,
                        thermal / divisor, timestamp);
    }

    /** @brief 加法赋值运算符 */
    Pixel<T> &operator+=(const Pixel<T> &other) {
        red += other.red;
        green += other.green;
        blue += other.blue;
        nir += other.nir;
        thermal += other.thermal;
        return *this;
    }

    /** @brief 减法赋值运算符 */
    Pixel<T> &operator-=(const Pixel<T> &other) {
        red -= other.red;
        green -= other.green;
        blue -= other.blue;
        nir -= other.nir;
        thermal -= other.thermal;
        return *this;
    }

    /** @brief 等号运算符 */
    bool operator==(const Pixel<T> &other) const {
        return red == other.red && green == other.green && blue == other.blue && nir == other.nir &&
               thermal == other.thermal;
    }
    bool operator!=(const Pixel<T> &other) const { return !(*this == other); }

    /** @brief 类型转换运算符 */
    operator T() const { return getBrightness(); } // 转换为亮度值

    /** @brief 转换为字符串 */
    operator std::string() const {
        return "Pixel" + std::to_string(timestamp) + "," + std::to_string(red) + "," +
               std::to_string(green) + "," + std::to_string(blue) + "," + std::to_string(nir) +
               "," + std::to_string(thermal);
    }

    /** @brief 下标运算符（访问波段） */
    T &operator[](int band) {
        switch (band) {
        case 0:
            return red;
        case 1:
            return green;
        case 2:
            return blue;
        case 3:
            return nir;
        case 4:
            return thermal;
        default:
            throw std::out_of_range("Band index must be between 0 and 4");
        }
    }

    /** @brief 下标运算符（访问波段） */
    const T &operator[](int band) const {
        switch (band) {
        case 0:
            return red;
        case 1:
            return green;
        case 2:
            return blue;
        case 3:
            return nir;
        case 4:
            return thermal;
        default:
            throw std::out_of_range("Band index must be between 0 and 4");
        }
    }

    /** @brief 友元函数（流输入输出） */
    friend std::ostream &operator<<(std::ostream &os, const Pixel<T> &pixel) {
        os << "R:" << pixel.red << " G:" << pixel.green << " B:" << pixel.blue
           << " NIR:" << pixel.nir << " Thermal:" << pixel.thermal;
        return os;
    }

    /** @brief 遥感指数计算 */
    double getNDVI() const { // 归一化植被指数
        double numerator = nir - red;
        double denominator = nir + red;
        return (denominator == 0) ? 0 : (numerator / denominator);
    }

    /** @brief 遥感指数计算 */
    double getNDWI() const { // 归一化水体指数
        double numerator = green - nir;
        double denominator = green + nir;
        return (denominator == 0) ? 0 : (numerator / denominator);
    }

    /** @brief 遥感指数计算 */
    double getSAVI() const { // 土壤调节植被指数
        double L = 0.5;
        double numerator = (nir - red) * (1 + L);
        double denominator = nir + red + L;
        return (denominator == 0) ? 0 : (numerator / denominator);
    }

    /** @brief 遥感指数计算 */
    double getBrightness() const { // 亮度值
        return (red + green + blue) / 3.0;
    }

    /** @brief 遥感指数计算 */
    double getTemperature() const { // 亮温（基于热红外）
        double t = static_cast<double>(thermal);
        if (t <= 0)
            return -273.15; // 绝对零度保护

        // 假设是 Landsat 8 Band 10 的标准定标参数
        double L = 0.0003342 * t + 0.1;
        double tempK = 1321.08 / std::log(774.89 / L + 1.0);
        return tempK - 273.15; // 转为摄氏度
    }

    /** @brief 获取红色波段值 */
    T getRed() const {
        accessCount++;
        return red;
    }

    /** @brief 获取绿色波段值 */
    T getGreen() const {
        accessCount++;
        return green;
    }

    /** @brief 获取蓝色波段值 */
    T getBlue() const {
        accessCount++;
        return blue;
    }

    /** @brief 获取近红外波段值 */
    T getNir() const {
        accessCount++;
        return nir;
    }

    /** @brief 获取热红外波段值 */
    T getThermal() const {
        accessCount++;
        return thermal;
    }

    /** @brief 获取访问次数 */
    int getAccessCount() const { return accessCount; }

    /** @brief 获取最大值 */
    T getMax() const { return std::max({red, green, blue, nir, thermal}); }

    /** @brief 获取最小值 */
    T getMin() const { return std::min({red, green, blue, nir, thermal}); }

    /** @brief 设置红色波段值 */
    void setRed(T r) { red = r; }

    /** @brief 设置绿色波段值 */
    void setGreen(T g) { green = g; }

    /** @brief 设置蓝色波段值 */
    void setBlue(T b) { blue = b; }

    /** @brief 设置近红外波段值 */
    void setNir(T n) { nir = n; }

    /** @brief 设置热红外波段值 */
    void setThermal(T t) { thermal = t; }
};

#endif // PIXEL_H
