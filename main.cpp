#pragma once
#include "includes.h"
#include "popInd.h"
#include "Params.h"
#include "QHH.h"
using namespace std;

bool fileSeedsCmp(const string &a, const string &b)
{
	return stoi(a.substr(a.find("_s") + 2, 100000)) < stoi(b.substr(b.find("_s") + 2, 100000));
}

bool cmp(string str1, string str2)
{
	return stoi(str1.substr(str1.find('n') + 1, str1.find("-k") - str1.find('n') - 1)) <
				 stoi(str2.substr(str2.find('n') + 1, str2.find("-k") - str2.find('n') - 1));
}

int main(int argc, char *argv[])
{
	/*
		argv 给入两个参数,
		argv[1]: 0——调参（根据685规模算例） 1——按参数跑遍所有算例
		argv[2]: 指定的随机种子 用于复现实验

	*/
	vector<string> s = {
			"1-E-n22-k4.evrp",
			"2-E-n23-k3.evrp",
			"3-E-n30-k3.evrp",
			"4-E-n33-k4.evrp",
			"5-E-n51-k5.evrp",
			"6-E-n76-k7.evrp",
			"7-E-n101-k8.evrp",
			"8-X-n143-k7.evrp",
			"9-X-n214-k11.evrp",
			"10-X-n351-k40.evrp",
			"11-X-n459-k26.evrp",
			"12-X-n573-k30.evrp",
			"13-X-n685-k75.evrp",
			"14-X-n749-k98.evrp",
			"15-X-n819-k171.evrp",
			"16-X-n916-k207.evrp",
			"17-X-n1001-k43.evrp",
			"18-E-n22-k4-s7.evrp",
			"19-E-n23-k3-s7.evrp",
			"20-E-n30-k3-s5.evrp",
			"21-E-n33-k4-s4.evrp",
			"22-F-n45-k4-s4.evrp",
			"23-E-n51-k5-s9.evrp",
			"24-F-n72-k4-s8.evrp",
			"25-E-n76-k7-s13.evrp",
			"26-E-n101-k8-s11.evrp",
			"27-M-n101-k10-s9.evrp",
			"28-M-n121-k7-s5.evrp",
			"29-F-n135-k5-s5.evrp",
			"30-X-n143-k7-s4.evrp",
			"31-M-n151-k12-s12.evrp",
			"32-M-n200-k16-s12.evrp",
			"33-X-n214-k11-s7.evrp",
			"34-X-n351-k40-s9.evrp",
			"35-X-n459-k26-s10.evrp",
			"36-X-n573-k30-s4.evrp",
			"37-X-n685-k75-s13.evrp",
			"38-X-n749-k98-s10.evrp",
			"39-X-n819-k171-s11.evrp",
			"40-X-n916-k207-s4.evrp",
			"41-X-n1001-k43-s5.evrp",
			"42-X-n195-k51.evrp",
			"43-X-n284-k15.evrp",
			"44-X-n298-k31.evrp",
			"45-X-n384-k52.evrp",
			"46-X-n393-k38.evrp",
			"47-X-n411-k19.evrp",
			"48-X-n420-k130.evrp",
			"49-X-n548-k50.evrp",
			"50-X-n561-k42.evrp",
			"51-X-n613-k62.evrp",
			"52-X-n627-k43.evrp",
			"53-X-n655-k131.evrp",
			"54-X-n670-k130.evrp",
			"55-X-n701-k44.evrp",
			"56-X-n783-k48.evrp",
			"57-X-n801-k40.evrp",
			"58-X-n856-k95.evrp",
			"59-X-n936-k151.evrp",
			"60-X-n979-k58.evrp"
			};

	// cin >> adjustedParam;
	// vector<double> addedTs = {-1, 4, 9, 15, 21};
	// vector<double> epsilons = {-1, 0.04, 0.08, 0.12, 0.16};
	// vector<double> percDiscs = {-1, 0.82, 0.86, 0.90, 0.94};
	// vector<int> reconstitutionHHTimesUnion = {-1, 1500, 2000, 2500, 3000};
	// the idxs below is used for 4 factors
	vector<vector<int>> idxs = {
			{1, 1, 1, 1},
			{1, 2, 2, 2},
			{1, 3, 3, 3},
			{1, 4, 4, 4},
			{2, 1, 2, 3},
			{2, 2, 1, 4},
			{2, 3, 4, 1},
			{2, 4, 3, 2},
			{3, 1, 3, 4},
			{3, 2, 4, 3},
			{3, 3, 1, 2},
			{3, 4, 2, 1},
			{4, 1, 4, 2},
			{4, 2, 3, 1},
			{4, 3, 2, 4},
			{4, 4, 1, 3}};
	// the idxs above is used for 4 factors

	// the idxs above is used for 5 factors
	/* vector<vector<int> > idxs = {
	 {1, 1, 1, 1, 1},
	 {1, 2, 2, 2, 2},
	 {1, 3, 3, 3, 3},
	 {1, 4, 4, 4, 4},
	 {2, 1, 2, 3, 4},
	 {2, 2, 1, 4, 3},
	 {2, 3, 4, 1, 2},
	 {2, 4, 3, 2, 1},
	 {3, 1, 3, 4, 2},
	 {3, 2, 4, 3, 1},
	 {3, 3, 1, 2, 4},
	 {3, 4, 2, 1, 3},
	 {4, 1, 4, 2, 3},
	 {4, 2, 3, 1, 4},
	 {4, 3, 2, 4, 1},
	 {4, 4, 1, 3, 2}
 };*/
	// the idxs above is used for 4 factors

	int cnt = 0;
	double executeTime;
	clock_t start, end;
	int seed = stoi(argv[2]);

	// for (int i = 0; i < 1; ++i) {

	for (auto ii = s.begin(); ii != s.end(); ++ii)
	{
		cout << *ii << endl;
		start = clock();
	
		if (stoi(argv[1]) == 0)
		{
			if (stoi(ii->substr(ii->find('n') + 1, ii->find("-k") - ii->find('n') - 1)) == 685)
			{
				QHH QHHModel;
				QHHModel.ParamAdjuster(*ii, idxs, seed, ++cnt);
			}
		}
		else if (stoi(argv[1]) == 1)
		{
			QHH QHHModel;
			QHHModel.ExperimentEnterance(*ii, seed, ++cnt);
		}
		else
		{ // perform ablation study on X-214 instance
			if (stoi(ii->substr(ii->find('n') + 1, ii->find("-k") - ii->find('n') - 1)) == 214)
			{
				QHH QHHModel;
				QHHModel.ExperimentEnterance(*ii, seed, ++cnt);
			}
		}

		end = clock();
	}
	return 0;
}
