// MSimCoup2.cpp : Defines the entry point for the console application.
//


#include "../CoupModel/NewBase/Doc.h"
#include "../CoupModel/Util/FUtil.h"

#ifdef MS_CODE
  #include <windows.h>
  //#include<conio.h>
  #include <direct.h>
  #define PATH_SEP "\\"
  #include "../CoupModel/std.h"
#else
  #define PATH_SEP "/"
 #include <unistd.h>
#endif




#include "../CoupModel/NewBase/SimArchive.h"
#include "../CoupModel/SoilData/PlotPF/PFCurve.h"
#include "../CoupModel/Model/FunctionProperties.h"
#include "../CoupModel/Util/Register.h"

#include "../CoupModel/SoilData/GetSoilStorage.h"

#include <thread>

#define BUFSIZE 4096

void SimProc(size_t i, Doc *pDoc, bool Multi)
{
	if (Multi) {
		pDoc->CheckAndUpdateFileName(true);
		pDoc->MakeMultiRun();
	}
	else {
		pDoc->CheckAndUpdateFileName(false);
		pDoc->MakeSingleRun();
	}
}

Doc* CreateDoc(size_t i, string str){
	Doc *pDoc;
	pDoc = new Doc();
	pDoc->SetCurrentFileName(str);

	bool makesim;
	makesim = pDoc->ReadDocFile(pDoc->GetCurrentSimFile());
	if(!makesim) {
		std::cout << "\n";
		std::cout << "Simulation file not correctly read - Check File !";
		std::cin >> str;
		return NULL;
	}


	
	if (i == 0) {
		if (pDoc->IsFinishedSimulation()) {
			std::cout << "\n";
			std::cout << "Document :" + pDoc->GetCurrentSimFile() + " is with completed run. Do you want to create new document (Y/N) ?";
			std::cin >> str;
			if (str.find("Y") != string::npos) {
				if (!pDoc->CreateNewDocFromCurrentDoc()) makesim = false;
			}
		}
	}
	else {
		pDoc->CreateNewDocFromCurrentDoc();
	}
	return pDoc;


}
bool MakeMulti(size_t i, Doc *pDoc) {
		string str;
		bool out;
		out = false;
		if (i == 0) {
			if (pDoc->m_MultiRun_Array.size() > 0) {
				std::cout << "\n";
				std::cout << "Document :" + pDoc->GetCurrentSimFile() + " can make Multi or Single Runs.\nDo you want Multi Runs (Y/N) ?";
				std::cin >> str;
				if ((str.find("Y") || str.find('y')) ) out = true;
				pDoc->SetMultiSimulation();
			}
		}
		else if (pDoc->m_MultiRun_Array.size() > 0) {
			Ps *pPs;
			pPs = pDoc->GetPsPointer("Random Seed");
			double v= pPs->GetValue()+i+0.00001;
			pPs->SetValue(v);
			out = true;
			pDoc->SetMultiSimulation();
		}
		return out;
}
 
size_t MergeAllFiles(vector<Doc*> pDocs) {

/*	Doc *pMainDoc;
	pMainDoc = pDocs[0];
	bool valid = false;
	
	pMainDoc->UnLockMultiStorageFile();
	pMainDoc->m_MStorage.Reset();
	pMainDoc->m_MStorage.OpenFile(pMainDoc->GetMBinFileName());
	for (size_t i = 1; i < pDocs.size(); i++) {
		valid=pMainDoc->m_MStorage.AddExistingFile(pDocs[i]->GetMBinFileName());
	}
	pMainDoc->m_MStorage.UpdateAcceptedRuns(true, true);
	pMainDoc->m_MStorage.Reset();
	size_t itotrun=pMainDoc->MR_Get_TotalNumberofRuns();
	size_t ifiles = pMainDoc->m_MStorage.GetNumberOfAddedFiles(pMainDoc->GetMBinFileName());
	size_t koll;
	for (size_t i = 0; i < ifiles; i++) {

		koll=pMainDoc->m_MStorage.GetNumberOfAddedRuns(i);

	}

	pMainDoc->m_MStorage.Save();
/*	pMainDoc->m_MStorage.Reset();
	pMainDoc->m_MStorage.OpenFile(pMainDoc->GetMBinFileName());
	itotrun = pMainDoc->MR_Get_TotalNumberofRuns();
	ifiles = pMainDoc->m_MStorage.GetNumberOfAddedFiles(pMainDoc->GetMBinFileName());

	MultiStorage *pStorage;
	pStorage = new MultiStorage();
	pStorage->Init(pMainDoc->m_MStorage.m_pNewMap);
//	pMainDoc->UnLockMultiStorageFile();
	pStorage->m_Lock = false;
	ifiles = pStorage->GetNumberOfAddedFiles(pMainDoc->GetMBinFileName());
	delete pStorage;
	*/



	//pDocs[0]->WriteDocFile();

	return pDocs.size() - 1;
}



