#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
}

void MainWindow::on_btnImport_clicked() {
    QString fileName =
        QFileDialog::getOpenFileName(this, "打开遥感影像", "", "Images (*.png *.jpg *.tif)");
    if (!fileName.isEmpty()) {
        // 使用 OpenCV 读取图像 (支持 .tif 等遥感格式)
        spdlog::debug("开始读取图像: {}", fileName.toStdString());
        cv::Mat image = cv::imread(fileName.toStdString());
        if (!image.empty()) {
            // --- 转换 5 通道逻辑开始 ---
            spdlog::debug("转换 5 通道逻辑开始");
            cv::Mat image5C;
            std::vector<cv::Mat> channels;

            spdlog::debug("拆分原始通道 (假设是 BGR)");
            cv::split(image, channels);

            // 2. 补齐通道 (如果原始是单通道，补4个；如果是3通道，补2个)
            spdlog::debug("补齐通道");
            while (channels.size() < 5) {
                // 创建一个大小一致、类型一致的空通道（全黑）
                cv::Mat extraChannel = cv::Mat::zeros(image.rows, image.cols, CV_8UC1);
                channels.push_back(extraChannel);
            }

            // 3. 合并成 5 通道
            spdlog::debug("合并通道");
            cv::merge(channels, image5C);
            image5C.convertTo(image5C, CV_64F);
            // --- 转换 5 通道逻辑结束 ---
            SatelliteImage satImage("1", "1", fileName.toStdString(), image5C);
            spdlog::debug("创建卫星图像, 通道数为{}", satImage.getMat().channels());
            satelliteImages.push_back(satImage);
            displayImage(image5C);
        }
    }
}

void MainWindow::on_btnExportImage_clicked() {
    QString fileName = QFileDialog::getSaveFileName(this, "保存遥感影像", "", "Images (*.jpg)");
    if (!fileName.isEmpty()) {
        // 使用 OpenCV 保存图像 (支持 .tif 等遥感格式)
        spdlog::debug("开始保存图像: {}", fileName.toStdString());
        cv::imwrite(fileName.toStdString(), satelliteImages[0].get8UC3Mat());
        spdlog::debug("图像保存完成");
    }
}

void MainWindow::displayImage(const cv::Mat &mat) {
    spdlog::debug("开始显示图像");
    cv::Mat displayMat;
    QImage qimg;

    // 1. 处理通道逻辑：如果是 5 通道，提取前 3 个通道用于显示

    spdlog::debug("检测到 5 通道影像，正在提取前 3 通道 (BGR) 进行显示");
    displayMat = satelliteImages[0].get8UC3Mat();
    spdlog::debug("显示图像, 通道数为{}", displayMat.channels());

    // 2. 转换为 Qt 格式
    if (displayMat.channels() == 3) {
        cv::Mat rgb;
        cv::cvtColor(displayMat, rgb, cv::COLOR_BGR2RGB);

        // 注意：使用 rgb.step 确保内存对齐正确
        qimg = QImage((const unsigned char *)(rgb.data), rgb.cols, rgb.rows, rgb.step,
                      QImage::Format_RGB888)
                   .copy(); // .copy() 立即深拷贝内存
    } else if (displayMat.channels() == 1) {
        qimg = QImage((const unsigned char *)(displayMat.data), displayMat.cols, displayMat.rows,
                      displayMat.step, QImage::Format_Indexed8)
                   .copy();
    } else {
        spdlog::error("无法显示通道数为 {} 的图像", displayMat.channels());
        return;
    }

    // 3. 更新 UI
    if (!qimg.isNull()) {
        ui->labelImage->setAlignment(Qt::AlignCenter);
        ui->labelImage->setPixmap(QPixmap::fromImage(qimg));
    }
}

void MainWindow::on_btnGaussianBlur_clicked() {
    spdlog::debug("高斯模糊");
    double simga = ui->sbSigma->value();
    if (!satelliteImages.empty()) {
        satelliteImages[0].applyGaussianBlur(simga);
        displayImage(satelliteImages[0].getMat());
    }
}

void MainWindow::on_btnMedianFilter_clicked() {
    spdlog::debug("中值滤波");
    int kernelSize = ui->sbKernelSize->value();
    if (!satelliteImages.empty()) {
        satelliteImages[0].applyMedianFilter(kernelSize);
        displayImage(satelliteImages[0].getMat());
    }
}

void MainWindow::on_btnImportPointCloud_clicked() {
    spdlog::debug("导入点云");
    QString fileName = QFileDialog::getOpenFileName(this, "打开点云文件", "", "PLY (*.ply)");
    if (!fileName.isEmpty()) {
        PointCloudData pointCloud(fileName.toStdString(), fileName.toStdString(),
                                  fileName.toStdString());
        pointCloud.loadPLY(fileName.toStdString());
        pointCloud.display();
        pointClouds.push_back(pointCloud);
    }
}

void MainWindow::on_btnVoxelFilter_clicked() {
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

void MainWindow::on_btnExportPLY_clicked() {
    spdlog::debug("导出PLY");
    if (!pointClouds.empty()) {
        QString fileName = QFileDialog::getSaveFileName(this, "保存点云文件", "", "PLY (*.ply)");
        if (!fileName.isEmpty()) {
            if (pointClouds[0].exportDataInPath("PLY", fileName.toStdString()))
                spdlog::debug("导出PLY成功");
            else
                spdlog::error("导出PLY失败");
        }
    }
}

MainWindow::~MainWindow() { delete ui; }