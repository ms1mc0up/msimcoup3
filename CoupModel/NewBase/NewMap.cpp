
#include "../std.h"

#include "../Util/FUtil.h"
#define SECS_TO_FT_MULT 10000000
#define TIMBASE_SHIFT 11644473600

//#include "../External.h"
#include "./NewMap.h"
#include "../ModelTypes/NewModelType.h"
#include "../SoilData/GetSoilStorage.h"

#ifndef COUPSTD

#include "../MainFrm.h"
#include "../SP/SP_Retrieve.h"

#endif

//#include <fstream.h> Do no know about this header file


extern "C" bool ReadResultsFromFile=true;

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*=-=-=-=-=-                  CLASS: NewMap                   -=-=-=-=-=*/
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
NewMap::NewMap() {

	m_DocFileName="Kalle.Sim";
	//PARAMETERS_mp_SUPERFLAG=0;

	m_Protect=false;
	m_SavedSum=false;
	m_SavedTimeSerie=false;
	//PARAMETERS_mp_PRESIMULATIONDAYS=0;
	m_BayesianStepSize=0.5f;
	m_NumOutputs=0;//PARAMETERS_mp_NUM_OUTPUTS;
	MR_UseBayesianCoVar=false;

	m_MultiStopped=false;
	m_CurrentSubDirectory="";

	DB_Names.clear();
	DB_Names.push_back("Plant");
	DB_Names.push_back("Soil Properties");
	DB_Names.push_back("Soil evaporation");
	DB_Names.push_back("Snow");
	DB_Names.push_back("Soil frost");
	DB_Names.push_back("Soil water flows");
	DB_Names.push_back("Plant Growth");
	DB_Names.push_back("Site Management");
	DB_Names.push_back("Soil Organic Processes");
	DB_Names.push_back("Soil Mineral N Processes");
	MR_AddExtraMonitor(4);

	m_pValidIndexSet=nullptr;
	m_ReReadMStorageFile=false;
	for(size_t i=0; i<MAXSIMVAL;i++)
		m_ValidationData.m_NumberSelectedValidationVar[i]=0;
	
	m_OutputBinFileExist=true;
	m_OutputValidBinFileExist=true;
	m_MultiStorageBinFileExist=true;
	m_OutputValidXBinFileExist=false;

	m_NoUpdateRunNo=false;
	m_MultiStoragelocked=true;
	m_LinkedDocNumber=1;
	m_MultiRunsOriginal=0;
	m_CountError=0;
	m_iError=0;
	for (size_t i = 0; i < m_ResidualFileVector.size(); i++) {
		for (size_t ii = 0; ii < m_ResidualFileVector[i].size(); ii++)
			m_ResidualFileVector[i][ii]->InitDoc(this, i + 1);
	}
	m_ErrorCount=0;

	ReadResultsFromFile=true;

}

//Destructor for NewMap
//The deletion of the dynamic objects created is taken care of here.
NewMap::~NewMap() 
{
  //Call to functions for deletion of the dynamic pointers in the pointermaps

	string str=m_DocFile2.m_OriginFileName;



}

// Function that processes the parameters lists that comes from the 
// FORTRAN part and places pointers to the objects in these lists into the structure 
// wmap. 
//
// OBSERVE that the lists from the FORTRAN part are completely 
// destroyed by this routine and if they are to be reused they 
// must be restored.


// This function calls BuildVariable for each map in wmap.
// The BuildVariable function in SimB for E_map is special and 
// not the same as it is for the other maps.
//
// One important question is what the variable xtret does.
// It seems to me as if it does nothing at all; it isn"t used 
// anywhere except outside this function.





bool NewMap::ReadDocFile(string file)
{

	#ifndef COUPSTD
		CMainFrame *p_mainframe;
		p_mainframe = (CMainFrame*) AfxGetMainWnd();
		string txt;
		txt="Reading Document File:";
		txt+=file;
		p_mainframe->UpdatePaneText(0,txt);
	#endif
	if(file.size()>0) {
	//	PARAMETERS_mp_PARTY_OVER();
		SetPathName(file);

	}

	long long totlength=0;
	bool good=false;
	if(FUtil::IsFileExisting(file)) {
		ifstream DocFileIn;
		//DocFileIn.open(file, ios::in|ios::binary);
		m_DocFileIn.open(file, ios::in|ios::binary);
		m_DocFileIn.seekg(0,ios::beg);
		good=m_DocFileIn.good();
	} 
	if(m_DocFileIn.is_open()) {
		m_DocFileIn.seekg(0,ios::end);
		totlength=m_DocFileIn.tellg();
		Init_BlankDocument();
		good=m_DocFileIn.good();
		if(totlength==0&&!good) {
			m_DocFileIn.close();
			return true;

		}
		else if(totlength==0) {
			m_DocFileIn.close();
			return true;

		};
	}


	if(m_DocFileIn.is_open()&&totlength>0) {



		long long actpos{ 0 };
		m_DocFileIn.seekg(0,ios::beg);
		good=m_DocFileIn.good();
		string problem;

		size_t ReadSectionCount=0;
		while(good&&ReadSectionCount<16) {

			switch(ReadSectionCount) {
			case 0:
				problem="Header";
				good=Info_Header(true);
				break;
			case 1:
				problem="Switches";
				good=Info_Switches(true);break;
			case 2:
				problem="Parameters";
				good=Info_Parameters(true);	break;
			case 3:
				problem="Parameter Tables";
				good=Info_ParTables(true); 
				if(!good&&m_continueRead) {
				//	MapVariables(); 
					m_DocFileIn.seekg(0,ios::beg);	
					good=m_DocFileIn.good();
					ReadSectionCount=-1;
				}
				break;
			case 4:
				problem="File Names";
				good=Info_Files(true);	break;
			case 5:
				problem="Outputs";
				good=Info_Outputs(true); break;
			case 6:
				problem="Validation Variables";
				good=Info_Validation(true);
				break;
			case 7:
				problem="Summary Validation";
				good=Info_SumValidation(true); break;
			case 8:
				problem="Run Reports";
				good=Info_RunReport(true); break;
			case 9:
				problem="Multi Run options";
				good=Info_MultiRun(true); break;
			case 10:
				problem="Within Run options";
				good=Info_WithinRunOptions(true); break;
			case 11:
				problem="Data base information";
				good=Info_DataBase(true); break;
			case 12:
				problem="Final State Variables";
				good=Info_FinalStateVariables(true); break;
			case 13:
				problem="Plottings";
				good=Info_Plottings(true); break;
			case 14:
				problem="History (log)";
				good=Info_History(true); break;
			case 15:
				problem="Annimation specification";
				good=Info_Annim(true); break;
			}
			if(good) {
				ReadSectionCount++;
				if(ReadSectionCount<16)
				actpos=m_DocFileIn.tellg();
				if(actpos<0) good=false; 
			}
		}

		if(m_Report_Array.size()>0) {
			m_HasBeenRunning=true;
			m_Protect=true;
			if(m_FileVersionNumberRead<36) m_DocFile.m_FinishedSimulation=true;
		}
		if(m_DocFileIn.good()) {
			m_DocFileIn.close();
		}
		else {
			m_DocFileIn.close();
		}
		auto iv=m_Val_Array.size();
		Tab *pTab=(Tab*)GetPtr(TABLE, "Scaling of Observed Variables");
		if(pTab!=nullptr) {
				auto oldnelements=pTab->GetNumOfRow();
		  		if (iv!=oldnelements&&iv!=string::npos)  pTab->SetNumOfRow(iv);
		}


		if(ReadSectionCount>14) {
			bool test = false;
			for (size_t i = 0; i < m_OptionsForReading.size(); i++) {
				test=LinkedChangeToDB_Parameters(m_OptionsForReading[i].pSimB, m_OptionsForReading[i].options);
			}
			if (test) m_OptionsForReading.clear();
			

// Completly read file
#ifdef COUPSTD
	cout<<" Reading of file succesful "; cout<<m_DocFileName; cout<<endl;

#endif


	#ifndef COUPSTD
			p_mainframe->UpdatePaneText(0,"Reading of file succesfully completed");
			
			m_PlotStorage.Update(m_DocFile.m_SimulationRunNo, m_FileVersionNumberRead);
	#endif
			return true;
		}

// Partly read file
		string mes;
		char buf[50];
//        printf(&buf[0],30,"%u", ReadSectionCount);
        mes="Only ";
        mes.append(buf);
		mes.append(" of 15 section read");
        mes.append("Error when reading:");
        mes.append(problem);
#ifndef COUPSTD
		MFC_Util::MessageBox( mes,"Input Error",	MB_OK|MB_ICONEXCLAMATION);
	


#endif
#ifdef COUPSTD
//    cout<<" Reading of file "; cout<<m_DocFileName; cout<<mes; cout<<endl;

#endif

			Init_BlankDocument();
			return true;

		
		
	}
	else {
// No file existing

			// Select Temperature as standard output for new blank file
			Init_BlankDocument();

#ifndef COUPSTD
			m_PlotStorage.Update(m_DocFile.m_SimulationRunNo, m_FileVersionNumberRead);
#endif

#ifdef COUPSTD
	cout<<" Reading of file failure, file was not find ("; cout<<m_DocFileName; cout<<")"; cout<<endl;

#endif

			return false;
	}

return true;

}

