#include <MainWindow.h>
#include <QApplication>
#include <iostream>
#include "spdlog/spdlog.h"

int main(int argc, char *argv[]) {
    spdlog::set_level(spdlog::level::debug);
    std::vector<SatelliteImage> satelliteImages;
    QApplication app(argc, argv);
    MainWindow mainWindow;
    mainWindow.show();
    return app.exec();
}