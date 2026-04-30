#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <opencv2/opencv.hpp> // 引入 OpenCV
#include <spdlog/spdlog.h>
#include "SatelliteImage.h"
#include "PointCloudData.h"
#include "spdlog/spdlog.h"
#include "ui_MainWindows.h"
#include <QFileDialog>
#include <QImage>
#include <QPixmap>
#include <SatelliteImage.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    std::vector<SatelliteImage> satelliteImages;
    std::vector<PointCloudData> pointClouds;
    ~MainWindow();

private slots:
    void on_btnImport_clicked(); // 导入图像按钮
    void on_btnExportImage_clicked(); // 导出图像按钮
    void on_btnGaussianBlur_clicked(); // 高斯模糊按钮
    void on_btnMedianFilter_clicked(); // 中值滤波按钮
    void on_btnImportPointCloud_clicked(); // 导入点云按钮
    void on_btnVoxelFilter_clicked(); // 体素滤波器按钮
    void on_btnExportPLY_clicked(); // 导出PLY按钮

private:
    Ui::MainWindow *ui;
    void displayImage(const cv::Mat& mat); // 图像显示核心函数
};

#endif