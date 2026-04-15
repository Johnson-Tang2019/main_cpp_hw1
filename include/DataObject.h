#ifndef DATAOBJECT_H
#define DATAOBJECT_H

#include <map>
#include <string>
#include <ctime>

class DataObject {
protected:
    std::string id;                          // 唯一标识符
    std::string name;                        // 数据名称
    std::string path;                        // 存储路径
    double size;                        // 数据大小(MB)
    time_t createTime;                  // 创建时间
    std::map<std::string, std::string> metadata;       // 元数据字典
    mutable int accessCount;            // 访问计数
    
public:
    // 构造函数与析构函数
    DataObject(const std::string& id, const std::string& name, const std::string& path);
    virtual ~DataObject();
    
    // 拷贝构造函数（深拷贝）
    DataObject(const DataObject& other);
    
    // 赋值运算符
    DataObject& operator=(const DataObject& other);
    
    // 纯虚函数（使DataObject成为抽象类）
    virtual void display() const = 0;
    virtual DataObject* clone() const = 0;
    virtual bool exportData(const std::string& format) const = 0;
    
    // 虚函数（可被子类重写）
    virtual std::string getType() const { return "DataObject"; }
    virtual double getQualityScore() const { return 100.0; }
    virtual bool isValid() const { return true; }
    
    // 运算符重载
    bool operator==(const DataObject& other) const { return id == other.id; }
    bool operator!=(const DataObject& other) const { return id != other.id; }
    bool operator<(const DataObject& other) const { return size < other.size; }
    
    // 类型转换运算符
    operator std::string() const { return name + " (" + id + ")"; }
    operator double() const { return size; }
    
    // 获取器（const成员函数）
    std::string getId() const { accessCount++; return id; }
    std::string getName() const { accessCount++; return name; }
    std::string getPath() const { return path; }
    double getSize() const { return size; }
    time_t getCreateTime() const { return createTime; }
    int getAccessCount() const { return accessCount; }
    
    // 元数据操作
    void addMetadata(const std::string& key, const std::string& value);
    std::string getMetadata(const std::string& key) const;
    bool hasMetadata(const std::string& key) const;
    void removeMetadata(const std::string& key);
    
    // 静态成员（所有对象共享）
    static int getTotalObjects();
    static void resetTotalObjects();
    
protected:
    void updateSize(double newSize);
    void setPath(const std::string& newPath);
    
private:
    static int totalObjects;  // 统计创建的对象总数
};


#endif  // DATAOBJECT_H