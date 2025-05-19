#pragma once
#include "includes.h"
using namespace std;

class cityMap
{
public:
    class cityP {
    public:
        int pId;
        double x;
        double y;
        int demand;
        cityP() {};
        cityP(const int& pIdTmp, double xTmp=0, double yTmp=0) : 
            pId(pIdTmp), x(xTmp), y(yTmp) {};
        const double operator- (const cityP& a);
        string printer() {
            string resStr = "";
            return resStr + "pId:" + to_string(this->pId) + " x:" +to_string(this->x) + " y: " + to_string(this->y);
        }
        static bool sortById(const cityP& a, const cityP& b) {
            return a.pId < b.pId;
        }
    };
    vector<vector<double> > disMap;
    // 点集
    vector<cityP> cityPs;
    // 需求点Map
    map<int, int> demands;
    // 需求点编号
    vector<int> demandStations;
    // 充电桩编号
    vector<int> chargeStations;
    // 需求点数和充电桩数
    int demandCnt;
    int stationCnt;
    int originalP;
    void initCityMap();
};


struct cityPIDwithDistance {
public:
    int pId;

    // route-index of customer pId
    int curRouteIndex;
    // index of customer pId in the curRoute
    int cusIndexofRoute;

    double dis;

    cityPIDwithDistance() {};
    cityPIDwithDistance(int id, int routeIndex, int cusIndex, double d): pId(id), curRouteIndex(routeIndex), 
        cusIndexofRoute(cusIndex), dis(d) {};
};


struct cityPDistanceSmallerCMP {
    bool operator ()  (const cityPIDwithDistance& p1, const cityPIDwithDistance& p2) {
        return p1.dis > p2.dis;
    }
};