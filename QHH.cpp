#include "QHH.h"
#include "fileOperates.h"
#include "Params.h"
#include "cityMap.h"
#include "agent.h"
#include "commensParam.h"
#include "rsRecorder.h"
#include "commonOperations.h"
#include "LLHEfectionExperiment.h"
using namespace std;


pair<int, int> QHH::stateCulculater(const double& formerFit, int& remainTimes) {
	pair<int, int> res;
	double fitUpdate;

	remainTimes += (commonOperations::isClose(formerFit, this->bmAgent.indGetter().getFitness()) ||
		formerFit < this->bmAgent.indGetter().getFitness()) ?
		1 : (-1) * remainTimes;

	fitUpdate = (this->bmAgent.indGetter().getFitness() - formerFit) / formerFit;

	for (int i = 0; i < Params::improvementSplit.size() - 1; ++i)
		if ((fitUpdate > Params::improvementSplit[i] ||
			commonOperations::isClose(fitUpdate, Params::improvementSplit[i]))
			&& fitUpdate < Params::improvementSplit[i + 1]) {
			res.first = i;
			break;
		}
	/*fstream fout;
	fout.open("D:\\CCode\\QHH_EVRP\\fit.txt", ios::out | ios::app);
	fout << fixed << setprecision(5) <<  fitUpdate << endl;
	fout.close();*/

	if (remainTimes == commensParam::INTT_MAX)
		remainTimes = Params::persistentSplit[Params::persistentSplit.size() - 2];


	for (int i = 0; i < Params::persistentSplit.size(); ++i)
		if (remainTimes >= Params::persistentSplit[i] && remainTimes < Params::persistentSplit[i + 1]) {
			res.second = i;
			break;
		}

	return res;
}


double QHH::QHHSearch(const string& runModel) {
	double addedT = Params::addedT;
	double accAdded = addedT;
	double perc0Discont = Params::perc0Discont;

	double baseLine = 1;
	double originBaseLine = baseLine;
	bool isBetterGloabl = false;

	double fitFormer = 1e6;
	int remainTimes = 0;
	pair<int, int> stateNew;
	pair<int, int> state = this->stateCulculater(fitFormer, remainTimes);
	int action;


	int reconstitutionContainTimes = 0;
	int epoch = 1;
	bool isAccept = false;

	double perc0 = Params::thread0;
	int searchTimes = 5;
	int originSearchTimes = searchTimes;

	double bFitBefore = this->recorder.getBest().first;
	double fitBefore = this->bmAgent.indGetter().getBFit();

	cout << "with seed: " << this->recorder.getSeed() << endl;
	popInd currbst = popInd(this->bmAgent.indGetter());



	this->recorder.begin = clock();

	// while (epoch <= this->recorder.getMaxEpoch()) {
	while (this->timeused < this->timelimited) { // with time limited
		bFitBefore = currbst.getFitness();
		// 1.选择动作
		action = this->bmAgent.actionChooser(state);
		// 2.执行动作
		fitFormer = this->bmAgent.indGetter().getFitness();

		pair<bool, popInd> res = this->bmAgent.executeAction_TH(action, this->recorder, this->bmCityMap, isAccept);
		if (!res.first)
			break;

		currbst = res.second.getFitness() < currbst.getFitness() ?
			res.second : currbst;


		// 3.计算状态
		stateNew = this->stateCulculater(fitFormer, remainTimes);
		// 4.更新Q表
		// this->bmAgent.updateQTable(state, stateNew, fitFormer, res.second.getFitness(), action,
		// 	max(Params::iteBaisMin, (double)epoch / this->recorder.getMaxEpoch()));
		// search by limited time
		this->bmAgent.updateQTable(state, stateNew, fitFormer, res.second.getFitness(), action,
			max(Params::iteBaisMin, this->timeused / this->timelimited));

		// 5.LocalSearch2
		popInd tmpBest = this->recorder.getBestInd();
		agent::localSearch2INSDEL(tmpBest, this->bmCityMap, false);
		this->recorder.indCompareRecord(tmpBest);

		tmpBest = this->bmAgent.indGetter();
		agent::localSearch2INSDEL(tmpBest, this->bmCityMap, false);
		this->recorder.indCompareRecord(tmpBest);

		if (!commonOperations::isClose(bFitBefore, currbst.getFitness())) {
			isBetterGloabl = true;
			accAdded = addedT;
			reconstitutionContainTimes = 0;
			searchTimes = originSearchTimes;
		}
		else {
			++reconstitutionContainTimes;
		}

		// 6.状态转变
		state = stateNew;


		if (this->bmAgent.indGetter().getOFitness() * Params::thread0 < baseLine &&
			reconstitutionContainTimes >= Params::reconstitutionHHTimes) {
			// 重构
			reconstitutionContainTimes = 0;
			isBetterGloabl = false;
			remainTimes = 0;

			this->bmAgent.initAgent(bmCityMap, recorder, 1);

			Params::thread0 = perc0;
			fitFormer = 1e6;
			state = this->stateCulculater(fitFormer, remainTimes);
			currbst = this->bmAgent.indGetter();
			accAdded = addedT;
			baseLine = originBaseLine;
		}
		else {
			if (epoch % Params::perc0DiscontTimes == 0) {
				if (this->bmAgent.indGetter().getOFitness() * Params::thread0 > baseLine) {
					Params::thread0 *= perc0Discont;
				}
				else {
					if (searchTimes > 0) {
						--searchTimes;
					}
					else {
						Params::thread0 += accAdded / this->bmAgent.indGetter().getOFitness();
						accAdded += 5;
						searchTimes = originSearchTimes;
					}
				}
				isBetterGloabl = false;
			}
		}
	
		this->recorder.end = clock();
		this->timeused = (double)(this->recorder.end - this->recorder.begin) / (double)CLOCKS_PER_SEC;
		this->convergenceStream << fixed << setprecision(4) << this->timeused << "seconds," <<
		fixed << setprecision(8) <<
		this->recorder.getBest().first << endl;
		++epoch;	
	}
	// 参数复位
	Params::thread0 = perc0;

	return this->recorder.getBest().first;
}


