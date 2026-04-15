template<typename T>
class DataManager {
private:
    vector<T> dataList;                     // 数据列表
    map<string, vector<T>> categoryMap;     // 按类别分组
    set<T> uniqueData;                      // 唯一数据集合
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
    void removeData(const string& id);
    T findData(const string& id) const;
    bool contains(const string& id) const;
    void clear();
    
    // 批量操作
    void addDataList(const vector<T>& dataList);
    vector<T> getDataByType(const string& type) const;
    
    // 排序
    vector<T> sortBySize(bool ascending = true) const;
    vector<T> sortByName() const;
    vector<T> sortByTime() const;
    
    // 过滤（函数式编程）
    vector<T> filter(function<bool(const T&)> condition) const;
    
    // 统计
    double getTotalSize() const;
    int getCount() const;
    map<string, int> getTypeStatistics() const;
    void printStatistics() const;
    
    // 运算符重载
    T& operator[](int index);
    const T& operator[](int index) const;
    DataManager<T> operator+(const DataManager<T>& other) const;
    DataManager<T>& operator+=(const DataManager<T>& other);
    
    // 迭代器支持
    typename vector<T>::iterator begin() { return dataList.begin(); }
    typename vector<T>::iterator end() { return dataList.end(); }
    typename vector<T>::const_iterator begin() const { return dataList.begin(); }
    typename vector<T>::const_iterator end() const { return dataList.end(); }
    
    // 模板方法：通用处理
    template<typename Func>
    void forEach(Func func) const;
    
    template<typename ResultType>
    vector<ResultType> transform(function<ResultType(const T&)> func) const;
    
    // 静态方法
    static DataManager<T> merge(const DataManager<T>& a, const DataManager<T>& b);
};