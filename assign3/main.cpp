#include <iostream>
#include "algo.h"
#include "csv.h"
#include <boost/algorithm/string.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string>

int main(int, char **)
{
   CalibrationParam calibParam("../7708.6-11/calparam");
   ImageContainers imageContainers("imglist", "../7708.6-11/");
   calibParam.readData();
   imageContainers.readData();
   for (auto expoRatio : calibParam.exposureRatio) {
       std::cout << expoRatio << std::endl;
   }
   std::cout << calibParam.numExposureRatios << std::endl;
   for (auto img : imageContainers.imgPath) {
       std::cout << img << std::endl;
   }
   std::cout << imageContainers.numFiles << std::endl;
   for (auto path : imageContainers.imgPath) {
        cv::Mat data;
        data = cv::imread("../7708.6-11/s7708_06.bmp", cv::IMREAD_COLOR);
        if (data.empty()) {
            std::cout << "image file cannot be opened: " << path << std::endl;
            continue;
        }
        cv::namedWindow("display image", cv::WINDOW_AUTOSIZE);
        cv::imshow("display image", data);
        cv::waitKey(0);
        imageContainers.imgData.push_back(data);
    }
}