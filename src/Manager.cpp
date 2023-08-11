#include "Manager.h"
#include <iostream>
#include <fstream>
#include "Decoder.h"
#include "Encoder.h"

using namespace std;
using namespace cv;

Manager::Manager(std::vector<std::string> arguments)
{
	m_EncodeDecode = arguments[0];
	m_OriginalImgPath = arguments[1];
	m_EncodedImgPath = arguments[2];
	if (arguments.size() > 3)
	{
		m_MessagePath = arguments[3];
	}

	if (m_EncodeDecode == "-encode")
	{
		ManageEncoder();
	}
	else if (m_EncodeDecode == "-decode")
	{
		ManageDecoder();
	}
	else if (m_EncodeDecode == "-transparency")
	{
		RemoveTransparancy();
	}
	else
	{
		cout << "Enter the type of operation -encode or -decode" << endl;
	}
}

bool Manager::SingleOrMultiple()
{
	for (size_t i = m_OriginalImgPath.size() - 1; i >= 0; i--)
	{
		char theChar = m_OriginalImgPath[i];
		if (theChar == '.')
		{
			return true; //Single
		}
		else if (theChar == '\\')
		{
			return false; //Multiple
		}
	}
	return true;
}

void Manager::ManageEncoder()
{
	if (SingleOrMultiple())
	{
		EncodeSingleImage();
		system(m_EncodedImgPath.c_str());
	}
	else
	{
		EncodeMultipleImages();
		system(("explorer " + m_EncodedImgPath).c_str());
	}

	/*namedWindow("image", WINDOW_AUTOSIZE);
	imshow("image", encodedImg);
	waitKey(0);*/
}

void Manager::ManageDecoder()
{
	if (SingleOrMultiple())
	{
		DecodeSingleImage();
	}
	else
	{
		DecodeMultipleImages();
	}

	//cout << txt;
	system(m_MessagePath.c_str());
}

void Manager::EncodeSingleImage()
{
	std::cout << "Path to image to encode:    " << m_OriginalImgPath << "\n"; //arg2
	std::cout << "Path to save encoded image: " << m_EncodedImgPath << "\n"; //arg3
	std::cout << "Path to txt file to encode: " << m_MessagePath << "\n"; //arg4

	cv::Mat img = imread(m_OriginalImgPath);
	if (img.empty())
	{
		cerr << "Unable to open image in" << m_OriginalImgPath;
		exit(1);   // call system to stop
	}
	
	std::ifstream ifs;
	ifs.open(m_MessagePath);
	if (!ifs) {
		cerr << "Unable to open txt file in" << m_MessagePath;
		exit(1);   // call system to stop
	}
	std::string inputMessage((std::istreambuf_iterator<char>(ifs)),
		(std::istreambuf_iterator<char>()));
	ifs.close();

	Encoder encoder(img, inputMessage);
	if (encoder.SingleEncode())
	{
		Mat encodedImg = encoder.getEncodedImages()[0];

		imwrite(m_EncodedImgPath, encodedImg);
	}
	else
	{
		cerr << "Failed to encode";
		exit(1);   // call system to stop
	}
}

void Manager::DecodeSingleImage()
{
	cout << "Path to key image:                " << m_OriginalImgPath << "\n"; //arg2
	cout << "Path to image to decode:          " << m_EncodedImgPath << "\n"; //arg3
	cout << "Path to txt file to save message: " << m_MessagePath << "\n"; //arg4

	cv::Mat encodedImg = imread(m_EncodedImgPath);
	
	if (encodedImg.empty())
	{
		cerr << "Unable to open image in" << m_EncodedImgPath;
		exit(1);   // call system to stop
	}
	cv::Mat keyImg = imread(m_OriginalImgPath);

	if (keyImg.empty())
	{
		cerr << "Unable to open image in" << m_OriginalImgPath;
		exit(1);   // call system to stop
	}

	Decoder decoder(encodedImg, keyImg);
	if (decoder.SingleDecode())
	{
		string txt = decoder.GetMessage();

		ofstream ofs;
		ofs.open(m_MessagePath);
		ofs << txt;
		ofs.close();
	}
	else
	{
		cerr << "Failed to decode";
		exit(1);   // call system to stop
	}
}

