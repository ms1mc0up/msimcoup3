#pragma once
#include "OutVector.h"	
class T :public OutVector {

public:
	T(vector<double> *target, NE *p, enum datatype data, enum elements elem, enum fysprocess fproc, enum bioprocess bproc, enum userlevel ulev);

	~T();

	void Def(size_t module_no, string varname, vector<double> initv, string unit, string conditions);  

protected:


};