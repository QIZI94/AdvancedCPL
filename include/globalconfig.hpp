#pragma once

struct GlobalConfig{
public:
	static GlobalConfig& Get(){
		static GlobalConfig globalCfg;
		return globalCfg;
	}
	static const GlobalConfig& GetConst(){
		return Get();
	}

public:
	bool firstTest = false;


};