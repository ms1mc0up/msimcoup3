#pragma once
#include "../../../../std.h"

class EmergenceMoistureControl {
public:
	EmergenceMoistureControl(double WiltingPoint, double ThetaRange) : WiltingPoint(WiltingPoint), ThetaRange(ThetaRange) {};
	double operator() (double Theta) {
		if (Theta < WiltingPoint) return 0.;
		else if (Theta < (WiltingPoint + ThetaRange)) return (Theta - WiltingPoint) / ThetaRange;
		else return 1.;
	};
private:
	double WiltingPoint, ThetaRange;
};

class PlantTempSum {
public: PlantTempSum(vector<double> T_Thres, vector<double> T_Sum) :
	T_Thres(T_Thres), T_Sum(T_Sum) {
	Sum.resize(T_Sum.size());
	for (size_t i = 0; i < Sum.size(); i++) Sum[i] = 0.;
}
	PlantTempSum(vector<double> T_Thres, vector<double> T_Sum, double WiltingPoint, double ThetaRange) :
			T_Thres(T_Thres), T_Sum(T_Sum) {
			Sum.resize(T_Sum.size());
			for (size_t i = 0; i < Sum.size(); i++) Sum[i] = 0.;
			f_EmergenceMoistResponse = EmergenceMoistureControl(WiltingPoint, ThetaRange);
			
	}
	double operator() (size_t index, double Temp, double Theta, double T_Step) {
			Sum[index] += max(0., Temp - T_Thres[index])*T_Step*f_EmergenceMoistResponse(Theta);
			return SumFraction(index, Sum[index]);
	}
	double operator() (size_t index, double Temp, double T_Step) {
			Sum[index] += max(0., Temp - T_Thres[index])*T_Step;
			return SumFraction(index, Sum[index]);
	}
	double operator() (size_t index, double CurrentTSum) {
		return SumFraction(index, CurrentTSum);
	}

private:
	double SumFraction(size_t index, double CurrentTSum) {
		return min(1., CurrentTSum / T_Sum[index]);
	};
	vector<double> T_Thres, T_Sum, Sum;
	function<double(double)> f_EmergenceMoistResponse;
};





class EmergenceStart {
public:
	EmergenceStart(vector<double> T_Thres, vector<double> T_Sum, double WiltingPoint, double ThetaRange)  { 
	
		f_TempSum = PlantTempSum(T_Thres, T_Sum);
	
	
	};
	double operator() (size_t index, double Temp) {


        return 0.0;
	};




private:
	function<double(size_t, double, double)> f_TempSum;


};



class GrainStart {
public: GrainStart(vector<double> GrainCommonStep,vector<double> GrainStepDay, vector<double> Grain_ThresDay, vector<double> GrainStepTemp, vector<double> GrainThresTemp ) :
	GrainStepDay(GrainStepDay), Grain_ThresDay(Grain_ThresDay), GrainStepTemp(GrainStepTemp), GrainThresTemp(GrainThresTemp), GrainCommonStep(GrainCommonStep) {
	Sum.resize(GrainStepDay.size());
	for (size_t i = 0; i < Sum.size(); i++) Sum[i] = 0.;
};
double operator ()(size_t index, double DayLength, double Temp, double T_Step) {
			Sum[index]+=DayLengthScaling(index, DayLength)*TempScaling(index, Temp)*T_Step;
			return min(1., max(0., Sum[index] * GrainCommonStep[index]));
	}
	double operator ()(size_t index, double DayLength) {return DayLengthScaling(index, DayLength);	};
	double operator ()(size_t index, double Temp, double T_Step) {	return TempScaling(index, Temp); };
private:
	double DayLengthScaling(size_t index, double DayLength) {
		return (1. - exp(-1 * GrainStepDay[index] * max(0., DayLength / 60. - Grain_ThresDay[index])));
	};
	double TempScaling(size_t index, double Temp) {
		return (1. - exp(-1.*GrainStepTemp[index] * max(0., Temp - GrainThresTemp[index])));
	};
	vector<double> GrainStepDay, Grain_ThresDay, GrainStepTemp, GrainThresTemp, GrainCommonStep;
	vector<double> Sum;

};
class HarvestStart {
public: HarvestStart(size_t n) { HarvestSum.resize(n); }
	double operator() (size_t index) {
		HarvestSum[index] += 0.2;
		return HarvestSum[index];
}
private:
	vector<double>HarvestSum;

};


