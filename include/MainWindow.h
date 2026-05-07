#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "PointCloudData.h"
#include "SatelliteImage.h"
#include "spdlog/spdlog.h"
#include "ui_MainWindows.h"
#include <QFileDialog>
#include <QImage>
#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
#include <QPixmap>
#include <SatelliteImage.h>
#include <opencv2/opencv.hpp> // 引入 OpenCV
#include <spdlog/spdlog.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    MainWindow(QWidget *parent = nullptr);
    std::vector<SatelliteImage> satelliteImages;
    std::vector<PointCloudData> pointClouds;
    int selectedSatelliteIndex = -1;
    int selectedPointCloudIndex = -1;
    ~MainWindow();

  private slots:
    void on_btnImport_clicked();                         // 导入图像按钮
    void on_btnExportImage_clicked();                    // 导出图像按钮
    void on_btnGaussianBlur_clicked();                   // 高斯模糊按钮
    void on_btnMedianFilter_clicked();                   // 中值滤波按钮
    void on_btnImportPointCloud_clicked();               // 导入点云按钮
    void on_btnVoxelFilter_clicked();                    // 体素滤波器按钮
    void on_btnExportPLY_clicked();                      // 导出PLY按钮
    void updateImageComboBox();                          // 更新图像下拉框内容
    void updatePointCloudComboBox();                     // 更新点云下拉框内容
    void on_cbImage_currentIndexChanged(int index);      // 图像下拉框索引改变槽
    void on_cbPointCloud_currentIndexChanged(int index); // 点云下拉框索引改变槽
    void cheakNullImageError();                          // 空图像错误提示
    void cheakNullPointCloudError();                     // 空点云错误提示
    void updateImageInfo();                              // 更新图像信息显示
    void updatePointCloudInfo();                         // 更新点云信息显示

  private:
    Ui::MainWindow *ui;
    void displayImage(); // 图像显示核心函数
};

#endif