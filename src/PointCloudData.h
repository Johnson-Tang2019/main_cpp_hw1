#ifndef POINTCLOUDDATA_H
#define POINTCLOUDDATA_H

#include "DataObject.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <numeric>
#include <algorithm>
#include <cmath>

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
    std::vector<Point3D> points;
    double minX, maxX, minY, maxY, minZ, maxZ;  // 边界框
    
    void updateBounds();
    
public:
    PointCloudData(const std::string& id, const std::string& name, const std::string& path);
    PointCloudData(const PointCloudData& other);
    ~PointCloudData();
    
    // 实现基类虚函数
    void display() const override;
    DataObject* clone() const override;
    bool exportData(const std::string& format) const override;
    std::string getType() const override { return "PointCloud"; }
    
    // 运算符重载
    PointCloudData operator+(const PointCloudData& other) const;
    PointCloudData& operator+=(const PointCloudData& other);
    Point3D& operator[](int index);
    const Point3D& operator[](int index) const;
    
    // 点云操作
    void addPoint(const Point3D& point);
    void addPoints(const std::vector<Point3D>& points);
    int getPointCount() const;
    
    // 点云滤波
    PointCloudData voxelFilter(double voxelSize) const;
    PointCloudData statisticalOutlierRemoval(int k, double stdDev) const;
    
    // 统计信息
    double getAverageHeight() const;
    double getHeightRange() const;
    void printStatistics() const;
    
    // 获取器
    const std::vector<Point3D>& getPoints() const { return points; }
    double getMinX() const { return minX; }
    double getMaxX() const { return maxX; }
    double getMinY() const { return minY; }
    double getMaxY() const { return maxY; }
    double getMinZ() const { return minZ; }
    double getMaxZ() const { return maxZ; }

};

#endif  // POINTCLOUDDATA_H