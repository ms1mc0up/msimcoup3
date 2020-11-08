
#include "../std.h"
#include "../Util/FUtil.h"
#include "./History.h"
#include <time.h>
//#include "../CoupModel.h"





History::History() {

}
bool History::History_Add(SimB *pBase,int Lindex, time_t t, float v, string User) {
	auto itype=pBase->GetType();
	itype=p_ModelInfo->GetTypeIndex(pBase->GetTypeName());
	if(pBase->GetType()==PAR_TABLE)
		itype=2;


	return History_Add(pBase,int(pBase->GetType()), Lindex, t, v, User);

}
bool History::History_Add(SimB *pBase,int Lindex, time_t t, string str, string User) {
	auto itype=pBase->GetType();
	if (itype == DB) {

		itype=1;
	}
	else 
		itype=p_ModelInfo->GetTypeIndex(pBase->GetTypeName());
	if(pBase->GetType()==PAR_TABLE)
		itype=2;
	return History_Add(pBase,int(itype), Lindex, t, str, User);

}

bool History::History_Add(SimB *pBase,int iType, int Lindex, time_t t, float v, string User)
{

#ifndef COUPSTD
	//if(p_ModelInfo->IsRunning()) return false;
#endif
	
	string strApp=m_AppString;
	HISTv Node;

	Node.pBase=pBase;
	Node.LocalIndex=Lindex;
	Node.RunInfoType=iType;

	size_t i=m_History_Array.size();
	if(Lindex<0) Lindex=0;
	pBase->AddHistoryIndex(Lindex,i);



	Node.tvalue=t;
	Node.value=v;
	Node.strvalue=FUtil::STD_FtoAscii(v);
	if(User=="")
		User=FUtil::GetProfileStringStd("Signature", User);
	string Comp=FUtil::GetProfileStringStd("ComputerName", User);

	Node.User=User;
	Node.RunNo=m_DocFile.m_SimulationRunNo;
	Node.Computer=FUtil::GetProfileStringStd("ComputerName", User);
	m_History_Array.push_back(Node);
	return true;

}
bool History::History_Add(SimB *pBase, int iType, int Lindex, time_t t, string str, string User)
{
	HISTv Node;

#ifndef COUPSTD
	//if(p_ModelInfo->IsRunning()) return false;
#endif
	Node.pBase=pBase;
	Node.LocalIndex=Lindex;
	Node.RunInfoType=iType;
	size_t i=m_History_Array.size();
	if(Lindex<0) Lindex=0;
	pBase->AddHistoryIndex(Lindex,i);
	Node.tvalue=t;
	Node.value=float(MISSING);
	Node.strvalue=str;
	Node.RunNo=m_DocFile.m_SimulationRunNo;
	if(User=="")
		User=FUtil::GetProfileStringStd("Signature", User);
	Node.User=User;
	Node.Computer=FUtil::GetProfileStringStd("ComputerName", User);
	m_History_Array.push_back(Node);
	return true;

}

size_t History::History_GetNumber()
{
	return m_History_Array.size();
}
bool History::History_RemoveAll()
{
	//while(!m_History_Array.IsEmpty()) {
		//	CHistoryNode *pNode=(CHistoryNode*)m_History_Array.RemoveHead();
			//delete pNode;
	//}
	m_History_Array.clear();
	return true;
}
bool History::History_Remove(size_t index)
{	
	if(index<m_History_Array.size())
		m_History_Array.erase(m_History_Array.begin()+index);
	else
		return false;


	return true;
}

SimB* History::History_GetPointer(size_t index)
{
	if(index<m_History_Array.size())
		return m_History_Array[index].pBase;
	else 
		return nullptr;
}

