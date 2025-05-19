#pragma once
#include "includes.h"
using namespace std;

class Params
{
public:
	// 调参运行的次数
	static int adjustTrainTimes;
	// 实验运行的次数
	static int experimentTrainTimes;
	static int innerActionTimes;
	// seed运行
	static int runInSeedTrainTimes;
	// 重构算子
	static int reconstitutionHHTimes;
	static int local2StationInserTimes;
	// 适应度不变减去的百分比数
	static double persistentDiscontPerc;
	// 动作的评价次数
	static int actionTryTimes;
	// Q表初始数
	static double initedQ;

	// 每n世代减少perc0大小
	static int perc0DiscontTimes;

	// 减少量
	static double perc0Discont;
	static double iteBaisMin;
	static double addedT;

	// 提升量划分
	static vector<double> improvementSplit;
	// 持续没变好代数划分
	static vector<int> persistentSplit;

	static double gama;
	static double alpha;
	static double epsilon;
	static double t0;
	static double QAddBias;
	static double QSubBias;

	static double threadR;
	static double thread0;
	static int maxRemainEpoches;
	static double intraTwoOptProbility;

	// 需要从文件里读出来的参数
	static vector<string> fileParams;


	static void param_reader2(int problemSize);
};