void NewMap::Init_BlankDocument() 
{
		//CreateSubMaps();
		string group, name, type, test;
		type="Auxiliary Variables";
		name="Temperature";
		group="Soil heat flows";
		multimap<string, SimB*>::iterator it;
		size_t itype, inum;
		

		SimB *pSimB;
		auto in=GlobalMap.count(name);

		if(GlobalMap.count(name)>0) {
			for (it=GlobalMap.equal_range(name).first; it!=GlobalMap.equal_range(name).second; ++it) {
				 pSimB=(*it).second;
				 itype=pSimB->GetType();
				 test=pSimB->GetName();
			}
		}

		//map<string, void*>::iterator it2;
		G *pGD;
		pGD=dynamic_cast<G*>(GetPtr(AUX,name, group));
		pGD=dynamic_cast<G*>(GetPtr(AUX,name));
		OutVector* pStore;
		OutSingle* pSingle;

		pStore=dynamic_cast<OutVector*>(GetPtr(AUX,"Temperature"));
		pSingle=dynamic_cast<OutSingle*>(GetPtr(AUX_SINGLE,"TempSoilSurf"));
		if(pGD!=nullptr) {
				pGD->SetValue(2.);
				inum=pGD->GetNumVector();
				pStore->SetStoreFlag(7,true);
				pStore->SetStoreFlag(5,true);
				pStore->SetStoreFlag(2,true);
				pStore->SetStoreFlag(0,true);
				pSingle->SetStoreFlag(true);

				size_t num=pGD->GetNumberOfSelectedFlags();

				

		}
		RemoveOriginalValues("Outputs","ALL",false);
		//RemoveOriginalValues("Outputs","ALL",true);
	
		m_pValidIndexSet=(Sw*)GetPtr("Switches", "Technical", "IndexForValidation");
		if (m_pValidIndexSet == nullptr) {
			m_pValidIndexSet = (Sw*)GetPtrByName("IndexForValidation");
		}


		Sw *pSw;
		pSw=(Sw*)GetPtr("Switches", "Technical", "ValidationMode");
		if (pSw != nullptr) {
			m_NumValidFiles = ((Sw*)GetPtr("Switches", "Technical", "ValidationMode"))->GetIntValue();
			m_pValidIndex = (Ps*)GetPtr("Parameters", "Technical", "Index in val file");
			m_pValidIndexRep = (Ps*)GetPtr("Parameters", "Technical", "No of internal index loop");
			m_pValidIndexLoopSize = (Ps*)GetPtr("Parameters", "Technical", "Size of internal loop");


		}

		m_ValidationData.Init(this);
		m_MStorage.Init(this);
		//

}
bool NewMap::WriteDocFile()
{

	m_DocFileOut.open(m_DocFileName,ios_base::binary|ios_base::out);
	if(m_DocFileOut.is_open()) {

	#ifndef COUPSTD
		CMainFrame *p_mainframe;
		p_mainframe = (CMainFrame*) AfxGetMainWnd();
		p_mainframe->UpdatePaneText(0, "Writing to Documument file Started");
	#endif
		bool good=false;
			
		long long actpos=m_DocFileOut.tellp();
		good=Info_Header(false);
		actpos=m_DocFileOut.tellp();
		good=Info_Switches(false);
		actpos=m_DocFileOut.tellp();
		good=Info_Parameters(false);
		actpos=m_DocFileOut.tellp();
		good=Info_ParTables(false);
		actpos=m_DocFileOut.tellp();
	
		good=Info_Files(false);
		actpos=m_DocFileOut.tellp();
		good=Info_Outputs(false);
		actpos=m_DocFileOut.tellp();
		good=Info_Validation(false);
		good=Info_SumValidation(false);
		good=Info_RunReport(false);
		
		good=Info_MultiRun(false);
		good=Info_WithinRunOptions(false);
		good=Info_DataBase(false);
		
		good=Info_FinalStateVariables(false);
		good=Info_Plottings(false);
		good=Info_History(false);
		good=Info_Annim(false);


		m_DocFileOut.close();

#ifdef COUPSTD
		cout<<"File writing completed "<<m_DocFileName<<endl;

#endif
#ifndef COUPSTD
		p_mainframe->UpdatePaneText(0, "Writing to Documument file Completed");
#endif

		m_CurrentFile=m_DocFileName;
		return true;
	}
	return false;


}
bool NewMap::Info_Header(bool reading)
{
	size_t iv;
	ifstream::pos_type size;
	string str;
	char cbuf[80];
//  Get date of exe-file
	long long pos;


	Sw* pSw;

	m_OptionsForReading.clear();
	if(reading) {
		pSw=nullptr;
		m_DocFileIn.seekg (0, ios::beg);
		rSize_t(&m_FileVersionNumberRead);
		if(m_FileVersionNumberRead>25)
			rString(&m_DocFile2.m_OriginFileName);
		rString(&m_DocFile2.m_ExeFileDate);
		size_t ipos=m_DocFile2.m_ExeFileDate.find("1970");
		if(ipos==25)
			m_DocFile2.m_ExeFileDate=m_DocFile2.m_ExeFileDate.substr(0, 11);
		if(m_FileVersionNumberRead>11)
			rString(&m_DocFile2.m_DateRun);
		if(m_FileVersionNumberRead>35) {
			if(m_FileVersionNumberRead<50) {
				rBOOL(&m_DocFile.m_MultiRunning);
				rBOOL(&m_DocFile.m_FinishedSimulation);

			}
			else {
				rbool(&m_DocFile.m_MultiRunning);
				rbool(&m_DocFile.m_FinishedSimulation);
			}
		
			rString(&m_DocFile2.m_Multi_MBin_File);
		}
			rString(&str);
			if(str=="Run Options") {
				rInt(&RunOpt.oiminutes);
				rInt(&RunOpt.oidays);
				rUint(&RunOpt.longMinStart);
				rUint(&RunOpt.longMinEnd);
				rInt(&RunOpt.noofiter);
				rSize_t(&m_DocFile.m_SimulationRunNo);
				if(m_FileVersionNumberRead>17)
					rSize_t(&m_DocFile.m_TimePeriodScaleFactor);
				rString(&m_DocFile2.m_Comments);
				rString(&m_DocFile2.m_RunId);
				if(m_FileVersionNumberRead>=7) {
					rSize_t(&iv);
					pSw=dynamic_cast<Sw*>(GetPtr(SWITCH,"TimeResolution"));
					if(pSw!=nullptr) {
						if(iv<0||iv>3) iv=0;
						pSw->SetIntValue(iv);
					}
				}
				if(m_FileVersionNumberRead>=29) {
					rSize_t(&m_DocFile.ipredays);
					RunOpt.longMinPreStart=RunOpt.longMinStart-m_DocFile.ipredays*1440;
	//				PARAMETERS_mp_PRESIMULATIONDAYS=m_DocFile.ipredays;
					if (m_FileVersionNumberRead >= 68) 	rSize_t(&m_DocFile.ipostdays);
					if (m_DocFile.ipostdays > 1000)  m_DocFile.ipostdays = 0;
					RunOpt.longMinPostEnd = RunOpt.longMinEnd + m_DocFile.ipostdays * 1440;
				}

				pos=m_DocFileIn.tellg();
				m_DocFileIn.read((char*)cbuf,40);
				m_DocFileIn.seekg(pos,ios::beg);

				if(m_FileVersionNumberRead>36) {
					rTime(&m_DocFile.m_TimeCreated);
					rTime(&m_DocFile.m_TimeModified);
					
				}
				size_t apos=0;
				while(!(cbuf[apos]=='S'&&cbuf[apos+1]=='w')&&apos<38) 
					apos++;		
				if(apos==33) m_ShortTimeRead=false;
				if(m_FileVersionNumberRead<36) {
					apos=0;
				}
				else if(m_FileVersionNumberRead==46||m_FileVersionNumberRead<=40||m_FileVersionNumberRead==45)
					apos-=5;
				else if(m_FileVersionNumberRead<50)
					apos-=9;
				else
					apos-=6;
				pos+=apos;
				m_DocFileIn.seekg(pos,ios::beg);
				if(m_FileVersionNumberRead>46) {
					if(m_FileVersionNumberRead<50)
						rBOOL(&m_DocFile.m_MultiStoragelocked);
					else
						rbool(&m_DocFile.m_MultiStoragelocked);
					if(!m_DocFile.m_MultiStoragelocked)
						UnLockMultiStorageFile();
				}
			}
			SetNextChaParDate(RunOpt.longMinStart);
	}
	else {
		wSize_t(&m_DocFile.m_FileVersionNumber);

		wString(&m_DocFile2.m_OriginFileName);
		if(m_Protect) {
			wString(&m_DocFile2.m_ExeFileDate);
		}
		else { //kk
			string ExeDate; 
			ExeDate="Version 6.1.2, ";
#ifndef COUPSTD
			CWinApp *pApp;
			pApp=AfxGetApp();
			CFileStatus status;
			string str=CT2A(pApp->m_pszHelpFilePath).m_psz;
			size_t ipos= str.find(".CHM");
			if(ipos<0) ipos=str.find(".chm");
			if(ipos>0) {
				str=str.substr(0,ipos);
				str+=".exe";
				CFile::GetStatus(CString(str.c_str()), status);
				CTime kk=status.m_mtime;
				ExeDate+=CT2A(kk.Format(_T("%d %B, %Y"))).m_psz;
			}
#endif
            string exdate = string(ExeDate);
            
			wString(&exdate );
		}
		wString(&m_DocFile2.m_DateRun);
		wbool(&m_DocFile.m_MultiRunning);
		wbool(&m_DocFile.m_FinishedSimulation);
		wString(&m_DocFile2.m_Multi_MBin_File);
		string str="Run Options";
		wString(&str);
		wInt(&RunOpt.oiminutes);
		wInt(&RunOpt.oidays);
		wUint(&RunOpt.longMinStart);
		wUint(&RunOpt.longMinEnd);
		wInt(&RunOpt.noofiter);
		wSize_t(&m_DocFile.m_SimulationRunNo);
		wSize_t(&m_DocFile.m_TimePeriodScaleFactor);
		wString(&m_DocFile2.m_Comments);	
		wString(&m_DocFile2.m_RunId);
		pSw=dynamic_cast<Sw*>(GetPtr(SWITCH,"TimeResolution"));
		iv=pSw->GetIntValue();
		wSize_t(&iv);
		wSize_t(&m_DocFile.ipredays);	 
		if (RunOpt.longMinPostEnd > RunOpt.longMinEnd)
			m_DocFile.ipostdays = (RunOpt.longMinPostEnd - RunOpt.longMinEnd) / 1440;
		else
			m_DocFile.ipostdays = 0;
		wSize_t(&m_DocFile.ipostdays);
		wTime(&m_DocFile.m_TimeCreated);
		wTime(&m_DocFile.m_TimeModified);
		wbool(&m_DocFile.m_MultiStoragelocked);


	} 
return true;
}
bool NewMap::Info_Switches(bool reading)
{ 
	Sw *pSw=nullptr;
	string type,group,name;
	size_t  value;
	size_t iv = 0;

	if(reading) {
		ResidualFile_Reset();
		m_Sw_Array.clear();
		rSize_t(&iv);
		rString(&type);
		if(type!="Switches")
			return false;
		else {
			for(size_t i=0; i<iv;i++) {
				rString(&group);
				if(group=="Driving variables") group="Meteorological Data";
				if(group=="Soil mineral nitrogen processes") group="Soil mineral N processes";
				rString(&name);
				rSize_t(&value);
				if(group=="Model Structure"&&name=="TimeResolution") group="Hidden";
 				if(group=="Technical"&&name=="TimeResolution") group="Hidden";
 				if(group=="Irrigation"&&name=="IndexForValidation") group="Technical";
				if(m_FileVersionNumberRead<=24&&group=="Potential Transpiration"&&name=="Aerodyn. Resistance") name="Aerodynamic Resistance";
				pSw=dynamic_cast<Sw*>(GetPtr(SWITCH,name));
				if(pSw!=nullptr) {
					size_t recalc=pSw->SetIntValue(value);
					if(recalc>0) LinkedChangeToSwitches(pSw, recalc);
					pSw->SetNotOldValue();
					//Sw->Apply();
					m_Sw_Array.push_back(pSw);
				} 
				else {
					string koll=name;


				}



			}
		}

		m_pValidIndexSet=dynamic_cast<Sw*>(GetPtr(SWITCH, "IndexForValidation"));
		if (m_pValidIndexSet != nullptr) {
			auto iv = m_pValidIndexSet->GetIntValue();
			m_pValidIndex = dynamic_cast<Ps*>(GetPtr(PAR_SINGLE, "Index in val file"));
			m_pValidIndexRep = dynamic_cast<Ps*>(GetPtr(PAR_SINGLE, "No of internal index loop"));
			m_pValidIndexLoopSize = dynamic_cast<Ps*>(GetPtr(PAR_SINGLE, "Size of internal loop"));

			pSw = dynamic_cast<Sw*>(GetPtr(SWITCH, "ValidationMode"));
			m_NumValidFiles = pSw->GetIntValue();
			ResidualFile_AddFile(m_NumValidFiles);
		}

	}
	else {
		RemoveOriginalValues("Switches","ALL",false);
		iv=m_Sw_Array.size();
		wSize_t(&iv);
		type="Switches";
		wString(&type);
        string group_psw;
        string name_psw;
        
		for(size_t i=0;i<iv;i++) {
			pSw=m_Sw_Array[i];
            group_psw = pSw->GetGroup();
            name_psw = pSw->GetName();
			wString(&group_psw);
			wString(&name_psw);
            
			auto ivalue=pSw->GetIntValue();
			wSize_t(&ivalue);
		}
	}

	

return true;
}
bool NewMap::Info_Parameters(bool reading)
{	
	Ps *pPs=nullptr;
	string type,group,name;
	size_t iv;
	float fvalue;
	if(reading) {
		m_P_Array.clear();
		rSize_t(&iv);
		rString(&type);
        cout << "Parameteres\n***********\n";
		if(type=="Parameters") {
			for(size_t i=0; i<iv;i++) {
				rString(&group);
				if(group=="Driving variables") group="Meteorological Data";
				if(group=="Soil mineral nitrogen processes") group="Soil mineral N processes";
		
				rString(&name);
				rFloat(&fvalue);
				if(name=="RateCoefHumus"&&m_FileVersionNumberRead<=21) {
					fvalue=fvalue*2;
				}
				pPs=dynamic_cast<Ps*>(GetPtr(PAR_SINGLE,name));
				if(pPs==nullptr) {
						if(name=="Intial Soil Storage") name="Initial Soil Storage";
						if(name=="Intial Peak Storage") name="Initial Peak Storage";
						if(name=="Intial Base Storage") name="Initial Base Storage";
						pPs=dynamic_cast<Ps*>(GetPtr(PAR_SINGLE,name));
				}

                cout << name << fvalue << "\n";



				if(pPs!=nullptr) {
					double orgvalue=pPs->GetOriginalValue();
					double reltest=(orgvalue-pPs->GetMinValue())/(pPs->GetMaxValue()-pPs->GetMinValue());
					double relnew=(fvalue-pPs->GetMinValue())/(pPs->GetMaxValue()-pPs->GetMinValue());
					double current=pPs->GetValue();
					double ratio=(fvalue+1.E-10)/(current+1.E-10);
					if(abs(reltest-relnew)>1.E-5||ratio>1.01||ratio<0.99) {
						size_t recalc;
						if(fvalue>pPs->GetMaxValue()) {
							recalc=pPs->SetValue(pPs->GetMaxValue());

						}
						else if(fvalue<pPs->GetMinValue()) {
							recalc=pPs->SetValue(pPs->GetMinValue());


						}
						else
						 recalc=pPs->SetValue(fvalue);
						if(recalc>0) {
							PAR_OPTIONS p;
							p.pSimB = pPs;
							p.options = recalc;
							m_OptionsForReading.push_back(p);
							//LinkedChangeToDB_Parameters(pPs, recalc);




						}
						pPs->SetNotOldValue();
						m_P_Array.push_back(pPs);					
					}

				}
			}
		}
	}
	else {
		RemoveOriginalValues("Parameters","ALL",false);
		iv=m_P_Array.size();
		wSize_t(&iv);
		type="Parameters";
		wString(&type);
        string group_psw;
        string name_psw;
		for(size_t i=0;i<iv;i++) {
			pPs=m_P_Array[i];
            group_psw = pPs->GetGroup();
            name_psw = pPs->GetName();
			wString(&group_psw);
			wString(&name_psw);
			fvalue=float(pPs->GetValue());
			wFloat(&fvalue);
		}
	}

return true;
}
bool NewMap::Info_ParTables(bool reading)
{

	P *pPt=nullptr;
	Tab* pTab=nullptr;
	m_continueRead=false;
	string type,group,name;
	size_t iv, nColSize, aNE;
	float fvalue;
	string str, comp_str;
	if(reading) {
		m_Tab_Array.clear();
		rSize_t(&iv);
		rString(&type);
        cout << "Table Parameteres\n***********\n";
		if(type=="Tables") {
			for(size_t i=0; i<iv;i++) {
				rString(&group);
				if(group=="Driving variables") group="Meteorological Data";
				if(group=="Soil mineral nitrogen processes") group="Soil mineral N processes";	
				rString(&name);
				pTab=dynamic_cast<Tab*>(GetPtr(TABLE,name));
				rSize_t(&nColSize);
                
                cout << name << "\n";
				vector <string> TableVarNames;
				for(size_t nCol=0;nCol<nColSize;nCol++) {

					rString(&str);
					TableVarNames.push_back(str);
					if(pTab!=nullptr) {
						comp_str=string(pTab->GetTitle(nCol));
						if(str!=comp_str&&name!="Plant diameter - multiple canopies"&&name!="Snow Albedo Coefficients") {
								string mes="Mismatching variable in table:";
								mes+=name;
								mes+=" - ";
								mes+=str;
								mes+=" not equal";
								mes+=comp_str;
	#ifndef COUPSTD
							//MFC_Util::MessageBox(mes,"Input Error",	MB_OK|MB_ICONEXCLAMATION);
	#endif
						}
					}

				}
				rSize_t(&aNE);
				if(pTab!=nullptr) {
					size_t oldNE=pTab->GetNumOfRow();
					bool validnewinfo=false;
					if(aNE!=oldNE&&oldNE>0) {
						pTab->SetNumOfRow(aNE);
						//pTab->Apply_CE_only();
						//m_continueRead=true;
					}
					P* pP;
					auto NewTest = [&](string name) {
						return GetPPointer(name);

					};

					vector<size_t> ReCalc; ReCalc.assign(nColSize,0);
					for(size_t nRow=0; nRow<aNE ; nRow++){
	  		  				for (size_t nCol=0; nCol<nColSize; nCol++){
								pP = pTab->GetP_Pointer(nCol);
								string testparname = TableVarNames[nCol];
								auto pVar=GetPPointer(testparname);
								if (pVar == nullptr) {
									if (testparname.find("I C OldRoots") != string::npos) pVar = NewTest("I C OldRoot");
									if (testparname.find("I C OldCRoots") != string::npos) pVar = NewTest("I C OldCoarseRoot");
									if (testparname.find("I C Roots") != string::npos) pVar = NewTest("I C Root");
									if (testparname.find("I C CRoots") != string::npos) pVar = NewTest("I C CoarseRoot");
									if (testparname.find("I N OldRoots") != string::npos) pVar = NewTest("I N OldRoot");
									if (testparname.find("I N OldCRoots") != string::npos) pVar = NewTest("I N OldCoarseRoot");
									if (testparname.find("I N Roots") != string::npos) pVar = NewTest("I N Root");
									if (testparname.find("I N CRoots") != string::npos) pVar = NewTest("I N CoarseRoot");
								}

								if (pVar != pP&&pVar != nullptr) pP = pVar;

								rFloat(&fvalue);

								if(pP!=nullptr) {
									double orgvalue, currentvalue;
									orgvalue=pP->GetOriginalValue(nRow);
									currentvalue=pP->GetValue(nRow);

									double range=pP->GetMaxValue(nRow)-pP->GetMinValue(nRow);
									if(range==0) {
										if(nRow>0) {
											double newrange=pP->GetMaxValue(nRow-1)-pP->GetMinValue(nRow-1);
											if(newrange>0) {
												pP->SetMaxValue(nRow,pP->GetMaxValue(nRow-1));
												pP->SetMinValue(nRow,pP->GetMinValue(nRow-1));
											}
											range=newrange;
										}
									}
									if(range>0) {
										double relcurrent=(pP->GetMaxValue(nRow)-currentvalue)/(pP->GetMaxValue(nRow)-pP->GetMinValue(nRow));
										double newrel=(pP->GetMaxValue(nRow)-fvalue)/(pP->GetMaxValue(nRow)-pP->GetMinValue(nRow));
										double ratio=(fvalue+1.E-10)/(currentvalue+1.E-10);
										if(abs(newrel-relcurrent)>1.E-6||ratio<0.99||ratio>1.01) {
											ReCalc[nCol]=pP->SetValue(nRow, fvalue);
											pP->SetNotOldValue(size_t(nRow));
											validnewinfo=true;
										}
									}
								}
	  		  				}
					}
					for (size_t nCol=0; nCol<nColSize; nCol++){
							pP=pTab->GetP_Pointer(nCol);
							auto pVar = GetPPointer(TableVarNames[nCol]);
							if (pVar != pP&&pVar != nullptr) pP = pVar;


							if(pP!=nullptr) {
								string str=pP->GetName();
								if(ReCalc[nCol]>0) LinkedChangeToParameters(pP, ReCalc[nCol], nCol);
							}
					}
					if(validnewinfo)
						m_Tab_Array.push_back(pTab);
					//pTab->SetNotOriginalValue();
				}
				else {
				  for(size_t nRow=0;nRow<aNE;nRow++) {
					   for (size_t nCol=0; nCol<nColSize;nCol++)
						   rFloat(&fvalue);
				  }

				 // if(name!="Plant diameter - multiple canopies");
				  if(name!="Snow Albedo Coefficients"&&name!="Brunts Net long wave Coefficients"&&name!="Brunts incoming long wave Coefficients"
					  &&name!="Satterlunds incoming long wave Coefficients"&&name!="Konzelmann incoming long wave Coefficients") {
							  string mes="Table name not Identified and input file not fully read";
							  m_continueRead=false;

							  mes+=name;
							 #ifndef COUPSTD
							  MFC_Util::MessageBox(mes,"Fatal Input Error",	MB_OK|MB_ICONEXCLAMATION);
							#endif
				   }
				  //return false;

				}

		}
		m_HideAllTableContents=false;
		if(m_continueRead) return false;
	}

	RemoveOriginalValues("Tables","ALL",true);

}
else {
		RemoveOriginalValues("Tables","ALL",false);
		iv=m_Tab_Array.size();
		wSize_t(&iv);
		type="Tables";
		wString(&type);
    string group_psw;
    string name_psw;
		for(size_t i=0;i<iv;i++) {
			pTab=(Tab*)m_Tab_Array[i];
            group_psw = pTab->GetGroup();
            name_psw = pTab->GetName();
			wString(&group_psw);
			wString(&name_psw);
			auto v=pTab->GetNumOfCol();
			wSize_t(&v);
			for(size_t j=0; j<v;j++) {
				str=pTab->GetTitle(j);
				wString(&str);
			}
			v =pTab->GetNumOfRow();
			wSize_t(&v);
			P *pP;
			for(size_t nRow=0; nRow<pTab->GetNumOfRow(); nRow++) {
				for(size_t nCol=0; nCol<pTab->GetNumOfCol(); nCol++) {
						pP=pTab->GetP_Pointer(nCol);
						fvalue=float(pP->GetValue(nRow));
						wFloat(&fvalue);
				}
			}
		}

}  
return true;
}
bool NewMap::Info_Files(bool reading)
{ 
	F *pF=nullptr;
	string type,group,name, FileName, IdVar;
	size_t iv;
	size_t multilistnum;


	if (reading) {
		m_F_Array.clear();
		rSize_t(&iv);
		rString(&type);
		if (type == "Files") {
			if (iv == 0) UpdateArchiveLists(7);
			for (size_t i = 0; i < iv; i++) {
				rString(&group);
				if (group == "Driving variables") group = "Meteorological Data";
				if (group == "Soil mineral nitrogen processes") group = "Soil mineral N processes";

				rString(&name);

				if (name == "Validation File") {
					name = "Validation File1";
					Sw *pSw;
					pSw = dynamic_cast<Sw*>(GetPtr(SWITCH, "ValidationMode"));
					pSw->SetIntValue(1);
					//pSw->Apply();			
				}
				rString(&FileName);
				FileName = GetExistingDocFileName(FileName);
				pF = dynamic_cast<F*>(GetPtr(PGFILE, name));
				if (pF != nullptr&&group != "Hidden") {
					pF->SetValue(FileName);
					pF->CheckFileNameAndAssignNameToPGClass();
					m_F_Array.push_back(pF);

				}

			}
			if (m_FileVersionNumberRead > 61) {
				m_ValsumFlexFiles.resize(0);

				VALSUMLINK sumlink;
				rSize_t(&iv);
				for (size_t i = 0; i < iv; i++) {
					rString(&FileName);
					rString(&sumlink.IDVarKey);
					rSize_t(&sumlink.MultiSimNumKey);
					FileName = GetExistingDocFileName(FileName);

					if (FUtil::IsFileExisting(FileName)) {
						sumlink.FileName = FileName;
						m_ValsumFlexFiles.push_back(sumlink);
						CreateMapSvFlexFile(i);
					}
					else {





					}
				}
			}
			if (m_FileVersionNumberRead > 66) {
				UpdateArchiveLists(8);
				string fileid;

				string filename;
				rSize_t(&iv);
				for (size_t i = 0; i < iv; i++) {
					rString(&fileid);
					rString(&filename);
					filename = GetExistingDocFileName(filename);
					FCSV *pCSV = dynamic_cast<FCSV*>(GetPtr(CSVFILE, fileid));
					if (pCSV != nullptr) {
						pCSV->SetValue(filename);
					if (pCSV->CheckFileName(this) && fileid == "MASCUR File") {


							if(!pGetSoil->IsOpen())
								pGetSoil->Init(filename);
						}
						else {
							string filename;
							if (fileid == "MASCUR File") {
								
#ifndef COUPSTD
								MFC_Util::MessageBox("Specify File Name for MASCUR soil File");
								FILEDLG FileDlg = MFC_Util::GetFileDialog("*.csv", "", "Comma separated file (*.csv)|*.csv|");
								if (FileDlg.opengood != IDOK) {
									return false;
								}
								filename = FileDlg.filename;
#else
								cout << "Specify file Name Mascur file";
								cin >> filename;

#endif
								pCSV->SetValue(filename);
								if (pCSV->CheckFileName(this)) {
										if (!pGetSoil->IsOpen()) {
										pGetSoil->Init(filename);
									}
								}

							}
							else if (!pCSV->CheckFileName(this)&& fileid == "FertTable File") {
#ifndef COUPSTD
								MFC_Util::MessageBox("Specify File Name for Fert Table File");
								FILEDLG FileDlg = MFC_Util::GetFileDialog("*.csv", "", "Comma separated file (*.csv)|*.csv|");
								if (FileDlg.opengood != IDOK) {
									return false;
								}
								string filename = FileDlg.filename;
#else
								cout << "Specify file Name Fert Table file";
								cin >> filename;

#endif
								pCSV->SetValue(filename);
								pCSV->CheckFileName(this);
							}
							else if (pCSV->CheckFileName(this) && fileid == "FertTable File") {

								string filename = pCSV->GetStrValue();
							}
						}
					}
				}
			}
			//m_DocFileIn.read( &iv,4);

		}
		SetSimPeriodFromClimateFile();
	}
	else {
		iv=m_F_Array.size();
		type="Files";
		wSize_t(&iv);
		wString(&type);
        string group_psw;
        string name_psw;
        string strval_psw;
        
		for(size_t i=0; i<iv;i++) {
			pF=m_F_Array[i];
            group_psw = pF->GetGroup();
            name_psw = pF->GetName();
            strval_psw = pF->GetStrValue();
			wString(&group_psw);
			wString(&name_psw);
			wString(&strval_psw);
		}
// New Flex Validation file - FileVersion 62

		iv = m_ValsumFlexFiles.size();
		wSize_t(&iv);
		for (size_t i = 0; i < iv; i++) {
			wString(&m_ValsumFlexFiles[i].FileName);
			wString(&m_ValsumFlexFiles[i].IDVarKey);
			wSize_t(&m_ValsumFlexFiles[i].MultiSimNumKey);
		}
	// New Multi Parfiles - FileVersion 67
		UpdateArchiveLists(8);
		iv = m_FCSV_Array.size();
		wSize_t(&iv);
        
		for (size_t i = 0; i < iv; i++) {
            name_psw = m_FCSV_Array[i]->GetName();
            strval_psw = m_FCSV_Array[i]->GetStrValue();
			wString(&name_psw);
			wString(&strval_psw);
		}


	}

return true;
}
bool NewMap::Info_Outputs(bool reading)
{
	//X *pXT=nullptr;
	//G *pGD=nullptr;
	OutSingle *pSingle;
	OutVector *pVector;
	string type,group,name;
	string FlagString, FlagString2;
	size_t nOutputs;
	size_t iv;
	size_t pgindex=0, pgmulindex=0;
	size_t iType;
	enum simtype typ { PAR };
	float initial,final,min,max,mean,accumulated;
	size_t localindex{ 0 }, validFlagIndex;
	size_t ivalue;
	size_t ivalshift=45;
	Clear_XTGD();
	//m_X_Array.clear();	m_T_Array.clear();	m_G_Array.clear();	m_D_Array.clear();

	if(reading) {
        cout << "\nOutput Table Parameteres\n***********\n";
		pgindex=pgmulindex=0;
		rSize_t(&iv);
		rString(&type);
		//if(IsFinishedSimulation())
			m_NumOutPuts=0;
		if(type=="Outputs") {
			for(size_t i=0; i<iv;i++) {
				rSize_t(&iType);
				rString(&group);
				if(group=="Driving variables") group="Meteorological Data";
				if(group=="Soil mineral nitrogen processes") group="Soil mineral N processes";
		
				rString(&name);
                cout << name << "\n";
				if(m_FileVersionNumberRead<60) {
					rString(&FlagString);
					if(m_FileVersionNumberRead>ivalshift)
						rString(&FlagString2);
				}

				if(m_FileVersionNumberRead>13&&m_FileVersionNumberRead<61) rSize_t(&nOutputs);
				else
					nOutputs=1;
				m_NumOutPuts+=nOutputs;
				switch (iType) {
				case 1:
				case 8:
					typ=STATE;
					break;
				case 2:
				case 9:
					typ=FLOW;
					break;
				case 3:
				case 10:
					typ=AUX;
					break;
				case 4:
				case 11:
					typ=DRIVE;
					break;
				}
					OutVector *ptest;
					pVector=dynamic_cast<OutVector*>(GetPtr(typ,name));
					ptest=dynamic_cast<OutVector*>(GetPtr(AUX,name));
					if(ptest!=nullptr&&pVector==nullptr) {
						pVector=ptest;

					}
					if(ptest==nullptr&&pVector==nullptr) {
						name = FUtil::NameRevision2018(name);
						pVector = dynamic_cast<OutVector*>(GetPtr(typ, name));
						ptest = dynamic_cast<OutVector*>(GetPtr(AUX, name));
					}

					SIMB out;
					size_t storeflag{ string::npos };
					if(pVector!=nullptr) {
						if(m_FileVersionNumberRead<60) {
							pVector->SetFlags(FlagString.c_str(),1);
							if(m_FileVersionNumberRead>ivalshift)
								pVector->SetFlags(FlagString2.c_str(),2);
						}
						else 
							nOutputs=1;
						for(size_t ii=0;ii<nOutputs;ii++){
							rSize_t(&localindex);rFloat(&initial);rFloat(&final);rFloat(&min);
							rFloat(&max);rFloat(&mean);rFloat(&accumulated);rSize_t(&validFlagIndex);
							if(m_FileVersionNumberRead>35&&m_FileVersionNumberRead<61)
								rSize_t(&pgindex);
							if(m_FileVersionNumberRead>47&&m_FileVersionNumberRead<61)
								rSize_t(&pgmulindex);
							if(m_FileVersionNumberRead>59) {
								rSize_t(&storeflag);
								if(m_FileVersionNumberRead>60) {
									if(storeflag>=1) pgindex++;
									if(storeflag==2) pgmulindex++;
								}
							}
							if(ReadResultsFromFile){
								if(m_FileVersionNumberRead>59)
									pVector->AddSumVar(localindex,initial, final, min, max, mean,accumulated, size_t(pgindex), size_t(pgmulindex), storeflag);
								else {
									pVector->AddSumVar(localindex,initial, final, min, max, mean,accumulated, size_t(pgindex), size_t(pgmulindex));
									pVector->SetStoreFlag(localindex,1);
								}
								pVector->SetValidFlagIndex(localindex,size_t(validFlagIndex));
								if((pgmulindex>0&&pgmulindex<1000)&&m_FileVersionNumberRead<61) {
									pVector->SetMultiStoreFlag(localindex,true);
									pVector->SetStoreFlag(localindex,2);
								}
								else if(pgmulindex>0&&storeflag==2) {
									pVector->SetMultiPGFileIndex(localindex, pgmulindex);
									pVector->SetStoreFlag(localindex,2);
								}
									 
								

							}
						}
						if(nOutputs>0) {
							out.pBase=pVector;
							out.TabIndex=localindex;
						}
						else {
							out.pBase=nullptr;
							out.TabIndex=0;

						}
					}
					else {
						typ=simtype(size_t(typ)+7);
						pSingle=(OutSingle*)GetPtr(typ,name);
						if(pSingle==nullptr) {
							if(name=="Soil Resp no roots") name="Soil Resp (no roots)";
							pSingle=(OutSingle*)GetPtr(typ,name);
							if(typ==AUX_SINGLE&&pSingle==nullptr) {
								typ=FLOW_SINGLE;
								pSingle=(OutSingle*)GetPtr(typ,name);

							}



						}

						if(nOutputs>0) {
							rSize_t(&localindex);rFloat(&initial);rFloat(&final);rFloat(&min);
							rFloat(&max);rFloat(&mean);rFloat(&accumulated);rSize_t(&validFlagIndex);
							if(m_FileVersionNumberRead>35&&m_FileVersionNumberRead<61)
								rSize_t(&pgindex);
							if(m_FileVersionNumberRead>47&&m_FileVersionNumberRead<61)
								rSize_t(&pgmulindex);
							if(m_FileVersionNumberRead>59) {
								rSize_t(&storeflag);
								if(m_FileVersionNumberRead>60) {
									if(storeflag>=1) pgindex++;
									if(storeflag==2) pgmulindex++;
								}
							}
							
							if(ReadResultsFromFile&&pSingle!=nullptr){
								if(m_FileVersionNumberRead>59)
									pSingle->AddSumVar(initial, final, min, max, mean,accumulated, size_t(pgindex), size_t(pgmulindex), storeflag);

								else {
									pSingle->AddSumVar(initial, final, min, max, mean,accumulated, size_t(pgindex), size_t(pgmulindex));
									pSingle->SetStoreFlag(1);	
								}
								if((pgmulindex>0&&pgmulindex<1000)&&m_FileVersionNumberRead<61) {
									pSingle->SetMultiStoreFlag(true);
									pSingle->SetStoreFlag(2);
								}
								else if(pgmulindex>0&&storeflag==2) {
									pSingle->SetMultiPGFileIndex(pgmulindex);
									pSingle->SetStoreFlag(2);
								}
								pSingle->SetValidFlagIndex(size_t(validFlagIndex));
							}
						}
						out.pBase=pSingle;
						out.TabIndex=-1;
					}

					if(out.pBase!=nullptr&&nOutputs>0) {
						if(iType==1||iType==8) 
							m_X_Array.push_back(out);
						else if(iType==2||iType==9)
							m_T_Array.push_back(out);
						else if(iType==3||iType==10)
							m_G_Array.push_back(out);
						else if(iType==4||iType==11)
							m_D_Array.push_back(out);
					}
					else {
						auto koll = out;

					}
				}
			}
			m_X_Array=SortSimB_ByGroup(m_X_Array);
			m_T_Array=SortSimB_ByGroup(m_T_Array);
			m_G_Array=SortSimB_ByGroup(m_G_Array);
			m_D_Array=SortSimB_ByGroup(m_D_Array);
	}
	else { // Output of Outputs
		size_t lin;
		iv=GetSize_XTGD();
		type="Outputs";

		vector<SIMB> pvv;
		pvv=GetAllSelectedOutputs();
		size_t ivv=pvv.size();
		wSize_t(&ivv);
		wString(&type);

			for(size_t i=0; i<pvv.size();i++) {
				auto itt=pvv[i].pBase->GetType();
				wSize_t(&itt);
				if(itt==1||itt==8) 
						m_X_Array.push_back(pvv[i]);
				else if(itt==2||itt==9)
						m_T_Array.push_back(pvv[i]);
				else if(itt==3||itt==10)
						m_G_Array.push_back(pvv[i]);
				else if(itt==4||itt==11)
						m_D_Array.push_back(pvv[i]);

				pVector=dynamic_cast<OutVector*>(pvv[i].pBase);
				if(pVector!=nullptr){
					group=pVector->GetGroup();
					wString(&group);
					name=pVector->GetName();
					wString(&name);
				//	iv=pVector->GetNumberOfSelectedFlags();
				//	wInt(&iv);
					lin=pvv[i].TabIndex;
					wSize_t(&lin);
					float fvalue;
					fvalue=pVector->GetInitial(lin);wFloat(&fvalue);
					fvalue=pVector->GetFinal(lin);wFloat(&fvalue);
					fvalue=pVector->GetMin(lin);wFloat(&fvalue);
					fvalue=pVector->GetMax(lin);wFloat(&fvalue);
					fvalue=pVector->GetMean(lin);wFloat(&fvalue);
					fvalue=pVector->GetAccumulated(lin);wFloat(&fvalue);
					ivalue=pVector->GetValidFlagIndex(lin);wSize_t(&ivalue);
					//ivalue=pVector->GetPgSingleFileIndex(lin);wInt(&ivalue);
					//ivalue=pVector->GetPgMultiFileIndex(lin);wInt(&ivalue);
					ivalue=pVector->GetStoreFlag(lin);wSize_t(&ivalue);
				}
				else {
					pSingle=dynamic_cast<OutSingle*>(pvv[i].pBase);
					if(pSingle!=nullptr) {
						group=pSingle->GetGroup();
						wString(&group);
						name=pSingle->GetName();
						wString(&name);
						//iv=1;
						//wInt(&iv);
						lin=string::npos;
						wSize_t(&lin);
						float fvalue;
						fvalue=pSingle->GetInitial();wFloat(&fvalue);
						fvalue=pSingle->GetFinal();wFloat(&fvalue);
						fvalue=pSingle->GetMin();wFloat(&fvalue);
						fvalue=pSingle->GetMax();wFloat(&fvalue);
						fvalue=pSingle->GetMean();wFloat(&fvalue);
						fvalue=pSingle->GetAccumulated();wFloat(&fvalue);
						ivalue=string::npos;wSize_t(&ivalue);
						//ivalue=pSingle->GetPgSingleFileIndex();wInt(&ivalue);
						//ivalue=pSingle->GetPgMultiFileIndex();wInt(&ivalue);
						ivalue=pSingle->GetStoreFlag();wSize_t(&ivalue);
						//}
					}
				}
		//	}
		}
	}
	return true;
}
bool NewMap::Info_Validation(bool reading)
{
	Ps *pP=nullptr;
	string type,group,name;
	size_t iv;
	Sw *pSw;
//	G *pGD;
	SimB *pSimB;
	size_t m_NewLogFunc, m_NewBoxCox;


	
	VALv vst;
	vst.Monitoring=1;
	vst.Duration=60;
	vst.P_Error=0.15f;
	vst.A_Error=0.1f;
	vst.P_NewError=0.;
	vst.A_NewError=0.1f;
	vst.NewBoxCox_Power=1;
	vst.NewBoxCox_Offset=1;
	vst.ValidationFileNumber=1;
	vst.LogTrans=false;
	for(size_t i=0;i<MAXSIMVAL;i++)
		m_ValidationData.m_NumberSelectedValidationVar[i]=0;


	if(reading) {
		m_Val_Array.clear();
		rSize_t(&iv);
		rString(&type);
		if(type!="Validation")
			return false;
		else 
		{
			for(size_t i=0; i<iv;i++) {
				rSize_t(&vst.ValidationFileIndex);rSize_t(&vst.OutputType);rString(&vst.Group);rString(&vst.Name);rSize_t(&vst.LocalIndex);
				rSize_t(&vst.nNumber);	rFloat(&vst.R2);rFloat(&vst.A0);rFloat(&vst.A1);
				rFloat(&vst.ME);rFloat(&vst.RMSE);rFloat(&vst.MeanSim);rFloat(&vst.MeanVal);
				if(m_FileVersionNumberRead>49) {
					rFloat(&vst.P_Error);rFloat(&vst.A_Error);rDouble(&vst.LogLi);rbool(&vst.AccTest);
					rSize_t(&vst.ValidationFileNumber);rbool(&vst.LogTrans);rSize_t(&vst.Duration);
					rSize_t(&vst.Monitoring);rFloat(&vst.NSE);
					rFloat(&vst.P_NewError);rFloat(&vst.A_NewError);
					rFloat(&vst.NewBoxCox_Power);rFloat(&vst.NewBoxCox_Offset);
				}
				else if(m_FileVersionNumberRead>48) {
					rFloat(&vst.P_Error);rFloat(&vst.A_Error);rDouble(&vst.LogLi);rBOOL(&vst.AccTest);
					rSize_t(&vst.ValidationFileNumber);rBOOL(&vst.LogTrans);rSize_t(&vst.Duration);
					rSize_t(&vst.Monitoring);rFloat(&vst.NSE);
					rFloat(&vst.P_NewError);rFloat(&vst.A_NewError);
					rFloat(&vst.NewBoxCox_Power);rFloat(&vst.NewBoxCox_Offset);
				}
				else if(m_FileVersionNumberRead>43) {
					rFloat(&vst.P_Error);rFloat(&vst.A_Error);rDouble(&vst.LogLi);rBOOL(&vst.AccTest);
					rSize_t(&vst.ValidationFileNumber);rBOOL(&vst.LogTrans);rSize_t(&vst.Duration);
					rSize_t(&vst.Monitoring);
				}
				else if(m_FileVersionNumberRead>=40) {
					rFloat(&vst.P_Error);rFloat(&vst.A_Error);rDouble(&vst.LogLi);rBOOL(&vst.AccTest);
				}
				size_t koll;
				if(vst.ValidationFileNumber>0&&vst.ValidationFileNumber<MAXSIMVAL+1) {
					
					m_ValidationData.m_NumberSelectedValidationVar[vst.ValidationFileNumber-1]++;
					F* pF=ValidationFilePointer(vst.ValidationFileNumber);
					if(pF->GetNumberOfSelectedVal()==0) pF->ResetValNumbers();
					if(pF->SetValNumber(vst.ValidationFileIndex, size_t(i+1))) {
						koll=pF->GetNumberOfSelectedVal();
					}
				}

				string typstr="";
				switch(vst.OutputType) {
					case 1:	typstr="State Variables"; break;
					case 2:	typstr="Flow Variables"; break;
					case 3:	typstr="Auxiliary Variables"; break;
					case 4:	typstr="Driving Variables"; break;	
				}
				pSimB=GetPtr(typstr,vst.Group,vst.Name);
				if(pSimB==nullptr) {

							if(vst.Name=="Soil Resp no roots") vst.Name="Soil Resp (no roots)";
							pSimB=GetPtr(typstr,vst.Group,vst.Name);

				}
				m_Val_Array.push_back(vst);				
				if(pSimB!=nullptr) {
					if(pSimB->GetType()<5)
						((OutVector*)pSimB)->SetValidFlagIndex(vst.LocalIndex, i+1);
					else 
						((OutSingle*)pSimB)->SetValidFlagIndex( i+1);
				}
			}

			size_t count_valid_files=0;
			for(size_t i=0;i<MAXSIMVAL;i++) {
				if(ValidationFilePointer(i+1)->m_Exist) {
					ValidationFilePointer(i+1)->SetNumberOfSelectedVal(m_ValidationData.m_NumberSelectedValidationVar[i]);
					count_valid_files++;
				}
				else if(m_ValidationData.m_NumberSelectedValidationVar[i]>0){
					//m_F_Array.push_back(ValidationFilePointer(i));
					ValidationFilePointer(i+1)->SetNumberOfSelectedVal(m_ValidationData.m_NumberSelectedValidationVar[i]);
					count_valid_files++;
				}
			}
			Sw *pSw;
			pSw=(Sw*)GetPtr(SWITCH, "ValidationMode");
			m_NumValidFiles=GetSw( "ValidationMode")->GetIntValue();
			if(count_valid_files>m_NumValidFiles) {
				pSw->SetIntValue(count_valid_files);
				m_NumValidFiles=GetSw( "ValidationMode")->GetIntValue();
			}

			Tab *pTab=(Tab*)GetPtr(TABLE,"Scaling of Observed Variables" );
			if(pTab!=nullptr) {
				auto oldnelements=pTab->GetNumOfRow();
		  		if (iv!=oldnelements)  pTab->SetNumOfRow(iv);
			
			}
			if(m_FileVersionNumberRead>48) {
				rSize_t(&m_NewLogFunc);rSize_t(&m_NewBoxCox);
				enum simtype typ=SWITCH;
				group="Technical";
				name="Likelihood Function";
				pSw=(Sw*)GetPtr(typ,  name);
				pSw->SetIntValue(m_NewLogFunc);
				name="Box-Cox Tranformation";
				pSw=(Sw*)GetPtr(typ, name);
				pSw->SetIntValue(m_NewBoxCox);
			}
			if(IsFinishedSimulation())
				m_ValidationData.SetPointersToOutputValidationFiles();
		}
		
	}
	else {
		iv=m_Val_Array.size();
		if(m_pSimDoc!=nullptr)
		if (m_pSimDoc->MR_GetValidIndex_IndexInValFile_InDim1() != string::npos) {
			auto NumberofDim1 = m_pSimDoc->m_MultiRun_Array[0].NumberOfRepeatitionsWithinDimension;
			if (NumberofDim1 / iv > 1) 	iv =  iv/NumberofDim1;
			else iv = 1;
		}

		type="Validation";
		wSize_t(&iv); wString(&type);
		for (size_t i=0; i<iv; i++) {
			vst=m_Val_Array[i];
			wSize_t(&vst.ValidationFileIndex);wSize_t(&vst.OutputType);wString(&vst.Group);wString(&vst.Name);wSize_t(&vst.LocalIndex);
				wSize_t(&vst.nNumber);	wFloat(&vst.R2);wFloat(&vst.A0);wFloat(&vst.A1);
				wFloat(&vst.ME);wFloat(&vst.RMSE);wFloat(&vst.MeanSim);wFloat(&vst.MeanVal);
				wFloat(&vst.P_Error);wFloat(&vst.A_Error);wDouble(&vst.LogLi);wbool(&vst.AccTest);
				wSize_t(&vst.ValidationFileNumber);wbool(&vst.LogTrans);wSize_t(&vst.Duration);
				wSize_t(&vst.Monitoring);wFloat(&vst.NSE);
				wFloat(&vst.P_NewError);wFloat(&vst.A_NewError);
				wFloat(&vst.NewBoxCox_Power);wFloat(&vst.NewBoxCox_Offset);
		}


		enum simtype typ=SWITCH;
		name="Likelihood Function";
		group="Technical";
		pSw=(Sw*)GetPtr(typ, name);
		m_NewLogFunc=pSw->GetIntValue();
		name="Box-Cox Tranformation";
		pSw=(Sw*)GetPtr(typ, name);
		m_NewBoxCox=pSw->GetIntValue();
		wSize_t(&m_NewLogFunc);wSize_t(&m_NewBoxCox);
	}	
	return true;
}
bool NewMap::Info_SumValidation(bool reading)
{
	Ps *pP=nullptr;
	SimB *pSimB;
	string type,group,name;
	size_t iv;
	VALSUMv vst;
	vst.SimValueSource = SIMSUMSOURCE::MULTIFILESTAT;
	vst.Sim_Value_Residual_Index = 0;



	if(reading) {
		m_ValSum_Array.clear();

		if(m_FileVersionNumberRead<32) return true;
		rSize_t(&iv);
		rString(&type);
		if(type=="ExtraValidation") {
			for(size_t i=0; i<iv;i++) {
				vst.ObsScaling = false;
				vst.MultiBin_StartMin = vst.MultiBin_EndMin = 0;
				rSize_t(&vst.OutputType);rSize_t(&vst.SumVarType);
				rString(&vst.Group);rString(&vst.Name);
				rSize_t(&vst.LocalIndex);
				float fvalue; double dvalue;
				rFloat(&vst.ME_mean); rFloat(&vst.RMSE_mean);rFloat(&vst.SimValue);
				rFloat(&vst.ObsValue);rFloat(&vst.P_Error);
				rFloat(&vst.A_Error);
				rDouble(&vst.LogLi_mean);
				if (m_FileVersionNumberRead > 61) {
					rbool(&vst.FlexSumVar);
					rString(&vst.IDFileName);
					rString(&vst.IDVarName);
					if (m_FileVersionNumberRead > 62) {

						if(m_FileVersionNumberRead > 63) rSize_t(&vst.ObsScaling);
						else {
							bool bvalue;
							rbool(&bvalue);
							if (bvalue)
								vst.ObsScaling = 1;
							else
								vst.ObsScaling = 0;
						}
						rDouble(&vst.ObsScalingFactor);
						if (m_FileVersionNumberRead > 64) {
							size_t source;
							rSize_t(&source); vst.SimValueSource = SIMSUMSOURCE(source);
							rSize_t(&vst.Sim_Value_Residual_Index);
							if (vst.SimValueSource == SIMSUMSOURCE(2)) {
								rUint(&vst.MultiBin_StartMin);
								rUint(&vst.MultiBin_EndMin);
							}
						}
					}
					else
						vst.ObsScaling = 0;
				}
				else {
					vst.FlexSumVar = false;
					vst.IDFileName = "";
					vst.IDVarName = "";
				}
				m_ValSum_Array.push_back(vst);
				switch(vst.OutputType) {
					case 1:
						type="State Variables";
						break;
					case 2:
		 				type="Flow Variables";
		 				break;
					case 3:
						type="Auxiliary Variables";
	 					break;
					 case 4:
		 				type="Driving Variables";
	 					break;
				}
				
				pSimB=GetPtrFromIntType(vst.OutputType, vst.Group, vst.Name);
				if(pSimB==nullptr)  pSimB=GetPtrFromIntType(1, vst.Group, vst.Name);
				if(pSimB==nullptr)  pSimB=GetPtrFromIntType(2, vst.Group, vst.Name);
				if(pSimB==nullptr)  pSimB=GetPtrFromIntType(3, vst.Group, vst.Name);

				if(pSimB!=nullptr) {
					if(pSimB->Is_Vector())
						((OutVector*)pSimB)->SetValidFlagIndex(vst.LocalIndex,i+1);
					else
						((OutSingle*)pSimB)->SetValidFlagIndex(i+1);

				}

			}
		}
		CheckFileNameLinkedto_SV_Variables();

	}
	else {
		iv = m_ValSum_Array.size();
		type = "ExtraValidation";
		wSize_t(&iv);	wString(&type);

		for (size_t i = 0; i < iv; i++) {
			vst = m_ValSum_Array[i];
			wSize_t(&vst.OutputType); wSize_t(&vst.SumVarType);
			wString(&vst.Group); wString(&vst.Name);
			wSize_t(&vst.LocalIndex);
			wFloat(&vst.ME_mean); wFloat(&vst.RMSE_mean); wFloat(&vst.SimValue);
			wFloat(&vst.ObsValue); wFloat(&vst.P_Error);
			wFloat(&vst.A_Error);
			wDouble(&vst.LogLi_mean);
			wbool(&vst.FlexSumVar);
			wString(&vst.IDFileName);
			wString(&vst.IDVarName);
			wSize_t(&vst.ObsScaling);
			wDouble(&vst.ObsScalingFactor);
			int itemp = int(SIMSUMSOURCE(vst.SimValueSource));
			wInt(&itemp);
			wSize_t(&vst.Sim_Value_Residual_Index);
			if (vst.SimValueSource == SIMSUMSOURCE::MULTI_OUTPGFILE) {
				wUint(&vst.MultiBin_StartMin);
				wUint(&vst.MultiBin_EndMin);
			}
		}
	}
	//if(iv>0&&IsFinishedSimulation())
			//UpdateValidationPerformance_SmartFlexVariables();
	
	return true;
}
bool NewMap::Info_RunReport(bool reading)
{
	size_t iv;
	string str;
	if(reading) {
		m_Report_Array.clear();
		rSize_t(&iv);
		rString(&str);
		if(str=="RunReport") {
			for(size_t i=0; i<iv;i++) {
				rString(&str);
				FUtil::trim(str);
				if(ReadResultsFromFile&&iv<100) m_Report_Array.push_back(str);
			}
		}
	}
	else {
		iv=m_Report_Array.size();
		str="RunReport";
		wSize_t(&iv);
		wString(&str);
		for(size_t i=0; i<iv;i++) {
			str=m_Report_Array[i];
			wString(&str);
		}
	}
	return true;
}
bool NewMap::Info_MultiRun(bool reading)
{	
	SimB *pP=nullptr;
	string type,group,name;
	size_t iv;
	float fvalue;
	SIMB valPAR;
	MRv valMR;
	size_t numpar;
	size_t imethod, imonitor;

	if(reading) {
		m_MultiRun_Array.clear();
		m_P_MR_Array.clear();
		rSize_t(&iv);
		rString(&type);
		if(m_FileVersionNumberRead>32)
			rFloat(&m_BayesianStepSize);
		if(m_FileVersionNumberRead>52) {
			size_t ib;
			rSize_t(&ib);
			SetNumBurnIn(ib);
		}
		if(type=="MultiRunOptions"&&iv>0) {

			m_MStorage.Init(this);
			for(size_t i=0; i<iv;i++) {
				rSize_t(&valMR.NumberOfRepeatitionsWithinDimension);
				rSize_t(&valMR.nCount);
				m_MultiRun_Array.push_back(valMR);
				rSize_t(&numpar);
				for(size_t ii=0; ii<numpar;ii++) {
					rString(&group);
					rString(&name);
					if(group=="DataBase") {
						   CDB *pDB;
						   string strID, strIDKey;
						   pDB=DB_GetPointer(name);
						   valPAR.pBase=pDB;
						   rSize_t(&valPAR.TabIndex);
						   rSize_t(&imethod);
						   if(m_FileVersionNumberRead>43) {
							   rSize_t(&imonitor);  // no use of imonitoring Sept 2008
						   }
						   
						   pDB->MR_Set_Method(MR_METHOD(imethod));
						   for(size_t i3=0;i3<valMR.NumberOfRepeatitionsWithinDimension;i3++) {
							   	   string strID, strIDKey;
								   rString(&strID);
								   rString(&strIDKey);
  								   pDB->MR_AddSelection(strID, strIDKey);
						   }
						   size_t idim;
						   rSize_t(&idim);
						   pDB->MR_Set_Dim(idim);
						   pDB->MR_Set_Method(MR_METHOD(5));
						   MR_CP_Set((Ps*)pDB,-1,true);
						   

					}
					else {  // Non-data base
						type="Parameters";
  						if(name=="Index in val file"&&group=="Irrigation") group="Technical";
						if(name=="Size of internal loop"&&group=="Irrigation") group="Technical";
						if(name=="No of internal index loop"&&group=="Irrigation") group="Technical";
						if(name=="SaltHalfReductionn") name="";
						pP=GetPtr(type, group, name);
						if(pP==nullptr) {
							if(name=="Intial Soil Storage") name="Initial Soil Storage";
							if(name=="Intial Peak Storage") name="Initial Peak Storage";
							if(name=="Intial Base Storage") name="Initial Base Storage";
							if (name == "Index for CsvFile") name = "Index Fert Table CsvFile";
							pP=GetPtr(type, group, name);
						}


						if(pP==nullptr) {
  							type="Table Parameters";
							if(name=="SaltHalfReduction") {
								name="Salt c50";
								valPAR.TabIndex=0;
							}
							size_t iread;
							rSize_t(&iread); valPAR.TabIndex=size_t(iread);
							
							SimB *pPar;
							//pPar=GetPtr(type,group,name);
							pPar=GetPtr(PAR_TABLE, name);
							rSize_t(&imethod);
							P* pkoll= dynamic_cast<P*>(pPar);
						   valPAR.pBase=dynamic_cast<SimB*>(pPar);
						   if(pPar==nullptr)  return false;
							   if(valPAR.TabIndex<0||valPAR.TabIndex==string::npos) valPAR.TabIndex=0;
							   if(m_FileVersionNumberRead>43) {
								   rSize_t(&imonitor);  // no use of imonitoring Sept 2008
								   if(!pPar->Is_PTable())
  											((Ps*)pPar)->MR_Set_Monitoring(imonitor);
										else
											((P*)pPar)->MR_Set_Monitoring(valPAR.TabIndex,imonitor);
							   }
							   if(!pPar->Is_PTable())
								   ((Ps*)pPar)->MR_Set_Method(MR_METHOD(imethod));
							   else {
								   if(((P*)pPar)->GetSize()<=valPAR.TabIndex) valPAR.TabIndex=((P*)pPar)->GetSize()-1;
								   ((P*)pPar)->MR_Set_Method(valPAR.TabIndex, MR_METHOD(imethod));
							   }
							   if(imethod==4) {
  									for(size_t iii=0;iii<valMR.NumberOfRepeatitionsWithinDimension;iii++) {
  										   rFloat(&fvalue);
										   double dvalue=fvalue;
	  									   ((P*)pPar)->MR_Append_TableValue(valPAR.TabIndex,dvalue);
									}	

							   }
							   else if(imethod>5&&imethod<9) {
								   size_t deptabindex;
								   rString(&group);
								   rString(&name);
								   rSize_t(&deptabindex);
								  // if(m_FileVersionNumberRead<60) deptabindex--;
								   if(deptabindex<0) 
										type="Parameters";
									else
								 		type="Table Parameters";
								   SimB *pParDep;
								   pParDep=GetPtr(type,group, name);
								   if(pParDep==nullptr&&deptabindex<0) {
									   type="Table Parameters";
									   pParDep=GetPtr(type,group, name);
									   deptabindex=0;
									}
							 
								   if(pParDep!=nullptr) {								
										if(deptabindex<0)
											((P*)pPar)->Set_MR_ParDependence(valPAR.TabIndex, pParDep);
										else
											((P*)pPar)->Set_MR_ParDependence(valPAR.TabIndex, pParDep, deptabindex);
											
								  }
									else {
											// not identified parameter dependence

									}
									if(imethod>6) {
											rFloat(&fvalue);
											if(valPAR.TabIndex<0) 
												((Ps*)pPar)->MR_Set_Min(fvalue);
											else
												((P*)pPar)->MR_Set_Min(valPAR.TabIndex, fvalue);
											rFloat(&fvalue);
											if(valPAR.TabIndex<0) 
												((Ps*)pPar)->MR_Set_Max(fvalue);
											else
												((P*)pPar)->MR_Set_Max(valPAR.TabIndex, fvalue);
									}
								}
							   else {
										rFloat(&fvalue);
										if(valPAR.TabIndex<0) 
											((Ps*)pPar)->MR_Set_Min(fvalue);
										else
											((P*)pPar)->MR_Set_Min(valPAR.TabIndex, fvalue);
										rFloat(&fvalue);
										if(valPAR.TabIndex<0) 
											((Ps*)pPar)->MR_Set_Max(fvalue);
										else
											((P*)pPar)->MR_Set_Max(valPAR.TabIndex, fvalue);
										if(m_FileVersionNumberRead>30) {
											rFloat(&fvalue);
											double dvalue=fvalue;
											if(valPAR.TabIndex<0) 
												((Ps*)pPar)->MR_Set_Start(dvalue);
											else
												((P*)pPar)->MR_Set_Start(valPAR.TabIndex, dvalue);
										}

								 }
								 size_t idim;
								 rSize_t(&idim);
								 if(valPAR.TabIndex==string::npos) 
									((Ps*)pPar)->MR_Set_Dim(idim);
								 else
									((P*)pPar)->MR_Set_Dim(valPAR.TabIndex, idim);
								
	
					}

//// single value PARAMETER CASE BELOW

					else {  // Non-data base
						   valPAR.pBase=pP;
						   //valPAR.Type=0;
							rSize_t(&valPAR.TabIndex);
							if(valPAR.TabIndex>=0)
								valPAR.TabIndex=-1;
							rSize_t(&imethod);
						   if(m_FileVersionNumberRead>43) {
							   rSize_t(&imonitor);  // no use of imonitoring Sept 2008
							   if(valPAR.pBase->Is_P())
  										((Ps*)pP)->MR_Set_Monitoring(imonitor);
									else
										((P*)valPAR.pBase)->MR_Set_Monitoring(valPAR.TabIndex,imonitor);
						   }
						   ((Ps*)pP)->MR_Set_Method(MR_METHOD(imethod));
						   if(imethod==4) {
  								for(size_t iii=0;iii<valMR.NumberOfRepeatitionsWithinDimension;iii++) {
  									   rFloat(&fvalue);
	  								   ((Ps*)pP)->MR_Append_TableValue(fvalue);
								}	

						   }
						   else if(imethod>5&&imethod<9) {
							   size_t deptabindex;
							   rString(&group);
							   rString(&name);
							   rSize_t(&deptabindex);
							   if(deptabindex==string::npos) 
									type="Parameters";
								else
								 	type="Table Parameters";
							   Ps *pParDep;
							   pParDep=(Ps*)GetPtr(type,group, name);
							 
							   if(pParDep!=nullptr) {								
									if(deptabindex==string::npos)
											((Ps*)pP)->Set_MR_ParDependence(pParDep);
										else
											((Ps*)pP)->Set_MR_ParDependence(pParDep, deptabindex);
								}
								else {
									// not identified parameter dependence

								}
								if(imethod>6) {
									rFloat(&fvalue);
									if(valPAR.pBase->Is_P()) 
										((Ps*)pP)->MR_Set_Min(fvalue);
									else
										((P*)pP)->MR_Set_Min(valPAR.TabIndex, fvalue);
									rFloat(&fvalue);
									if(valPAR.pBase->Is_P()) 
										((Ps*)pP)->MR_Set_Max(fvalue);
									else
										((P*)pP)->MR_Set_Max(valPAR.TabIndex, fvalue);
								}
						   
						}
						else {
							
								rFloat(&fvalue);
								if(pP->Is_P()) 
									((Ps*)pP)->MR_Set_Min(fvalue);
								else 
									((P*)pP)->MR_Set_Min(valPAR.TabIndex, fvalue);
								rFloat(&fvalue);
								if(pP->Is_P()) 
									((Ps*)pP)->MR_Set_Max(fvalue);
								else
									((P*)pP)->MR_Set_Max(valPAR.TabIndex, fvalue);
								if(m_FileVersionNumberRead>30) {
									rFloat(&fvalue);
									if(pP->Is_P()) 
										((Ps*)pP)->MR_Set_Start(fvalue);
									else
										((P*)pP)->MR_Set_Start(valPAR.TabIndex, fvalue);
								}

						 }
						 size_t idim;
						 rSize_t(&idim);
						 if(valPAR.pBase->Is_P()) 
							((Ps*)pP)->MR_Set_Dim(idim);
						 else
							 ((P*)pP)->MR_Set_Dim(valPAR.TabIndex, idim);
					}
					m_P_MR_Array.push_back(valPAR);
					}
				}
			}
		}
		else {

			//No MultiRun -No action
		}
	
		MR_CheckCalibMethod();
			if(m_FileVersionNumberRead>32&&m_iError<5) {
				rSize_t(&iv);
				if(iv>0&&iv<1000000) {
						MR_Init_BayesCovar(iv);
						MR_Init_BayesCocor(iv);
					   // Num_BayesPar=iv;
						MR_Set_NumBayesPar(iv);
						for(size_t i=0;i<iv;i++) {
							rFloat(&fvalue);
							MR_Set_BayesMean(i, fvalue);
							for(size_t ii=0;ii<iv;ii++) {
								rFloat(&fvalue);
								MR_Set_BayesCoVar(i,ii,fvalue);
								if(m_FileVersionNumberRead>33) {
									rFloat(&fvalue);
									MR_Set_BayesCoCor(i,ii,fvalue);
								}
							}
					}
				}
			}
			//if(IsBayesianCalibration()) {
				MR_ReCalculatePostDist(IsBayesianCalibration());

			auto pSw=(Sw*)GetPtr(SWITCH,"TimeSerie Errors File");
			if(pSw!=nullptr) {
				auto i=pSw->GetIntValue();
				if(i>0)
					m_OutputValidXBinFileExist=true;
				else
					m_OutputValidXBinFileExist=false;
			}
			//}
			if(m_FileVersionNumberRead>60&&m_ValidationData.GetNumSumVarVariables()>0) {
			if(!m_ValidationData.IsSumValSet()&&IsMultiSimulation()&&IsFinishedSimulation()) {
				bool result;
				string pgfilename = GetOutputFileName(0, false, true);
				if (pgfilename.size() > 0) {
					m_PG_MultiOutputFile.Open(pgfilename);
					m_PG_MultiOutputFile.CloseFile();
				}


				result=CalculatePerformanceAndSetValues();
				m_ValidationData.SetSumValSet(result);
			}


		}


		}

	else { //Writing Section

			iv=m_MultiRun_Array.size();
			numpar=m_P_MR_Array.size();
			type="MultiRunOptions";
			wSize_t(&iv);
			wString(&type);
			wFloat(&m_BayesianStepSize);
			auto nb=GetNumBurnIn();
			wSize_t(&nb);
			

			for(size_t i=0; i<iv;i++) {
				valMR=m_MultiRun_Array[i];
				wSize_t(&valMR.NumberOfRepeatitionsWithinDimension);
				wSize_t(&valMR.nCount);
				numpar=MR_Get_NumberOfParametersWithinDim(i+1);
				wSize_t(&numpar);
                string group_psw;
                string name_psw;
				for(size_t ii=0; ii<numpar;ii++) {

					valPAR.pBase = MR_Get_pPar(i + 1, ii + 1);
					valPAR.TabIndex = MR_GetTabIndex(i + 1, ii + 1);
                    group_psw = MR_GetParGroup(i+1,ii+1);
                    name_psw = MR_GetParName(i+1,ii+1);
					wString(&group_psw);
					wString(&name_psw);
	
					wSize_t(&valPAR.TabIndex);
	
					auto imethod=MR_GetMethod(i+1,ii+1);
					wSize_t(&imethod);
					auto imon=MR_GetMonitoring(i+1,ii+1);
					wSize_t(&imon);
					if(imethod==4) {
						SimB* pCP=MR_Get_pPar(i+1,ii+1);
						for(size_t iii=0;iii<MR_GetNumberOfRepeationsForThisDimension(i);iii++){
							if(MR_GetTabIndex(i+1,ii+1)==-1)
								 fvalue=float(((Ps*)pCP)->MR_Get_TabValue(iii));
							else 
								 fvalue=float(((P*)pCP)->MR_Get_TabValue(MR_GetTabIndex(i+1,ii+1),iii));
							wFloat(&fvalue);
						}
					}
					else if(imethod==5) {
						CDB* pCP=(CDB*)MR_Get_pPar(i+1,ii+1);
                        string selection_pcp;
                        string keyselection_pcp;
						for(size_t iii=0;iii<MR_GetNumberOfRepeationsForThisDimension(i);iii++){
                            selection_pcp = pCP->MR_GetSelection(iii);
                            keyselection_pcp = pCP->MR_GetKeySelection(iii);
							wString(&selection_pcp);
							wString(&keyselection_pcp);
						}
					}
					else if(imethod>5&&imethod<9) {
						SimB* pParDep;
						SimB* pCP=MR_Get_pPar(i+1,ii+1);
						size_t indpar,ind_dep;
						indpar=MR_GetTabIndex(i+1,ii+1);
						if(pCP->Is_P()) {
							pParDep=((Ps*)pCP)->Get_MR_ParDependence();
							ind_dep=((Ps*)pCP)->Get_MR_ParDependenceIndex();
						}
						else {
							pParDep=((P*)pCP)->Get_MR_ParDependence(indpar);
							ind_dep=((P*)pCP)->Get_MR_ParDependenceIndex(indpar);
						}
						if (pParDep != nullptr) {
                            string group_psw = pParDep->GetGroup();
                            string name_psw = pParDep->GetName();
							wString(&group_psw);
							wString(&name_psw);
							wSize_t(&ind_dep);
							if (MR_GetMethod(i + 1, ii + 1)>6) {
								fvalue = float(MR_GetMin(i + 1, ii + 1)); wFloat(&fvalue);
								fvalue = float(MR_GetMax(i + 1, ii + 1)); wFloat(&fvalue);
							}
						}
						else {

							int ivvv = 0;
						}
					}
					else {
						fvalue=float(MR_GetMin(i+1,ii+1));wFloat(&fvalue);
						fvalue=float(MR_GetMax(i+1,ii+1));wFloat(&fvalue);						
						fvalue=float(MR_GetStart(i+1,ii+1));wFloat(&fvalue);	
					}
					size_t i1=i+1;
					wSize_t(&i1);
				}
			}
			iv=MR_Get_NumTotalPar();
			//Num_BayesPar=iv;
			wSize_t(&iv);
			for(size_t i=0;i<iv;i++) {
				fvalue=MR_Get_BayesMean(i);
				wFloat(&fvalue);
				for(size_t ii=0;ii<iv;ii++) {
					fvalue=MR_Get_BayesCoVar(i,ii);wFloat(&fvalue);
					fvalue=MR_Get_BayesCoCor(i,ii);wFloat(&fvalue);
				}
			}



		}	 
	return true;
}
bool NewMap::Info_WithinRunOptions(bool reading)
{

	Ps *pP=nullptr;
	string type,group,name;
	size_t iv;
	float fvalue;
	CHPARv ch;
	P* pPt;
	unsigned int datemin;
	ch.nCount=0;

	if(reading) {
		m_P_CH_Array.clear();
		rSize_t(&iv);
		rString(&type);
		if(type=="WithinRunOptions") {
			for(size_t i=0; i<iv;i++) {
				rString(&group);
				rString(&name);
				rSize_t(&ch.NumDates);
				if(group!="DataBase") {
					ch.TabIndex=0;
					rSize_t(&ch.TabIndex);
					//ch.NumDates--;
					type="Parameters";
					pP=(Ps*)GetPtr(type, group, name);

					if(pP==nullptr) {
							type="Table Parameters";
							ch.Type=1;

							pPt=(P*)GetPtr(type, group, name);
							if(pPt==nullptr) return false;
							ch.pBase=pPt;
							if(m_FileVersionNumberRead<38) {
								ch.TabIndex--;
								if(ch.TabIndex<0) ch.TabIndex=0;
							}
							for(size_t ii=0;ii<ch.NumDates;ii++){
								rUint(&datemin);
								rFloat(&fvalue);	
								pPt->SetChaDateAndValue(datemin,ch.TabIndex, fvalue);
							}
						}
					else {
						// "Parameters";
						ch.Type=0;
						ch.pBase=pP;
						ch.TabIndex=-1;
						//rLong(&datemin);
						//rFloat(&fvalue);
						for(size_t ii=0;ii<ch.NumDates;ii++){
								rUint(&datemin);
								rFloat(&fvalue);	
								pP->SetChaDateAndValue(datemin, fvalue);
						}
					}
				}
				else {
					// "DataBase";
					ch.Type=2;
					CDB *pDB;
					pDB=DB_GetPointer(name);
					ch.pBase=pDB;
					string Id, Key;
					for(size_t ii=0; ii<ch.NumDates;ii++) {
						rUint(&datemin);
						rString(&Id);
						rString(&Key);
						pDB->SetChaDateAndValues(datemin, Id, Key);
					}
				}
				m_P_CH_Array.push_back(ch);
			}
		}
	}
	else {
		iv=m_P_CH_Array.size();
		wSize_t(&iv);
		type="WithinRunOptions";
		wString(&type);
		for(size_t i=0; i<iv;i++) {
			ch=m_P_CH_Array[i];
			Ps* pPar=(Ps*)ch.pBase;
			auto TabIndex=ch.TabIndex;
			if(ch.Type==0) {
				pPar=(Ps*)ch.pBase;
                string group_psw = pPar->GetGroup();
                string name_psw = pPar->GetName();
				wString(&group_psw);
				wString(&name_psw);
				wSize_t(&ch.NumDates);
				wSize_t(&ch.TabIndex);
				for(size_t ii=0; ii<ch.NumDates;ii++) {
					unsigned int min=pPar->GetChaDate( ii);
					wUint(&min);
					fvalue=float(pPar->GetChaParValue(ii));
					wFloat(&fvalue);
				}
			}
			else if(ch.Type==1) {
				P* pPar=(P*)ch.pBase;
                string group_psw = pPar->GetGroup();
                string name_psw = pPar->GetName();
				wString(&group_psw);
				wString(&name_psw);
				wSize_t(&ch.NumDates);
				wSize_t(&ch.TabIndex);
				for(size_t ii=0; ii<ch.NumDates;ii++) {
					unsigned int min=pPar->GetChaDate(ch.TabIndex, ii);
					wUint(&min);
					fvalue=float(pPar->GetChaParValue(ch.TabIndex, ii));
					wFloat(&fvalue);
				}
			}
			else if(ch.Type==2) {
				CDB *pDB;
				pDB=(CDB*)ch.pBase;
				group="DataBase";
				wString(&group);
				name=pDB->GetName();
				wString(&name);
				wSize_t(&ch.NumDates);
                string chaparvalue;
                string chaparkey;
				for(size_t ii=0; ii<ch.NumDates;ii++) {
					unsigned int min=pDB->GetChaDate(ii);
					wUint(&min);
                    chaparvalue = pDB->GetChaParValue(ii);
                    chaparkey = pDB->GetChaParKey(ii);
					wString(&chaparvalue);
					wString(&chaparkey);
				}
			}

		}

	}

return true;
}
bool NewMap::Info_FinalStateVariables(bool reading)
{

	X *pXT=nullptr;
	string type,group,name;
	size_t iv;
	float fvalue;
	size_t nindex;
	vector<SimB*> vpp, vp_final;
	long long pos;
	bool trying=true;

	size_t testcount=0;

//	CFloatArray m_WShed_Slope;
//	CUIntArray m_WShed_ID_Receiver;
//	CUIntArray m_WShed_ID;

	if(reading) {
		if(m_FileVersionNumberRead<28) return false;
		pos=m_DocFileIn.tellg();
		m_DocFileIn.read(m_cbuf,40);
		m_DocFileIn.seekg(pos,ios::beg);
		while(trying) {
			rSize_t(&iv);
			rString(&type);
			if(type=="Final State Variable values") {
				type="State Variables";
				for(size_t ii=0; ii<iv;ii++) {
					rString(&group);
					rString(&name);
					rSize_t(&nindex);
					pXT=(X*)GetPtr(STATE,name);
					for( size_t i=0; i<nindex;i++) {
						rFloat(&fvalue);
						if(pXT!=nullptr) pXT->SetOptInitialValue(i,fvalue);
					}
				}
				trying=false;
			}
			else {
				m_DocFileIn.seekg(pos-80,ios::beg);
				m_DocFileIn.read(m_cbuf,200);

				size_t apos=5;
				while(!(m_cbuf[apos]=='F'&&m_cbuf[apos+1]=='i'&&m_cbuf[apos+2]=='n')&&apos<160) 
						apos++;			
				apos-=5;
				pos+=-80+apos;
				m_DocFileIn.seekg(pos,ios::beg);
				testcount++;
				
				if(!m_DocFileIn.good()||testcount>2) return false;




			}
		}
// Also Reading Watershed options

		rString(&type);
		rSize_t(&iv);
		if(type=="WaterShedOptions") {
			if(m_FileVersionNumberRead<50)
				rBOOL(&m_WShed_Active);
			else
				rbool(&m_WShed_Active);
			rSize_t(&m_WShed_ActiveIndex);
			rSize_t(&m_WShed_Number_X_grids);
		}
// The WaterShedoptions are not complete




	}
	else {
		vp_final.clear();
		SimB* pPtr;
		iv=0;
		vpp=GetPtrVector("State Variables","");
		for(size_t i=0; i<vpp.size(); i++) {
			pPtr=(SimB*)vpp[i];
			name=pPtr->GetName();
			if(name=="WaterStorage"||name=="SoilHeat") {
				iv++;
				vp_final.push_back(pPtr);
			}

		}
		type="Final State Variable values";
		wSize_t(&iv);
		wString(&type);
		for(size_t i=0; i<vp_final.size();i++) {
			pXT=(X*)vp_final[i];
			group=pXT->GetGroup();
			name=pXT->GetName();
			auto nn=pXT->GetNumberOfFlags();
			wString(&group);wString(&name);wSize_t(&nn);
			for(size_t ii=0; ii<nn;ii++) {
				fvalue=pXT->GetFinal(ii);
				//if(fvalue<MISSING*0.5f) fvalue=float(pXT->GetValue(ii));
				if(fvalue<MISSING*0.5f) fvalue=pXT->GetInitial(ii);
				wFloat(&fvalue);
			}
		}
		type="WaterShedOptions";
		iv=m_WShed_ID.size();
		wString(&type);
		wSize_t(&iv);
		wbool(&m_WShed_Active);
		wSize_t(&m_WShed_ActiveIndex);
		wSize_t(&m_WShed_Number_X_grids);
		for(size_t i=0; i<iv;i++) {
//REMAIN
	//		ar<<m_WShed_ID[i];
	//		ar<<m_WShed_ID_Receiver[i];
	//		Value=*(float*)m_WShed_Slope[i];
	//		ar<<Value;
		}



	}	
	return true;
}
bool NewMap::Info_Plottings(bool reading)
{

	Ps *pP=nullptr;
	string type,group,name,str;
	size_t  ncount;
	size_t iChart, iIndex,idoc, nCharts;
	CHART_TYPES iChartType;
	size_t iv;
	SimB *pBase;

	if(reading) {
		rString(&type);
		rSize_t(&ncount);
		rSize_t(&nCharts);

		//m_PlotStorage.SetNumberSavedCharts(ncount);

		rSize_t(&idoc);
		if(idoc==string::npos||idoc>100) {
			ncount=0;
			nCharts=0;
			idoc=0;
			return false;

		}

		if(m_FileVersionNumberRead>38) {
				for(size_t i=0;i<idoc;i++) {
					rString(&str);
			//		m_PlotStorage.m_XTGD_DocNameArray.push_back(str);
				}
		}
		else {
				//m_PlotStorage.m_XTGD_DocNameArray.push_back(GetCurrentSimFile());
		}






		if(type=="PlotSelections"&&ncount>0){
#ifndef COUPSTD
			m_PlotStorage.RemoveAll();

#endif


			void *Ptr;
			size_t ivalfile, imrun, ivarno, iType, id;
			size_t prev_IChart=0, ichart_type;
			for(size_t i=0;i<ncount;i++) {
				rString(&group);
				rString(&name);
				rSize_t(&iType);
				rSize_t(&iIndex);
				rSize_t(&iChart);
				rSize_t(&ichart_type);
				iChartType = CHART_TYPES(ichart_type);
				if(m_FileVersionNumberRead>38) 
					rSize_t(&id);
				else
					id=0;
				if(m_FileVersionNumberRead>42) 
					rSize_t(&ivalfile);
				else
					ivalfile=0;
				if(m_FileVersionNumberRead>44) {
					rSize_t(&ivarno);
					rSize_t(&imrun);
				}
				else {
					ivarno=0;
					imrun=0;
				}

				bool Add=true;
				enum simtype typ;
				typ=STATE;
		   		switch (iType) {
					case 0:
						type="Files";
						typ=PGFILE;
						break;
					case 11: ;case 21:;
					case 1:; case 101:;
						type="State Variables";
						typ=STATE;
						break;
					case 12:; case 22:;
					case 2:;case 102:;
						type="Flow Variables";
						typ=FLOW;
						break;
					case 13:; case 23:;
					case 3: case 103:;
						type="Auxiliary Variables";
						typ=AUX;
						break;
					case 14:; case 24:;
					case 4:; case 104:;
						type="Driving Variables";
						typ=DRIVE;
						break;
					case 5: // Validation Variables
						Ptr=nullptr;
						break;
					case 10:
						bool opennew=true;
#ifndef COUPSTD
						CPG *pPG;
						for(size_t i=0;i<m_PlotStorage.m_PGFileArray.size();i++){
							pPG=(CPG*)m_PlotStorage.m_PGFileArray[i];
							if(pPG->GetFileName()==name.c_str()) {
								opennew=false;			
								Ptr=pPG;
							}
						}
						if(opennew) {
							pPG=new CPG;
							if(pPG->Open(name.c_str(),false)) {
								Ptr=pPG;
								m_PlotStorage.m_PGFileArray.push_back(pPG);
								m_PlotStorage.m_PGFileArrayShow.push_back(0);
								pPG->CloseFile();
							}
							else {
								Add=false;
								delete pPG;
							}
						}
#endif
					}

					if(Add) {
						if (iChart <= prev_IChart) iChart = prev_IChart;
						if(iType!=10&&iType%10!=5) {
							Ptr=GetPtr(typ,  name); 
#ifndef COUPSTD							
							if(m_FileVersionNumberRead>38&&m_FileVersionNumberRead<=44)
								m_PlotStorage.Add(Ptr, iType, iIndex, iChart, iChartType, id, ivalfile);
							if(m_FileVersionNumberRead>44)
								m_PlotStorage.Add(Ptr, iType, iIndex, iChart, iChartType, id, ivalfile, ivarno, imrun);
#endif
							prev_IChart = iChart;
						}
					}
			}

		}


		iv=0;
		size_t idoc=0;
		size_t ivalue=0;
#ifndef COUPSTD
		iv=m_PlotStorage.NumSelected();
		ivalue=m_PlotStorage.GetNumberofCharts();
		idoc=m_PlotStorage.m_XTGD_DocNameArray.size();
#endif



	}
	else {
		type="PlotSelections";
		iv=0;
		size_t idoc=0;
		size_t ivalue=0;
#ifndef COUPSTD
		iv=m_PlotStorage.NumSelected();
		ivalue=m_PlotStorage.GetNumberofCharts();
		idoc=m_PlotStorage.m_XTGD_DocNameArray.size();
#endif

		wString(&type); 
		wSize_t(&iv);
		wSize_t(&ivalue);
		wSize_t(&idoc);
#ifndef COUPSTD
		for(size_t i=0; i<idoc;i++) {
			name=m_PlotStorage.GetDocFileName(i);
			wString(&name);
		}
		for(size_t i=0; i<iv; i++) {
			void *ptr=m_PlotStorage.GetPtr(i);

			auto iType=m_PlotStorage.GetDataType(i);
			
			if(iType==DATA_TYPES::PGDATA&&ptr!=nullptr) {
				CPG *pPG=(CPG*)ptr;
				group="PG File";
				name=pPG->GetFileName();
			}
			else if(ptr==nullptr) {
				//Validation
				group="Validation";
				name="Validation";
			}
			else {
				pBase=(SimB*)ptr;
				group=pBase->GetGroup();
				name=pBase->GetName();
			}

			iIndex=m_PlotStorage.GetLocalIndex(i);
			iChart=m_PlotStorage.GetChart(i);
			iChartType=m_PlotStorage.GetChartType(i);
			idoc=m_PlotStorage.GetDocIndex(i);
			auto ivalfile=m_PlotStorage.GetValFileNo(i);
			auto ivarno=m_PlotStorage.GetValVarNo(i);
			auto imrun=m_PlotStorage.GetValRunNo(i);
			wString(&group);
			wString(&name);
			int ivalue = int(iType);
			wInt(&(ivalue));
			wSize_t(&iIndex);
			wSize_t(&iChart);
			ivalue = int(iChartType);
			wInt(&ivalue);
			wSize_t(&idoc);
			wSize_t(&ivalfile);
			wSize_t(&ivarno);
			wSize_t(&imrun);

		}
#endif
	}



return true;
}
bool NewMap::Info_History(bool reading)
{

	Ps *pP=nullptr;
	string type,group,name, str, test, str2;
	size_t iv;
	size_t intValue;
	time_t tt;
	HISTv h_node;
	RUNINFO i_node;

	if(reading) {
		m_History_Array.clear();
		rSize_t(&iv);
		rString(&type);
		if(type=="History List") {
			for(size_t i=0; i<iv;i++) {
				if(m_FileVersionNumberRead>49) rSize_t(&h_node.RunInfoType);
				rString(&group);
				rString(&type);
				rString(&name);
				rString(&str);
				rSize_t(&intValue);
				rTime(&tt);
				rString(&test);
				rString(&str2);
				if(i==98) {

					size_t koll=intValue;
				}
				if(m_FileVersionNumberRead>49)
					History_NewAdd(h_node.RunInfoType, group, type, name, str, intValue, tt, test, str2);
				else if(intValue<14&&intValue>=0)
					History_Add(group, type, name, str, intValue, tt, test, str2);


			}
		}
	}
	else {
		string str;
		SimB *pBase;
		iv=m_History_Array.size();
		type="History List";
		wSize_t(&iv);
		wString(&type);
		for(size_t i=0; i<iv; i++) {
			h_node=m_History_Array[i];
			pBase=h_node.pBase;
			
			type=p_ModelInfo->GetVarType(h_node.RunInfoType);
			if(h_node.RunInfoType<0||pBase==nullptr) {
				group="";
				name="";
				str="";

			}
			else if(h_node.RunInfoType==10) {
				// DAtaBase
				group="";				
				name=((CDB*)pBase)->GetName();
				str=History_GetString(i);

			}
			else if(h_node.RunInfoType==11) {
				group=((CRunInfo*)pBase)->GetGroup();
				name=((CRunInfo*)pBase)->GetName();
				str=History_GetString(i);

			}
			else if(h_node.RunInfoType==0) {
				group=((Sw*)pBase)->GetGroup();

			}
			else if(h_node.RunInfoType==1) {
				if (pBase->GetName() == "Soil Properties"|| pBase->Is_DB()) {
					group = "";
					name = ((Sw*)pBase)->GetName();
					str = History_GetString(i);
				}
				else {
					group = "";
					name = pBase->GetName();
					str = FUtil::STD_FtoAscii(History_GetFloat(i));
				}
			}
			else if(h_node.RunInfoType==2) {
				group=pBase->GetGroup();
				name=pBase->GetName();
				str=FUtil::STD_FtoAscii(History_GetFloat(i));
			}
			else if(h_node.RunInfoType==3) {
				group=pBase->GetGroup();
				name=pBase->GetName();
				str=History_GetString(i);
			}
			else {
				group=pBase->GetGroup();
				name=pBase->GetName();
				str=History_GetString(i);
			}

			wSize_t(&h_node.RunInfoType);
			wString(&group);
			wString(&type);
			wString(&name);
			wString(&str);

			wSize_t(&h_node.LocalIndex);
			wTime(&h_node.tvalue);
			wString(&h_node.User);
			wString(&h_node.Computer);
		}




	}

return true;
}
bool NewMap::Info_DataBase(bool reading)
{

	string type, str;
	size_t iv;
	time_t time;

	if(reading) {
		//m_P_Array.clear();
		rSize_t(&iv);
		rString(&type);
		if(type=="DataBaseSelections") {
			for(size_t i=0; i<iv;i++) {
				rString(&str);
				DB_SetChoice(DB_types(i),str);

			}
			if(m_FileVersionNumberRead>36) {
				for(size_t i=0;i<iv;i++)
					rTime(&time);
			}
		}

	}
	else {
		iv=10;
		type="DataBaseSelections";
		wSize_t(&iv);
		wString(&type);
		for(size_t i=0; i<iv;i++) {
			str=DB_GetChoice(DB_types(i));
			wString(&str);
		}
		for(size_t i=0;i<iv;i++) {
			time=DB_GetImportTimeT(i);
			wTime(&time);
		}

	//	m_DocFileOut.write(&iv ,4);


	}



return true;
}
bool NewMap::Info_Annim(bool reading)
{

	size_t iv;
	iv=0;
	string type, str, group, name;
	size_t ityp, ind, ich;	
	SimB* pBase;
	Annim anode;
	if(reading) {
		//m_DocFileIn>>iv;
		//m_DocFileIn.read( &iv,4);
		rSize_t(&iv);
		rString(&type);
		if(type=="AnnimationSelections") {
			rSize_t(&m_AnnimChartX);
			rSize_t(&m_AnnimChartY);
			if(m_AnnimChartX<1||m_AnnimChartX>2) m_AnnimChartX=2;
			if(m_AnnimChartY<1||m_AnnimChartY>2) m_AnnimChartY=2;
			for(size_t i=0; i<iv;i++) {
				rString(&name);
				rString(&group);
				rSize_t(&ityp);
				rSize_t(&ind);
				rSize_t(&ich);

				pBase=(SimB*)GetPtr(type,group,name);
				if(pBase!=nullptr) {
					anode.pBase=pBase;
					anode.type=ityp;
					anode.LocalIndex=ind;
					anode.iChart=ich;
					AnnimVarList.push_back(anode);
					auto seq=AnnimVarList.size();
					if(pBase->GetType()<<5)
						pBase->SetAnnimateChart(ind,size_t(ich),size_t(seq));
					else 
						pBase->SetAnnimateChart(size_t(ich),size_t(seq));

				}
			}
		}
		else
			return false;
	}
	else {
		type="AnnimationSelections";
		iv=AnnimVarList.size();
		

		wSize_t(&iv);
		wString(&type);
		wSize_t(&m_AnnimChartX);wSize_t(&m_AnnimChartY);
		for(auto ii=AnnimVarList.begin(); ii!=AnnimVarList.end();++ii) {
			ityp=(*ii).type;
			pBase=(*ii).pBase;
			ind=(*ii).LocalIndex;
			ich= (*ii).iChart;
			if(ityp<6) {
				name=((X*)pBase)->GetName();
				group=((X*)pBase)->GetGroup();
			}
			else {
				name=((G*)pBase)->GetName();
				group=((G*)pBase)->GetGroup();
			}
			wString(&name); wString(&group);
			wSize_t(&ityp);wSize_t(&ind);wSize_t(&ich);
		}

	}
return true;
}





