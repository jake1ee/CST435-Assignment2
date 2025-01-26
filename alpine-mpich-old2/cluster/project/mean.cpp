#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <mpi.h>
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
	MPI_Init(NULL, NULL);
	int rankWorld;
	MPI_Comm_rank(MPI_COMM_WORLD, &rankWorld);

	if(rankWorld == 0)
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
		int sizeWorld;
		MPI_Comm_size(MPI_COMM_WORLD, &sizeWorld);
		
		double nDataPerWorker = (double)vecData.size() / (sizeWorld - 1);
		int indSta = 0;
		double dIndEnd = nDataPerWorker;
		int iIndEnd = std::round(dIndEnd);
		int nDataRound = iIndEnd - indSta;

		for(int i = 0; i < (sizeWorld - 1); i++)
		{
			std::vector<double> vecDataPerWorker(vecData.begin() + indSta, vecData.begin() + iIndEnd);
			MPI_Send(&vecDataPerWorker[0], nDataRound, MPI_DOUBLE, i + 1, 0, MPI_COMM_WORLD);

			indSta = iIndEnd;
			dIndEnd += nDataPerWorker;
			iIndEnd = std::round(dIndEnd);
			nDataRound = iIndEnd - indSta;
		}

		double sumDataPerWorker;
		double sumData = 0;

		for(int i = 0; i < (sizeWorld - 1); i++)
		{
			MPI_Recv(&sumDataPerWorker, 1, MPI_DOUBLE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			sumData += sumDataPerWorker;
		}

		double meanData = sumData / vecData.size();
		std::chrono::steady_clock::time_point timeEnd = std::chrono::steady_clock::now();

		std::cout << "Time = " << std::chrono::duration_cast<std::chrono::microseconds>(timeEnd - timeSta).count() << " us" << std::endl;
		std::cout << "Number of data = " << vecData.size() << std::endl;
		std::cout << "Sum = " << sumData << std::endl;
		std::cout << "Mean = " << meanData << std::endl;
	}
	else
	{
		MPI_Status status;
		MPI_Probe(0, 0, MPI_COMM_WORLD, &status);

		int nDataRound;
		MPI_Get_count(&status, MPI_DOUBLE, &nDataRound);

		std::vector<double> vecDataPerWorker(nDataRound);
		MPI_Recv(&vecDataPerWorker[0], nDataRound, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		std::for_each(vecDataPerWorker.begin(), vecDataPerWorker.end(), [](double& i)
			{
				for(int j = 0; j < 1000; j++)
				{
					i = sqrt(i);
				}
			});
		double sumDataPerWorker = std::accumulate(vecDataPerWorker.begin(), vecDataPerWorker.end(), 0.0);
		MPI_Send(&sumDataPerWorker, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
	}

	return 0;
}
