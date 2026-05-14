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

/**
 * @struct Point3D
 * @brief 单个三维点结构体，包含坐标、强度及分类。
 */
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

/**
 * @struct VoxelIdx
 * @brief 体素索引结构，用于体素滤波中的空间划分。
 * 每个三维网格格网通过其整数坐标 (lx, ly, lz) 进行唯一标识。
 */
struct VoxelIdx {
    long long x, y, z;
    bool operator==(const VoxelIdx &o) const { return x == o.x && y == o.y && z == o.z; }
};

/**
 * @struct VoxelSum
 * @brief 体素统计结构，用于体素滤波中的点云统计。
 * 每个体素包含其点云的总强度、总点数和中心坐标。
 */
struct VoxelSum {
    double x = 0, y = 0, z = 0;
    int count = 0;
};

/**
 * @struct VoxelHasher
 * @brief 体素哈希结构，用于体素滤波中的点云统计。
 * 每个体素的哈希值通过 3D 坐标合并为一个值，确保唯一性。
 */
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

/**
 * @class PointCloudData
 * @brief 三维点云管理类。
 * 包含点云数据、边界框信息、点云操作方法等。
 */
class PointCloudData : public DataObject {
  private:
    std::vector<Point3D> points;
    double minX, maxX, minY, maxY, minZ, maxZ; // 边界框
    /** @brief 遍历所有点，同步更新 AABB 边界范围 */
    void updateBounds();

  public:
    PointCloudData(const std::string &id, const std::string &name, const std::string &path);
    PointCloudData(const std::string &id, const std::string &name, const std::string &path,
                   const std::vector<Point3D> &points);
    PointCloudData(const PointCloudData &other);
    ~PointCloudData();

    // =================================================================
    // 1. 基类虚函数实现
    // =================================================================
    void display() const override;
    DataObject *clone() const override;
    bool exportData(const std::string &format) const override;

    /** @brief 将点云导出至指定路径（如 PLY 格式） */
    bool exportDataInPath(const std::string &format, const std::string &path) const override;
    std::string getType() const override { return "PointCloud"; }

    // =================================================================
    // 2. 运算符重载 (点云合并)
    // =================================================================

    /** @brief 合并两个点云对象，产生新的点云 */
    PointCloudData operator+(const PointCloudData &other) const;
    PointCloudData &operator+=(const PointCloudData &other);

    /** @brief 索引访问：直接获取或修改单个点 */
    Point3D &operator[](int index);
    const Point3D &operator[](int index) const;

    void addPoint(const Point3D &point);
    void addPoints(const std::vector<Point3D> &points);
    int getPointCount() const;

    // =================================================================
    // 3. 核心滤波算法 (Processing Algorithms)
    // =================================================================

    /**
     * @brief 体素网格滤波器（下采样）
     * 将空间划分为立方体网格，每个网格仅保留一个重心点，大幅减少点云冗余。
     * @param voxelSize 立方体边长（单位通常为米，如 0.1m 代表 10 厘米网格）
     */
    PointCloudData voxelFilter(double voxelSize);

    /**
     * @brief 统计学离群点去除 (SOR)
     * 计算点与其邻域内 $k$ 个点的平均距离，若超过标准差阈值则判定为噪声点。
     */
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

    /** @brief 读取标准 PLY 文件：支持 ASCII 格式解析 */
    bool loadPLY(const std::string &filename);

    /** @brief 手动清理无效点（如坐标为 NaN 或异常远的点） */
    void deleteErrorPoints();

    void updateSize(double size) { this->size = size; }
};

#endif // POINTCLOUDDATA_H