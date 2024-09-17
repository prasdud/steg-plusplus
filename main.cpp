#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    std::cout << "OpenCV version: " << CV_VERSION << std::endl;
    std::cout << "WORKING " << std::endl;
    
    cv::Mat image = cv::imread("input.png");

    if (image.empty())
    {
    	std::cout << "error :: image cant be loaded"<<std::endl;
    	return -1;
    }

    cv::namedWindow("image", cv::WINDOW_AUTOSIZE);
    cv::imshow("image", image);
    cv::waitKey(0);
    return 0;
}
