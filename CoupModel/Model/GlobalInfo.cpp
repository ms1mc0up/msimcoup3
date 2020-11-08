#include "GlobalInfo.h"

GlobalInfo::GlobalInfo() 
{
	 StartStateValue=true;


}
GlobalInfo::~GlobalInfo() 
{



}
void GlobalInfo::AddModule(string name, void* ptr)
{
	ptrModule.insert(pair<string, void*>(name, ptr));
}
void* GlobalInfo::GetModulePtr(string name) 
{

	map<string, void*>::iterator it;
	it=ptrModule.find(name);
	if(it!=ptrModule.end())
		return it->second;
	else
		return nullptr;

}