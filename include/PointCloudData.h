#ifndef POINTCLOUDDATA_H
#define POINTCLOUDDATA_H

#include "DataObject.h"
#include "spdlog/spdlog.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <ctime>
#include <format>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <unordered_map>
#include <vector>

struct Point3D {
    double x, y, z;     // 三维坐标
    double intensity;   // 回波强度
    int classification; // 分类标签

    Point3D(double x = 0, double y = 0, double z = 0, double intensity = 0, int cls = 0);

    // 运算符重载
    Point3D operator+(const Point3D &other) const;
    Point3D operator-(const Point3D &other) const;
    double distanceTo(const Point3D &other) const;
};

struct VoxelIdx {
    long long x, y, z;
    bool operator==(const VoxelIdx &o) const { return x == o.x && y == o.y && z == o.z; }
};

struct VoxelSum {
    double x = 0, y = 0, z = 0;
    int count = 0;
};

struct VoxelHasher {
    size_t operator()(const VoxelIdx &v) const {
        // 使用质数扰动来减少哈希碰撞
        size_t h1 = std::hash<long long>{}(v.x);
        size_t h2 = std::hash<long long>{}(v.y);
        size_t h3 = std::hash<long long>{}(v.z);
        // 位移并异或，将 3D 坐标合并为一个哈希值
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

class PointCloudData : public DataObject {
  private:
    std::vector<Point3D> points;
    double minX, maxX, minY, maxY, minZ, maxZ; // 边界框

    void updateBounds();

  public:
    PointCloudData(const std::string &id, const std::string &name, const std::string &path);
    PointCloudData(const std::string &id, const std::string &name, const std::string &path,
                   const std::vector<Point3D> &points);
    PointCloudData(const PointCloudData &other);
    ~PointCloudData();

    // 实现基类虚函数
    void display() const override;
    DataObject *clone() const override;
    bool exportData(const std::string &format) const override;
    bool exportDataInPath(const std::string &format, const std::string &path) const override;
    std::string getType() const override { return "PointCloud"; }

    // 运算符重载
    PointCloudData operator+(const PointCloudData &other) const;
    PointCloudData &operator+=(const PointCloudData &other);
    Point3D &operator[](int index);
    const Point3D &operator[](int index) const;

    // 点云操作
    void addPoint(const Point3D &point);
    void addPoints(const std::vector<Point3D> &points);
    int getPointCount() const;

    // 点云滤波
    PointCloudData voxelFilter(double voxelSize);
    PointCloudData statisticalOutlierRemoval(int k, double stdDev);

    // 统计信息
    double getAverageHeight() const;
    double getHeightRange() const;
    void printStatistics() const;

    // 获取器
    const std::vector<Point3D> &getPoints() const { return points; }
    double getMinX() const { return minX; }
    double getMaxX() const { return maxX; }
    double getMinY() const { return minY; }
    double getMaxY() const { return maxY; }
    double getMinZ() const { return minZ; }
    double getMaxZ() const { return maxZ; }

    void loadPLY(const std::string &filename);

    void deleteErrorPoints();
};

#endif // POINTCLOUDDATA_H