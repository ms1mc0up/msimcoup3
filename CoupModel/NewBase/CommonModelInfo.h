#pragma once

#include "../Model/Units.h"
struct RunStatusOfDocumentInfo
{
	bool StartMode;
	int ViewStatus;
	bool Running;
	bool MultiRunning;
	bool ReadyToSimulate;
	bool NoSingleRunSimFile;

};
class Sim;
class SimB;
class CommonModelInfo : public Units
{ 
public:
	CommonModelInfo();
	~CommonModelInfo();
	size_t GetTypeIndex(std::string str);
	size_t GetGroupIndex(std::string str);
	size_t GetGroupCategoryNo(size_t index) {
		auto it = m_GroupCategoryNo.find(index);
		if (it != m_GroupCategoryNo.end())
			return it->second;
		else
			return string::npos;
;};

	std::string GetGroupCategoryNames(size_t index) {if(index<m_GroupCategoryNames.size()) return m_GroupCategoryNames[index];return "All Modules";};
	std::string GetGroupNames(size_t index) {if(index<m_GroupNames.size()) return m_GroupNames[index];return "";};
	std::string GetVarType(size_t ityp) { if(ityp<m_TypeNames.size()) return m_TypeNames[ityp]; return "";};
	std::string GetFysProcName(size_t index) {if(index<m_FysProcessNames.size()) return m_FysProcessNames[index]; else return "";};
	std::string GetBioProcName(size_t index) {if(index<m_BioProcessNames.size()) return m_BioProcessNames[index]; else return "";};
	std::string ElementName(size_t index) {if(index<m_ElementNames.size()) return m_ElementNames[index]; else return "";};

	int GetGroupCategory(std::string str);
	int GetGroupCategoryNo(int moduleNo) {
		auto it = m_GroupCategoryNo.find(moduleNo);
		if (it != m_GroupCategoryNo.end())
			return it->second;
		else
			return -1;
		//return int(GetGroupCategoryNo(size_t(moduleNo)));
	
	
	};
	void SetCommandLineMode(bool value) {m_CommandInfo=value;};
	bool GetCommandLineMode() {return m_CommandInfo;};
	bool IsRunning() {return m_RunInfo.Running;};
	void SetRunning(bool value) {m_RunInfo.Running=value;};
	bool IsMultiRunning() {return m_RunInfo.MultiRunning;};
	void SetMultiRunning(bool value) {m_RunInfo.MultiRunning=value;};
	bool IsStartMode() {return m_RunInfo.StartMode;};
	void SetStartMode(bool value) {m_RunInfo.StartMode=value;};
	bool IsReady() {return m_RunInfo.ReadyToSimulate;};
	void SetReady(bool value) {m_RunInfo.ReadyToSimulate=value;};
	void SetRunDoc(Sim* pSim) {m_pRunDoc=pSim;};
	int GetViewStatus() {return m_RunInfo.ViewStatus;};
	void SetViewStatus(int value) {m_RunInfo.ViewStatus=value;};
	bool GetNoSingleRun() {return m_RunInfo.NoSingleRunSimFile;};
	void SetNoSingleRun(bool value) {m_RunInfo.NoSingleRunSimFile=value;};
	std::vector<std::string> GetSortedListofElements();
	std::vector<std::string> GetSortedListofElements(vector<elements> v_elements);

	std::vector<std::string> GetSortedListofPhysProc();
	std::vector<std::string> GetSortedListofPhysProc(vector<fysprocess> v_fysprocess);

	std::vector<std::string> GetSortedListofBiolProc();
	std::vector<std::string> GetSortedListofBiolProc(vector<bioprocess> v_bioprocess);






	std::vector<std::string> GetSortedListofGroupNames() { return m_GroupNamesSorted; };
	std::vector<std::string> GetGroupNames() {
		vector<string> out;
		for (string name : m_GroupNames) {
			out.push_back(name);
		}
		return out;
	}
	size_t GetNumberOfElements() { return m_ElementSorted.size(); };
	std::string GetStringOfElement(elements elem);
	elements GetElementFromString(std::string str);
	fysprocess GetPhysProcFromString(std::string str);
	bioprocess GetBioProcFromString(std::string str);



	Sim *GetRunDoc() {return m_pRunDoc;};
	std::vector<SimB*> SortSimB_ByGroup(vector<SimB*> v);
	std::vector<SimB*> SortSimB_ByCategory(vector<SimB*> v);


private:
	std::map <std::string, size_t> m_TypeMap, m_GroupMap;
	std::map <std::string, int> m_GroupCategory;
	std::map <size_t, int> m_GroupCategoryNo;
	std::vector<std::string> m_TypeNames, m_GroupNames, m_GroupCategoryNames;
	std::vector<std::string> m_GroupNamesSorted;
	std::vector<std::string> m_FysProcessNames, m_BioProcessNames, m_ElementNames, m_ElementSorted;
	std::vector<std::string> m_FysProcessNamesSorted, m_BioProcessNamesSorted;
	RunStatusOfDocumentInfo m_RunInfo;
	
	std::map <elements, std::string> m_ElementNamesMap;
	std::map <std::string,elements> m_NamesElementMap;

	std::map <fysprocess, std::string> m_PhysProcNamesMap;
	std::map <std::string, fysprocess> m_NamesFysProcMap;

	std::map <bioprocess, std::string> m_BioProcNamesMap;
	std::map <std::string, bioprocess> m_NamesBioProcMap;


	bool m_CommandInfo;
	Sim *m_pRunDoc;
};
