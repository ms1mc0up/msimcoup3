/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*=-=-=-=-=-                  FILE: NewMAP.H                   -=-=-=-=-=*/
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#pragma once

#include "./MR.h"
#ifndef COUPSTD
#include "../Chart/PlotDesignInfo/PlotStorage.h"
#endif



class NewMap :public MR{

public:
	bool MakeMapComplete();
	bool CheckStart_EndTime();
	void UpdateArchiveLists(size_t icat=8);
	void RemoveOriginalValues(string typ, string GroupSet, bool original=true, size_t ViewCategory=0);
	void SetOriginalValues(string typ, string GroupSet="") {RemoveOriginalValues(typ, GroupSet,true);};
	void CleanAllLists();
	bool Sort_TableList_Arrays(bool) {return true;};
	NewMap();						// standard constructor
	~NewMap();						// standard constructor
			// updates all variable interactions
	void SetPathName(string filename) {m_DocFileName=filename; m_CurrentFile=filename;};
	bool ReadDocFile(string filename="");
	bool WriteDocFile();
	void Init_BlankDocument();
	bool CreateMapSvFlexFile(size_t index=0);

	bool Info_Header(bool ReadFlag=true);
	bool Info_Switches(bool ReadFlag=true);
	bool Info_Parameters(bool ReadFlag=true);
	bool Info_ParTables(bool ReadFlag=true);
	bool Info_Files(bool ReadFlag=true);
	bool Info_Outputs(bool ReadFlag=true);
	bool Info_Validation(bool ReadFlag=true);
	bool Info_SumValidation(bool ReadFlag=true);
	bool Info_RunReport(bool ReadFlag=true);
	bool Info_MultiRun(bool ReadFlag=true);
	bool Info_WithinRunOptions(bool ReadFlag=true);
	bool Info_FinalStateVariables(bool ReadFlag=true);
	bool Info_Plottings(bool ReadFlag=true);
	bool Info_History(bool ReadFlag=true);
	bool Info_DataBase(bool ReadFlag=true);


	ValidationData* GetValidDataPtr() {return &m_ValidationData;};

	bool Info_Annim(bool ReadFlag=true);


#ifndef COUPSTD
	CPlotStorage m_PlotStorage;
#endif



protected:

	vector<SIMB> UpdateOutVector(vector<SimB*> vp,string GroupSet, bool original, int ViewCategory=0);
	bool m_continueRead;
	size_t m_iError;
	vector<SIMB> SortSimB_ByGroup(vector<SIMB> v);
	vector<SIMB> SortSimB_ByCategory(vector<SIMB> v);



private:

};



