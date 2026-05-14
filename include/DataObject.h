#ifndef DATAOBJECT_H
#define DATAOBJECT_H

#include <ctime>
#include <map>
#include <string>

/**
 * @class DataObject
 * @brief 遥感数据抽象基类。
 * 定义了所有地理空间数据（栅格、矢量、点云）的通用属性和行为接口。
 *
 */
class DataObject {
  protected:
    std::string id;    // 唯一标识符（UUID或文件名哈希）
    std::string name;  // 数据友好名称
    std::string path;  // 磁盘存储路径
    double size;       // 数据占用的存储空间 (MB)
    time_t createTime; // 对象创建的时间戳
    std::map<std::string, std::string>
        metadata;            // 元数据字典：存储如“传感器类型”、“空间分辨率”等键值对
    mutable int accessCount; // 访问计数：标记该对象被读取的次数（mutable 允许在 const 函数中修改）

  public:
    // =================================================================
    // 1. 构造与析构
    // =================================================================

    /** @brief 构造函数：初始化核心属性，并累加全局对象计数 */
    DataObject(const std::string &id, const std::string &name, const std::string &path);

    /** @brief 虚析构函数：确保派生类对象通过基类指针释放时能正确调用子类析构函数 */
    virtual ~DataObject();

    /** @brief 拷贝构造函数（深拷贝）：确保元数据字典等容器正确复制 */
    DataObject(const DataObject &other);

    /** @brief 赋值运算符重载：处理自赋值并同步内部状态 */
    DataObject &operator=(const DataObject &other);

    // =================================================================
    // 2. 核心接口（纯虚函数，子类必须实现）
    // =================================================================

    /** @brief 多态显示：在 UI 界面或控制台打印数据的详细描述信息 */
    virtual void display() const = 0;

    /** @brief 克隆模式：返回一个该对象的深拷贝副本（用于 DataManager 的某些操作） */
    virtual DataObject *clone() const = 0;

    /** @brief 导出接口：将内存数据持久化为指定格式（如 .tif, .csv, .json） */
    virtual bool exportData(const std::string &format) const = 0;

    /** @brief 指定路径导出：将数据保存到特定文件夹 */
    virtual bool exportDataInPath(const std::string &format, const std::string &path) const = 0;

    // =================================================================
    // 3. 虚函数（可被子类重写以扩展行为）
    // =================================================================

    virtual std::string getType() const { return "DataObject"; }
    virtual double getQualityScore() const { return 100.0; } // 默认质量得分
    virtual bool isValid() const { return true; }            // 校验数据完整性

    // =================================================================
    // 4. 运算符重载（用于排序与比对）
    // =================================================================

    /** @brief 基于 ID 的唯一性比对 */
    bool operator==(const DataObject &other) const { return id == other.id; }
    bool operator!=(const DataObject &other) const { return id != other.id; }

    /** @brief 基于大小的排序：支持 DataManager 使用 std::sort 进行排序 */
    bool operator<(const DataObject &other) const { return size < other.size; }

    /** @brief 类型转换：方便直接输出对象信息 (std::cout << static_cast<std::string>(dataObj);) */
    operator std::string() const {
        // 注意：ctime 返回的字符串末尾通常自带换行符
        char *dt = std::ctime(&createTime);
        return "Type: " + getType() + "\n" + "Name: " + name + " (" + id + ")" + "\n" +
               "Path: " + path + "\n" + "Size: " + std::to_string(size) + " MB\n" +
               "Create Time: " + (dt ? dt : "Unknown");
    }

    /** @brief 隐式转换为 double，返回数据大小 */
    operator double() const { return size; }

    // =================================================================
    // 5. 获取器（Getter）
    // =================================================================

    std::string getId() const {
        accessCount++;
        return id;
    }
    std::string getName() const {
        accessCount++;
        return name;
    }
    std::string getPath() const { return path; }
    double getSize() const { return size; }
    time_t getCreateTime() const { return createTime; }
    int getAccessCount() const { return accessCount; }

    // =================================================================
    // 6. 元数据管理
    // =================================================================

    void addMetadata(const std::string &key, const std::string &value);
    std::string getMetadata(const std::string &key) const;
    bool hasMetadata(const std::string &key) const;
    void removeMetadata(const std::string &key);

    // =================================================================
    // 7. 静态成员（用于全局管理）
    // =================================================================

    static int getTotalObjects();    // 获取当前内存中存在的总对象数
    static void resetTotalObjects(); // 重置计数器

  protected:
    void updateSize(double newSize); // 仅允许子类修改数据大小
    void setPath(const std::string &newPath);

  private:
    static int totalObjects; // 静态变量，用于对象生命周期追踪
};

#endif // DATAOBJECT_H