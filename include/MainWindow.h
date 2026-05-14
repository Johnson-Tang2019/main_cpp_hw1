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
#include <opencv2/opencv.hpp>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/**
 * @class MainWindow
 * @brief 主窗口类，负责处理 UI 交互逻辑、遥感影像与点云数据的可视化及业务流程控制。
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // ---------------------------------------------------------
    // 数据存储区（
    // ---------------------------------------------------------
    std::vector<SatelliteImage> satelliteImages; // 已加载的卫星影像序列
    std::vector<PointCloudData> pointClouds;     // 已加载的点云数据序列

    int selectedSatelliteIndex = -1;  // 当前 UI 选中的影像索引
    int selectedPointCloudIndex = -1; // 当前 UI 选中的点云索引

  private slots:
    // --- 遥感影像槽函数 ---
    void on_btnImport_clicked();       // 响应“Import Image”按钮，打开文件对话框加载影像
    void on_btnExportImage_clicked();  // 响应“Export Image”按钮，保存当前处理后的影像
    void on_btnGaussianBlur_clicked(); // 执行高斯模糊算法
    void on_btnMedianFilter_clicked(); // 执行中值滤波算法
    void on_cbImage_currentIndexChanged(int index); // 切换下拉框时，同步更新当前选中的影像

    // --- 点云数据槽函数 ---
    void on_btnImportPointCloud_clicked(); // 响应“Import PointCloud”按钮，加载 PLY/PCD 文件
    void on_btnVoxelFilter_clicked();      // 执行体素下采样滤波
    void on_btnExportPLY_clicked();        // 导出处理后的点云为 PLY 格式
    void on_cbPointCloud_currentIndexChanged(int index); // 切换点云索引

    // --- 界面同步与辅助函数 ---
    void updateImageComboBox();      // 当影像列表变动时，刷新下拉框条目
    void updatePointCloudComboBox(); // 当点云列表变动时，刷新下拉框条目

    void cheakNullImageError();      // 统一检查是否有影像被选中，若无则弹出提示
    void cheakNullPointCloudError(); // 统一检查是否有点云被选中，若无则弹出提示

    void updateImageInfo();      // 在 tbImage (TextBrowser) 中显示影像分辨率、波段等元数据
    void updatePointCloudInfo(); // 在 tbPointCloud 中显示点云数量、包围盒等信息

  private:
    Ui::MainWindow *ui; // 指向 UI 布局对象的指针

    /**
     * @brief 图像渲染核心函数
     * 将 OpenCV 的 cv::Mat 转换为 Qt 的 QPixmap，并缩放以适应 labelImage 的显示区域。
     */
    void displayImage();
};

#endif // MAINWINDOW_H