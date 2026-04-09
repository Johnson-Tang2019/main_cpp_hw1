struct Point3D {
    double x, y, z;      // 三维坐标
    double intensity;    // 回波强度
    int classification;  // 分类标签
    
    Point3D(double x = 0, double y = 0, double z = 0, 
            double intensity = 0, int cls = 0);
    
    // 运算符重载
    Point3D operator+(const Point3D& other) const;
    Point3D operator-(const Point3D& other) const;
    double distanceTo(const Point3D& other) const;
};

class PointCloudData : public DataObject {
private:
    vector<Point3D> points;
    double minX, maxX, minY, maxY, minZ, maxZ;  // 边界框
    
    void updateBounds();
    
public:
    PointCloudData(const string& id, const string& name, const string& path);
    PointCloudData(const PointCloudData& other);
    ~PointCloudData();
    
    // 实现基类虚函数
    void display() const override;
    DataObject* clone() const override;
    bool exportData(const string& format) const override;
    string getType() const override { return "PointCloud"; }
    
    // 运算符重载
    PointCloudData operator+(const PointCloudData& other) const;
    PointCloudData& operator+=(const PointCloudData& other);
    Point3D& operator[](int index);
    const Point3D& operator[](int index) const;
    
    // 点云操作
    void addPoint(const Point3D& point);
    void addPoints(const vector<Point3D>& points);
    int getPointCount() const;
    
    // 点云滤波
    PointCloudData voxelFilter(double voxelSize) const;
    PointCloudData statisticalOutlierRemoval(int k, double stdDev) const;
    
    // 统计信息
    double getAverageHeight() const;
    double getHeightRange() const;
    void printStatistics() const;
    
    // 获取器
    const vector<Point3D>& getPoints() const { return points; }
    double getMinX() const { return minX; }
    double getMaxX() const { return maxX; }
    // ... 其他边界获取器
};