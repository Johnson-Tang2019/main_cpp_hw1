#include "../include/PointCloudData.h"

Point3D::Point3D(double x, double y, double z, double intensity, int cls)
    : x(x), y(y), z(z), intensity(intensity), classification(cls) {}

Point3D Point3D::operator+(const Point3D& other) const {
    return Point3D(x + other.x, y + other.y, z + other.z, intensity + other.intensity, classification);
}

Point3D Point3D::operator-(const Point3D& other) const {
    return Point3D(x - other.x, y - other.y, z - other.z, intensity - other.intensity, classification);
}

double Point3D::distanceTo(const Point3D& other) const {
    return std::sqrt(std::pow(x - other.x, 2) + 
                     std::pow(y - other.y, 2) + 
                     std::pow(z - other.z, 2));
}

// 构造函数与析构函数
PointCloudData::PointCloudData(const std::string& id, const std::string& name, const std::string& path)
    : DataObject(id, name, path), minX(0), maxX(0), minY(0), maxY(0), minZ(0), maxZ(0) {}

PointCloudData::PointCloudData(const PointCloudData& other) : DataObject(other), points(other.points), 
    minX(other.minX), maxX(other.maxX), minY(other.minY), maxY(other.maxY), minZ(other.minZ), maxZ(other.maxZ) {}

PointCloudData::~PointCloudData() {}

// 实现基类虚函数
void PointCloudData::updateBounds() {
    if (points.empty()) {
        minX = maxX = minY = maxY = minZ = maxZ = 0;
        return;
    }
    minX = maxX = points[0].x;
    minY = maxY = points[0].y;
    minZ = maxZ = points[0].z;
    for (const auto& point : points) {
        if (point.x < minX) minX = point.x;
        if (point.x > maxX) maxX = point.x;
        if (point.y < minY) minY = point.y;
        if (point.y > maxY) maxY = point.y;
        if (point.z < minZ) minZ = point.z;
        if (point.z > maxZ) maxZ = point.z;
    }
}

// 实现基类虚函数
void PointCloudData::display() const {
    std::cout << "PointCloudData: " << name << " (" << id << ")\n";
    std::cout << "Path: " << path << "\n";
    std::cout << "Size: " << size << " MB\n";
    std::cout << "Created: " << ctime(&createTime);
    std::cout << "Points: " << points.size() << "\n";
    std::cout << "Bounds: X[" << minX << ", " << maxX << "] Y[" 
              << minY << ", " << maxY << "] Z[" 
              << minZ << ", " << maxZ << "]\n";
}

DataObject* PointCloudData::clone() const {
    return new PointCloudData(*this);
}

bool PointCloudData::exportData(const std::string& format) const {
    // 这里可以根据format参数实现不同格式的导出逻辑
    std::cout << "Exporting PointCloudData in format: " << format << "\n";
    std::ofstream outFile(name + "." + format);

    // 导出逻辑
    if (format == "PLY") {
        // 导出为PLY格式
    } else if (format == "LAS") {
        // 导出为LAS格式
    } else if (format == "TXT") {
        // 导出为TXT格式
        outFile << "PointCloudData: " << name << " (" << id << ")\n";
        outFile << "Path: " << path << "\n";
        outFile << "Size: " << size << " MB\n";
        outFile << "Created: " << ctime(&createTime);
        outFile << "Bounds: X[" << minX << ", " << maxX << "] Y[" 
              << minY << ", " << maxY << "] Z[" 
              << minZ << ", " << maxZ << "]\n";
        outFile << "Points: " << points.size() << "\n";
        for(auto& point : points) {
            outFile << point.x << " " << point.y << " " << point.z 
                    << " " << point.intensity << " " << point.classification << "\n";
        }

    } else {
        std::cerr << "Unsupported export format: " << format << "\n";
        return false;
    }

    return true;  // 导出成功
}

// 运算符重载
PointCloudData PointCloudData::operator+(const PointCloudData& other) const {
    PointCloudData result(*this);
    result.points.insert(result.points.end(), other.points.begin(), other.points.end());
    result.updateBounds();
    return result;
}

PointCloudData& PointCloudData::operator+=(const PointCloudData& other) {
    points.insert(points.end(), other.points.begin(), other.points.end());
    updateBounds();
    return *this;
}

Point3D& PointCloudData::operator[](int index) {
    return points[index];
}

