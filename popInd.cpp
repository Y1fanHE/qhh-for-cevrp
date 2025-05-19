#include "popInd.h"
#include "includes.h"
#include "cityMap.h"
#include "commonOperations.h"
#include "commensParam.h"
using namespace std;


int popInd::vehicleCnt = 0;
int popInd::CARRYCAPCITY = 0;
double popInd::ENERAGECOMSUMPTION = 0;
int popInd::ENERAGECAPCITY = 0;


popInd::popInd(const popInd& ind) {
	this->route = ind.route;
	this->fitness = ind.fitness;
	this->eRoute = ind.eRoute;
	this->bFit = ind.bFit;
	this->orgFitness = ind.orgFitness;
}

vector<int> popInd::getChargesInRadius(const double& radius, const int& formerPId, const int& afterPId, const cityMap& bmCityMap) {
	vector<int> res;
	for (auto ii : bmCityMap.chargeStations) {
		if (bmCityMap.disMap[formerPId][ii] < radius && ii != formerPId && ii != afterPId)
			res.push_back(ii);
	}

	return res;
}


double popInd::eRoutesFiexed(const cityMap& bmCityMap) {
	//cout << "chargeStationFixing......" << endl;
	double accumulatedFit = 0;
	for (int i = 0; i < this->route.size(); ++i) {
		vector<int> routeTmp = this->route[i];
		reverse(routeTmp.begin(), routeTmp.end());

		vector<int> route1 = this->chargeStationInsertion(bmCityMap, this->route[i]);
		vector<int> route2 = this->chargeStationInsertion(bmCityMap, routeTmp);
		double fit1 = this->routeLenCul(route1, bmCityMap);
		double fit2 = this->routeLenCul(route2, bmCityMap);
		this->eRoute[i] = fit1 < fit2 ? route1 : route2;
		this->route[i] = fit1 < fit2 ? this->route[i] : routeTmp;
		accumulatedFit += fit1 < fit2 ? fit1 : fit2;
	}
	return accumulatedFit;
}


double popInd::routeLenCul(const vector<int>& route, const cityMap& bmCityMap)
{
	double res = 0;
	for (int i = 1; i < route.size(); ++i)
		res += bmCityMap.disMap[route[i - 1]][route[i]];
	return res;
}


bool popInd::electricityCheck(const cityMap& bmCityMap) {
	for (int vIdx = 0; vIdx < this->eRoute.size(); ++vIdx) {
		if (!this->singleRouteElectricityCheck(this->eRoute[vIdx], bmCityMap))
			return false;
	}
	return true;
}


bool popInd::singleRouteElectricityCheck(const vector<int>& eRoute, const cityMap& bmCityMap) {
	double currentQ = popInd::ENERAGECAPCITY;

	for (int pIdx = 1; pIdx < eRoute.size(); ++pIdx) {
		currentQ -=
			bmCityMap.disMap[eRoute[pIdx - 1]][eRoute[pIdx]] * popInd::ENERAGECOMSUMPTION;

		if (currentQ < 0)
			return false;

		if (bmCityMap.chargeStations.end() != find(bmCityMap.chargeStations.begin(), bmCityMap.chargeStations.end(),
			eRoute[pIdx]))
			currentQ = popInd::ENERAGECAPCITY;
	}
	return true;
}


bool popInd::singleRouteElectricityCheck(const list<int>& eRoute, const cityMap& bmCityMap) {
	double currentQ = popInd::ENERAGECAPCITY;
	list<int> tmpRoute = eRoute;

	list<int>::iterator ibefore = tmpRoute.begin();
	list<int>::iterator iafter = tmpRoute.begin();
	++iafter;

	for (; iafter != tmpRoute.end(); ++ibefore, ++iafter) {
		currentQ -=
			bmCityMap.disMap[*ibefore][*iafter] * popInd::ENERAGECOMSUMPTION;

		if (currentQ < 0)
			return false;

		if (bmCityMap.demands.end() == bmCityMap.demands.find(*iafter))
			currentQ = popInd::ENERAGECAPCITY;
	}


	return true;
}


int popInd::singleRouteCarryCul(const vector<int>& route, const cityMap& bmCityMap) {
	int accumulateV = 0;
	for (int i = 0; i < route.size(); ++i)
		accumulateV += bmCityMap.demands.find(route[i])->second;

	return accumulateV;
}


bool popInd::singleRouteCarryCheck(const vector<int>& route, const cityMap& bmCityMap) {
	int accumulateV = popInd::singleRouteCarryCul(route, bmCityMap);

	return (accumulateV < popInd::CARRYCAPCITY ||
		commonOperations::isClose(accumulateV, popInd::CARRYCAPCITY)) ? true : false;
}