class GrowingSumF {
public: GrowingSumF(vector<double> ThresHold, double Latitude) : ThresHold(ThresHold), Latitude(Latitude) { Growing = false; 
		ContinStopSum = 0; ContinStartSum = 0.;  GSum.resize(ThresHold.size()); 	for (size_t i = 0; i < GSum.size(); i++) GSum[i] = 0.;
}
public: GrowingSumF(vector<double> ThresHold, vector<double> ThresHold_Dorming, double Latitude) : ThresHold(ThresHold), ThresHold_Dorming(ThresHold_Dorming), Latitude(Latitude) {
	Growing = false;
	ContinStopSum = 0; ContinStartSum = 0.;  GSum.resize(ThresHold.size()); 	for (size_t i = 0; i < GSum.size(); i++) GSum[i] = 0.;
}

		double operator() (size_t index, double TAir, double DayNum, double T_Step) {
			GrowSumUpdate(index, TAir, DayNum, T_Step);

			return GSum[index];

}
		double operator() (size_t index, double TAir, double DayNum,   double T_Step, double *pDormSum, size_t *pSpringDev) {
			GrowSumUpdate(index, TAir, DayNum, T_Step);
			if (TAir < -5.)
				ContinStopSum = ContinStopSum + T_Step;
			else
				ContinStopSum = 0;
			if (TAir < ThresHold_Dorming[index] && (DayNum > 180 && Latitude > 0) || (DayNum < 180 && Latitude < 0.) )
				*pDormSum += max(0., ThresHold_Dorming[index] - TAir)*T_Step;

			if (ContinStopSum > 3) Growing = false;
			if (GSum[index] > 20. && (DayNum < 180.&&Latitude>0) || (DayNum > 360 && Latitude < 0))
				*pSpringDev = true;
			else
				*pSpringDev = false;

			return GSum[index];
		}

private:
	void GrowSumUpdate(size_t index, double TAir, double DayNum, double T_Step) {
		if ((DayNum == 1 && Latitude > 0) || (DayNum == 180 && Latitude <= 0.)) {
			GSum[index] = max(0., TAir - ThresHold[index]);
			Growing = false;
		}

		if (!Growing) {
			if (TAir - ThresHold[index] > 0)
				ContinStartSum = ContinStartSum + T_Step;
			else
				ContinStartSum = 0;
			if (ContinStartSum > 3) Growing = true;
		}

		if (Growing) {
			GSum[index] += max(0., TAir - ThresHold[index])*T_Step;
		}
	}

	vector<double> ThresHold, ThresHold_Dorming;
	double Latitude;
	vector<double> GSum;
	bool Growing;
	double ContinStartSum, ContinStopSum;

};

class ZadokFunctionFromGSI {
public: ZadokFunctionFromGSI(double ZadokRangeGSI2) :Zadok_Delta_GSI2(ZadokRangeGSI2) {
	Zadok_GSI1 = 10; Zadok_GSI2 = Zadok_Delta_GSI2 + Zadok_GSI1; Zadok_GSI3 = 90.; Zadok_GSI4 = 100.; Zadok_Delta_GSI3 = Zadok_GSI3 - Zadok_GSI2;
	Zadok_Delta_GSI4 = Zadok_GSI4 - Zadok_GSI3;
};
	double operator() (double GSI) {
			if (GSI < 0. || GSI >= 4) return 100.;
			else if (GSI < 1.) return GSI * Zadok_GSI1;
			else if (GSI < 2.)  return Zadok_GSI1 + (GSI - 1.)*Zadok_Delta_GSI2;
			else if (GSI < 3.) return Zadok_GSI2 + (GSI - 2.)*Zadok_Delta_GSI3;
			else return Zadok_GSI3 + (GSI - 3.)*Zadok_Delta_GSI4;
	};
private:
	double Zadok_Delta_GSI2, Zadok_Delta_GSI3, Zadok_Delta_GSI4, Zadok_GSI1, Zadok_GSI2, Zadok_GSI3, Zadok_GSI4;
};








