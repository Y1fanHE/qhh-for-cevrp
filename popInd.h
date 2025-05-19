#pragma once
#include "includes.h"
#include "cityMap.h"
using namespace std;

class popInd
{
private:
    double bFit;
    double orgFitness;
    double fitness;
    // 原路径
    vector<vector<int> > route;
    // 插入充电桩后的路径
    vector<vector<int> > eRoute;
public:
    // 路径数量
    static int vehicleCnt;
    static int ENERAGECAPCITY;
    static double ENERAGECOMSUMPTION;
    static int CARRYCAPCITY;
    popInd() { this->bFit = 1e6; };
    popInd(const popInd& ind);
    double& getBFit() { return this->bFit; }
    void setBFit(const double& fit) { this->bFit = fit; }
    void initInd(const cityMap& bmCityMap);
    double evaluateFitness(const cityMap& bmCityMap, const bool& isStationIns);
    int getBestChargeStationBetween(const int& front, const int& next, const vector<int>& charges,
        const cityMap& bmCityMap);
    vector<int> chargeStationInsertion(const cityMap& bmCityMap, const vector<int>& originalRoute);
    vector<int> eRouteStationInsertion(vector<int> originalRoute, const int& insertionPos, const int& delPos, const cityMap& bmCityMap);
    vector<int> getChargesInRadius(const double& radius, const int& formerPId, const int& afterPId, const cityMap& bmCityMap);
    double eRoutesFiexed(const cityMap& bmCityMap);
    static double routeLenCul(const vector<int>& route, const cityMap& bmCityMap);
    double getFitness() { return this->fitness; }
    double getOFitness() { return this->orgFitness; }
    void setFitness(const double& fit) { this->fitness = fit; }
    void setOFitness(const double& ofit) { this->orgFitness = ofit; }
    bool electricityCheck(const cityMap& bmCityMap);
    static int singleRouteCarryCul(const vector<int>& route, const cityMap& bmCityMap);
    static bool singleRouteElectricityCheck(const vector<int>& eRoute, const cityMap& bmCityMap);
    static bool singleRouteElectricityCheck(const list<int>& eRoute, const cityMap& bmCityMap);
    static bool singleRouteCarryCheck(const vector<int>& route, const cityMap& bmCityMap);
    vector<vector<int> >& routeGetter() { return this->route; }
    vector<vector<int> >& eRouteGetter() { return this->eRoute; }
    vector<int> eRouteStationRouletteDel(const vector<int>& originalRoute, const cityMap& bmCityMap);
    void fullRouterEInsertion(const int& vIdx, const cityMap& bmCityMap);
    double eRoutesFixed(const cityMap& bmCityMap, const bool& isStationIns);
    double evaluateEFitness(const cityMap& bmCityMap, const bool& isStationIns);
    double evaluateOFitness(const cityMap& bmCityMap);

    // 2025-03-10 17:58 caculate the best place and the cost of the given customer in the given route
    static pair<int, double> calBestPlaceandCostofCus(const vector<int>& route, const int customerId, const cityMap& bmCityMap);
    // 2025-03-10 17:58 caculate the best place and the cost of the given customer in the given route

    // 2025-03-10 18:11 cost-value function
    static vector<double> cost_value_func(const vector<double>& costs, double delta = 1);
    // 2025-03-10 18:11 cost-value function

    // 2025-03-10 18:31 rottle wheel selection for funcVal
    static int RWSFuncVal(vector<double> funVs);
    // 2025-03-10 18:11 cost-value function
};

