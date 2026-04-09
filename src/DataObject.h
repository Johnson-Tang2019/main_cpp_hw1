class DataObject {
protected:
    string id;                          // 唯一标识符
    string name;                        // 数据名称
    string path;                        // 存储路径
    double size;                        // 数据大小(MB)
    time_t createTime;                  // 创建时间
    map<string, string> metadata;       // 元数据字典
    mutable int accessCount;            // 访问计数
    
public:
    // 构造函数与析构函数
    DataObject(const string& id, const string& name, const string& path);
    virtual ~DataObject();
    
    // 拷贝构造函数（深拷贝）
    DataObject(const DataObject& other);
    
    // 赋值运算符
    DataObject& operator=(const DataObject& other);
    
    // 纯虚函数（使DataObject成为抽象类）
    virtual void display() const = 0;
    virtual DataObject* clone() const = 0;
    virtual bool exportData(const string& format) const = 0;
    
    // 虚函数（可被子类重写）
    virtual string getType() const { return "DataObject"; }
    virtual double getQualityScore() const { return 100.0; }
    virtual bool isValid() const { return true; }
    
    // 运算符重载
    bool operator==(const DataObject& other) const { return id == other.id; }
    bool operator!=(const DataObject& other) const { return id != other.id; }
    bool operator<(const DataObject& other) const { return size < other.size; }
    
    // 类型转换运算符
    operator string() const { return name + " (" + id + ")"; }
    operator double() const { return size; }
    
    // 获取器（const成员函数）
    string getId() const { accessCount++; return id; }
    string getName() const { accessCount++; return name; }
    string getPath() const { return path; }
    double getSize() const { return size; }
    time_t getCreateTime() const { return createTime; }
    int getAccessCount() const { return accessCount; }
    
    // 元数据操作
    void addMetadata(const string& key, const string& value);
    string getMetadata(const string& key) const;
    bool hasMetadata(const string& key) const;
    void removeMetadata(const string& key);
    
    // 静态成员（所有对象共享）
    static int getTotalObjects();
    static void resetTotalObjects();
    
protected:
    void updateSize(double newSize);
    void setPath(const string& newPath);
    
private:
    static int totalObjects;  // 统计创建的对象总数
};