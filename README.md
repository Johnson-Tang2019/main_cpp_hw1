# RS App - 遥感影像与点云处理平台

RS App 是一款基于 C++、Qt 和 OpenCV 开发的轻量级遥感数据处理桌面应用。它集成了多通道栅格影像处理、三维点云数据分析以及现代 AI 辅助功能，旨在为用户提供高效的地理空间数据可视化与分析工具。

🚀 核心功能

1. 栅格影像处理 (Raster Data)
   多通道支持：支持卫星遥感影像（Satellite Image）的导入与管理。

影像滤波：

高斯模糊 (Gaussian Blur)：平滑影像，支持自定义 Sigma 参数。

中值滤波 (Median Filter)：有效去除遥感影像中的椒盐噪声。

参数交互：通过动态 SpinBox 实时调整处理参数。

2.  三维点云分析 (Point Cloud)
    数据导入/导出：支持 PLY 等标准格式的点云文件。

    点云下采样：内置 体素滤波 (Voxel Filter)，支持自定义体素栅格大小，有效处理百万级（如 286 万）点云数据。

    实时日志：双文本浏览器设计，分类记录影像与点云的操作状态。

🛠 技术架构
项目采用了严格的面向对象设计与 C++ 模板技术，确保了良好的扩展性与性能。

类继承关系
根据项目设计架构图，核心类层次如下：

DataObject (抽象基类)：定义了 display()、clone()、exportData() 等核心纯虚函数。

数据分支：

RasterData (栅格分支) -> SatelliteImage (卫星影像), DEMData (数字高程)

VectorData (矢量分支)

PointCloudData (点云分支)

管理与工具：

DataManager<T>：模板管理类，负责数据的存储与生命周期管理。

DataExporter：友元模板类，负责多种格式的数据导出。

Pixel<T>：底层像素处理模板。

🖥 界面预览
项目 UI 采用 Qt 响应式布局设计，确保在不同分辨率下均能完美填充窗口。

主显示区：占据界面 80% 空间，支持高分辨率影像展示。

右侧日志区：实时反馈影像处理与点云分析的详细参数及报错信息。

底部工具栏：分类布局，操作逻辑清晰。

🛠 安装与编译
依赖环境
C++ 标准：C++17 或更高版本

框架：Qt 6.x (推荐)

库：OpenCV 4.x (需包含 DNN 模块)

构建工具：CMake

编译步骤
克隆仓库：

Bash
git clone https://github.com/your-repo/RS-App.git
创建编译目录：

Bash
mkdir build && cd build
运行 CMake：

Bash
cmake ..
编译并运行：

Windows (MinGW/MSVC): make 或在 VS 中打开并运行。

📦 分发说明
若需在未安装 Qt 环境的电脑上运行：

切换至 Release 模式编译。

使用 windeployqt MainHomework.exe 抓取 Qt 依赖库。

手动将 opencv_world4xx.dll 以及 models 文件夹（含 AI 模型）放入 exe 同级目录。

✍️ 作者
Tang Jun (汤骏) 项目开发于 2026 年，作为遥感影像处理与 C++ 深度开发的课程作业。

注：本项目仅供学术交流使用。
