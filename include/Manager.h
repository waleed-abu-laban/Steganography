#pragma once

#include "opencv2/opencv.hpp"

class Manager
{
public:
	Manager(std::vector<std::string> arguments);
	
private:
	void ManageEncoder();
	void ManageDecoder();

	bool SingleOrMultiple();

	void EncodeSingleImage();
	void EncodeMultipleImages();

	void DecodeSingleImage();
	void DecodeMultipleImages();

	void RemoveTransparancy();

	std::string m_EncodeDecode;
	std::string m_OriginalImgPath;
	std::string m_EncodedImgPath;
	std::string m_MessagePath;
};