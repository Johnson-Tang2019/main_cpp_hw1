class DataExporter {
private:
    string exportPath;
    int exportCount;
    
public:
    DataExporter(const string& path);
    ~DataExporter();
    
    // 友元函数声明（将作为DataObject的友元）
    template<typename T>
    friend bool exportToCSV(const DataObject& data, const string& filename);
    
    template<typename T>
    friend bool exportToJSON(const DataObject& data, const string& filename);
    
    // 批量导出
    template<typename T>
    int batchExport(const vector<T>& dataList, const string& format);
    
    // 统计
    void printExportStats() const;
};

// 全局友元函数
template<typename T>
bool exportToCSV(const DataObject& data, const string& filename);

template<typename T>
bool exportToJSON(const DataObject& data, const string& filename);