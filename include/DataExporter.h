#ifndef DATA_EXPORTER_H
#define DATA_EXPORTER_H

#include "DataObject.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

/**
 * @class DataExporter
 * @brief 数据导出管理类，负责将 DataObject 及其派生类数据导出为不同格式文件。
 * * 该类被设计为 DataObject 的友元，以便直接访问其受保护的元数据（如 path, metadata 等）。
 */
class DataExporter {
  private:
    std::string exportPath; // 默认导出根目录
    int exportCount;        // 累计导出任务计数

  public:
    /**
     * @brief 构造函数
     * @param path 指定导出的基础路径（例如 "C:/Users/js/Documents/作业/c++/Main_HW/export/"）
     */
    DataExporter(const std::string &path) : exportPath(path), exportCount(0) {}

    ~DataExporter() = default;

    /**
     * @brief 友元函数声明：导出为 CSV 格式
     * 作为 DataObject 的友元，它可以访问 DataObject 的抽象成员进行序列化。
     */
    template <typename T>
    friend bool exportToCSV(const DataObject &data, const std::string &filename);

    /**
     * @brief 友元函数声明：导出为 JSON 格式
     */
    template <typename T>
    friend bool exportToJSON(const DataObject &data, const std::string &filename);

    /**
     * @brief 批量导出接口
     * @tparam T 数据类型（应为 DataObject 的子类，如 SatelliteImage 或 PointCloudData）
     * @param dataList 待导出的数据对象列表
     * @param format 导出格式（"CSV" 或 "JSON"）
     * @return 成功导出的数量
     */
    template <typename T>
    int batchExport(const std::vector<T> &dataList, const std::string &format) {
        int success = 0;
        for (const auto &item : dataList) {
            std::string fileName = exportPath + "/" + item.getName() + "." + format;
            bool result = false;
            if (format == "CSV")
                result = exportToCSV<T>(item, fileName);
            else if (format == "JSON")
                result = exportToJSON<T>(item, fileName);

            if (result) {
                success++;
                exportCount++;
            }
        }
        return success;
    }

    /** @brief 在控制台打印导出统计信息 */
    void printExportStats() const {
        std::cout << "--- Export Statistics ---" << std::endl;
        std::cout << "Target Path: " << exportPath << std::endl;
        std::cout << "Total Files Exported: " << exportCount << std::endl;
    }
};

// =================================================================
// 全局友元函数模板实现（通常放在头文件或对应的 .tpp 文件中）
// =================================================================

/**
 * @brief 将 DataObject 导出为 CSV
 * 注意：由于 DataObject 是抽象基类，具体实现时需要调用 data.exportData() 获取数据流。
 */
template <typename T> bool exportToCSV(const DataObject &data, const std::string &filename) {
    std::ofstream file(filename);
    if (!file.is_open())
        return false;

    // 写入 CSV 表头
    file << "ID,Name,Type,CreateTime\n";
    // 利用友元权限访问基类属性或调用虚函数
    file << data.getId() << "," << data.getName() << "," << typeid(data).name() << "\n";

    file.close();
    return true;
}

/**
 * @brief 将 DataObject 导出为 JSON
 */
template <typename T> bool exportToJSON(const DataObject &data, const std::string &filename) {
    std::ofstream file(filename);
    if (!file.is_open())
        return false;

    file << "{\n  \"metadata\": {\n";
    file << "    \"name\": \"" << data.getName() << "\",\n";
    file << "    \"id\": " << data.getId() << "\n";
    file << "  }\n}";

    file.close();
    return true;
}

#endif // DATA_EXPORTER_H