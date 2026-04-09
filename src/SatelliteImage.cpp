#include "SatelliteImage.h"

SatelliteImage::SatelliteImage(const std::string& id, const std::string& name, const std::string& path)
    : DataObject(id, name, path), resolution(0.0), cloudCoverage(0.0) {}