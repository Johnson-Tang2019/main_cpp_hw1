template<typename T>
class Pixel {
private:
    T red, green, blue, nir;     // 多光谱波段
    T thermal;                    // 热红外波段
    double timestamp;             // 采集时间戳
    mutable int accessCount;      // 访问计数（mutable演示）
    
public:
    // 构造函数（支持默认参数）
    Pixel(T r = 0, T g = 0, T b = 0, T n = 0, T t = 0, double ts = 0);
    
    // 拷贝构造函数
    Pixel(const Pixel<T>& other);
    
    // 析构函数
    ~Pixel();
    
    // 赋值运算符
    Pixel<T>& operator=(const Pixel<T>& other);
    
    // 算术运算符重载
    Pixel<T> operator+(const Pixel<T>& other) const;
    Pixel<T> operator-(const Pixel<T>& other) const;
    Pixel<T> operator*(T factor) const;
    Pixel<T> operator/(T divisor) const;
    Pixel<T>& operator+=(const Pixel<T>& other);
    Pixel<T>& operator-=(const Pixel<T>& other);
    
    // 比较运算符
    bool operator==(const Pixel<T>& other) const;
    bool operator!=(const Pixel<T>& other) const;
    
    // 类型转换运算符
    operator T() const { return getBrightness(); }  // 转换为亮度值
    operator string() const;                         // 转换为字符串
    
    // 下标运算符（访问波段）
    T& operator[](int band);
    const T& operator[](int band) const;
    
    // 友元函数（流输入输出）
    friend ostream& operator<<(ostream& os, const Pixel<T>& pixel) {
        os << "R:" << pixel.red << " G:" << pixel.green 
           << " B:" << pixel.blue << " NIR:" << pixel.nir
           << " Thermal:" << pixel.thermal;
        return os;
    }
    
    // 遥感指数计算
    double getNDVI() const;       // 归一化植被指数
    double getNDWI() const;       // 归一化水体指数
    double getSAVI() const;       // 土壤调节植被指数
    double getBrightness() const;  // 亮度值
    double getTemperature() const;  // 亮温（基于热红外）
    
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