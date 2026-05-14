#include "DataObject.h"

// 初始化静态成员变量：在内存中追踪当前存活的所有 DataObject 派生类对象总数
int DataObject::totalObjects = 0;

// =================================================================
// 1. 构造与析构
// =================================================================

/**
 * @brief 构造函数
 * @param id 唯一标识符
 * @param name 数据显示名称
 * @param path 物理存储路径
 */
DataObject::DataObject(const std::string &id, const std::string &name, const std::string &path)
    : id(id), name(name), path(path), size(0.0), createTime(time(nullptr)), accessCount(0) {
    // 每创建一个对象（无论是直接创建子类还是拷贝），全局计数加 1
    DataObject::totalObjects++;
}

/**
 * @brief 析构函数
 * 由于是虚析构函数，当子类对象被销毁时，会先调用子类析构再调用此基类析构
 */
DataObject::~DataObject() {
    // 对象销毁时，全局计数减 1，确保内存监控准确
    DataObject::totalObjects--;
}

// =================================================================
// 2. 拷贝与赋值 (三/五法则)
// =================================================================

/**
 * @brief 拷贝构造函数（深拷贝）
 * 用于 DataManager 存储副本或函数按值传递时
 */
DataObject::DataObject(const DataObject &other)
    : id(other.id), name(other.name), path(other.path), size(other.size),
      createTime(other.createTime), metadata(other.metadata), accessCount(0) {
    // 拷贝产生的新对象也需要计入总数
    DataObject::totalObjects++;
}

/**
 * @brief 赋值运算符重载
 * 注意：通常不建议在赋值时修改 ID 和创建时间，因为它们定义了对象的身份。
 */
DataObject &DataObject::operator=(const DataObject &other) {
    // 1. 防止自赋值 (e.g., obj = obj)，避免冗余操作和潜在错误
    if (this != &other) {
        // 2. 仅同步可变属性
        name = other.name;
        path = other.path;
        size = other.size;
        metadata = other.metadata;
        // 注意：accessCount 通常不从 old 对象拷贝，而是重新开始计数

        // id 和 createTime 保持不变，确保“这个”对象依然是“这个”对象
    }
    return *this;
}

// =================================================================
// 3. 元数据字典操作 (Metadata Operations)
// =================================================================

/** @brief 添加或更新元数据 (如: "sensor", "Landsat-9") */
void DataObject::addMetadata(const std::string &key, const std::string &value) {
    metadata[key] = value;
}

/** @brief 获取元数据：如果键不存在则返回空字符串 */
std::string DataObject::getMetadata(const std::string &key) const {
    accessCount++; // 记录一次读取访问
    auto it = metadata.find(key);
    if (it != metadata.end()) {
        return it->second;
    }
    return "";
}

/** @brief 检查是否存在特定元数据键 */
bool DataObject::hasMetadata(const std::string &key) const {
    accessCount++;
    return metadata.find(key) != metadata.end();
}

/** @brief 移除不需要的元数据项 */
void DataObject::removeMetadata(const std::string &key) { metadata.erase(key); }

// =================================================================
// 4. 静态与受保护成员 (Management & Protected)
// =================================================================

/** @brief 返回当前存活的对象总数（用于内存泄露检查） */
int DataObject::getTotalObjects() { return totalObjects; }

/** @brief 强制重置计数（危险操作，仅建议在单元测试中使用） */
void DataObject::resetTotalObjects() { totalObjects = 0; }

/** @brief 更新数据大小：通常由子类（如 SatelliteImage 滤波后）调用 */
void DataObject::updateSize(double newSize) { size = newSize; }

/** @brief 修改路径：当执行“另存为”或文件移动操作时使用 */
void DataObject::setPath(const std::string &newPath) { path = newPath; }