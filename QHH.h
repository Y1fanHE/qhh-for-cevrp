#pragma once
#include "includes.h"
#include "cityMap.h"
#include "agent.h"
using namespace std;


class QHH
{
private:
	pair<int, int> stateCulculater(const double& fomerFit, int& remainTimes);

public:
	ofstream convergenceStream;
	double timelimited;
	double timeused;

	cityMap bmCityMap;
	agent bmAgent;
	rsRecorder recorder;

	double QHHSearch(const string& runModel);

	void resultRecoder(const int& solutionNumb, const string& paramDir = "");

	void init(const string& fileName, const string& searchModel, const int& solutionNumb, unsigned int seed);

	void runInSeed(const string& fileName, const string& searchModel, const int& seed);

	void ParamAdjuster(const string& fileName, vector<vector<int> > idxs,  const int& solutionNumb, unsigned int seed);

	void ExperimentEnterance(const string& fileName, unsigned int seed, const int& solutionNumb);

	void localSearch2INSANDDEL();
};