int main(int argc, char* argv[])
{
	SimArchive arch;
	CommonModelInfo info;
	GetSoilStorage m_MascurSois;
	PFCurve curve;
	FunctionProperties func;
	Register reg;

	size_t nproc;
#ifdef MS_CODE
	SYSTEM_INFO siSysInfo;
	GetSystemInfo(&siSysInfo);
	nproc=siSysInfo.dwNumberOfProcessors;
#else
    nproc = std::thread::hardware_concurrency();
#endif


	string filename;
    string str;
    vector<string> input_flag;
    for(int iarg=1; iarg<argc; iarg++) {
        string out;
        size_t i=0;
        str.clear();
        str.assign(argv[iarg]);

		while (str.size() > 0)
            switch (str[1]) {
                case 't':
                    if (str.find(' ') != string::npos) {
                        out = str.substr(2, str.find(' ') - 1);
                        str = str.substr(str.find(' '));

                    } else {
                        out = str.substr(2);
                        str.clear();
                    }
                    if (out.size() > 0) {
                        nproc = FUtil::AtoInt(out);
                    }
                    break;
                case 'i':
                    if (str.find(' ') != string::npos) {
                        out = str.substr(2, str.find(' ') - 1);
                        str = str.substr(str.find(' '));

                    } else {
                        out = str.substr(1);
                        str.clear();
                    }
                    input_flag.push_back(out);
                    break;
                case 'f':
                    if (str.find(' ') != string::npos) {
                        out = str.substr(2, str.find(' ') - 1);
                        str = str.substr(str.find(' '));

                    } else {
                        out = str.substr(2);
                        str.clear();
                    }
                    filename = out;
                    break;
                case 'n':
                    if (str.find(' ') != string::npos) {
                        out = str.substr(2, str.find(' ') - 1);
                        str = str.substr(str.find(' '));
                    } else {
                        out = str.substr(2);
                        str.clear();
                    }
                    auto runno = out;
                    if (runno.size() > 0) {
                        int no = FUtil::AtoInt(runno);
                        FUtil::WriteProfileInt("SimulationRunNo", no);
                    }
                    break;
            }
	}
	if(argc<=1) {
		char *buf;
#ifdef MS_CODE
		buf = _getcwd(NULL, 0);
#else
		buf =getcwd(NULL, 0);
#endif

		string path = buf;

        path += PATH_SEP;

        FUtil::WriteProfileStringStd("WorkingDirectory", path);
		FUtil::WriteProfileStringStd("UserDirectory", path);

		str = "TestRun.Sim";
		std::cout << "Specify file name : ";
		std::cin >> str;

        if (str.rfind(PATH_SEP) == string::npos)
        {
			str = path + str;
		}
		else {
			path = str.substr(0, str.rfind(PATH_SEP) + 1);
			FUtil::WriteProfileStringStd("UserDirectory", path);
		}

		filename = str;
		string runno;
		std::cout << "Specify Run No for first run:";
		std::cin >> runno;
		if (runno.size() > 0) {
			int no = FUtil::AtoInt(runno);
			FUtil::WriteProfileInt("SimulationRunNo", no);
		}
         string text;
         std::cout << "You can make up to " + FUtil::STD_ItoAscii(nproc) + " parallel runs" + "\n";
         std::cout << "Please give a new number if you want less runs:";
         std::cin >> text;
         if (text.size() > 0) {
             nproc = FUtil::AtoInt(text);
         }
     }


	vector<Doc*> pDocs;
	std::vector < std::thread > mythreads;
    mythreads.resize(nproc);
	for (size_t i = 0; i < mythreads.size(); i++) {

		Doc *pDoc = CreateDoc(i, filename);
		if(pDoc== nullptr) {
			std::cout << "Something went wrong when reading your file:";
			std::cout << filename;
			std::cin >> str;
			return 0;
		}
		pDocs.push_back(pDoc);
		bool Multi = MakeMulti(i, pDoc);
		mythreads[i] = thread(SimProc, i, pDoc, Multi);
	}

	for (size_t i = 0; i < mythreads.size(); i++) {
		mythreads[i].join();
	}
	std::cout << "All Runs have now been completed \n";

	if (pDocs.size()>1) {
		size_t num_merge = MergeAllFiles(pDocs);
	}

	while (pDocs.size()>0) {
		delete pDocs.at(0);
		pDocs.erase(pDocs.begin());
	}
	std::cout << "Hopefully you have good results ! - Use CoupModel windows interface to attach files and evaluate ! ";
	std::cin >> str;
	return 0;
}