void QHH::resultRecoder(const int& solutionNumb, const string& paramDir) {
	fileOperates::resRecorder(this->recorder, this->bmCityMap.demandCnt, paramDir, solutionNumb);
}


// 初始化算法各属性
void QHH::init(const string& fileName, const string& searchModel,  const int& solutionNumb, unsigned int seed) {
	// 读取算例文件
	fileOperates::benchmarkReader2(string("ins_60"), fileName, this->bmCityMap, this->bmAgent);
	// 读取当前文件的参数
	Params::param_reader2(bmCityMap.demandCnt);
	// 内部属性的初始化
	this->bmCityMap.initCityMap();
	srand(seed);

	this->recorder.initRsRecorder(this->bmCityMap.demandCnt + this->bmCityMap.stationCnt, seed,
		searchModel);

	this->bmAgent.initAgent(this->bmCityMap, recorder, commensParam::initIndUsedToChoose);

	string filePath = "convergence" + to_string(solutionNumb) +
		"." + to_string(seed) + ".txt";

	if (searchModel.compare(commensParam::PARAM_ADJUSTER_MODEL) == 0){
		filePath = string("convergence") + to_string(solutionNumb) + "_e" + to_string(Params::epsilon) +
							 "_alpha" + to_string(Params::alpha) + +"_gama" + to_string(Params::gama) + "_addedT" + to_string(Params::addedT) +
							 "_th0" + to_string(Params::thread0) + "_recons" +
							 to_string(Params::reconstitutionHHTimes) + "_percDisc" + to_string(Params::perc0Discont) +
							 "_perc0DiscontTimes" + to_string(Params::perc0DiscontTimes) +
							 "_intraTwoOptProbility" + to_string(Params::intraTwoOptProbility) +
							 "_s" + to_string(seed) + ".txt";
	}
		
	
	this->convergenceStream.open(filePath, ios::out | ios::app);

	// 初始化时间限制
	this->timelimited = (this->bmCityMap.stationCnt + this->bmCityMap.demandCnt) * 3.6;
	int multX = 1;
	if (this->bmCityMap.demandCnt < 143)
		multX = 1;
	else if (this->bmCityMap.demandCnt < 1001)
		multX = 2;
	else
		multX = 3;

	this->timelimited *= multX;

	this->timeused = 0;
}


void QHH::runInSeed(const string& fileName, const string& searchModel, const int& seed) {
	// 读取算例文件
	fileOperates::benchmarkReader2(string("evrp-benchmark-set"), fileName, this->bmCityMap, this->bmAgent);
	// 读取当前文件的参数
	Params::param_reader2(bmCityMap.demandCnt);

	// 内部属性的初始化
	this->bmCityMap.initCityMap();
	// 播撒随机种子
	srand(seed);
	// 初始化记录器
	this->recorder.initRsRecorder(this->bmCityMap.demandCnt + this->bmCityMap.stationCnt + 1, seed,
		searchModel);

	// 初始化代理
	this->bmAgent.initAgent(this->bmCityMap, recorder, commensParam::initIndUsedToChoose);
}


void QHH::ParamAdjuster(const string& fileName,
	vector<vector<int> > idxs, const int& solutionNumb,
	unsigned int seed) {
	int fileDimention = stoi(fileName.substr(fileName.find("n") + 1,
		fileName.find("-k") - fileName.find("n") - 1));

	vector<double> percDiscs = {  -1, 0.86, 0.9, 0.94, 0.98 };
	vector<double> addedTs = { -1, 4, 9, 15, 21 };
	vector<int> reconstitutionHHTimesUnion = { -1, 1500, 2000, 2500, 3000 };
	vector<double> epsilons = { -1, 0.04, 0.08, 0.12, 0.16 };

	string paramInTxt;

	string paramPrefix = "adj";

	string model = commensParam::PARAM_ADJUSTER_MODEL;

	for (int i = 0; i < idxs.size(); ++i) {
		this->init(fileName, commensParam::PARAM_ADJUSTER_MODEL, solutionNumb, seed);

		Params::addedT = addedTs[idxs[i][0]];
		Params::epsilon = epsilons[idxs[i][1]];
		Params::perc0Discont = percDiscs[idxs[i][2]];
		Params::reconstitutionHHTimes = reconstitutionHHTimesUnion[idxs[i][3]];
		
		this->QHHSearch(model);
		this->resultRecoder(solutionNumb, paramPrefix);	
	}

}


void QHH::ExperimentEnterance(const string& fileName, unsigned int seed, const int& solutionNumb) {
	this->init(fileName, commensParam::EXPERIMENT_MODEL, solutionNumb, seed);
	this->QHHSearch(commensParam::EXPERIMENT_MODEL);
	this->resultRecoder(solutionNumb);

	this->convergenceStream.close();
}


void QHH::localSearch2INSANDDEL() {




}
