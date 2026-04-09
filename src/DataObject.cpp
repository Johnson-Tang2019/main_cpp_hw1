#include "DataObject.h"

int DataObject::totalObjects = 0;

// 构造函数与析构函数
DataObject::DataObject(const std::string& id, const std::string& name, const std::string& path)
    : id(id), name(name), path(path), size(0.0), createTime(time(nullptr)), accessCount(0) {
        DataObject::totalObjects++;
    }

DataObject::~DataObject() {
    DataObject::totalObjects--;
}


// 拷贝构造函数（深拷贝）
DataObject::DataObject(const DataObject& other) : id(other.id), name(other.name), path(other.path), size(other.size), 
    createTime(other.createTime), metadata(other.metadata), accessCount(0) {
        DataObject::totalObjects++;
    }


// 赋值运算符
DataObject& DataObject::operator=(const DataObject& other) {
    if (this != &other) {
        name = other.name;
        path = other.path;
        size = other.size;
        metadata = other.metadata;
        // id和createTime不变，保持对象唯一性和创建时间不变 
    }
    return *this;
}

// 元数据操作
void DataObject::addMetadata(const std::string& key, const std::string& value) {
    metadata[key] = value;
}

// 获取元数据
std::string DataObject::getMetadata(const std::string& key) const {
    accessCount++;  
    auto it = metadata.find(key);
    if (it != metadata.end()) {
        return it->second;
    }
    return "";
}   

// 检查是否存在元数据
bool DataObject::hasMetadata(const std::string& key) const {
    accessCount++;  
    return metadata.find(key) != metadata.end();
}

// 移除元数据
void DataObject::removeMetadata(const std::string& key) {
    metadata.erase(key);
}

// 静态成员函数
int DataObject::getTotalObjects() {
    return totalObjects;
}   
void DataObject::resetTotalObjects() {
    totalObjects = 0;
}

// 受保护的成员函数
void DataObject::updateSize(double newSize) {
    size = newSize;
}
void DataObject::setPath(const std::string& newPath) {
    path = newPath;
}