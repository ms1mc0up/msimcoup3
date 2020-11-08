#include "../std.h"
#include "./CommonModelInfo.h"
#include "../ModelTypes/SimB.h"
CommonModelInfo::CommonModelInfo() {

	m_TypeNames.clear();
	m_TypeNames.resize(13);
	
	m_TypeNames[0]="Switches";
	m_TypeNames[1]="Parameters";
	m_TypeNames[2]="Tables";
	m_TypeNames[3]="Table Parameters";
	m_TypeNames[4]="State Variables";
	m_TypeNames[5]="Flow Variables";
	m_TypeNames[6]="Auxiliary Variables";
	m_TypeNames[7]="Driving Variables";
	m_TypeNames[8]="Files";
	m_TypeNames[9]="NE";
	m_TypeNames[10]="DataBase";
	m_TypeNames[11]="RunInfo";
	m_TypeNames[12]="Validation";

	m_ElementNames.resize(45);
	m_ElementNames[0]="Water";
	m_ElementNames[1]="Heat";
	m_ElementNames[2]="Carbon";
	m_ElementNames[3] ="Nitrogen";
	m_ElementNames[4] ="Phosphorus";
	m_ElementNames[5]="Radiation";
	m_ElementNames[6]="Snow";
	m_ElementNames[7]="Salt";
	m_ElementNames[8]="Methane";
	m_ElementNames[9]="Vapour";
	m_ElementNames[10]="General";
	m_ElementNames[11]="Temperature";
	m_ElementNames[12]="Water Potential";
	m_ElementNames[13]="Atmosphere";
	m_ElementNames[14]="Air";
	m_ElementNames[15]="Precipitation";
	m_ElementNames[16]="Climate";
	m_ElementNames[17]="Organic";
	m_ElementNames[18]="Dissolved Carbon";
	m_ElementNames[19]="Dissolved Nitrogen";
	m_ElementNames[20] = "Dissolved Phosphorus";
	m_ElementNames[21]="Mineral N";
	m_ElementNames[22] = "Mineral P";
	m_ElementNames[23]="Ammonium";
	m_ElementNames[24]="Nitrate";
	m_ElementNames[25]="Mycorrhize";
	m_ElementNames[26]="Denitrifiers";
	m_ElementNames[27]="Nitrofiers";
	m_ElementNames[28]="Anarobic gas";
	m_ElementNames[29]="Oxygen";
	m_ElementNames[30]="Manure";
	m_ElementNames[31]="Plant Shape";
	m_ElementNames[32]="Distance";
	m_ElementNames[33] = "CO2 SOIL";
	m_ElementNames[34] = "Time";
	m_ElementNames[35] = "Area";
	m_ElementNames[36] = "Plant Stress";
	m_ElementNames[37] = "Wind Speed";
	m_ElementNames[38] = "Global Radiation";
	m_ElementNames[39] = "LongWave Radiation";
	m_ElementNames[40] = "Humidity";
	m_ElementNames[41] = "GreenHouseGas";
	m_ElementNames[42] = "Plant Biomass";
	m_ElementNames[43] = "Nitric Oxide";
	m_ElementNames[44] = "Nitrous Oxide";
	


	m_FysProcessNames.resize(46);
	m_FysProcessNames[0]="Root Water Uptake";
	m_FysProcessNames[1]="Evapotranspiration";
	m_FysProcessNames[2]="Transpiration";
	m_FysProcessNames[3]="Soil Evaporation";
	m_FysProcessNames[4]="Potential Transpiration";
	m_FysProcessNames[5]="Interception";
	m_FysProcessNames[6]="Turbulence";
	m_FysProcessNames[7]="Surface Runoff";
	m_FysProcessNames[8]="Aerodynamic Resistance";
	m_FysProcessNames[9]="Snow Melting";
	m_FysProcessNames[10]="Redistribution";
	m_FysProcessNames[11]="Storage";
	m_FysProcessNames[12]="Transport";
	m_FysProcessNames[13]="Infiltration";
	m_FysProcessNames[14]="Drainage";
	m_FysProcessNames[15]="Percolation";
	m_FysProcessNames[16]="Runoff";
	m_FysProcessNames[17]="biotic process";
	m_FysProcessNames[18]="Initial Flow";
	m_FysProcessNames[19]="Shrinking";
	m_FysProcessNames[20]="Physical Shape";
	m_FysProcessNames[21]="Heat Extraction";
	m_FysProcessNames[22]="Heat Pipe";
	m_FysProcessNames[23]="Heat Source";
	m_FysProcessNames[24]="Lower Boundary";
	m_FysProcessNames[25]="Upper Boundary";
	m_FysProcessNames[26]="Water Source";
	m_FysProcessNames[27]="Salt Source";
	m_FysProcessNames[28]="Enerby Balance Partitioning";
	m_FysProcessNames[29]="Heat Conduction";
	m_FysProcessNames[30]="Heat Convection";
	m_FysProcessNames[31]="Technical Regulation";
	m_FysProcessNames[32]="Emission";
	m_FysProcessNames[33]="Adsorption";
	m_FysProcessNames[34]="Reflection";
	m_FysProcessNames[35]="Mass Balance";
	m_FysProcessNames[36]="Measurement Sensor";
	m_FysProcessNames[37]="Geographic Position";
	m_FysProcessNames[38]="Frost Bondary";
	m_FysProcessNames[39]="Hysteresis";
	m_FysProcessNames[40]="Irrigation";
	m_FysProcessNames[41]="Budget";
	m_FysProcessNames[42]="Geometric";
	m_FysProcessNames[43]="Scattering";
	m_FysProcessNames[44] = "Soil Freezing";
	m_FysProcessNames[45] = "Diffusion";

	m_BioProcessNames.resize(39);
	m_BioProcessNames[0]="Decomposition";
	m_BioProcessNames[1]="Mineralisation";
	m_BioProcessNames[2]="Nitrification";
	m_BioProcessNames[3]="Denitrification";
	m_BioProcessNames[4]="Respiration";
	m_BioProcessNames[5]="Production";
	m_BioProcessNames[6]="Oxidation";
	m_BioProcessNames[7]="Re Allocation";
	m_BioProcessNames[8]="Phenology";
	m_BioProcessNames[9] = "Soil Gas Dynamics";
	m_BioProcessNames[10]="Photosynthesis";
	m_BioProcessNames[11]="Litterfall";
	m_BioProcessNames[12]="abiotic process";
	m_BioProcessNames[13]="Initial NC Conditions";
	m_BioProcessNames[14] = "Initial P Conditions";
	m_BioProcessNames[15]="Forcing Biological Conditions";
	m_BioProcessNames[16]="Organic Transformation";
	m_BioProcessNames[17]="Balancing Ratio";
	m_BioProcessNames[18]="Concentration";
	m_BioProcessNames[19]="Organic Plant Uptake";
	m_BioProcessNames[20]="Plant Nitrogen Uptake";
	m_BioProcessNames[21] = "Plant Phosphorus Uptake";
	m_BioProcessNames[22]="Sorption";
	m_BioProcessNames[23]="Harvest";
	m_BioProcessNames[24]="Grazing";
	m_BioProcessNames[25]="Mycon Nitrogen Uptake";
	m_BioProcessNames[26]="Soil mixing";
	m_BioProcessNames[27]="Emergence";
	m_BioProcessNames[28]="Plant Management";
	m_BioProcessNames[29]="Nitrogen Fixation";
	m_BioProcessNames[30]="Living";
	m_BioProcessNames[31] ="Non Living";
	m_BioProcessNames[32] ="Growth";
	m_BioProcessNames[33] ="WoodEating";
	m_BioProcessNames[34] = "Weathering";
	m_BioProcessNames[35] = "Methane Oxidation";
	m_BioProcessNames[36] = "Methane Production";
	m_BioProcessNames[37] = "Methane Transport";
	m_BioProcessNames[38] = "Gas Equilibrium";




	for(size_t i=0; i<m_TypeNames.size();i++)
		m_TypeMap[m_TypeNames[i]]=m_TypeMap.size();

	m_GroupNames.resize(38);
	m_GroupNames[0]="Hidden";
	m_GroupNames[1]="Technical";
	m_GroupNames[2]="Model Structure";
	m_GroupNames[3]="Numerical";
	m_GroupNames[4]="Soil Profile";
	m_GroupNames[5]="Soil Hydraulic";
	m_GroupNames[6]="Soil Thermal";
	m_GroupNames[7]="Meteorological Data";

	m_GroupNames[8]="Irrigation";
	m_GroupNames[9]="Plant";
	m_GroupNames[10]="Interception";
	m_GroupNames[11]="Potential Transpiration";	
	m_GroupNames[12]="Radiation properties";	
	m_GroupNames[13]="Soil evaporation";	
	m_GroupNames[14]="SnowPack";

	m_GroupNames[15]="Soil frost";	
	m_GroupNames[16]="Soil heat flows";	
	m_GroupNames[17]="Soil heat pump";	
	m_GroupNames[18]="Soil water flows";	
	m_GroupNames[19]="Surface water";	
	m_GroupNames[20]="Water uptake";	
	m_GroupNames[21]="Drainage and deep percolation";	
	m_GroupNames[22]="Salt Tracer";	
	m_GroupNames[23]="Additional Variables";	
	m_GroupNames[24]="Plant Growth";	
	m_GroupNames[25]="Soil organic processes";
	m_GroupNames[26]="Soil mineral N processes";
	m_GroupNames[27]="Abiotic driving variables";
	m_GroupNames[28]="Common abiotic responses";
	m_GroupNames[29]="External N inputs";
	m_GroupNames[30]="Soil Management";
	m_GroupNames[31]="Additional Biotic Variables";
	m_GroupNames[32]="Gas processes";
	m_GroupNames[33]="Minteq";
	m_GroupNames[34]="HBV";
	m_GroupNames[35]="Validation";
	m_GroupNames[36] = "Soil mineral P Processes";
	m_GroupNames[37] = "External P Inputs";

	m_GroupMap.clear();
	for(size_t i=0; i<m_GroupNames.size();i++)
		m_GroupMap[m_GroupNames[i]]=i;

	m_GroupCategoryNames.resize(9);

	m_GroupCategoryNames[0]="All Modules";
	m_GroupCategoryNames[1]="Atmospheric Related Processes";
	m_GroupCategoryNames[2]="Plant Water Processes";
	m_GroupCategoryNames[3]="Soil Heat Processes";
	m_GroupCategoryNames[4]="Soil Water Processes";
	m_GroupCategoryNames[5]="Nitrogen and Carbon";
	m_GroupCategoryNames[6]="Nitrogen and Carbon - Below Ground";
	m_GroupCategoryNames[7]="Model Structure";
	m_GroupCategoryNames[8]="Model Facilities";

	m_GroupCategoryNo.insert(pair<size_t,size_t>(0,0));
	m_GroupCategoryNo.insert(pair<size_t,size_t>(1,8));
	m_GroupCategoryNo.insert(pair<size_t,size_t>(2,7));
	m_GroupCategoryNo.insert(pair<size_t,size_t>(3,8));
	m_GroupCategoryNo.insert(pair<size_t,size_t>(4,7));
	m_GroupCategoryNo.insert(pair<size_t,size_t>(5,4));
	m_GroupCategoryNo.insert(pair<size_t,size_t>(6,3));
	m_GroupCategoryNo.insert(pair<size_t,size_t>(7,1));
	m_GroupCategoryNo.insert(pair<size_t,size_t>(8,8));
	m_GroupCategoryNo.insert(pair<size_t,size_t>(9,2));
	m_GroupCategoryNo.insert(pair<size_t,size_t>(10,2));
	m_GroupCategoryNo.insert(pair<size_t,size_t>(11,2));
	m_GroupCategoryNo.insert(pair<size_t,size_t>(12,1));
	m_GroupCategoryNo.insert(pair<size_t,size_t>(13,1));
	m_GroupCategoryNo.insert(pair<size_t,size_t>(14,1));
	m_GroupCategoryNo.insert(pair<size_t,size_t>(15,3));
	m_GroupCategoryNo.insert(pair<size_t,size_t>(16,3));
	m_GroupCategoryNo.insert(pair<size_t,size_t>(17,3));
	m_GroupCategoryNo.insert(pair<size_t,size_t>(18,4));
	m_GroupCategoryNo.insert(pair<size_t,size_t>(19,4));
	m_GroupCategoryNo.insert(pair<size_t,size_t>(20,2));
	m_GroupCategoryNo.insert(pair<size_t,size_t>(21,4));
	m_GroupCategoryNo.insert(pair<size_t,size_t>(22,2));
	m_GroupCategoryNo.insert(pair<size_t,size_t>(23,7));
	m_GroupCategoryNo.insert(pair<size_t,size_t>(24,5));
	m_GroupCategoryNo.insert(pair<size_t,size_t>(25,6));
	m_GroupCategoryNo.insert(pair<size_t,size_t>(26,6));
	m_GroupCategoryNo.insert(pair<size_t,size_t>(27,6));
	m_GroupCategoryNo.insert(pair<size_t,size_t>(28,7));
	m_GroupCategoryNo.insert(pair<size_t,size_t>(29,5));
	m_GroupCategoryNo.insert(pair<size_t,size_t>(30,6));
	m_GroupCategoryNo.insert(pair<size_t,size_t>(31,7));
	m_GroupCategoryNo.insert(pair<size_t,size_t>(32,6));
	m_GroupCategoryNo.insert(pair<size_t,size_t>(33,6));
	m_GroupCategoryNo.insert(pair<size_t,size_t>(34,0));
	m_GroupCategoryNo.insert(pair<size_t,size_t>(35,0));
	m_GroupCategoryNo.insert(pair<size_t, size_t>(36, 6));
	m_GroupCategoryNo.insert(pair<size_t, size_t>(37, 5));


		m_GroupCategory.insert(pair<string,int>("Technical",8));
		m_GroupCategory.insert(pair<string,int>("Model Structure",7));
		m_GroupCategory.insert(pair<string,int>("Numerical",8));
		m_GroupCategory.insert(pair<string,int>("Soil Profile",7));
		m_GroupCategory.insert(pair<string,int>("Soil Hydraulic",4));
		m_GroupCategory.insert(pair<string,int>("Soil Thermal",3));
		m_GroupCategory.insert(pair<string,int>("Meteorological Data",1));

		m_GroupCategory.insert(pair<string,int>("Irrigation" ,4));
		m_GroupCategory.insert(pair<string,int>( "Plant",2));
		m_GroupCategory.insert(pair<string,int>( "Interception",2));
		m_GroupCategory.insert(pair<string,int>("Potential Transpiration" ,2));
		m_GroupCategory.insert(pair<string,int>("Radiation properties" ,1));
		m_GroupCategory.insert(pair<string,int>("Soil evaporation" ,1));
		m_GroupCategory.insert(pair<string,int>( "SnowPack",1));

		m_GroupCategory.insert(pair<string,int>( "Soil frost",3));
		m_GroupCategory.insert(pair<string,int>("Soil heat flows" ,3));
		m_GroupCategory.insert(pair<string,int>("Soil heat pump" ,3));
		m_GroupCategory.insert(pair<string,int>("Soil water flows" ,4));
		m_GroupCategory.insert(pair<string,int>( "Surface water",4));
		m_GroupCategory.insert(pair<string,int>("Water uptake" ,2));

		m_GroupCategory.insert(pair<string,int>("Drainage and deep percolation" ,4));
		m_GroupCategory.insert(pair<string,int>("Salt Tracer" ,4));
		m_GroupCategory.insert(pair<string,int>("Additional Variables" ,7));
		m_GroupCategory.insert(pair<string,int>( "Plant Growth",5));
		m_GroupCategory.insert(pair<string,int>("Soil organic processes",6));
		m_GroupCategory.insert(pair<string, int>("Soil mineral P processes",6));
		m_GroupCategory.insert(pair<string,int>("Soil mineral N processes",6));
		m_GroupCategory.insert(pair<string,int>( "Abiotic driving variables",7));
		m_GroupCategory.insert(pair<string,int>("Common abiotic responses" ,7));
		m_GroupCategory.insert(pair<string,int>( "External N inputs" ,5));
		m_GroupCategory.insert(pair<string, int>("External P inputs", 5));
		m_GroupCategory.insert(pair<string,int>( "Soil Management" ,6));
		m_GroupCategory.insert(pair<string,int>( "Additional Biotic Variables" ,7));
		m_GroupCategory.insert(pair<string,int>( "Gas processes" ,6));
		m_GroupCategory.insert(pair<string,int>( "Minteq" ,6));
		m_GroupCategory.insert(pair<string,int>( "HBV" ,4));
		m_GroupCategory.insert(pair<string,int>( "Validation" ,0));
		m_RunInfo.StartMode=true;
		m_RunInfo.ReadyToSimulate=true;
		m_RunInfo.Running=false;
		m_RunInfo.MultiRunning=false;
		m_RunInfo.ViewStatus=0;
		size_t count = 0;
		map <string, elements> SortedListElementNames;

		for_each(m_ElementNames.begin(), m_ElementNames.end(), [&](string &ElementStr) {
			SortedListElementNames.insert(pair<string, elements>(ElementStr, elements(count)));
			m_ElementNamesMap.insert(pair<elements, string>(elements(count), ElementStr));
			count++;
		});
		m_NamesElementMap = SortedListElementNames;
		for (auto it = SortedListElementNames.begin(); it != SortedListElementNames.end(); ++it) {
			m_ElementSorted.push_back((*it).first);
		}
		map <string, fysprocess> SortedListPhysProcNames;
		count = 0;
		for_each(m_FysProcessNames.begin(), m_FysProcessNames.end(), [&](string& PhysProc) {
			SortedListPhysProcNames.insert(pair<string, enum fysprocess>(PhysProc, fysprocess(count)));
			m_PhysProcNamesMap.insert(pair<enum fysprocess, string>(fysprocess(count), PhysProc));
			count++;
		});
		m_NamesFysProcMap = SortedListPhysProcNames;
		for (auto it = SortedListElementNames.begin(); it != SortedListElementNames.end(); ++it) {
			m_FysProcessNamesSorted.push_back((*it).first);
		}
		map <string, bioprocess> SortedBiolProcessNames;
		count = 0;
		for_each(m_BioProcessNames.begin(), m_BioProcessNames.end(), [&](string& Names) {
			SortedBiolProcessNames.insert(pair<string, enum bioprocess>(Names,bioprocess(count)));
			m_BioProcNamesMap.insert(pair<bioprocess, string>(bioprocess(count), Names));
			count++;
		});
		m_NamesBioProcMap = SortedBiolProcessNames;
		for (auto it = SortedBiolProcessNames.begin(); it != SortedBiolProcessNames.end(); ++it) {
			m_BioProcessNamesSorted.push_back((*it).first);
		}


}
CommonModelInfo::~CommonModelInfo() {

}
size_t CommonModelInfo::GetTypeIndex(string str) {
	auto it=m_TypeMap.find(str);
	if(it!=m_TypeMap.end())
		return it->second;
	else
		return string::npos;
}
size_t CommonModelInfo::GetGroupIndex(string str) {
	auto it=m_GroupMap.find(str);
	if(it!=m_GroupMap.end())
		return it->second;
	else
		return string::npos;
}
int CommonModelInfo::GetGroupCategory(string str) {
	auto it=m_GroupCategory.find(str);
	if(it!=m_GroupCategory.end())
		return it->second;
	else
		return string::npos;
}
/*size_t CommonModelInfo::GetGroupCategory(size_t groupno) {
	auto it = m_GroupCategoryNo.find(groupno);
	if (it != m_GroupCategoryNo.end())
		return it->second;
	else
		return string::npos;
}*/

