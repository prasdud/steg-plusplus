#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>


int main() {
    std::cout << "OpenCV version: " << CV_VERSION << std::endl;
    std::cout << "WORKING " << std::endl;
    
    cv::Mat image = cv::imread("../assets/input.png");

    if (image.empty())
    {
    	std::cout << "error :: image cant be loaded"<<std::endl;
    	return -1;
    }

    std::ofstream outfile("image_dat.rgb");
    if (!outfile) {
        std::cerr << "Error opening output file." << std::endl;
        return -1;
    }

    for( int i = 0 ; i < image.rows ; i++)
    {
        for( int j = 0 ; j < image.cols ; j++ )
        {
            if(image.type() == CV_8UC1)
            {
                image.at<uchar>(i,j) = 255;
            }
            else if(image.type() == CV_8UC3)
            {
                std::cout << (int)image.at<cv::Vec3b>(i,j)[0] << " " << (int)image.at<cv::Vec3b>(i,j)[1] << " " << (int)image.at<cv::Vec3b>(i,j)[2] << std::endl;
                outfile << (int)image.at<cv::Vec3b>(i,j)[0] << " " << (int)image.at<cv::Vec3b>(i,j)[1] << " " << (int)image.at<cv::Vec3b>(i,j)[2] << std::endl;

                //image.at<cv::Vec3b>(i,j)[0] = 255;
                //image.at<cv::Vec3b>(i,j)[1] = 255;
                //image.at<cv::Vec3b>(i,j)[2] = 255;

                //std::cout << (int)image.at<cv::Vec3b>(i,j)[0] << " " << (int)image.at<cv::Vec3b>(i,j)[1] << " " << (int)image.at<cv::Vec3b>(i,j)[2] << std::endl;
            }
            else
            {
                std::cout << "Unknown image format" << std::endl;
                return 0;
            }
        }
    }

    cv::namedWindow("image", cv::WINDOW_AUTOSIZE);
    cv::imshow("image", image);
    cv::waitKey(0);
    return 0;
}
//g++ main.cpp -o main `pkg-config --cflags --libs opencv`
