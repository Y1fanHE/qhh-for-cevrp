#include "cityMap.h"
#include "includes.h"
using namespace std;

// const float& operator- (const cityMap& a);
/*返回欧氏距离*/
const double cityMap::cityP::operator-(const cityP &a)
{
	return sqrt(pow(this->x - a.x, 2) + pow(this->y - a.y, 2));
}

// 初始化CityMap，包括计算距离矩阵以及初始化demandStations
void cityMap::initCityMap()
{
	this->demandStations.resize(this->demandCnt);
	int cnt = 0;
	for (auto ii = this->demands.begin(); ii != this->demands.end(); ++ii)
	{
		this->demandStations[cnt++] = ii->first;
	}
	sort(this->demandStations.begin(), this->demandStations.end());

	this->disMap.resize(this->demandCnt + this->stationCnt + 1);
	for (int i = 0; i < this->disMap.size(); ++i)
		this->disMap[i].resize(this->demandCnt + this->stationCnt + 1);

	for (int i = 1; i < this->cityPs.size(); ++i)
		for (int j = 1; j < i; ++j)
		{
			this->disMap[this->cityPs[i].pId][this->cityPs[j].pId] =
					this->disMap[this->cityPs[j].pId][this->cityPs[i].pId] =
							this->cityPs[i] - this->cityPs[j];
		}
}