vector<string> CommonModelInfo::GetSortedListofElements()
{
	return m_ElementSorted;
}
vector<string> CommonModelInfo::GetSortedListofElements(vector<elements> velements) {
	vector<string> out;
	for (elements e : velements) {
		size_t koll = e;
		out.push_back(m_ElementNames[koll]);
	}

	return out;
}
vector<string> CommonModelInfo::GetSortedListofPhysProc()
{
	return m_FysProcessNamesSorted;
}
vector<string> CommonModelInfo::GetSortedListofPhysProc(vector<fysprocess> v_process) {
	vector<string> out;
	for (fysprocess e : v_process) {
		size_t koll = e;
		out.push_back(m_FysProcessNames[koll]);
	}

	return out;
}
vector<string> CommonModelInfo::GetSortedListofBiolProc()
{
	return m_BioProcessNamesSorted;
}
vector<string> CommonModelInfo::GetSortedListofBiolProc(vector<bioprocess> v_process) {
	vector<string> out;
	for (bioprocess e : v_process) {
		size_t koll = e;
		out.push_back(m_BioProcessNames[koll]);
	}

	return out;
}

string CommonModelInfo::GetStringOfElement(elements elem) {
	auto it = m_ElementNamesMap.find(elem);
	if (it != m_ElementNamesMap.end())
		return (*it).second;
	else
		return "";
}
elements CommonModelInfo::GetElementFromString(string str) {
	auto it = m_NamesElementMap.find(str);
	if (it != m_NamesElementMap.end())
		return (*it).second;
	else
		return GENERAL;
}
fysprocess CommonModelInfo::GetPhysProcFromString(string str) {
	auto it = m_NamesFysProcMap.find(str);
	if (it != m_NamesFysProcMap.end())
		return (*it).second;
	else
		return NOFPROC;
}bioprocess CommonModelInfo::GetBioProcFromString(string str) {
	auto it = m_NamesBioProcMap.find(str);
	if (it != m_NamesBioProcMap.end())
		return (*it).second;
	else
		return NOBPROC;
}
vector<SimB*> CommonModelInfo::SortSimB_ByGroup(vector<SimB*> v_in) {
	vector<SimB*> v_out;
	multimap<string, SimB*> SortMap;
	for(size_t i=0; i<v_in.size(); i++) {	
		SortMap.insert(pair<string, SimB*>(v_in[i]->GetGroup(),v_in[i]));
	}

	for(auto it=SortMap.begin(); it!=SortMap.end(); ++it) {
		v_out.push_back((*it).second);
	}
	return v_out;

}
vector<SimB*> CommonModelInfo::SortSimB_ByCategory(vector<SimB*> v_in) {

	vector<SimB*> v_out;
	multimap<int, SimB*> SortMap;
	for(size_t i=0; i<v_in.size(); i++) {	
		SortMap.insert(pair<int, SimB*>(GetGroupCategory(v_in[i]->GetGroup()),v_in[i]));
	}

	for(auto it=SortMap.begin(); it!=SortMap.end(); ++it) {
		v_out.push_back((*it).second);
	}
	return v_out;
}

