#pragma once

#include "../std.h"
#include "../Util/ReadWriteTypes.h"

// ResidualsBase command target
class NewMap;

struct RESIDUAL_MEAN{
	vector<vector <float>> mean;
	vector<vector <float>> min;
	vector<vector <float>> max;
	vector<vector <float>> std;
	vector<vector <float>> p10;
	vector<vector <float>> p50;
	vector<vector <float>> p90;
};
struct POS_ENSEMBLE {
	bool valid;
	size_t pos;
	string name;
	bool EnsembleFileRead;
};

class ResidualsBase 
{
public:
	ResidualsBase();
	virtual ~ResidualsBase();
	bool EnsembleReadWrite(bool reading, string EnsembleName, string NewEnsembleName="", size_t ensembleno=string::npos);
	bool EnsembleReadWrite(bool reading, size_t ensembleno);

	bool ReadWrite_Header(bool reading);
	bool ReadWrite_PriorStat();
	bool ReadWrite_Ensemble(string EnsembleName);
	bool OpenResidualFile(bool reading=false);
	bool CloseResidualFile(bool reading);
	bool Residuals_ReadWrite_Header(bool reading);
	bool SetNewEnsembleByNumber(size_t ensembleno);
	bool AlloResVector();
	string GetXStatFileName(size_t ensembleno=string::npos);
	bool IsOpen(bool reading=true);
	bool EnableEstimatesMade(size_t enableno=0);
	bool m_IsUpdated, m_IsUpdated_Extra;
	bool ShiftEnsembleName(string oldname, string newname);
	void SetResidualBaseFileNumVarScaling(size_t NumVarScaling) { m_ScalingNumVarWithDim1 = NumVarScaling; };
	bool m_MeanDefined;

protected:
	NewMap *m_pNewMap;
	vector<float> m_Mean;
	RESIDUAL_MEAN m_MeanPrior;
	RESIDUAL_MEAN m_MeanPost;
	RESIDUAL_MEAN m_MeanTest;
	vector<POS_ENSEMBLE> m_Ensembles;
	vector<float> m_Residuals;
	size_t m_RecL, m_NumRec, m_NumVar, m_ScalingNumVarWithDim1;
	string m_FileName, m_ExtraFileName, m_SortedFileName;
	bool m_IsOpen, m_NewPrior;
	bool m_EnsembleFileRead;
	int m_FileNo;
	string m_NewEnsembleName;

private :
	ReadWriteTypes m_EnsembleResFile, m_ResidualFile, m_ExtraFile, m_SortedFile;
	bool meanValid;
	size_t m_MaxNumRec;
	std::streamsize m_FileSize;
	map<string, size_t> m_MapofEnsemblePositions;
	std::streampos m_PosToRead, m_PosToWrite;
	string m_EnsembleName;
	string m_TempOutFile;
	bool m_TwoFilesOpen;
	size_t m_SizeOfMeanVector;



};


