#pragma once
#include "OutSingle.h"	
class Xs :public OutSingle {

public:
	Xs(double *target, enum datatype data, enum elements elem, enum fysprocess fproc, enum bioprocess bproc, enum userlevel ulev);

	~Xs();

	void Def(size_t module_no, string varname, double initv, string unit, string conditions);  

protected:


};