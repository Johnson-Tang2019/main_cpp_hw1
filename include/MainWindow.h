#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <opencv2/opencv.hpp> // 引入 OpenCV
#include <spdlog/spdlog.h>
#include "SatelliteImage.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    std::vector<SatelliteImage> satelliteImages;
    ~MainWindow();

private slots:
    void on_btnImport_clicked(); // 修改为你点击导入按钮后的逻辑
    void on_btnGaussianBlur_clicked(); // 高斯模糊按钮
    void on_btnMedianFilter_clicked(); // 中值滤波按钮

private:
    Ui::MainWindow *ui;
    void displayImage(const cv::Mat& mat); // 图像显示核心函数
};

#endif