#pragma once

#include "opencv2/opencv.hpp"

class Encoder
{
public:
	Encoder();
	Encoder(cv::Mat originalImage, std::string message);
	Encoder(std::vector<cv::Mat> originalImages, std::string message);
	bool SingleEncode();
	bool MultiEncode();
	std::vector<cv::Mat> getEncodedImages();

private:
	bool processText(std::vector<uchar> &Buffer);

	std::vector<cv::Mat> m_originalImage;
	std::vector<cv::Mat> m_encodedImage;
	std::vector<uchar> m_message;
};