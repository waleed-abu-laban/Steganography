#include <iostream>
#include "Manager.h"

using namespace std;

int main(int argc, char** argv)
{
	if (argc == 5)
	{
		std::vector<std::string> Arguments;
		Arguments.push_back(argv[1]);   // operation
		Arguments.push_back(argv[2]);   // path to images to encode or key image
		Arguments.push_back(argv[3]);   // path to save encoded image or already encoded image
		Arguments.push_back(argv[4]);   // path to text to encode or message text
		Manager manager(Arguments);
	}
	else if (argc == 4)
	{
		std::vector<std::string> Arguments;
		Arguments.push_back(argv[1]);   // operation
		Arguments.push_back(argv[2]);   // path to images to remove transparency
		Arguments.push_back(argv[3]);   // path to save new images
		Manager manager(Arguments);
	}
	else
	{
		cout << endl;
		cout << "************************************************" << endl;
		cout << "Enter the type of operation:" << endl;
		cout << "-encode" << endl;
		cout << "-decode" << endl;
		cout << "-transparency" << endl;
		cout << endl;
		cout << "If -encode enter the following 3 arguments after:" << endl;
		cout << "Full path to single image to encode or Full path to images directory to encode" << endl;
		cout << "Full path to save encoded image or images" << endl;
		cout << "Full path to txt file to encode" << endl;
		cout << endl;
		cout << "if -decode enter the following 3 arguments after:" << endl;
		cout << "Full path to key image or Full path to key images directory" << endl;
		cout << "Full path to image to decode or Full path to images directory to decode" << endl;
		cout << "Full path to txt file to save message" << endl;
		cout << endl;
		cout << "if -transparency enter the following 2 arguments after:" << endl;
		cout << "Full path to images directory that you need to remove transparency from" << endl;
		cout << "Full path to save clean images" << endl;
		cout << "************************************************" << endl;
	}
	
	return 0;
}