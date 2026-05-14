#include <PointCloudData.h>

Point3D::Point3D(double x, double y, double z, double intensity, int cls)
    : x(x), y(y), z(z), intensity(intensity), classification(cls) {}

Point3D Point3D::operator+(const Point3D &other) const {
    return Point3D(x + other.x, y + other.y, z + other.z, intensity + other.intensity,
                   classification);
}

Point3D Point3D::operator-(const Point3D &other) const {
    return Point3D(x - other.x, y - other.y, z - other.z, intensity - other.intensity,
                   classification);
}

double Point3D::distanceTo(const Point3D &other) const {
    return std::sqrt(std::pow(x - other.x, 2) + std::pow(y - other.y, 2) +
                     std::pow(z - other.z, 2));
}

// 构造函数与析构函数
PointCloudData::PointCloudData(const std::string &id, const std::string &name,
                               const std::string &path)
    : DataObject(id, name, path), minX(0), maxX(0), minY(0), maxY(0), minZ(0), maxZ(0) {}

PointCloudData::PointCloudData(const std::string &id, const std::string &name,
                               const std::string &path, const std::vector<Point3D> &points)
    : DataObject(id, name, path), points(points), minX(0), maxX(0), minY(0), maxY(0), minZ(0),
      maxZ(0) {
    updateBounds();
}

PointCloudData::PointCloudData(const PointCloudData &other)
    : DataObject(other), points(other.points), minX(other.minX), maxX(other.maxX), minY(other.minY),
      maxY(other.maxY), minZ(other.minZ), maxZ(other.maxZ) {}

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
    for (const auto &point : points) {
        if (point.x < minX)
            minX = point.x;
        if (point.x > maxX)
            maxX = point.x;
        if (point.y < minY)
            minY = point.y;
        if (point.y > maxY)
            maxY = point.y;
        if (point.z < minZ)
            minZ = point.z;
        if (point.z > maxZ)
            maxZ = point.z;
    }
}

// 实现基类虚函数
void PointCloudData::display() const {
    std::cout << "PointCloudData: " << name << " (" << id << ")\n";
    std::cout << "Path: " << path << "\n";
    std::cout << "Size: " << size << " MB\n";
    struct tm timeInfo;
    localtime_s(&timeInfo, &createTime);
    std::cout << "Created: " << std::put_time(&timeInfo, "%Y-%m-%d %H:%M:%S");
    std::cout << "Points: " << points.size() << "\n";
    std::cout << "Bounds: X[" << minX << ", " << maxX << "] Y[" << minY << ", " << maxY << "] Z["
              << minZ << ", " << maxZ << "]\n";
}

DataObject *PointCloudData::clone() const { return new PointCloudData(*this); }

bool PointCloudData::exportData(const std::string &format) const {
    // 这里可以根据format参数实现不同格式的导出逻辑
    std::cout << "Exporting PointCloudData in format: " << format << "\n";
    std::ofstream outFile(name + "." + format);

    // 导出逻辑
    if (format == "PLY") {
        // 1. 写入 PLY 头部
        outFile << "ply\n";
        outFile << "format ascii 1.0\n";
        outFile << "element vertex " << points.size() << "\n";
        outFile << "property double x\n";
        outFile << "property double y\n";
        outFile << "property double z\n";
        // 如果你的 Point3D 包含颜色或法线，可以在此处继续添加 property
        outFile << "end_header\n";
        // 2. 写入点云数据
        // 设置精度以防止测量数据丢失
        outFile << std::fixed << std::setprecision(6);
        for (const auto &pt : points) {
            outFile << pt.x << " " << pt.y << " " << pt.z << "\n";
        }
        outFile.close();
        return true;

    } else if (format == "TXT") {
        // 导出为TXT格式
        outFile << "PointCloudData: " << name << " (" << id << ")\n";
        outFile << "Path: " << path << "\n";
        outFile << "Size: " << size << " MB\n";
        struct tm timeInfo;
        localtime_s(&timeInfo, &createTime);
        outFile << "Created: " << std::put_time(&timeInfo, "%Y-%m-%d %H:%M:%S");
        outFile << "Bounds: X[" << minX << ", " << maxX << "] Y[" << minY << ", " << maxY << "] Z["
                << minZ << ", " << maxZ << "]\n";
        outFile << "Points: " << points.size() << "\n";
        for (auto &point : points) {
            outFile << point.x << " " << point.y << " " << point.z << " " << point.intensity << " "
                    << point.classification << "\n";
        }

    } else {
        std::cerr << "Unsupported export format: " << format << "\n";
        return false;
    }

    return true; // 导出成功
}

