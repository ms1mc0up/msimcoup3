 #include "Ts.h"

Ts::~Ts()
{
}
Ts::Ts(double *target,enum datatype data, enum elements elem, enum fysprocess fproc, enum bioprocess bproc, enum userlevel ulev)
	:OutSingle(data, elem, fproc, bproc, ulev)
{
	_ptarget=target; 
	SetSimType(FLOW_SINGLE);
	;
}
void Ts::Def(size_t ModuleNo, string name, double init, string unit, string conditions)
{

	SetValue(init);
	SetStoreFlag(false);
	SetModuleNo(ModuleNo);
	SetUnit(unit);
	SetConditions(conditions);

	SetName(name);



	
//	for (size_t i=0;i<numFlags;i++)
//	{
		//tmpflag[i]=*(flag+i);
		//m_AnnimateVector[i].ichart=-1;
		//m_AnnimateVector[i].sequence=0;

		//pNode.OptInitial=float(MISSING);
		_out.OptInitial=float(MISSING);
		_out.Initial=float(MISSING);
		_out.Final=float(MISSING);
		_out.Min=float(MISSING);
		_out.Max=float(MISSING);
		_out.Mean=float(MISSING);
		_out.Accumulated=float(MISSING);
		_out.ValidFlagIndex=-1;
		_out.PgFileIndex=-1;
		_out.StoreFlag=0;
//		pNode.ValidFlagIndex=-1;
//		pNode.PgFileIndex=-1;
//		SumList.push_back(pNode);
//	}


}
