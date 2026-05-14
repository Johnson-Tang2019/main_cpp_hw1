#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include <algorithm>
#include <functional>
#include <iostream>
#include <map>
#include <numeric>
#include <set>
#include <string>
#include <vector>

/**
 * @class DataManager
 * @brief 通用数据管理模板类，负责 DataObject 及其派生类的存储、索引与检索。
 * @tparam T 必须是 DataObject 的派生类，且需重载 < 运算符以支持 std::set。
 */
template <typename T> class DataManager {
  private:
    std::vector<T> dataList;                           // 顺序存储列表，用于索引访问
    std::map<std::string, std::vector<T>> categoryMap; // 分类索引，提升按类型查询的效率
    std::set<T> uniqueData;                            // 唯一数据集合，利用 < 运算符自动去重
    mutable int operationCount; // 操作计数器（使用 mutable 允许在 const 函数中修改）

    /** @brief 当数据变动时，同步更新类别映射表 */
    void updateCategory(const T &data) {
        categoryMap[data.getType()].push_back(data);
    }

    /** @brief 全局重新构建索引（通常在批量导入或清除后调用） */
    void rebuildIndex() {
        categoryMap.clear();
        uniqueData.clear();
        for (const auto &item : dataList) {
            updateCategory(item);
            uniqueData.insert(item);
        }
    }

  public:
    // 构造与析构
    DataManager() : operationCount(0) {}
    ~DataManager() = default;

    // 禁用拷贝构造与赋值，防止大数据量下的隐式拷贝消耗性能
    DataManager(const DataManager &) = delete;
    DataManager &operator=(const DataManager &) = delete;

    // =================================================================
    // 基本操作
    // =================================================================

    /** @brief 添加单个数据 */
    void addData(const T &data) {
        dataList.push_back(data);
        updateCategory(data);
        uniqueData.insert(data);
        operationCount++;
    }

    /** @brief 根据 ID 移除数据 */
    void removeData(const std::string &id) {
        auto it = std::remove_if(dataList.begin(), dataList.end(),
                                 [&id](const T &item) { return item.getId() == id; });
        if (it != dataList.end()) {
            dataList.erase(it, dataList.end());
            rebuildIndex(); // 移除后需重建索引以保持一致性
        }
    }

    /** @brief 清空所有数据 */
    void clear() {
        dataList.clear();
        categoryMap.clear();
        uniqueData.clear();
        operationCount = 0;
    }

    // =================================================================
    // 排序与过滤 (函数式编程支持)
    // =================================================================

    /** @brief 按数据大小排序（如点云点数或影像分辨率） */
    std::vector<T> sortBySize(bool ascending = true) const {
        std::vector<T> sorted = dataList;
        std::sort(sorted.begin(), sorted.end(), [ascending](const T &a, const T &b) {
            return ascending ? (a.getSize() < b.getSize()) : (a.getSize() > b.getSize());
        });
        return sorted;
    }

    /** @brief 通用过滤器：支持传入 Lambda 表达式进行自定义筛选 */
    std::vector<T> filter(std::function<bool(const T &)> condition) const {
        std::vector<T> result;
        std::copy_if(dataList.begin(), dataList.end(), std::back_inserter(result), condition);
        return result;
    }

    // =================================================================
    // 统计功能
    // =================================================================

    /** @brief 获取所有数据的总大小（例如总点数或总磁盘占用） */
    double getTotalSize() const {
        return std::accumulate(dataList.begin(), dataList.end(), 0.0,
                               [](double sum, const T &item) { return sum + item.getSize(); });
    }

    /** @brief 打印详细的数据分布统计 */
    void printStatistics() const {
        std::cout << "--- Data Inventory Statistics ---" << std::endl;
        std::cout << "Total Count: " << dataList.size() << std::endl;
        for (auto const &[type, list] : categoryMap) {
            std::cout << "Type [" << type << "]: " << list.size() << " items." << std::endl;
        }
    }

    // =================================================================
    // 运算符重载与迭代器
    // =================================================================

    /** @brief 下标运算符：直接访问数据列表 */
    T &operator[](int index) { return dataList[index]; }
    const T &operator[](int index) const { return dataList[index]; }

    // 迭代器支持，使得 DataManager 可以用于 range-based for 循环
    typename std::vector<T>::iterator begin() { return dataList.begin(); }
    typename std::vector<T>::iterator end() { return dataList.end(); }

    // =================================================================
    // 模板方法
    // =================================================================

    /** @brief 对每个元素执行操作（如批量修改元数据） */
    template <typename Func> void forEach(Func func) {
        std::for_each(dataList.begin(), dataList.end(), func);
        operationCount++;
    }

    /** @brief 变换：将数据列表转换为另一种类型的列表（如从对象转为 ID 字符串列表） */
    template <typename ResultType>
    std::vector<ResultType> transform(std::function<ResultType(const T &)> func) const {
        std::vector<ResultType> result;
        std::transform(dataList.begin(), dataList.end(), std::back_inserter(result), func);
        return result;
    }
};

#endif // DATA_MANAGER_H