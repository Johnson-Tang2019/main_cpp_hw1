#include "MainWindow.h"

/**
 * @brief 构造函数：初始化 UI 并设置窗口基本属性
 */
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setWindowTitle("RS App");
}

// =================================================================
// 1. 遥感影像业务逻辑 (Image Logic)
// =================================================================

/** @brief 响应导入按钮：处理多通道遥感图并转为科研常用的 64F 格式 */
void MainWindow::on_btnImport_clicked() {
    QString fileName =
        QFileDialog::getOpenFileName(this, "打开遥感影像", "", "Images (*.png *.jpg *.tif)");
    if (fileName.isEmpty())
        return;
    spdlog::debug("开始读取图像: {}", fileName.toStdString());

    // 使用 IMREAD_UNCHANGED 保持原始通道数和深度（如 16位或多通道）
    cv::Mat image = cv::imread(fileName.toStdString(), cv::IMREAD_UNCHANGED);
    if (image.empty()) {
        QMessageBox::critical(this, "错误", "无法读取图像文件！");
        return;
    }

    // --- 通道补齐逻辑 ---
    // 为了支持后续高阶光谱指数计算，本项目统一使用 5 通道格式
    cv::Mat image5C;
    std::vector<cv::Mat> channels;
    cv::split(image, channels);
    spdlog::debug("原始图像通道数: {}", channels.size());

    // 动态匹配深度补齐通道
    // 确保补充的通道与原图深度一致（防止 8位和 16位混合报错）
    int originalType = image.depth();
    while (channels.size() < 5) {
        cv::Mat extraChannel = cv::Mat::zeros(image.rows, image.cols, originalType);
        channels.push_back(extraChannel);
    }

    // 如果通道多于 5 个（某些遥感图），裁剪它
    if (channels.size() > 5) {
        channels.resize(5);
    }
    cv::merge(channels, image5C);

    // 统一转为 64F (Double)，方便后续科研计算
    image5C.convertTo(image5C, CV_64F);

    // 创建并存储对象
    SatelliteImage satImage(std::to_string(satelliteImages.size()), "1", fileName.toStdString(),
                            image5C);
    spdlog::debug("成功创建卫星图像，最终通道数: {}", satImage.getMat().channels());
    satelliteImages.push_back(satImage);

    // 计算文件大小（MB）
    QFileInfo fileInfo(fileName);
    qint64 sizeInBytes = fileInfo.size(); // 获得字节数
    double sizeInMB = static_cast<double>(sizeInBytes) / (1024 * 1024);
    satelliteImages.back().updateSize(sizeInMB);

    // UI 更新
    updateImageComboBox();
    ui->cbImage->setCurrentIndex(satelliteImages.size() - 1);
    spdlog::debug("当前选中图像索引: {}", satelliteImages.size() - 1);
    updateImageInfo();
}

/** @brief 导入函数：导入遥感影像到程序中 */
void MainWindow::on_btnExportImage_clicked() {
    cheakNullImageError();
    QString fileName = QFileDialog::getSaveFileName(this, "保存遥感影像", "", "Images (*.jpg)");
    if (!fileName.isEmpty()) {
        // 使用 OpenCV 保存图像 (支持 .tif 等遥感格式)
        spdlog::debug("开始保存图像: {}", fileName.toStdString());
        cv::imwrite(fileName.toStdString(), satelliteImages[selectedSatelliteIndex].get8UC3Mat());
        spdlog::debug("图像保存完成");
    }
}

