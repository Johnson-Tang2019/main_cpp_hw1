#include "MainWindow.h"
#include "spdlog/spdlog.h"
#include "ui_MainWindows.h"
#include <QFileDialog>
#include <QImage>
#include <QPixmap>
#include <SatelliteImage.h>

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

void MainWindow::displayImage(const cv::Mat &mat) {
    spdlog::debug("开始显示图像");
    cv::Mat displayMat;
    QImage qimg;

    // 1. 处理通道逻辑：如果是 5 通道，提取前 3 个通道用于显示

    spdlog::debug("检测到 5 通道影像，正在提取前 3 通道 (BGR) 进行显示");
    std::vector<cv::Mat> channels;
    cv::split(mat, channels);
    spdlog::debug("拆分通道");
    spdlog::debug("通道数: {}", channels.size());
    // 只保留前三个通道 (B, G, R)
    std::vector<cv::Mat> bgrChannels = {channels[0], channels[1], channels[2]};
    cv::merge(bgrChannels, displayMat);
    displayMat.convertTo(displayMat, CV_8UC3);

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

MainWindow::~MainWindow() { delete ui; }