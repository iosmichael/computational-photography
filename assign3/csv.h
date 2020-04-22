#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <boost/algorithm/string.hpp>


class CalibrationParam {
public:
    std::string fpath;
    int numExposureRatios;
    std::vector<float> exposureRatio;

    CalibrationParam(std::string _fpath);
    void readData();
};

class ImageContainers {
public:
    std::string fname;
    std::string fdir;
    int numSource;
    int numFiles;
    std::vector<std::string> imgPath;
    std::vector<cv::Mat> imgData;

    ImageContainers(std::string _fname, std::string _fdir);
    void readData();
};