#include "agent.h"
#include "includes.h"
#include "commensParam.h"
#include "cityMap.h"
#include "Params.h"
#include "rsRecorder.h"
#include "commonOperations.h"
#include "LLHEfectionExperiment.h"
using namespace std;

bool checkCustomers(const cityMap& bmCityMap, popInd& ind) {
	vector<bool> customers = vector<bool>(bmCityMap.demandCnt + 1, false);
	for (auto v : ind.eRouteGetter()) {
		for (auto p : v) {
			if (p > bmCityMap.demandCnt)
				continue;
			customers[p] = true;
		}
	}

	for (int c = 1; c < customers.size(); ++c)
		if (!customers[c]) {
			cout << c << endl;
			return false;
		}

	return true;
}


void agent::initAgent(const cityMap& bmCityMap, rsRecorder& recorder, const int& initedIndCnt) {
	this->actionCnt = commensParam::ACTINOCNT;
	//std::cout << "initing agent......" << "生成" << initedIndCnt  << "个个体" << endl;

	for (int i = 0; i < initedIndCnt; ++i) {
		this->ind = popInd();
		this->ind.initInd(bmCityMap);
		agent::HH::localSearch2OPT(this->ind, bmCityMap, commonOperations::arrange<int>(0,
			this->ind.routeGetter().size() - 1, 1));
		this->ind.evaluateOFitness(bmCityMap);
		this->localSearch2INSDEL(this->ind, bmCityMap, true);
		recorder.indCompareRecord(this->ind);
		++recorder.getEvalCnt();
	}

	// 初始化Q表
	this->QTable.resize(Params::improvementSplit.size() - 1);
	for (int i = 0; i < Params::improvementSplit.size() - 1; ++i) {
		this->QTable[i].resize(Params::persistentSplit.size() - 1);
		for (int j = 0; j < Params::persistentSplit.size() - 1; ++j) {
			this->QTable[i][j] = vector<double>(agent::actionCnt + 1, Params::initedQ);
		}
	}
}


double agent::localSearch2INSDEL(popInd& ind, const cityMap& bmCityMap, const bool& isInsert) {
	double accFit = 0;
	double fit;
	double fitTmp;
	double fitOrigin;
	popInd indTmp = popInd(ind);
	vector<int> routeTmp;

	for (int i = 0; i < ind.eRouteGetter().size(); ++i) {
		if (!isInsert)
			fitOrigin = ind.routeLenCul(ind.eRouteGetter()[i], bmCityMap);
		else
			fitOrigin = commensParam::DBLL_MAX;

		double singleINSDELRes = agent::singleRouteINSDEL(indTmp, i, bmCityMap);

		if (fitOrigin > singleINSDELRes) {
			fitOrigin = singleINSDELRes;
			ind.routeGetter()[i] = indTmp.routeGetter()[i];
			ind.eRouteGetter()[i] = indTmp.eRouteGetter()[i];
		}

		accFit += fitOrigin;
	}
	
	ind.setFitness(accFit);

	return accFit;
}


bool cmp(const pair<int, double>& p1, const pair<int, double>& p2) {
	return p1.second < p2.second;
}