int popInd::getBestChargeStationBetween(const int& front, const int& next, const vector<int>& charges,
	const cityMap& bmCityMap) {
	int bestP = -1;
	double minimumDis = 1e10;
	double disTmp;
	for (int i = 0; i < charges.size(); ++i) {
		if (charges[i] == front || charges[i] == next)
			continue;

		disTmp = bmCityMap.disMap[front][charges[i]] + bmCityMap.disMap[charges[i]][next];
		if (disTmp < minimumDis) {
			minimumDis = disTmp;
			bestP = charges[i];
		}
	}
	return bestP;
}


vector<int> popInd::chargeStationInsertion(const cityMap& bmCityMap, const vector<int>& originalRoute) {
	vector<int> chargedRoute;
	chargedRoute.push_back(bmCityMap.originalP);

	double current_Q = this->ENERAGECAPCITY;


	int p = 1;
	while (p < originalRoute.size()) {
		double dis = bmCityMap.disMap[chargedRoute[chargedRoute.size() - 1]][originalRoute[p]];
		if (current_Q > dis * this->ENERAGECOMSUMPTION ||
			commonOperations::isClose(current_Q, dis * this->ENERAGECOMSUMPTION)) {
			current_Q -= dis * this->ENERAGECOMSUMPTION;
			chargedRoute.push_back(originalRoute[p]);
		}
		else {
			int nextP = originalRoute[p];
			double radius = current_Q / this->ENERAGECOMSUMPTION;
			vector<int> chargeStationsInRadius = this->getChargesInRadius(radius,
				chargedRoute[chargedRoute.size() - 1], nextP, bmCityMap);
			--p;
			while (chargeStationsInRadius.size() < 1) {
				current_Q += this->ENERAGECOMSUMPTION *
					bmCityMap.disMap[chargedRoute[chargedRoute.size() - 2]]
					[chargedRoute[chargedRoute.size() - 1]];

				if (originalRoute.end() !=
					find(originalRoute.begin(), originalRoute.end(),
						chargedRoute[chargedRoute.size() - 1]))
					--p;


				nextP = chargedRoute[chargedRoute.size() - 1];
				chargedRoute.pop_back();

				radius = current_Q / this->ENERAGECOMSUMPTION;
				chargeStationsInRadius = this->getChargesInRadius(radius,
					chargedRoute[chargedRoute.size() - 1], nextP, bmCityMap);
			}
			int bestMediumChargeStation = this->getBestChargeStationBetween(
				chargedRoute[chargedRoute.size() - 1], nextP, chargeStationsInRadius, bmCityMap);

			chargedRoute.push_back(bestMediumChargeStation);
			current_Q = this->ENERAGECAPCITY;
		}
		++p;
	}

	if (!this->singleRouteElectricityCheck(chargedRoute, bmCityMap)) {
		commonOperations::vectorPrinter(originalRoute);
		exit(2);
	}

	return chargedRoute;
}


vector<int> popInd::eRouteStationInsertion(vector<int> originalRoute, const int& insertionPos,
	const int& delPos, const cityMap& bmCityMap) {
	//1 29 10 8 6 3 2 9 1
	if (!this->singleRouteElectricityCheck(originalRoute, bmCityMap)) {
		string str = "Electricity constrain is not satisfied.Can not insert any station in eRoute.\n";
		for (auto p : originalRoute)
			str += to_string(p) + " ";
		//cout << str << endl;
		originalRoute = popInd().chargeStationInsertion(bmCityMap, originalRoute);
	}

	vector<int> routeTmp = originalRoute;

	vector<int> res = { bmCityMap.originalP };

	int pInsertion = insertionPos < delPos ? insertionPos + 1 : insertionPos;

	double currentQ = popInd::ENERAGECAPCITY;
	if (insertionPos == 0) {
		// 如果是想在起点后面插入充电桩
		double radius = currentQ / popInd::ENERAGECOMSUMPTION;
		vector<int> tmp = this->getChargesInRadius(radius, bmCityMap.originalP, routeTmp[1], bmCityMap);
		int mediumBestCharge = this->getBestChargeStationBetween(bmCityMap.originalP,
			routeTmp[1], tmp, bmCityMap);
		if (mediumBestCharge > -1) {
			res.push_back(mediumBestCharge);
			currentQ = popInd::ENERAGECAPCITY;
		}
	}

	int curP = 1;

	while (curP < routeTmp.size()) {
		double dis = bmCityMap.disMap[res[res.size() - 1]][routeTmp[curP]];

		if (currentQ > dis * popInd::ENERAGECOMSUMPTION) {
			currentQ -= dis * popInd::ENERAGECOMSUMPTION;
			res.push_back(routeTmp[curP]);
			if (bmCityMap.chargeStations.end() !=
				find(bmCityMap.chargeStations.begin(), bmCityMap.chargeStations.end(), routeTmp[curP])) {
				currentQ = popInd::ENERAGECAPCITY;
			}
		}
		else {
			int nextP = routeTmp[curP];
			double radius = currentQ / popInd::ENERAGECOMSUMPTION;

			vector<int> charsInRadius = this->getChargesInRadius(radius, res[res.size() - 1], nextP, bmCityMap);

			--curP;
			while (charsInRadius.size() < 1) {
				currentQ += bmCityMap.disMap[res[res.size() - 2]][res[res.size() - 1]];

				if (routeTmp.end() != find(routeTmp.begin(), routeTmp.end(), res[res.size() - 1]))
					--curP;
				nextP = res[res.size() - 1];
				res.erase(res.end() - 1);

				radius = currentQ / popInd::ENERAGECOMSUMPTION;
				charsInRadius = this->getChargesInRadius(radius, res[res.size() - 1], nextP, bmCityMap);
			}
			int bestMediumStation = this->getBestChargeStationBetween(res[res.size() - 1], nextP, charsInRadius,
				bmCityMap);
			if (-1 < bestMediumStation) {
				res.push_back(bestMediumStation);
				currentQ = popInd::ENERAGECAPCITY;
			}
		}

		++curP;

		if (curP == pInsertion) {
			int radius = currentQ / popInd::ENERAGECOMSUMPTION;
			vector<int> charsInRadius = this->getChargesInRadius(radius, res[res.size() - 1], routeTmp[curP], bmCityMap);
			int bestMediumStation = this->getBestChargeStationBetween(res[res.size() - 1], routeTmp[curP], charsInRadius,
				bmCityMap);
			if (bestMediumStation > -1) {
				res.push_back(bestMediumStation);
				currentQ = popInd::ENERAGECAPCITY;
			}
		}
	}
	return res;
}


