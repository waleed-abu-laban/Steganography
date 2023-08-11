#pragma once

#include "opencv2/opencv.hpp"

class Decoder
{
public:
	Decoder();
	Decoder(cv::Mat inputImage, cv::Mat keyImage);
	Decoder(std::vector<cv::Mat> inputImages, std::vector<cv::Mat> keyImages);

	bool SingleDecode();
	bool MultiDecode();

	std::string GetMessage();
	double GetSimilarity(const cv::Mat* A, const cv::Mat* B);
	bool MatchImages(std::map<cv::Mat*, cv::Mat*> &similarImages);
	bool SortImages(std::map<int, cv::Mat*> &imagesOrder);

private:
	bool processBuffer(std::vector<uchar> Buffer);

	std::vector<cv::Mat> m_inputImage;
	std::vector<cv::Mat> m_keyImage;
	std::string m_message;
};