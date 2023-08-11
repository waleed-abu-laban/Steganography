#include "Decoder.h"

Decoder::Decoder(cv::Mat inputImage, cv::Mat keyImage)
{
	m_inputImage.push_back(inputImage);
	m_keyImage.push_back(keyImage);
	m_message.empty();
}

Decoder::Decoder(std::vector<cv::Mat> inputImages, std::vector<cv::Mat> keyImages)
{
	m_inputImage = inputImages;
	m_keyImage = keyImages;
	m_message.empty();
}

Decoder::Decoder()
{
	m_inputImage.empty();
	m_keyImage.empty();
	m_message.empty();
}

bool Decoder::SingleDecode()
{
	std::vector<uchar> txtBuffer;

	for (size_t i = 0; i < m_inputImage[0].rows; i++)
	{
		for (size_t j = 0; j < m_inputImage[0].cols; j++)
		{
			uchar blueDiff = std::abs(m_inputImage[0].at<cv::Vec3b>(i, j).val[0] - m_keyImage[0].at<cv::Vec3b>(i, j).val[0]);
			uchar redDiff = std::abs(m_inputImage[0].at<cv::Vec3b>(i, j).val[2] - m_keyImage[0].at<cv::Vec3b>(i, j).val[2]);
			if (blueDiff || redDiff)
			{
				txtBuffer.push_back(blueDiff * 2 + redDiff);
			}
		}
	}

	return processBuffer(txtBuffer);
}

bool Decoder::processBuffer(std::vector<uchar> Buffer)
{
	std::string txt;
	for (size_t i = 0; i < Buffer.size(); i++)
	{
		if (Buffer[i] == 1)
			txt.push_back(' ');
		else if (Buffer[i] >= 2 && Buffer[i] <= 27)
			txt.push_back(Buffer[i] + 95);
		else if (Buffer[i] >= 28 && Buffer[i] <= 37)
			txt.push_back(Buffer[i] + 20);
		else if (Buffer[i] == 38)
			txt.push_back('\n');
	}
	if (txt.size())
	{
		m_message = txt;
		return true;
	}
	else
		return false;
}

std::string Decoder::GetMessage()
{
	return m_message;
}

bool Decoder::MultiDecode()
{
	std::map<cv::Mat*, cv::Mat*> similarImages;
	if (!MatchImages(similarImages))
	{
		return false;
	}

	std::map<int, cv::Mat*> imagesOrder;
	if (!SortImages(imagesOrder))
	{
		return false;
	}
	int totalTxtSize = 0;
	std::map<cv::Mat*, std::vector<uchar>> txtPerImage;
	for (size_t k = 0; k < imagesOrder.size(); k++)
	{
		cv::Mat inputImage = *imagesOrder[k];
		cv::Mat keyImage = *similarImages[imagesOrder[k]];
		std::vector<uchar> imageTxtBuffer;

		for (size_t i = 0; i < inputImage.rows; i++)
		{
			for (size_t j = 0; j < inputImage.cols; j++)
			{
				uchar blueDiff = std::abs(inputImage.at<cv::Vec3b>(i, j).val[0] - keyImage.at<cv::Vec3b>(i, j).val[0]);
				uchar redDiff = std::abs(inputImage.at<cv::Vec3b>(i, j).val[2] - keyImage.at<cv::Vec3b>(i, j).val[2]);
				if (blueDiff || redDiff)
				{
					imageTxtBuffer.push_back(blueDiff * 2 + redDiff);
				}
			}
		}
		totalTxtSize += imageTxtBuffer.size();
		txtPerImage[imagesOrder[k]] = imageTxtBuffer;
	}
	//Reverse the vectors per image so it becomes easier to read and remove
	for (auto& TPI : txtPerImage)
	{
		std::reverse(TPI.second.begin(), TPI.second.end());
	}
	std::vector<uchar> txtBuffer;
	for (size_t i = 0; i < totalTxtSize; i++)
	{
		int circularCounter = i % imagesOrder.size(); //circulate around the images
		std::vector<uchar> imageTxtBufferReversed = txtPerImage[imagesOrder[circularCounter]];
		txtBuffer.push_back(imageTxtBufferReversed[imageTxtBufferReversed.size()- 1]);
		txtPerImage[imagesOrder[circularCounter]].pop_back();
	}
	
	return processBuffer(txtBuffer);
}

double Decoder::GetSimilarity(const cv::Mat* A, const cv::Mat* B)
{
	// Compare two images by getting the L2 error (square-root of sum of squared error).
	if (A->rows > 0 && A->rows == B->rows && A->cols > 0 && A->cols == B->cols)
	{
		// Calculate the L2 relative error between images.
		double errorL2 = cv::norm(*A, *B);
		// Convert to a reasonable scale, since L2 error is summed across all pixels of the image.
		double similarity = errorL2 / (double)(A->rows * A->cols);
		return similarity;
	}
	else
	{
		//Images have a different size
		return 100000000.0;  // Return a bad value
	}
}

bool Decoder::MatchImages(std::map<cv::Mat*, cv::Mat*> &similarImages)
{
	std::vector<size_t> indicesToAvoid;
	for (size_t j = 0; j < m_inputImage.size(); j++)
	{
		double min = DBL_MAX;
		size_t indexOfSimilarImage = INT_MAX;
		std::vector<size_t>::iterator it;
		for (size_t i = 0; i < m_keyImage.size(); i++)
		{
			it = std::find(indicesToAvoid.begin(), indicesToAvoid.end(), i);
			if (it != indicesToAvoid.end())
			{
				continue;
			}
			double similarity = GetSimilarity(&m_inputImage[j], &m_keyImage[i]);
			if (min > similarity)
			{
				min = similarity;
				indexOfSimilarImage = i;
			}
		}
		if (indexOfSimilarImage != INT_MAX)
		{
			indicesToAvoid.push_back(indexOfSimilarImage);
			similarImages[&m_inputImage[j]] = &m_keyImage[indexOfSimilarImage];
		}
		else
		{
			return false;
		}
	}
	return true;
}

bool Decoder::SortImages(std::map<int, cv::Mat*> &imagesOrder)
{
	if (m_inputImage.size())
	{
		for (size_t i = 0; i < m_inputImage.size(); i++)
		{
			int orderTag = std::abs(m_inputImage[i].at<cv::Vec3b>(0, 0).val[1] - m_keyImage[i].at<cv::Vec3b>(0, 0).val[1]);
			imagesOrder[orderTag] = &m_inputImage[i];
		}
		return true;
	}
	else
	{
		return false;
	}
	
}