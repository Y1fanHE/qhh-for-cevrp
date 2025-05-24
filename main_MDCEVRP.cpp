#pragma once
#include "includes.h"
#include "popInd.h"
#include "Params.h"
#include "QHH.h"
using namespace std;

int checkR()
{
	ifstream ifs;
	vector<vector<int>> eRoutes;
	ifs.open("solution22.5.txt", ios::in);
	char buf[1024];
	string s;
	ifs.getline(buf, sizeof(buf));
	while (ifs.getline(buf, sizeof(buf)))
	{
		s = string(buf);
		vector<int> tmp;
		string tmps = "";
		for (char c : s)
		{
			if ('[' == c)
				continue;
			if (' ' == c)
				continue;
			if (',' == c)
			{
				tmp.push_back(stoi(tmps));
				tmps = "";
				continue;
			}
			if (']' == c)
			{
				tmp.push_back(stoi(tmps));
				tmps = "";
				continue;
			}
			tmps += c;
		}
		eRoutes.push_back(move(tmp));
	}

	QHH QHHModel;
	QHHModel.init("E-n22-k4.evrp", commensParam::EXPERIMENT_MODEL, 5, 1);
	popInd ind;
	ind.eRouteGetter() = eRoutes;
	ind.evaluateFitness(QHHModel.bmCityMap, false);
	cout << ind.getFitness() << endl;
	ifs.close();
	return 0;
}

bool fileSeedsCmp(const string &a, const string &b)
{
	return stoi(a.substr(a.find("_s") + 2, 100000)) < stoi(b.substr(b.find("_s") + 2, 100000));
}

bool cmp(string str1, string str2)
{
	return stoi(str1.substr(str1.find('n') + 1, str1.find("-k") - str1.find('n') - 1)) <
				 stoi(str2.substr(str2.find('n') + 1, str2.find("-k") - str2.find('n') - 1));
}

vector<int> getDepots(const string &filePath)
{
	ifstream fin;
	fin.open(filePath, ios::in);
	char strBuff[1024];
	string s;
	int depot;
	vector<int> depots;
	while (fin.getline(strBuff, 1024))
	{
		s = string(strBuff);

		if (string::npos != s.find("DEPOT_SECTION"))
		{
			fin.getline(strBuff, 1024);
			depot = atoi(strBuff);
			while (-1 != depot)
			{
				depots.emplace_back(depot);
				fin.getline(strBuff, 1024);
				depot = atoi(strBuff);
			}
		}
	}

	fin.close();
	return depots;
}

int main(int argc, char *argv[])
{
	vector<string> s = {
			"p01-n54-k4.mdevrp",
			"p02-n54-k2.mdevrp",
			"p03-n80-k3.mdevrp",
			"p04-n102-k8.mdevrp",
			"p05-n102-k5.mdevrp",
			"p06-n103-k6.mdevrp",
			"p07-n104-k4.mdevrp",
			"p08-n251-k14.mdevrp",
			"p09-n252-k12.mdevrp",
			"p10-n253-k8.mdevrp",
			"p11-n254-k6.mdevrp",
			"pr01-n52-k1.mdevrp",
			"pr02-n100-k2.mdevrp",
			"pr03-n148-k3.mdevrp",
			"pr04-n196-k4.mdevrp",
			"pr05-n244-k5.mdevrp",
			"pr06-n292-k6.mdevrp",
			"pr07-n78-k1.mdevrp",
			"pr08-n150-k2.mdevrp",
			"pr09-n222-k3.mdevrp",
			"pr10-n294-k4.mdevrp"};


	int cnt = 0;
	double executeTime;
	clock_t start, end;
	int seed = stoi(argv[1]);
	string fileName;
	
	vector<int> depots;
	for (auto ii = s.begin(); ii != s.end(); ++ii)
	{
		cout << *ii << endl;
		depots = getDepots(string("MDEVRP") + commensParam::PATH_SPLIT_SIMBOL + *ii);
		++cnt;
		for (int d : depots)
		{
			fileName = ii->substr(0, ii->find(".mdevrp")) + "-depot" + to_string(d) + ".mdevrp";
			QHH QHHModel;
			QHHModel.depotId = d;
			QHHModel.ExperimentEnterance(fileName, seed, cnt);
		}
		cout << endl;
	}

	return 0;
	
}
