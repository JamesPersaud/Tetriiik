#include "D3DTetrisHelpers.h"
#include <sstream>

using namespace std;

vector<string> tokenize(const string& str, const string& delimiters)
{
	vector<string> tokens;

	string::size_type lastPos = str.find_first_not_of(delimiters,0);

	string::size_type pos = str.find_first_of(delimiters, lastPos);

	while(string::npos != pos || string::npos != lastPos)
	{
		tokens.push_back(str.substr(lastPos, pos - lastPos));
		lastPos = str.find_first_not_of(delimiters, pos);
		pos = str.find_first_of(delimiters, lastPos);
	}

	return tokens;
}

float GetRandomFloat(float low, float high)
{
	float returnfloat;
	returnfloat = (rand()/(static_cast<float>(RAND_MAX)+1.0))*(high-low)+low;
	return returnfloat;
}

int GetRandomInt(int low, int high)
{
	int returnint;
	returnint = (rand()/(static_cast<int>(RAND_MAX)+1.0))*(high-low)+low;
	return returnint;
}