#pragma once
#include "includes.h"
#include "popInd.h"
#include "cityMap.h"
using namespace std;

class agent
{
private:
	// 一个整数，表明这个代理可以做 1 ~ actionCnt 的动作
	int actionCnt;
	popInd ind;
	vector<vector<vector<double> > >  QTable;
	double saProbabilityGetter(const double& diff, const double& tk);

public:
	class HH {
	public:
		// actinos
		static int LLHS_SWAP1_VEHICLE_INNER_X1X2_SWAP(popInd& ind);
		static int LLHS_SWAP2_VEHICLE_INNER_X_NEIBOR_SWAP(popInd& ind);
		static int LLHS_SWAP3_VEHICLE_INNER_SEG_SWAP(popInd& ind);
		static pair<int, int> LLHS_SWAP4_VEHICLE_OUTER_X1X2_SWAP(popInd& ind, const cityMap& bmCityMap);
		static pair<int, int> LLHS_SWAP5_VEHICLE_OUTER_SEG_SWAP(popInd& ind, const  cityMap& bmCityMap);
		static int LLHS_INSER1_VEHICLE_INNER_X_TO(popInd& ind);
		static int LLHS_INSER2_VEHICLE_INNER_SEG_TO(popInd& ind);
		static pair<int, int> LLHS_INSER3_VEHICLE_OUTER_FROM_TO(popInd& ind, const cityMap& bmCityMap);
		static pair<int, int> LLHS_INSER4_VEHICLE_OUTER_SEG_TO(popInd& ind, const cityMap& bmCityMap);
		static int LLHS_UNITIE_VEHICLE(popInd& ind);
		static int LLHS_INNER_OPT(popInd& ind, const cityMap& bmCityMap);
		static pair<int, int> LLHS_OUTER_OPT(popInd& ind, const cityMap& bmCityMap);

		static pair<int, int> twoVehicleConstructer(popInd& ind);

		static void localSearch2OPT(popInd& ind, const cityMap& bmCityMap, const vector<int>& vIdxs);

		static void localSearchStationInserMoveDel(popInd& ind, const cityMap& bmCityMap,
			const bool& isFix = false);

		static void twoOPTSwap(vector<int>& arr, const int& x1, const int& x2);

		static bool optPlusSwap(vector<int>& route1, vector<int>& route2,
			const int& breakP1, const int& breakP2, const cityMap& bmCityMap);

		static void greedyIntersecion(vector<int>& origRoute, vector<int> psToInser, const cityMap& bmCityMap);

		static pair<int, int> LLHS_NEW_INTER_INSER5_P2P_PLAN1(popInd& ind, const  cityMap& bmCityMap);

		static pair<int, int> LLHS_NEW_INTER_INSER5_P2P_PLAN2(popInd& ind, const  cityMap& bmCityMap);

		static pair<int, int> LLHS_NEW_INTER_SWAP_PandP_PLAN1(popInd& ind, const cityMap& bmCityMap);

		static pair<int, int> LLHS_NEW_INTER_SWAP_PandP_PLAN2(popInd& ind, const cityMap& bmCityMap);

		static vector<int> LLHS_NEW_DELandREGRETINSER(popInd& ind, const cityMap& bmCityMap);

		static int LLHS_NEW_INTRA_P2P_SWAP(popInd& ind, const cityMap& bmCityMap);

	};

	agent() {};

	agent(const int& acCnt);

	void greedyInput(const cityMap& bmCityMap, rsRecorder& recorder, const int& initedIndCnt);

	pair<bool, popInd> executeAction_TH(const int& acId, rsRecorder& recorder, const cityMap& bmCityMap, bool& isAccepted);


	bool THInnerActionExecutor(popInd& ind, int (*ptr)(popInd&), rsRecorder& recorder, const cityMap& bmCityMap);


	bool THOuterActionExecutor(popInd& ind, pair<int, int>(*ptr)(popInd&, const cityMap&), rsRecorder& recorder,
		const cityMap& bmCityMap);

	bool THOPTActionExecutor(popInd& ind, pair<int, int>(*ptr)(popInd&, const cityMap&), rsRecorder& recorder, const cityMap& bmCityMap);

	int actionChooser(const pair<int, int>& state);

	void updateQTable(const pair<int, int>& state, const pair<int, int>& stateNew, const double& fitFormer
		, const double& fitAfter, const int& acId, const double& rewardDisconst);

	void initAgent(const cityMap& bmCityMap, rsRecorder& recorder, const int& initedIndCnt);

	popInd& indGetter() { return this->ind; }

	int actionCntGetter() { return this->actionCnt; }

	static double localSearch2INSDEL(popInd& ind, const cityMap& bmCityMap, const bool& isInsert);

	static double singleRouteINSDEL(popInd& ind, const int& vIdx, const cityMap& bmCityMap);

	vector<vector<vector<double> > > getQTable() {
		return this->QTable;
	}

};