vector<SIMB> NewMap::UpdateOutVector(vector<SimB*> vp_in,string GroupSet, bool original, int ViewCategory) {
	OutVector *pVector;
	OutSingle *pSingle;
	vector<SIMB> vect; 
	SIMB out ;
	vector<SimB*> vp;
	vector<SIMB> outcat;
	for (size_t i = 0; i < vp_in.size(); i++) {
		if (ViewCategory == 0 || p_ModelInfo->GetGroupCategory(vp_in[i]->GetGroup()) == ViewCategory)
			vp.push_back(vp_in[i]);
	}

	for( size_t j=0;j<vp.size();j++) {
			    pVector=dynamic_cast<OutVector*>(vp[j]);	
				if(pVector!=nullptr) {
						out.pBase=pVector;
						if(pVector->IsNotOriginalValue()&&pVector->IsEnabled()&&original) {
							for(size_t i=0; i<pVector->GetNumberOfFlags(); i++) {
								out.TabIndex=i;
							  	vect.push_back(out);
							}
						}
						else if((((m_GroupEnabled[pVector->GetGroupNo()]||original)&&pVector->IsEnabled()))) {
							for(size_t i=0; i<pVector->GetNumberOfFlags(); i++) {
								out.TabIndex=i;
								vect.push_back(out);
							}
						}
						else if(pVector->IsEnabled()) {	
							for(size_t i=0; i<pVector->GetNumberOfSelectedFlags(); i++) {
								out.TabIndex=pVector->GetSelectedLocalIndex(i);
								if(pVector->GetStoreFlag(out.TabIndex)>0) 
							  		vect.push_back(out);
							}
						}

				}
				else {
					pSingle=dynamic_cast<OutSingle*>(vp[j]);
					if(pSingle!=nullptr) {
						out.pBase=pSingle;
						out.TabIndex = string::npos;
						if(pSingle->IsNotOriginalValue()&&pSingle->IsEnabled()&&original)
							  	vect.push_back(out);
						else if(((( m_GroupEnabled[pSingle->GetGroupNo()]||original)&&pSingle->IsEnabled()))) {
								vect.push_back(out);
						}
						else if(pSingle->GetStoreFlag()>0&&pSingle->IsEnabled()) {	
							  	vect.push_back(out);
						}


					}
				}
	}
	return vect;
}



