#include "csv.h"
#include <unistd.h>

CalibrationParam::CalibrationParam(std::string _fpath):fpath(_fpath){ }
ImageContainers::ImageContainers(std::string _fname, std::string _fdir):fname(_fname), fdir(_fdir){ }

void CalibrationParam::readData(){
    std::ifstream in(fpath);
    if (!in.is_open()) {
        std::cout << "Cannot open the input file \n";
        return;
    }

    std::string str;
    while (!in.eof())
    {
        std::getline(in, str);
        std::stringstream ss(str);
        std::string token;
        while (std::getline(ss, token, '\t'))
        {
            if (token == "numExposureRatios")
            {
                std::getline(ss, token, '\t');
                numExposureRatios = std::stoi(token);
            }
            else if (token == "exposureRatio")
            {
                std::getline(ss, token, '\t');
                std::getline(ss, token, '\t');
                exposureRatio.push_back(std::stof(token));
            }
        }
    }
    in.close();
}

void ImageContainers::readData(){
    std::string fpath = fdir + fname;
    std::ifstream in(fpath);
    if (!in.is_open())
    {
        std::cout << "Cannot open input file path: " << fpath << std::endl;    
    }
    std::string line;
    while (!in.eof())
    {
        while(std::getline(in, line)){
            std::vector<std::string> tokens;
            boost::split(tokens, line, boost::is_any_of(" "));
            if (tokens.front() == "numFiles") numFiles = std::stoi(tokens.back());
            if (tokens.front() == "FileName") imgPath.push_back(fdir + tokens.back());
        }
    }
    in.close();
    for (auto path : imgPath) {
        cv::Mat data;
        std::cout << path << std::endl;
        data = cv::imread(path, cv::IMREAD_COLOR);
        // data = cv::imread("../7708.6-11/s7708_06.bmp", cv::IMREAD_COLOR);
        if (data.empty()) {
            std::cout << "image file cannot be opened: " << path << std::endl;
            continue;
        }
        cv::namedWindow("display image", cv::WINDOW_AUTOSIZE);
        cv::imshow("display image", data);
        cv::waitKey(0);
        imgData.push_back(data);
    }
}