#ifndef DATA_EXPORTER_H
#define DATA_EXPORTER_H
#include <string>
#include <vector>
#include "DataObject.h"

class DataExporter {
private:
    std::string exportPath;
    int exportCount;
    
public:
    DataExporter(const std::string& path);
    ~DataExporter();
    
    // 友元函数声明（将作为DataObject的友元）
    template<typename T>
    friend bool exportToCSV(const DataObject& data, const std::string& filename);
    
    template<typename T>
    friend bool exportToJSON(const DataObject& data, const std::string& filename);
    
    // 批量导出
    template<typename T>
    int batchExport(const std::vector<T>& dataList, const std::string& format);
    
    // 统计
    void printExportStats() const;
};

// 全局友元函数
template<typename T>
bool exportToCSV(const DataObject& data, const std::string& filename);

template<typename T>
bool exportToJSON(const DataObject& data, const std::string& filename);

#endif // DATA_EXPORTER_H
