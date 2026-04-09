namespace RemoteSensingAlgorithms {
    // 植被指数
    double calculateNDVI(double red, double nir);
    double calculateNDWI(double green, double nir);
    double calculateSAVI(double red, double nir, double L = 0.5);
    double calculateEVI(double blue, double red, double nir);
    
    // 水体指数
    double calculateMNDWI(double green, double swir);
    double calculateAWEI(double blue, double green, double nir, double swir1, double swir2);
    
    // 分类算法
    template<typename T>
    vector<int> kMeansClustering(const vector<T>& data, int k, int maxIter = 100);
    
    // 图像处理
    template<typename T>
    vector<vector<T>> convolve(const vector<vector<T>>& image, 
                                const vector<vector<double>>& kernel);
    
    // 统计分析
    template<typename T>
    double calculateEntropy(const vector<T>& data);
    
    template<typename T>
    vector<double> calculateHistogram(const vector<T>& data, int bins);
}