time_t History::History_GetTime(size_t index)
{
	if(index<m_History_Array.size())
		return m_History_Array[index].tvalue;
	else 
		return 0;
}
size_t History::History_GetRunInfoType(size_t index)
{
	if(index<m_History_Array.size())
		return m_History_Array[index].RunInfoType;
	else 
		return 0;
}
string History::History_GetUser(size_t index)
{
	if(index<m_History_Array.size())
		return m_History_Array[index].User;
	else 
		return "";
}
size_t History::History_GetRunNo(size_t index)
{
	if(index<m_History_Array.size())
		return m_History_Array[index].RunNo;
	else 
		return 0;
}
string History::History_GetComputer(size_t index)
{
	if(index<m_History_Array.size())
		return m_History_Array[index].Computer;
	else 
		return "";
}
size_t History::History_GetLocalIndex(size_t index)
{
	if(index<m_History_Array.size())
		return m_History_Array[index].LocalIndex;
	else 
		return 0;

}
string History::History_GetString(size_t index)
{
	if(index<m_History_Array.size())
		return m_History_Array[index].strvalue;
	else 
		return 0;
}
float History::History_GetFloat(size_t index)
{
	if(index<m_History_Array.size())
		return m_History_Array[index].value;
	else 
		return 0.;

}
bool History::History_Add(size_t index, string strvalue)
{
 
	size_t i=m_History_Array.size();
	SimB *pBase;
	pBase=(SimB*)m_RunInfo[index].pBase;
	if(pBase!=nullptr)
		pBase->AddHistoryIndex(i);
	HISTv Node;
	if(i>0)
		Node= m_History_Array[i-1];


 	string User,Computer;

	User=FUtil::GetProfileStringStd("Signature", User);
	Computer=FUtil::GetProfileStringStd("ComputerName", Computer);
	
	Node.tvalue=time(nullptr);
	Node.LocalIndex=index;
	Node.User=User;
	Node.Computer=Computer;
	auto ityp=p_ModelInfo->GetTypeIndex("RunInfo");
	Node.RunInfoType=ityp;
	Node.pBase=pBase;
	Node.strvalue=strvalue;

	m_History_Array.push_back(Node);
		  
	return true;
}
void History::History_Add(string Group, string Type, string Name,string StringValue, int intValue, time_t tt, string User, string Computer)
{
   
	SimB *pBase;
	float fvalue;
	size_t RunNo;
	HISTv Node, NodePrev;
	if(intValue<0) intValue=0;
	size_t ityp;
	RunNo=m_DocFile.m_SimulationRunNo;
	if(m_History_Array.size()>0) {
		auto end=m_History_Array.size()-1;
		NodePrev=m_History_Array[end];
		RunNo=NodePrev.RunNo;
	}
	size_t i=m_History_Array.size();
	if(Type=="Run Info") {
		pBase=(SimB*)m_RunInfo[intValue].pBase;
		pBase->AddHistoryIndex(i);
		ityp=p_ModelInfo->GetTypeIndex("RunInfo");
		if(intValue==13) 
			RunNo=FUtil::AtoInt(StringValue);
	}
	else if (Type=="Data Base Object") {
		pBase=(SimB*)m_DB_Array[intValue];
		pBase->AddHistoryIndex(intValue,i);
		ityp=p_ModelInfo->GetTypeIndex("DataBase");
	}
	else {
		if(Name=="Validation File") Name="Validation File1";
		pBase=(SimB*)GetPtr(Type, Group, Name);
		if(pBase!=nullptr) 
			pBase->AddHistoryIndex(intValue,i);
		ityp=p_ModelInfo->GetTypeIndex(Type);
	}

	
	Node.RunInfoType=ityp;
	Node.pBase=pBase;
	Node.LocalIndex=intValue;
	Node.tvalue=tt;
	if(pBase==nullptr) 
		fvalue=0;
	else if(StringValue.size()>0)
		fvalue=FUtil::AtoFloat(StringValue);
	else
		fvalue=0.;
	Node.value=fvalue;
	Node.strvalue=StringValue;
	Node.User=User;
	Node.Computer=Computer;
	Node.RunNo=RunNo;
	m_History_Array.push_back(Node);   
}	  
void History::History_NewAdd(size_t RunInfo, string Group, string Type, string Name,string StringValue, int intValue, time_t tt, string User, string Computer)
{

	SimB *pBase;
	float fvalue;
	size_t RunNo;
	HISTv Node, NodePrev;
	if(intValue<0) intValue=0;
	size_t ityp=0;
	RunNo=m_DocFile.m_SimulationRunNo;
	pBase=nullptr;
	if(m_History_Array.size()>0) {

		NodePrev=m_History_Array[m_History_Array.size()-1];
		RunNo=NodePrev.RunNo;
	}
	size_t i=m_History_Array.size();
	if(Type=="RunInfo"||Type=="Run Info") {
		if(intValue<14)	{
			pBase=(SimB*)m_RunInfo[intValue].pBase;
			pBase->AddHistoryIndex(i);
		}
		else
			return;
		ityp=p_ModelInfo->GetTypeIndex("RunInfo");
		if(intValue==13) 
			RunNo=FUtil::AtoInt(StringValue);
	}
	else if (Type=="Data Base Object") {
		if(intValue<10)	
		pBase=(SimB*)m_DB_Array[intValue];
		pBase->AddHistoryIndex(intValue,i);
		ityp=p_ModelInfo->GetTypeIndex("DataBase");
	}
	else if (Type == "Validation") {
		pBase = (SimB*)GetPtr(PAR_TABLE, Name);
		if (pBase != nullptr) {
			pBase->AddHistoryIndex(intValue, i);
			Type = "Table Parameters";
			ityp = p_ModelInfo->GetTypeIndex(Type);
			if (ityp == 0) {
				ityp = p_ModelInfo->GetTypeIndex(Type);
			}
			if (pBase->GetNumOfHistoryIndex(intValue) == 1) {
				// Create vector of Parameters in the HistoryPar_Array
				HistP element;
				element.LocalIndex = intValue;
				element.pBase = pBase;
				element.type = ityp;
				m_HistoryPar_Array.push_back(element);
			}



		}


	}
	else {
		if(Name=="Validation File") Name="Validation File1";
		pBase=(SimB*)GetPtr(Type, Group, Name);
		if(pBase!=nullptr)	  {
			pBase->AddHistoryIndex(intValue,i);
			ityp=p_ModelInfo->GetTypeIndex(Type);
			if(ityp==0) {
				ityp=p_ModelInfo->GetTypeIndex(Type);
			}		
			if(pBase->GetNumOfHistoryIndex(intValue)==1) {
			// Create vector of Parameters in the HistoryPar_Array
				HistP element;
				element.LocalIndex=intValue;
				element.pBase=pBase;
				element.type=ityp;
				m_HistoryPar_Array.push_back(element);
			}
		}
		else {
		   int i=0;
			ityp=0;
			return;
		}
	}

	
	Node.RunInfoType=ityp;
	Node.pBase=pBase;
	Node.LocalIndex=intValue;
	Node.tvalue=tt;
	if(pBase==nullptr) 
		fvalue=0;
	else if(StringValue.size()>0)
		fvalue=FUtil::AtoFloat(StringValue);
	else
		fvalue=0.;
	Node.value=fvalue;
	Node.strvalue=StringValue;
	Node.User=User;
	Node.Computer=Computer;
	Node.RunNo=RunNo;
	m_History_Array.push_back(Node);  
}