double popInd::evaluateFitness(const cityMap& bmCityMap, const bool& isStationIns) {
	double accumulatedFit = 0;
	if (isStationIns) {
		accumulatedFit = this->eRoutesFiexed(bmCityMap);
	}
	else
		for (int i = 0; i < this->eRoute.size(); ++i)
			accumulatedFit += this->routeLenCul(this->eRoute[i], bmCityMap);

	this->fitness = accumulatedFit;
	this->bFit = this->bFit > this->fitness ? this->fitness : this->bFit;
	return accumulatedFit;
}


double popInd::eRoutesFixed(const cityMap& bmCityMap, const bool& isStationIns) {
	//cout << "chargeStationFixing......" << endl;
	double accumulatedFit = 0;
	vector<int> tmpRoute;
	double fit;
	double tmpFit;
	for (int i = 0; i < this->route.size(); ++i) {
		this->fullRouterEInsertion(i, bmCityMap);
		tmpRoute = this->eRoute[i];
		reverse(tmpRoute.begin(), tmpRoute.end());

		this->eRoute[i] = this->eRouteStationRouletteDel(this->eRoute[i], bmCityMap);
		tmpRoute = this->eRouteStationRouletteDel(tmpRoute, bmCityMap);

		fit = this->routeLenCul(this->eRoute[i], bmCityMap);
		tmpFit = this->routeLenCul(tmpRoute, bmCityMap);
		if (tmpFit < fit) {
			this->eRoute[i] = tmpRoute;
			accumulatedFit += tmpFit;
			reverse(this->route[i].begin(), this->route[i].end());
		}
		else {
			accumulatedFit += fit;
		}
	}

	return accumulatedFit;
}


double popInd::evaluateEFitness(const cityMap& bmCityMap, const bool& isStationIns) {
	double accumulatedFit = 0;
	if (isStationIns) {
		accumulatedFit = this->eRoutesFixed(bmCityMap, isStationIns);
	}
	else
		for (int i = 0; i < this->eRoute.size(); ++i)
			accumulatedFit += this->routeLenCul(this->eRoute[i], bmCityMap);

	this->fitness = accumulatedFit;
	return accumulatedFit;
}


double popInd::evaluateOFitness(const cityMap& bmCityMap) {
	double accumulatedFit = 0;

	for (int i = 0; i < this->route.size(); ++i)
		accumulatedFit += this->routeLenCul(this->route[i], bmCityMap);

	this->orgFitness = accumulatedFit;
	return accumulatedFit;
}


/*
	given:
		route: the route which the customer will insert
		customerId: the customer
		bmCityMap: the map of the city
	return:
		pair<bestPlaceIdx, cost>
*/
pair<int, double> popInd::calBestPlaceandCostofCus(const vector<int>& route, const int customerId, const cityMap& bmCityMap) {
	int minPlace = -1;
	double minCost = commensParam::DBLL_MAX;
	double cost;

	// search for the best place
	for (int i = 0; i < route.size() - 1; ++i) {
		cost = bmCityMap.disMap[route[i]][customerId] + bmCityMap.disMap[customerId][route[i + 1]] -
			bmCityMap.disMap[route[i]][route[i + 1]];
		if (cost < minCost) {
			minCost = cost;
			minPlace = i;
		}
	}
	return pair<int, double>(minPlace, cost);
}

