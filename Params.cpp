#include "Params.h"
#include "includes.h"
#include "commensParam.h"
#include "commonOperations.h"
using namespace std;

// 调参运行的次数
int Params::adjustTrainTimes = 1;
// 实验运行的次数
int Params::experimentTrainTimes = 3;
int Params::innerActionTimes = 40;
int Params::runInSeedTrainTimes = 1;
int Params::reconstitutionHHTimes = 100;

// 适应度不变减去的百分比数
double Params::persistentDiscontPerc = 0.06;
// 充电桩插入重构个数
int Params::local2StationInserTimes = 5;

// 内循环次数
int Params::actionTryTimes = 3;
// Q表初始数
double Params::initedQ = 0.01;
double Params::gama = 1;

//double Params::alpha = 1;
double Params::alpha = 0.9;
//double Params::QAddBias = 0.01;
//double Params::QSubBias = -0.008;
double Params::QAddBias = 0.01;
double Params::QSubBias = -0.008;

double Params::iteBaisMin = 0.05;
double Params::addedT = 8;


vector<double> tmpVec1 = vector<double>({ -1 * commensParam::DBLL_MAX, -3.5 });
vector<double> tmpVec2 = vector<double>({ 3.5, commensParam::DBLL_MAX });

//vector<double> Params::improvementSplit = 
//commonOperations::template arrange<double>(-3.5, 3.5, 0.0001);
vector<double> Params::improvementSplit = commonOperations::concat(
	commonOperations::concat(tmpVec1, commonOperations::arrange<double>(-1,
		1, 0.0001)), tmpVec2);

//[0, 4, 10, 15, 20, np.inf]
//vector<int> Params::persistentSplit = { 0, 30, 60, 120, 200, commensParam::INTT_MAX };
vector<int> Params::persistentSplit = { 0, commensParam::INTT_MAX };

double Params::epsilon = 0;
double Params::t0 = 0.5;


//double Params::threadR = 0.986;
double Params::threadR = 1;
double Params::thread0 = 0.014;
int Params::maxRemainEpoches = 10;
int Params::perc0DiscontTimes = 40;
double Params::perc0Discont = 0.99;
double Params::intraTwoOptProbility = 0.2;


// 需要从文件里读出来的参数
vector<string> Params::fileParams = { "epsilon", "t_k", "beta" };


void Params::param_reader2(int problemSize) {
	//cout << "Param文件读取中....................." << endl;
	Params::thread0 = 0.014;

	string path = string("Params") + commensParam::PATH_SPLIT_SIMBOL + string("param.txt");
	try
	{
		ifstream fin;
		fin.open(path.c_str(), ios::in);

		if (!fin.is_open()) {
			cout << "打开Params文件失败！" << endl;
			exit(1);
		}

		char buf[1024];
		string str;

		while (fin.getline(buf, sizeof(buf))) {
			str = string(buf);
			int splitPos = str.find(":");
			string strTmp = str.substr(0, splitPos);
			if (!strTmp.compare("epsilon")) {
				Params::epsilon = stof(str.substr(splitPos + 1, 1024));
			}
			else if (!strTmp.compare("threadR")) {
				Params::threadR = stof(str.substr(splitPos + 1, 1024));
			}
			else if (!strTmp.compare("thread0Perc")) {
				Params::thread0 = stof(str.substr(splitPos + 1, 1024));
			}
			else if (!strTmp.compare("maxRemainEpoches")) {
				Params::maxRemainEpoches = stoi(str.substr(splitPos + 1, 1024));
			}
			else if (!strTmp.compare("reconstitutionHHTimes")) {
				Params::reconstitutionHHTimes = stoi(str.substr(splitPos + 1, 1024));
			}
			else if (!strTmp.compare("percDisc")) {
				Params::perc0Discont = stof(str.substr(splitPos + 1, 1024));
			}
			else if (!strTmp.compare("perc0DiscontTimes")) {
				Params::perc0DiscontTimes = stof(str.substr(splitPos + 1, 1024));
			}
			else if (!strTmp.compare("addedT")) {
				Params::addedT = stof(str.substr(splitPos + 1, 1024));
			}
			else if (!strTmp.compare("alpha")) {
				Params::alpha = stof(str.substr(splitPos + 1, 1024));
			}
			else if (!strTmp.compare("gama")) {
				Params::gama = stof(str.substr(splitPos + 1, 1024));
			}
			else if (!strTmp.compare("intraTwoOptProbility")) {
				Params::intraTwoOptProbility = stof(str.substr(splitPos + 1, 1024));
			}
		}

		fin.close();
		//cout << "Param文件读取完成！" << endl;
	}
	catch (const std::exception& e)
	{
		cout << e.what();
	}
}
