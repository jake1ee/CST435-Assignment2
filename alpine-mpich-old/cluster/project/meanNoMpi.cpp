#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <numeric>
#include <chrono>
#include <vector>

std::vector<double> getVecData(std::string data, const std::string delimiter)
{
	std::vector<double> vecData;
	size_t pos = 0;

	while((pos = data.find(delimiter)) != std::string::npos)
	{
		vecData.push_back(atof(data.substr(0, pos).c_str()));
		data.erase(0, pos + delimiter.length());
	}

	vecData.push_back(atof(data.c_str()));

	return vecData;
}

int main(int argc, char** argv)
{
	std::chrono::steady_clock::time_point timeSta = std::chrono::steady_clock::now();
	std::ifstream fData("data.csv");

	if(!fData.is_open())
	{
		std::cout << "Error is_open" << std::endl;

		return 1;
	}

	std::string data;
	getline(fData, data);

	std::vector<double> vecData = getVecData(data, ",");
	
	double sumData = std::accumulate(vecData.begin(), vecData.end(), 0.0);
	double meanData = sumData / vecData.size();
	
	std::chrono::steady_clock::time_point timeEnd = std::chrono::steady_clock::now();

	std::cout << "Time = " << std::chrono::duration_cast<std::chrono::microseconds>(timeEnd - timeSta).count() << " us" << std::endl;
	std::cout << "Number of data = " << vecData.size() << std::endl;
	std::cout << "Sum = " << sumData << std::endl;
	std::cout << "Mean = " << meanData << std::endl;

	return 0;
}
