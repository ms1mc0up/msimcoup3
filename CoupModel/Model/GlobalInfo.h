#pragma once

#include "../std.h"


class GlobalInfo {
	public:
	GlobalInfo();
	~GlobalInfo();
	void AddModule(string, void*);
	void* GetModulePtr(string);
	

	 bool StartStateValue;
	 map<string, void*> ptrModule;







};