void NewMap::RemoveOriginalValues(string typ, string GroupSet, bool original, size_t ViewCategory_to_use) {

		vector<SimB*> vp;
		vector<SimB*> vp_single;

		if (typ=="Parameters") {
			if(m_P_Array.size()>0)			m_P_Array.clear();
			vp=GetPtrVector(PAR_SINGLE, false);
			for(auto it=vp.begin();it!=vp.end();it++) {
					Ps* pPs=dynamic_cast<Ps*>(*it);
					if(original) {
						if((m_GroupEnabled[pPs->GetGroupNo()] ||GroupSet=="ALL")) {
							m_P_Array.push_back(pPs);
						}
						else if (pPs->IsNotOriginalValue()) m_P_Array.push_back(pPs);
					}
					else if(pPs->IsNotOriginalValue()) {
						if((m_GroupEnabled[pPs->GetGroupNo()]||GroupSet=="ALL")) 
							m_P_Array.push_back(pPs);
					}
					else {
						if (m_GroupEnabled[pPs->GetGroupNo()] )
							m_P_Array.push_back(pPs);
					}
			}
		}
		else if (typ=="Tables") {
			if(m_Tab_Array.size()>0)			m_Tab_Array.clear();
			vp=GetPtrVector(TABLE, false);
			for(auto it=vp.begin();it!=vp.end();it++) {
					Tab* pTab=dynamic_cast<Tab*>(*it);
					if(original) {
						if((m_GroupEnabled[pTab->GetGroupNo()] ||GroupSet=="ALL")) {
							if(pTab->CheckEnabled()) m_Tab_Array.push_back(pTab);
						}
						else if (pTab->IsNotOriginalValue()) m_Tab_Array.push_back(pTab);
					}
					else{
						pTab->CheckOriginalValue();
						if(pTab->IsNotOriginalValue()&& pTab->CheckEnabled()) {
							if(m_GroupEnabled[pTab->GetGroupNo()] ||GroupSet=="ALL")
								m_Tab_Array.push_back(pTab);
						}
					}

/*					if((pTab->GetGroup()==GroupSet||GroupSet=="ALL")&&original) {
							m_Tab_Array.push_back(pTab);
					}
					else if (pTab->IsNotOriginalValue()&&pTab->IsEnabled()&&original)
							m_Tab_Array.push_back(pTab);	
					else if(pTab->IsNotOriginalValue()&&pTab->IsEnabled())  //  m_UserLevel<=pPs->GetUserLevel())
							m_Tab_Array.push_back(pTab);
					else if(pTab->GetGroup()==GroupSet&&pTab->IsEnabled())
							m_Tab_Array.push_back(pTab);*/
			}
		}			
		else if(typ=="Switches") {
				Sw *pSw;
				if(m_Sw_Array.size()>0)          m_Sw_Array.clear();
				vp=GetPtrVector(typ,"");
				for( size_t j=0;j<vp.size();j++) {
					pSw=dynamic_cast<Sw*>(vp[j]);
					if(pSw->IsNotOriginalValue()&&pSw->IsEnabled()&&original)
						m_Sw_Array.push_back(pSw);
					else if((m_GroupEnabled[pSw->GetGroupNo()]||GroupSet=="ALL")&&original) {
						m_Sw_Array.push_back(pSw);
					}
					else if(pSw->IsNotOriginalValue()&&pSw->IsEnabled())
						m_Sw_Array.push_back(pSw);	
					else if(m_GroupEnabled[pSw->GetGroupNo()]&&pSw->IsEnabled())
						m_Sw_Array.push_back(pSw);	
				}
		}
		
		
		if(typ=="State Variables"||typ==""||typ=="Outputs") {
			if(m_X_Array.size()>0)		  m_X_Array.clear();
			vp=GetPtrVector(STATE,true);
			vp_single=GetPtrVector(STATE_SINGLE,true);
			for(size_t i=0; i<vp_single.size(); i++)
				vp.push_back(vp_single[i]);
			m_X_Array=UpdateOutVector(vp, GroupSet, original, ViewCategory_to_use);	
			m_X_Array=SortSimB_ByGroup(m_X_Array);
		}
		if(typ=="Flow Variables"||typ==""||typ=="Outputs") {
			if(m_T_Array.size()>0)		  m_T_Array.clear();
			vp=GetPtrVector(FLOW, false);
			vp_single=GetPtrVector(FLOW_SINGLE, false);
			for(size_t i=0; i<vp_single.size(); i++)
				vp.push_back(vp_single[i]);
			m_T_Array=UpdateOutVector(vp, GroupSet, original, ViewCategory_to_use);
			m_T_Array=SortSimB_ByGroup(m_T_Array);
		}
		if(typ=="Auxiliary Variables"||typ==""||typ=="Outputs") {
			if(m_G_Array.size()>0)		  m_G_Array.clear();
			vp=GetPtrVector(AUX, false);
			vp_single=GetPtrVector(AUX_SINGLE, false);
			for(size_t i=0; i<vp_single.size(); i++)
				vp.push_back(vp_single[i]);
			m_G_Array=UpdateOutVector(vp, GroupSet, original, ViewCategory_to_use);
			m_G_Array=SortSimB_ByGroup(m_G_Array);
		}
		if(typ=="Driving Variables"||typ==""||typ=="Outputs") {
			if(m_D_Array.size()>0)		  m_D_Array.clear();
			vp=GetPtrVector(DRIVE, false);
			vp_single=GetPtrVector(DRIVE_SINGLE, false);
			for(size_t i=0; i<vp_single.size(); i++)
				vp.push_back(vp_single[i]);
			m_D_Array=UpdateOutVector(vp, GroupSet, original, ViewCategory_to_use);
			m_D_Array=SortSimB_ByGroup(m_D_Array);
		}
		
		

	//Sort_TableList_Arrays();
	//The updating of pointer to document file was successful
}
void NewMap::CleanAllLists()
{
	if(m_P_Array.size()>0)			m_P_Array.clear();
	if(m_Pt_Array.size()>0)          m_Pt_Array.clear();
	if(m_Sw_Array.size()>0)          m_Sw_Array.clear();
	if(m_F_Array.size()>0)           m_F_Array.clear();
	if(m_Tab_Array.size()>0)        m_Tab_Array.clear();
	if(m_X_Array.size()>0)        m_X_Array.clear();
	if(m_T_Array.size()>0)        m_T_Array.clear();
	if(m_G_Array.size()>0)        m_G_Array.clear();
	if(m_D_Array.size()>0)        m_D_Array.clear();
}
void NewMap::UpdateArchiveLists(size_t iType){
	Ps *pP; Sw *pSw; Tab *pTab; F *pF; FCSV *pCSV;
	OutVector *pVector;
	OutSingle *pSingle;
	vector<SimB*> vp;
	SIMB out;
	string typ;
  
	pVector=nullptr;
	// Write Values on Switches parameters outputs

	size_t istart, iabove;
	if(iType<9) {
		istart=iType;
		iabove=iType+1;
		}
	else {
		istart=0;
		iabove=9;
	}

	for(size_t j=istart;j<iabove;j++) {	
	//Choose the current name for the type
		switch (j) {
			case 0:	typ="Parameters"; 
				if(m_P_Array.size()>0)			m_P_Array.clear();
				vp=GetPtrVector(typ,"");
				for(size_t j=0;j<vp.size();j++) {
					pP=dynamic_cast<Ps*>(vp[j]);
					if(pP!=nullptr) 
						if(pP->IsNotOriginalValue()&&pP->GetGroup()!="Hidden")
							m_P_Array.push_back(pP);
				}				
				break;
			case 1:	typ="Switches"; 
				if(m_Sw_Array.size()>0)          m_Sw_Array.clear();
					vp=GetPtrVector(typ,"");
					for( size_t j=0;j<vp.size();j++) {
						pSw=dynamic_cast<Sw*>(vp[j]);
						if(pSw!=nullptr)
							if (pSw->IsNotOriginalValue()&&pSw->GetGroup()!="Hidden")
								m_Sw_Array.push_back(pSw);
					}
				break;
			case 2:	typ="Tables";
					if(m_Tab_Array.size()>0)          m_Tab_Array.clear();	
					vp=GetPtrVector(typ,"");
					for(size_t j=0;j<vp.size();j++) {
						pTab=dynamic_cast<Tab*>(vp[j]);
						if (pTab->IsEnabled())
							m_Tab_Array.push_back(pTab);
					}
				break;
			case 3:	typ="State Variables";
				if(m_X_Array.size()>0)		 m_X_Array.clear();
					vp=GetPtrVector(typ,"");
					for( size_t j=0;j<vp.size();j++) {
						pVector=dynamic_cast<OutVector*>(vp[j]);
						out.pBase=pVector;
						if(pVector!=nullptr)
							if(pVector->IsFlagsSet()&&pVector->IsEnabled())
								m_X_Array.push_back(out);
					}
					for( size_t j=0;j<vp.size();j++) {
						pSingle=dynamic_cast<OutSingle*>(vp[j]);
						out.pBase=pVector;
						if(pVector!=nullptr)
							if(pVector->IsFlagsSet()&&pSingle->IsEnabled())
								m_X_Array.push_back(out);
					}

				break;
			case 4:	typ="Flow Variables";
				if(m_T_Array.size()>0)		 m_T_Array.clear();
					vp=GetPtrVector(typ,"");
					for( size_t j=0;j<vp.size();j++) {
						pVector=dynamic_cast<OutVector*>(vp[j]);
						out.pBase=pVector;
						if(pVector!=nullptr)
							if(pVector->IsFlagsSet()&&pVector->IsEnabled())
								m_T_Array.push_back(out);
					}
					for( size_t j=0;j<vp.size();j++) {
						pSingle=dynamic_cast<OutSingle*>(vp[j]);
						out.pBase=pSingle;
						if(pSingle!=nullptr)
							if(pVector->IsFlagsSet()&&pSingle->IsEnabled())
								m_T_Array.push_back(out);
					}
				
				
				break;
			case 5:	typ="Auxiliary Variables";
		
				if(m_G_Array.size()>0)        m_G_Array.clear();
					vp=GetPtrVector(typ,"");
					for( size_t j=0;j<vp.size();j++) {
						pVector=dynamic_cast<OutVector*>(vp[j]);
						out.pBase=pVector;
						if(pVector!=nullptr)
							if(pVector->IsFlagsSet()&&pVector->IsEnabled())
								m_G_Array.push_back(out);
					}
					for( size_t j=0;j<vp.size();j++) {
						pSingle=dynamic_cast<OutSingle*>(vp[j]);
						out.pBase=pVector;
						if(pVector!=nullptr)
							if(pVector->IsFlagsSet()&&pSingle->IsEnabled())
								m_G_Array.push_back(out);
					}
				
				break;
			case 6:	typ="Driving Variables";
				if(m_D_Array.size()>0)		 m_D_Array.clear();
					vp=GetPtrVector(typ,"");
					for( size_t j=0;j<vp.size();j++) {
						pVector=dynamic_cast<OutVector*>(vp[j]);
						out.pBase=pVector;
						if(pVector!=nullptr)
							if(pVector->IsFlagsSet()&&pVector->IsEnabled())
								m_D_Array.push_back(out);
					}
					for( size_t j=0;j<vp.size();j++) {
						pSingle=dynamic_cast<OutSingle*>(vp[j]);
						out.pBase=pVector;
						if(pVector!=nullptr)
							if(pVector->IsFlagsSet()&&pSingle->IsEnabled())
								m_D_Array.push_back(out);
					}
				break;
			case 7:	typ="Files";
				if(m_F_Array.size()>0)        m_F_Array.clear();
					vp=GetPtrVector(typ,"");
					for(size_t j=0;j<vp.size();j++) {
							pF=dynamic_cast<F*>(vp[j]);
							if(pF->IsEnabled())
								m_F_Array.push_back(pF);
					}
			case 8:	typ = "CSV Files";
				if (m_FCSV_Array.size()>0)        m_FCSV_Array.clear();
				vp = GetPtrVector(typ, "");
				for (size_t j = 0; j<vp.size(); j++) {
					pCSV = dynamic_cast<FCSV*>(vp[j]);
					if (pCSV->IsEnabled())
						m_FCSV_Array.push_back(pCSV);
				}

			}
	}

}
bool NewMap::MakeMapComplete() {
	vector<SimB*> vptr;
	for(size_t i=0;i<14;i++) {
		enum simtype typ { PAR };
		switch (i) {
			case 0:
			typ=SWITCH;
			break;
			case 1:
			typ=PAR_SINGLE;
			break;
			case 2:
			typ=TABLE;
			break;
			case 3:
			typ=PAR_TABLE;
			break;
			case 4:
			typ=STATE;
			break;
			case 5:
			typ=FLOW;break;
			case 6:	typ=AUX;break;
			case 7:	typ=DRIVE; break;
			case 8:	typ=PGFILE; break;
			case 9:	typ=FLOW_SINGLE;break;
			case 10:	typ=AUX_SINGLE;break;
			case 11:	typ=DRIVE_SINGLE; break;
			case 12:	typ=STATE_SINGLE; break;
			case 13:	typ = CSVFILE; break;
			
		}
		vptr=GetPtrVector(typ, true);
		for( size_t j=0; j<vptr.size();j++) {
			vptr[j]->BuildDependencies(this);
		}
	}
	return true;
}
vector<SIMB> NewMap::SortSimB_ByGroup(vector<SIMB> v_in) {
	vector<SIMB> v_out;
	multimap<string, SIMB> SortMap;
	for(size_t i=0; i<v_in.size(); i++) {	
		SortMap.insert(pair<string, SIMB>(v_in[i].pBase->GetGroup(),v_in[i]));
	}

	for(auto it=SortMap.begin(); it!=SortMap.end(); ++it) {
		v_out.push_back((*it).second);
	}
	return v_out;

}
vector<SIMB> NewMap::SortSimB_ByCategory(vector<SIMB> v_in) {

	vector<SIMB> v_out;
	multimap<size_t, SIMB> SortMap;
	for(size_t i=0; i<v_in.size(); i++) {	
		SortMap.insert(pair<size_t, SIMB>(p_ModelInfo->GetGroupCategory(v_in[i].pBase->GetGroup()),v_in[i]));
	}

	for(auto it=SortMap.begin(); it!=SortMap.end(); ++it) {
		v_out.push_back((*it).second);
	}
	return v_out;
}
bool NewMap::CreateMapSvFlexFile(size_t index) {

	VALSUMLINK valsumlink;
	valsumlink = m_ValsumFlexFiles[index];
	SoilStorage soilstorage;
	ifstream sumfile;
	sumfile.open(valsumlink.FileName, ifstream::in);
	string line;
	line.resize(2000);
	sumfile.getline(&line[0], 2000);
	size_t n=sumfile.gcount();
	line.resize(n);
	size_t ipos;
	vector<string> header;
	ipos = line.find(';'); if (ipos == string::npos) ipos = line.find(',');
	size_t count = 0;


	while (ipos != string::npos) {
		string sub;
		
		sub = line.substr(0, ipos);
		MapFlexId[index].insert(pair<string, size_t>(sub, count));
		count++;
		line = line.substr(ipos + 1);
		ipos = line.find(';');	if (ipos == string::npos) ipos = line.find(',');
		if (line.size() > 0 && ipos == string::npos) {
			size_t nn = line.size();
			sub = line.substr(0, nn-1);
			nn = sub.size();
			MapFlexId[index].insert(pair<string, size_t>(sub, count));
			count++;
		}
	}




	line.resize(2000);
	while (sumfile.good()) {
		sumfile.getline(&line[0], 2000);
		auto k=sumfile.gcount();
		line.resize(k);
		vector<float> values;

		ipos = line.find(';'); if (ipos == string::npos) ipos = line.find(',');
		string sub;
		sub = line.substr(0, ipos);
		size_t value = FUtil::AtoInt(sub);
		float fvalue = FUtil::AtoFloat(sub);
		values.push_back(fvalue);

		line = line.substr(ipos + 1);
		ipos = line.find(';');	if (ipos == string::npos) ipos = line.find(',');
		while (ipos != string::npos) {
			string sub;
			sub = line.substr(0, ipos);
			float value = FUtil::AtoFloat(sub);
			values.push_back(value);
			if (line[0] =='\n'&& ipos == 0) {
				values.push_back(MISSING);
				ipos = string::npos;
			}
			else {
				line = line.substr(ipos + 1);
		
				ipos = line.find(';');	if (ipos == string::npos) ipos = line.find(',');
				if (ipos == string::npos) {
					float value = FUtil::AtoFloat(line);
					values.push_back(value);
				}
			}
		}
		if (line.size() > 0 && ipos == string::npos) {
			float value = FUtil::AtoFloat(line);
			values.push_back(value);
		}

		m_ValFlexValue[index].push_back(values);
		//FlexVal *Flex;
		//Flex = new FlexVal;
		//Flex->val = values;
		MapFlexVal[index].insert(pair<size_t, size_t>(value, m_ValFlexValue[index].size() - 1));
		size_t koll = line.size();
		line.clear();
		line.resize(2000);

	}
	//soilstorage.ReadHeaderFile();



	return true;
}

