#pragma once
#include "../../../std.h"
#include "./../../Basic_Functors.h"
#include "./BasicGas_Functors.h"

#include "./NOxide_Functors.h"
#include "./Methane_Functors.h"

class FromAnaerobicToAerobicDiffusion {
public:
	FromAnaerobicToAerobicDiffusion(double DiffRedFrac, double DiffRedFracBase) :a(DiffRedFrac), b(DiffRedFracBase) {}
	double operator () (double anaerobic_frac) {
		return anaerobic_frac * max(0., (1 - anaerobic_frac)) * a + b;
}
private:
	double a, b;
};
class DenitrifiersAnaerobicMoistureResponse {
public:
	DenitrifiersAnaerobicMoistureResponse(double Denit_Act_PowerCoef): powercoef(Denit_Act_PowerCoef)  {}
	double operator() (double anaerobicfrac) {
		return pow(anaerobicfrac, powercoef);

}
private:
	double powercoef;
};