void agent::greedyInput(const cityMap& bmCityMap, rsRecorder& recorder, const int& initedIndCnt) {
	priority_queue<pair<int, double>, vector<pair<int, double> >, decltype(&cmp) > demandsPs(cmp);

	vector<int> demandsTmp = bmCityMap.demandStations;
	random_shuffle(demandsTmp.begin(), demandsTmp.end());
	this->ind.routeGetter() = vector<vector<int> >();
	this->ind.routeGetter().push_back(vector<int>(1, bmCityMap.originalP));


	vector<double> capcityLefts;
	capcityLefts.push_back(this->ind.CARRYCAPCITY);

	for (int i = 0; i < demandsTmp.size(); ++i) {
		if (demandsTmp[i] == bmCityMap.originalP)
			continue;

		int vIdx = -1;
		int capcityBestLeft = 1e6;

		for (int leftIdx = 0; leftIdx < capcityLefts.size(); ++leftIdx) {
			if (capcityLefts[leftIdx] > bmCityMap.demands.find(demandsTmp[i])->second) {
				if (capcityBestLeft > capcityLefts[leftIdx] - bmCityMap.demands.find(demandsTmp[i])->second) {
					capcityBestLeft = capcityLefts[leftIdx] - bmCityMap.demands.find(demandsTmp[i])->second;
					vIdx = leftIdx;
				}
			}
		}

		if (vIdx == -1) {
			// 新建车辆
			this->ind.routeGetter().push_back(vector<int>(1, bmCityMap.originalP));
			this->ind.routeGetter()[this->ind.routeGetter().size() - 1].push_back(demandsTmp[i]);
			capcityLefts.push_back(this->ind.CARRYCAPCITY - bmCityMap.demands.find(demandsTmp[i])->second);
		}
		else {
			this->ind.routeGetter()[vIdx].push_back(demandsTmp[i]);
			capcityLefts[vIdx] -= bmCityMap.demands.find(demandsTmp[i])->second;
		}
	}

	for (int i = 0; i < this->ind.routeGetter().size(); ++i) {
		this->ind.routeGetter()[i].push_back(bmCityMap.originalP);
	}
	cout << this->ind.routeGetter().size() << endl;
}


agent::agent(const int& acCnt) {
	this->actionCnt = acCnt;
}


double agent::saProbabilityGetter(const double& diff, const double& tk) {
	return diff < 0 ? 2 : exp((-1 * diff) / tk);
}


bool agent::THOPTActionExecutor(popInd& ind, pair<int, int>(*ptr)(popInd&, const cityMap&), rsRecorder& recorder,
	const cityMap& bmCityMap){
	popInd indAction = popInd(ind);
	(*ptr)(indAction, bmCityMap);

	recorder.evaluateInd(bmCityMap, true, &indAction, &popInd::evaluateFitness);

	agent::localSearch2INSDEL(indAction, bmCityMap, true);
	
	recorder.indCompareRecord(indAction);

	ind = indAction;
	return true;
}


int agent::actionChooser(const pair<int, int>& state) {
	// TODO del
	//return commonOperations::randInt(1, this->actionCnt);
	
	/*
	// 2025-03-09 18:00 abliation for LLH. Del follow line for experiment.
	return 3;
	// 2025-03-09 18:00 abliation for LLH. Del above line for experiment.
	*/

	
	// 2025-03-08 15:32 experiment block
	if (rand() / (double)RAND_MAX < Params::epsilon) {
		// 随机选
		return commonOperations::randInt(1, this->actionCnt);
	}
	else {
		int j;
		for (j = 2; j <= this->actionCnt; ++j) {
			if (!commonOperations::isClose(this->QTable[state.first][state.second][1],
				this->QTable[state.first][state.second][j]))
				break;
		}
		if (j == this->actionCnt + 1)
			// 全部和动作1一样，返回随机值
			return commonOperations::randInt(1, this->actionCnt);


		int bestAction;
		double bestVal = -1e6;

		for (int i = 1; i <= this->actionCnt; ++i) {
			if (bestVal < this->QTable[state.first][state.second][i]) {
				bestVal = this->QTable[state.first][state.second][i];
				bestAction = i;
			}
		}
		return bestAction;
	}
	// 2025-03-08 15:32 experiment block
	
}