bool PointCloudData::exportDataInPath(const std::string &format, const std::string &path) const {
    // 这里可以根据format参数实现不同格式的导出逻辑
    std::cout << "Exporting PointCloudData in format: " << format << "\n";
    std::ofstream outFile(path);

    // 导出逻辑
    if (format == "PLY") {
        // 1. 写入 PLY 头部
        outFile << "ply\n";
        outFile << "format ascii 1.0\n";
        outFile << "element vertex " << points.size() << "\n";
        outFile << "property double x\n";
        outFile << "property double y\n";
        outFile << "property double z\n";
        // 如果你的 Point3D 包含颜色或法线，可以在此处继续添加 property
        outFile << "end_header\n";
        // 2. 写入点云数据
        // 设置精度以防止测量数据丢失
        outFile << std::fixed << std::setprecision(6);
        for (const auto &pt : points) {
            outFile << pt.x << " " << pt.y << " " << pt.z << "\n";
        }
        outFile.close();
        return true;

    } else if (format == "TXT") {
        // 导出为TXT格式
        outFile << "PointCloudData: " << name << " (" << id << ")\n";
        outFile << "Path: " << path << "\n";
        outFile << "Size: " << size << " MB\n";
        struct tm timeInfo;
        localtime_s(&timeInfo, &createTime);
        outFile << "Created: " << std::put_time(&timeInfo, "%Y-%m-%d %H:%M:%S");
        outFile << "Bounds: X[" << minX << ", " << maxX << "] Y[" << minY << ", " << maxY << "] Z["
                << minZ << ", " << maxZ << "]\n";
        outFile << "Points: " << points.size() << "\n";
        for (auto &point : points) {
            outFile << point.x << " " << point.y << " " << point.z << " " << point.intensity << " "
                    << point.classification << "\n";
        }

    } else {
        std::cerr << "Unsupported export format: " << format << "\n";
        return false;
    }

    return true; // 导出成功
}

// 运算符重载
PointCloudData PointCloudData::operator+(const PointCloudData &other) const {
    PointCloudData result(*this);
    result.points.insert(result.points.end(), other.points.begin(), other.points.end());
    result.updateBounds();
    return result;
}

PointCloudData &PointCloudData::operator+=(const PointCloudData &other) {
    points.insert(points.end(), other.points.begin(), other.points.end());
    updateBounds();
    return *this;
}

Point3D &PointCloudData::operator[](int index) { return points[index]; }

const Point3D &PointCloudData::operator[](int index) const { return points[index]; }

// 点云操作
void PointCloudData::addPoint(const Point3D &point) {
    points.push_back(point);
    updateBounds();
}

void PointCloudData::addPoints(const std::vector<Point3D> &newPoints) {
    points.insert(points.end(), newPoints.begin(), newPoints.end());
    updateBounds();
}

int PointCloudData::getPointCount() const { return points.size(); }

// 点云滤波
PointCloudData PointCloudData::voxelFilter(double voxelSize) {
    // 1. 创建一个新的点云对象用于存放结果
    PointCloudData result(this->id + "_voxel", this->name + "_filtered", this->path);

    std::unordered_map<VoxelIdx, VoxelSum, VoxelHasher> grid;

    // 预估空间以减少 rehash 消耗
    grid.reserve(points.size() / 2);

    for (const auto &p : points) {
        if (!std::isfinite(p.x) || !std::isfinite(p.y) || !std::isfinite(p.z))
            continue;

        VoxelIdx idx = {static_cast<long long>(std::floor(p.x / voxelSize)),
                        static_cast<long long>(std::floor(p.y / voxelSize)),
                        static_cast<long long>(std::floor(p.z / voxelSize))};

        VoxelSum &vs = grid[idx]; // 引用获取，减少查找次数
        vs.x += p.x;
        vs.y += p.y;
        vs.z += p.z;
        vs.count++;
    }

    result.points.reserve(grid.size()); // 预分配结果空间
    for (auto const &[idx, sum] : grid) {
        result.addPoint(Point3D(sum.x / sum.count, sum.y / sum.count, sum.z / sum.count));
    }

    return result;
}