const Point3D& PointCloudData::operator[](int index) const {
    return points[index];
}


// 点云操作
void PointCloudData::addPoint(const Point3D& point) {
    points.push_back(point);
    updateBounds();
}

void PointCloudData::addPoints(const std::vector<Point3D>& newPoints) {
    points.insert(points.end(), newPoints.begin(), newPoints.end());
    updateBounds();
}

int PointCloudData::getPointCount() const {
    return points.size();
}

// 点云滤波
PointCloudData PointCloudData::voxelFilter(double voxelSize) const {
    // 1. 创建一个新的点云对象用于存放结果
    PointCloudData result(this->id + "_voxel", this->name + "_filtered", this->path);
    
    // 2. 使用 map 将点分配到体素网格中
    // Key: 体素的索引坐标 (ix, iy, iz), Value: 该体素内所有点的总和及计数
    struct VoxelIdx {
        long long x, y, z;
        bool operator<(const VoxelIdx& o) const {
            if (x != o.x) return x < o.x;
            if (y != o.y) return y < o.y;
            return z < o.z;
        }
    };

    struct VoxelSum {
        double x = 0, y = 0, z = 0;
        int count = 0;
    };

    std::map<VoxelIdx, VoxelSum> grid;

    // 3. 遍历所有点，归类到对应的体素
    for (const auto& p : points) {
        VoxelIdx idx = {
            static_cast<long long>(std::floor(p.x / voxelSize)),
            static_cast<long long>(std::floor(p.y / voxelSize)),
            static_cast<long long>(std::floor(p.z / voxelSize))
        };
        grid[idx].x += p.x;
        grid[idx].y += p.y;
        grid[idx].z += p.z;
        grid[idx].count++;
    }

    // 4. 计算每个体素的重心并添加到结果中
    for (auto const& [idx, sum] : grid) {
        result.addPoint(Point3D(sum.x / sum.count, sum.y / sum.count, sum.z / sum.count));
    }

    return result;
}


// 统计离群点
PointCloudData PointCloudData::statisticalOutlierRemoval(int k, double stdDev) const{
    int n = points.size();
    if (n < k) return *this;

    std::vector<double> avgDistances(n);

    // 1. 计算每个点到最近 k 个邻居的平均距离
    /****************  需 Kd-Tree 优化  *******************/
    for (int i = 0; i < n; ++i) {
        std::vector<double> dists;
        for (int j = 0; j < n; ++j) {
            if (i == j) continue;
            dists.push_back(points[i].distanceTo(points[j]));
        }
        std::sort(dists.begin(), dists.end());
        
        double sum = std::accumulate(dists.begin(), dists.begin() + k, 0.0);
        avgDistances[i] = sum / k;
    }

    // 2. 计算所有平均距离的 均值(mu) 和 标准差(sigma)
    double mu = std::accumulate(avgDistances.begin(), avgDistances.end(), 0.0) / n;
    double sq_sum = std::inner_product(avgDistances.begin(), avgDistances.end(), avgDistances.begin(), 0.0);
    double sigma = std::sqrt(sq_sum / n - mu * mu);

    // 3. 过滤出距离在合法范围内的点
    PointCloudData result(this->id + "_sor", this->name + "_cleaned", this->path);
    double threshold = mu + stdDev * sigma;

    for (int i = 0; i < n; ++i) {
        if (avgDistances[i] <= threshold) {
            result.addPoint(points[i]);
        }
    }

    return result;
}


// 统计信息
double PointCloudData::getAverageHeight() const {
    if (points.empty()) return 0.0;
    double sumZ = std::accumulate(points.begin(), points.end(), 0.0, [](double sum, const Point3D& p) {
        return sum + p.z;
    });
    return sumZ / points.size();
}

double PointCloudData::getHeightRange() const {
    return maxZ - minZ;
}

void PointCloudData::printStatistics() const {
    std::cout << "PointCloudData Statistics:\n";
    std::cout << "Total Points: " << points.size() << "\n";
    std::cout << "Average Height: " << getAverageHeight() << "\n";
    std::cout << "Height Range: " << getHeightRange() << "\n";
    std::cout << "Bounds: X[" << minX << ", " << maxX << "] Y[" 
              << minY << ", " << maxY << "] Z[" 
              << minZ << ", " << maxZ << "]\n";
}