pair<bool, popInd> agent::executeAction_TH(const int& acId, rsRecorder& recorder, const cityMap& bmCityMap,
	bool& isAccepted) {
	int tryTimes = Params::actionTryTimes;

	popInd bstInd;
	bstInd.setFitness(1e6);

	for (int i = 0; i < tryTimes; ++i) {
		popInd indTmp = popInd(this->ind);


		// 动作执行模块
		switch (acId) {
		case 1: {
			if (!this->THInnerActionExecutor(indTmp, &agent::HH::LLHS_SWAP1_VEHICLE_INNER_X1X2_SWAP,
				recorder, bmCityMap))
				return pair<bool, popInd>(false, popInd());
			break;
		}
		case 2: {
			if (!this->THOuterActionExecutor(indTmp, &agent::HH::LLHS_NEW_INTER_SWAP_PandP_PLAN1,
				recorder, bmCityMap))
				return pair<bool, popInd>(false, popInd());
			break;
		}
		case 3: { 
			if (!this->THOuterActionExecutor(indTmp, &agent::HH::LLHS_SWAP5_VEHICLE_OUTER_SEG_SWAP,
				recorder, bmCityMap))
				return pair<bool, popInd>(false, popInd());
			break;
		}
		case 4: {
			if (!this->THInnerActionExecutor(indTmp, &agent::HH::LLHS_INSER1_VEHICLE_INNER_X_TO,
				recorder, bmCityMap))
				return pair<bool, popInd>(false, popInd());
			break;
		}
		case 5: {
			if (!this->THOuterActionExecutor(indTmp, &agent::HH::LLHS_NEW_INTER_INSER5_P2P_PLAN1,
				recorder, bmCityMap))
				return pair<bool, popInd>(false, popInd());

			break;
		}
		case 6: {
			if (!this->THInnerActionExecutor(indTmp, &agent::HH::LLHS_UNITIE_VEHICLE,
				recorder, bmCityMap))
				return pair<bool, popInd>(false, popInd());
			break;
		}
		default: {exit(2); }
		}

		agent::localSearch2INSDEL(indTmp, bmCityMap, true);
		recorder.indCompareRecord(indTmp);
		bstInd = indTmp.getFitness() < bstInd.getFitness() ? indTmp : bstInd;

		this->ind = indTmp;
	}

	return  pair<bool, popInd>(true, bstInd);;
}


void agent::updateQTable(const pair<int, int>& state, const pair<int, int>& stateNew, const double& fitFormer,
	const double& fitAfter, const int& acId, const double& rewardDisconst) {
	double award;

	if (commonOperations::isClose(fitAfter, fitFormer) || fitAfter > fitFormer) {
		award = ((fitFormer - fitAfter) / fitFormer + Params::QSubBias) * rewardDisconst;
	}
	else {
		award = ((fitFormer - fitAfter) / fitFormer + Params::QAddBias) * rewardDisconst;
	}

	double stateNewMaxVal = -1e5;
	for (int i = 1; i < QTable[stateNew.first][state.second].size(); ++i)
		stateNewMaxVal = QTable[stateNew.first][state.second][i] > stateNewMaxVal ?
		this->QTable[stateNew.first][state.second][i] : stateNewMaxVal;
	//std::cout << "<----------------------------------有提升----------------------------------->" << endl;
	//std::cout << this->QTable[state.first][state.second][acId] << endl;
	this->QTable[state.first][state.second][acId] =
		this->QTable[state.first][state.second][acId] + Params::alpha * (
			award + Params::gama * stateNewMaxVal - this->QTable[state.first][state.second][acId]);
	// 保证概率不是0
	this->QTable[state.first][state.second][acId] = max(Params::initedQ * 0.1,
		this->QTable[state.first][state.second][acId]);
}


pair<int, int> agent::HH::twoVehicleConstructer(popInd& ind)
{
	int rIdx1 = commonOperations::randInt(0, ind.routeGetter().size() - 1);
	int rIdx2 = commonOperations::randInt(0, ind.routeGetter().size() - 1);
	while (rIdx1 == rIdx2) {
		rIdx2 = commonOperations::randInt(0, ind.routeGetter().size() - 1);
	}
	return pair<int, int>(rIdx1, rIdx2);
}


