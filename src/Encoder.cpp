#include "Encoder.h"

Encoder::Encoder(cv::Mat originalImage, std::string message)
{
	originalImage.copyTo(m_originalImage[0]);//there is problem here //fixed maybe
	m_originalImage[0].copyTo(m_encodedImage[0]);
	m_message.insert(m_message.begin(), message.begin(), message.end());
}

Encoder::Encoder(std::vector<cv::Mat> originalImages, std::string message)
{
	for (size_t i = 0; i < originalImages.size(); i++)
	{
		m_originalImage.push_back(cv::Mat());
		originalImages[i].copyTo(m_originalImage[i]);
		m_encodedImage.push_back(cv::Mat());
		m_originalImage[i].copyTo(m_encodedImage[i]);
	}
	m_message.insert(m_message.begin(), message.begin(), message.end());
}

Encoder::Encoder()
{
	m_originalImage.empty();
	m_encodedImage.empty();
	m_message.empty();
}

bool Encoder::SingleEncode()
{
	std::vector<uchar> txtBuffer;
	if (!processText(txtBuffer))
	{
		return false;
	}

	int hideDistance = (m_encodedImage[0].rows * m_encodedImage[0].cols) / (txtBuffer.size() + 1);

	for (size_t i = 1; i < txtBuffer.size() + 1; i++)
	{
		int r = (int)(i * hideDistance / m_encodedImage[0].cols);
		int c = i * hideDistance  % m_encodedImage[0].cols;

		/*Vec3b intensity = img.at<Vec3b>(r, c);
		uchar blue = intensity.val[0];
		uchar green = intensity.val[1];
		uchar red = intensity.val[2];*/

		uchar value1 = txtBuffer[i - 1] / 3;
		uchar value2 = txtBuffer[i - 1] % 3;

		//Blue
		if (m_encodedImage[0].at<cv::Vec3b>(r, c).val[0] < (255 - value1))
		{
			m_encodedImage[0].at<cv::Vec3b>(r, c).val[0] += value1;
		}
		else
		{
			m_encodedImage[0].at<cv::Vec3b>(r, c).val[0] -= value1;
		}

		//Green
		if (m_encodedImage[0].at<cv::Vec3b>(r, c).val[1] < (255 - value1))
		{
			m_encodedImage[0].at<cv::Vec3b>(r, c).val[1] += value1;
		}
		else
		{
			m_encodedImage[0].at<cv::Vec3b>(r, c).val[1] -= value1;
		}

		//Red
		if (m_encodedImage[0].at<cv::Vec3b>(r, c).val[2] < (255 - (value1 + value2)))
		{
			m_encodedImage[0].at<cv::Vec3b>(r, c).val[2] += (value1 + value2);
		}
		else
		{
			m_encodedImage[0].at<cv::Vec3b>(r, c).val[2] -= (value1 + value2);
		}

		/*uchar blue2 = img.at<Vec3b>(r, c).val[0];
		uchar green2 = img.at<Vec3b>(r, c).val[1];
		uchar red2 = img.at<Vec3b>(r, c).val[2];
		uchar nonUsefull = img.at<Vec3b>(r, c).val[2];*/
	}
	return true;
}

bool Encoder::processText(std::vector<uchar> &Buffer)
{
	for (size_t i = 0; i < m_message.size(); i++)
	{
		if (m_message[i] == ' ')//space
			Buffer.push_back(1);
		else if (m_message[i] >= 'A' && m_message[i] <= 'Z')//capital english letters to small
			Buffer.push_back(m_message[i] - 63);
		else if (m_message[i] >= 'a' && m_message[i] <= 'z')//small english letters
			Buffer.push_back(m_message[i] - 95);
		else if (m_message[i] >= '0' && m_message[i] <= '9')//arabic numbers
			Buffer.push_back(m_message[i] - 20);
		else if (m_message[i] == '\n')//enter
			Buffer.push_back(38);
	}
	if (Buffer.size())
	{
		return true;
	}
	else
	{
		return false;
	}
}

std::vector<cv::Mat> Encoder::getEncodedImages()
{
	return m_encodedImage;
}

bool Encoder::MultiEncode()
{
	std::vector<uchar> txtBuffer;
	if (!processText(txtBuffer))
	{
		return false;
	}

	int stuffing = m_encodedImage.size() - (txtBuffer.size() % m_encodedImage.size());
	for (size_t i = 0; i < stuffing; i++)
	{
		txtBuffer.push_back(0);
	}
	int textSizePerImage = txtBuffer.size() / m_encodedImage.size();
	std::vector<int> hideDistances;
	for (size_t i = 0; i < m_encodedImage.size(); i++)
	{
		hideDistances.push_back((m_encodedImage[i].rows * m_encodedImage[i].cols) / (textSizePerImage + 1));
		//Green
		if (m_encodedImage[i].at<cv::Vec3b>(0, 0).val[1] < (255 - i))
		{
			m_encodedImage[i].at<cv::Vec3b>(0, 0).val[1] += i;
		}
		else
		{
			m_encodedImage[i].at<cv::Vec3b>(0, 0).val[1] -= i;
		}
	}

	int txtCounterPerImage = 0;
	for (size_t i = 1; i < txtBuffer.size() + 1; i++)
	{
		int circularCounter = (i - 1) % m_encodedImage.size(); //circulate around the images
		if (!circularCounter)
		{
			txtCounterPerImage++ /*floor((i+1) / m_encodedImage.size())*/;
		}
		int r = (int)(txtCounterPerImage * hideDistances[circularCounter] / m_encodedImage[circularCounter].cols);
		int c = txtCounterPerImage * hideDistances[circularCounter] % m_encodedImage[circularCounter].cols;

		uchar value1 = txtBuffer[i - 1] / 3;
		uchar value2 = txtBuffer[i - 1] % 3;

		//Blue
		if (m_encodedImage[circularCounter].at<cv::Vec3b>(r, c).val[0] < (255 - value1))
		{
			m_encodedImage[circularCounter].at<cv::Vec3b>(r, c).val[0] += value1;
		}
		else
		{
			m_encodedImage[circularCounter].at<cv::Vec3b>(r, c).val[0] -= value1;
		}

		//Green
		if (m_encodedImage[circularCounter].at<cv::Vec3b>(r, c).val[1] < (255 - value1))
		{
			m_encodedImage[circularCounter].at<cv::Vec3b>(r, c).val[1] += value1;
		}
		else
		{
			m_encodedImage[circularCounter].at<cv::Vec3b>(r, c).val[1] -= value1;
		}

		//Red
		if (m_encodedImage[circularCounter].at<cv::Vec3b>(r, c).val[2] < (255 - (value1 + value2)))
		{
			m_encodedImage[circularCounter].at<cv::Vec3b>(r, c).val[2] += (value1 + value2);
		}
		else
		{
			m_encodedImage[circularCounter].at<cv::Vec3b>(r, c).val[2] -= (value1 + value2);
		}
	}
	return true;
}