void Manager::EncodeMultipleImages()
{
	std::cout << "Path to images directory to encode:    " << m_OriginalImgPath << "\n"; //arg2
	std::cout << "Path to save encoded images directory: " << m_EncodedImgPath << "\n"; //arg3
	std::cout << "Path to txt file to encode:            " << m_MessagePath << "\n"; //arg4

	vector<cv::String> fn;
	glob(m_OriginalImgPath + "/*.png", fn, false);

	vector<cv::Mat> images;
	size_t count = fn.size(); //number of png files in images folder
	for (size_t i = 0; i < count; i++)
	{
		images.push_back(imread(fn[i]));
	}

	std::ifstream ifs;
	ifs.open(m_MessagePath);
	if (!ifs) {
		cerr << "Unable to open txt file in " << m_MessagePath;
		exit(1);   // call system to stop
	}
	std::string inputMessage((std::istreambuf_iterator<char>(ifs)),
		(std::istreambuf_iterator<char>()));
	ifs.close();

	Encoder encoder(images, inputMessage);
	if (encoder.MultiEncode())
	{
		std::vector<Mat> encodedImgs = encoder.getEncodedImages();
		for (size_t i = 0; i < encodedImgs.size(); i++)
		{
			imwrite(m_EncodedImgPath + "/NewImage_" + std::to_string(i) + ".png", encodedImgs[i]);
		}
	}
	else
	{
		cerr << "Failed to encode";
		exit(1);   // call system to stop
	}
}

void Manager::DecodeMultipleImages()
{
	std::cout << "Path to key images directory:     " << m_OriginalImgPath << "\n"; //arg2
	std::cout << "Path to images directory to decode:    " << m_EncodedImgPath << "\n"; //arg3
	std::cout << "Path to save decoded txt file:         " << m_MessagePath << "\n"; //arg4

	vector<cv::String> fn1;
	glob(m_EncodedImgPath + "/*.png", fn1, false);
	vector<cv::Mat> encodedImgs;
	size_t count1 = fn1.size(); //number of png files in images folder
	for (size_t i = 0; i < count1; i++)
	{
		encodedImgs.push_back(imread(fn1[i]));
	}
	
	vector<cv::String> fn2;
	glob(m_OriginalImgPath + "/*.png", fn2, false);
	vector<cv::Mat> keyImgs;
	size_t count2 = fn2.size(); //number of png files in images folder
	for (size_t i = 0; i < count2; i++)
	{
		keyImgs.push_back(imread(fn2[i]));
	}

	Decoder decoder(encodedImgs, keyImgs);
	if (decoder.MultiDecode())
	{
		string txt = decoder.GetMessage();

		ofstream ofs;
		ofs.open(m_MessagePath);
		ofs << txt;
		ofs.close();
	}
	else
	{
		cerr << "Failed to decode";
		exit(1);   // call system to stop
	}
}

void Manager::RemoveTransparancy()
{
	vector<cv::String> fn;
	glob(m_OriginalImgPath + "/*.png", fn, false);
	vector<cv::Mat> Imgs;
	size_t count = fn.size(); //number of png files in images folder
	for (size_t i = 0; i < count; i++)
	{
		Imgs.push_back(imread(fn[i], IMREAD_UNCHANGED));
	}
	for (size_t i = 0; i < Imgs.size(); i++)
	{
		//make mask of where the transparent bits are
		std::vector<cv::Mat> pngChannels(4);
		cv::split(Imgs[i], pngChannels);

		// Set to 255(white) the RGB channels where the Alpha channel(mask) is 0(transparency)
		pngChannels[0].setTo(cv::Scalar(255), pngChannels[3] == 0);
		pngChannels[1].setTo(cv::Scalar(255), pngChannels[3] == 0);
		pngChannels[2].setTo(cv::Scalar(255), pngChannels[3] == 0);

		//new image without alpha channel...
		cv::merge(pngChannels, Imgs[i]);

		imwrite(m_EncodedImgPath + "/NewImage_" + std::to_string(i) + ".png", Imgs[i]);
	}

}