/** @brief 渲染函数：将 OpenCV Mat 映射到 Qt QLabel 上 */
void MainWindow::displayImage() {
    spdlog::debug("显示图像:{}", selectedSatelliteIndex);
    cheakNullImageError();
    cv::Mat mat = satelliteImages[selectedSatelliteIndex].get8UC3Mat();
    if (mat.empty())
        return;

    // 1. 确保是 8位 3通道
    cv::Mat rgb;
    cv::cvtColor(mat, rgb, cv::COLOR_BGR2RGB);

    // 2. 构造 QImage
    // 使用 mat.step 来确保内存对齐安全
    QImage qimg(rgb.data, rgb.cols, rgb.rows, static_cast<int>(rgb.step), QImage::Format_RGB888);

    // 3. 重要：强制深拷贝！
    // 因为 rgb 是局部变量，如果不 .copy()，qimg 在函数结束时会指向非法内存
    QPixmap pixmap = QPixmap::fromImage(qimg.copy());

    // 4. 自适应缩放显示到 QLabel 上
    ui->labelImage->setPixmap(
        pixmap.scaled(ui->labelImage->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    spdlog::debug("图像已成功渲染至界面");
}

/** @brief 应用高斯模糊 */
void MainWindow::on_btnGaussianBlur_clicked() {
    cheakNullImageError();

    spdlog::debug("高斯模糊");
    double simga = ui->sbSigma->value();
    if (simga <= 0) {
        QMessageBox::warning(this, "警告", "高斯模糊半径必须大于 0");
        return;
    }
    if (!satelliteImages.empty()) {
        satelliteImages[selectedSatelliteIndex].applyGaussianBlur(simga);
        displayImage();
    }
}

/** @brief 应用中值滤波 */
void MainWindow::on_btnMedianFilter_clicked() {
    cheakNullImageError();
    spdlog::debug("中值滤波");
    int kernelSize = ui->sbKernelSize->value();
    if (kernelSize <= 0) {
        QMessageBox::warning(this, "警告", "内核大小必须大于 0");
        return;
    }
    if (!satelliteImages.empty()) {
        satelliteImages[selectedSatelliteIndex].applyMedianFilter(kernelSize);
        displayImage();
    }
}

// =================================================================
// 2. 点云业务逻辑 (Point Cloud Logic)
// =================================================================

/** @brief 导入点云：支持 PLY 格式 */
void MainWindow::on_btnImportPointCloud_clicked() {
    spdlog::debug("导入点云");
    QString fileName = QFileDialog::getOpenFileName(this, "打开点云文件", "", "PLY (*.ply)");
    if (!fileName.isEmpty()) {
        PointCloudData pointCloud(std::to_string(pointClouds.size()), fileName.toStdString(),
                                  fileName.toStdString());
        pointCloud.loadPLY(fileName.toStdString());
        pointClouds.push_back(pointCloud);
        // UI 更新
        updatePointCloudComboBox();
        ui->cbPointCloud->setCurrentIndex(pointClouds.size() - 1);
        spdlog::debug("当前选中点云索引: {}", pointClouds.size() - 1);
        QFileInfo fileInfo(fileName);
        qint64 sizeInBytes = fileInfo.size(); // 获得字节数
        double sizeInMB = static_cast<double>(sizeInBytes) / (1024 * 1024);
        pointClouds[selectedPointCloudIndex].updateSize(sizeInMB);
        updatePointCloudInfo();
    }
}

/** @brief 点云下采样（体素滤波） */
void MainWindow::on_btnVoxelFilter_clicked() {
    cheakNullPointCloudError();
    spdlog::debug("体素滤波器");
    double voxelSize = ui->sbVoxelSize->value();
    if (voxelSize <= 0) {
        spdlog::error("体素大小必须大于 0");
        return;
    }
    if (!pointClouds.empty()) {
        spdlog::debug("开始体素滤波");
        pointClouds[0] = pointClouds[0].voxelFilter(voxelSize);
        pointClouds[0].display();
        spdlog::debug("体素滤波完成");
    }
}

/** @brief 导出点云：支持 PLY 格式 */
void MainWindow::on_btnExportPLY_clicked() {
    cheakNullPointCloudError();
    spdlog::debug("导出PLY");
    if (!pointClouds.empty()) {
        QString fileName = QFileDialog::getSaveFileName(this, "保存点云文件", "", "PLY (*.ply)");
        if (!fileName.isEmpty()) {
            if (pointClouds[selectedPointCloudIndex].exportDataInPath("PLY",
                                                                      fileName.toStdString()))
                spdlog::debug("导出PLY成功");
            else
                spdlog::error("导出PLY失败");
        }
    }
}

// =================================================================
// 3. UI 辅助函数 (UI Helpers)
// =================================================================

/** @brief 更新影像下拉菜单，保持 ID 同步 */
void MainWindow::updateImageComboBox() {
    ui->cbImage->clear();
    for (const auto &satImage : satelliteImages) {
        ui->cbImage->addItem(satImage.getId().c_str());
    }
}

/** @brief 更新点云下拉菜单，保持 ID 同步 */
void MainWindow::updatePointCloudComboBox() {
    ui->cbPointCloud->clear();
    for (const auto &pointCloud : pointClouds) {
        ui->cbPointCloud->addItem(pointCloud.getId().c_str());
    }
}

/** @brief 处理影像下拉菜单索引变化 */
void MainWindow::on_cbImage_currentIndexChanged(int index) {
    selectedSatelliteIndex = index;
    if (selectedSatelliteIndex != -1) {
        displayImage();
    }
}

/** @brief 处理点云下拉菜单索引变化 */
void MainWindow::on_cbPointCloud_currentIndexChanged(int index) {
    if (index >= 0 && index < pointClouds.size()) {
        selectedPointCloudIndex = index;
    }
}

/** @brief 检查影像是否已导入 */
void MainWindow::cheakNullImageError() {
    if (selectedSatelliteIndex == -1) {
        QMessageBox::warning(this, "错误", "请先导入图像");
    }
}

/** @brief 检查点云是否已导入 */
void MainWindow::cheakNullPointCloudError() {
    if (selectedPointCloudIndex == -1) {
        QMessageBox::warning(this, "错误", "请先导入点云");
    }
}

/** @brief 更新影像信息 */
void MainWindow::updateImageInfo() {
    if (selectedSatelliteIndex == -1) {
        return;
    }
    std::string name = std::string(satelliteImages[selectedSatelliteIndex]) + "\n";
    ui->tbImage->setText(QString(name.c_str()));
}

/** @brief 更新点云信息 */
void MainWindow::updatePointCloudInfo() {
    if (selectedPointCloudIndex == -1) {
        return;
    }
    std::string name = std::string(pointClouds[selectedPointCloudIndex]);
    ui->tbPointCloud->setText(QString(name.c_str()));
}

/** @brief 析构函数 */
MainWindow::~MainWindow() { delete ui; }