// 统计离群点
PointCloudData PointCloudData::statisticalOutlierRemoval(int k, double stdDev) {
    int n = points.size();
    if (n < k)
        return *this;

    std::vector<double> avgDistances(n);

    // 1. 计算每个点到最近 k 个邻居的平均距离
    /****************  需 Kd-Tree 优化  *******************/
    for (int i = 0; i < n; ++i) {
        std::vector<double> dists;
        for (int j = 0; j < n; ++j) {
            if (i == j)
                continue;
            dists.push_back(points[i].distanceTo(points[j]));
        }
        std::sort(dists.begin(), dists.end());

        double sum = std::accumulate(dists.begin(), dists.begin() + k, 0.0);
        avgDistances[i] = sum / k;
    }

    // 2. 计算所有平均距离的 均值(mu) 和 标准差(sigma)
    double mu = std::accumulate(avgDistances.begin(), avgDistances.end(), 0.0) / n;
    double sq_sum =
        std::inner_product(avgDistances.begin(), avgDistances.end(), avgDistances.begin(), 0.0);
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
    if (points.empty())
        return 0.0;
    double sumZ = std::accumulate(points.begin(), points.end(), 0.0,
                                  [](double sum, const Point3D &p) { return sum + p.z; });
    return sumZ / points.size();
}

double PointCloudData::getHeightRange() const { return maxZ - minZ; }

void PointCloudData::printStatistics() const {
    std::cout << "PointCloudData Statistics:\n";
    std::cout << "Total Points: " << points.size() << "\n";
    std::cout << "Average Height: " << getAverageHeight() << "\n";
    std::cout << "Height Range: " << getHeightRange() << "\n";
    std::cout << "Bounds: X[" << minX << ", " << maxX << "] Y[" << minY << ", " << maxY << "] Z["
              << minZ << ", " << maxZ << "]\n";
}

bool PointCloudData::loadPLY(const std::string &filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        spdlog::error("无法打开文件: {}", filename);
        return false;
    }

    std::string line;
    int numVertices = 0;

    // 1. 解析头文件 (保持原有逻辑获取 numVertices)
    while (std::getline(file, line)) {
        if (line.find("element vertex") != std::string::npos) {
            numVertices = std::stoi(line.substr(15));
        }
        if (line == "end_header")
            break;
    }

    this->points.clear();
    this->points.reserve(numVertices);

    // 2. 严格按照 float32 x, y, z 读取数据
    for (int i = 0; i < numVertices; ++i) {
        float x, y, z;

        // 必须读 4 字节的 float
        file.read(reinterpret_cast<char *>(&x), sizeof(float));
        file.read(reinterpret_cast<char *>(&y), sizeof(float));
        file.read(reinterpret_cast<char *>(&z), sizeof(float));

        // 检查读取是否成功
        if (file.gcount() < sizeof(float))
            break;

        // 存入你的 Point3D (这里会自动转换成 double)
        // 注意：因为头文件里没写有 intensity 和 class，所以这里只存坐标
        this->points.emplace_back(static_cast<double>(x), static_cast<double>(y),
                                  static_cast<double>(z));
    }

    // 3. 后处理：清理并更新边界
    deleteErrorPoints();
    updateBounds();

    spdlog::debug("成功加载点云: {} 个点", points.size());
    return true;
}

void PointCloudData::deleteErrorPoints() {
    double limit = 1e13; // 1e13 是一个比较大的数值，用于过滤掉异常值
    points.erase(std::remove_if(points.begin(), points.end(),
                                [limit](const Point3D &p) {
                                    if (!std::isfinite(p.x) || !std::isfinite(p.y) ||
                                        !std::isfinite(p.z)) {
                                        return true;
                                    }
                                    // 2. 检查数值是否大得离谱
                                    if (std::abs(p.x) > limit || std::abs(p.y) > limit ||
                                        std::abs(p.z) > limit) {
                                        return true;
                                    }
                                    return false;
                                }),
                 points.end());
}
