#pragma once
#include "../../std.h"

struct SolidMassFrac
{
	double gravel;
	double sand;
	double Organic;
	double QuartsFracOfSand;
};
struct ThermalCond
{
		double Air; 
		double Organic;// Organic matter
		double Quarts; 
		double Mineral;// Other Minerals
		double Water;
		double Ice;
};
struct SolidVolumesFrac
{
	double Quarts;
	double Org;
	double Sand;
	double Coarse;
};
struct SoilThermalCond
{
	double Dry;
	double Solid;
	double FixedSaturated;
};

class Thermal_Coef {
	public:
		Thermal_Coef();
	protected:
		ThermalCond m_ThermalCond;
		vector<SolidMassFrac> m_SolidMass;
		vector<SolidVolumesFrac> m_SolidVol;
		vector<SoilThermalCond> m_ThSoil;
		vector<double> m_BallardArp_PowerCoef;

};
