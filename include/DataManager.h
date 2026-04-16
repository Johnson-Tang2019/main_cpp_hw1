#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H
#include <vector>
#include <map>
#include <set>

template<typename T>
class DataManager {
private:
    std::vector<T> dataList;                     // 数据列表
    std::map<std::string, std::vector<T>> categoryMap;     // 按类别分组
    std::set<T> uniqueData;                      // 唯一数据集合
    mutable int operationCount;             // 操作计数
    
    void updateCategory(const T& data);
    void rebuildIndex();
    
public:
    // 构造函数
    DataManager();
    ~DataManager();
    
    // 禁止拷贝（单例模式可选）
    DataManager(const DataManager&) = delete;
    DataManager& operator=(const DataManager&) = delete;
    
    // 基本操作
    void addData(const T& data);
    void removeData(const std::string& id);
    T findData(const std::string& id) const;
    bool contains(const std::string& id) const;
    void clear();
    
    // 批量操作
    void addDataList(const std::vector<T>& dataList);
    std::vector<T> getDataByType(const std::string& type) const;
    
    // 排序
    std::vector<T> sortBySize(bool ascending = true) const;
    std::vector<T> sortByName() const;
    std::vector<T> sortByTime() const;
    
    // 过滤（函数式编程）
    std::vector<T> filter(std::function<bool(const T&)> condition) const;
    
    // 统计
    double getTotalSize() const;
    int getCount() const;
    std::map<std::string, int> getTypeStatistics() const;
    void printStatistics() const;
    
    // 运算符重载
    T& operator[](int index);
    const T& operator[](int index) const;
    DataManager<T> operator+(const DataManager<T>& other) const;
    DataManager<T>& operator+=(const DataManager<T>& other);
    
    // 迭代器支持
    typename std::vector<T>::iterator begin() { return dataList.begin(); }
    typename std::vector<T>::iterator end() { return dataList.end(); }
    typename std::vector<T>::const_iterator begin() const { return dataList.begin(); }
    typename std::vector<T>::const_iterator end() const { return dataList.end(); }
    
    // 模板方法：通用处理
    template<typename Func>
    void forEach(Func func) const;
    
    template<typename ResultType>
    std::vector<ResultType> transform(std::function<ResultType(const T&)> func) const;
    
    // 静态方法
    static DataManager<T> merge(const DataManager<T>& a, const DataManager<T>& b);
};

#endif // DATA_MANAGER_H
