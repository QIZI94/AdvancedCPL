#pragma once

#include "tools/property.hpp"
namespace acpl{
struct GlobalConfig : public tools::PropertiesHolder{
public:
	static GlobalConfig& Get(){
		static GlobalConfig globalCfg;
		return globalCfg;
	}
	static const GlobalConfig& GetConst(){
		return Get();
	}

public:
	bool debugging = false;
	bool debugLog = false;


	PROPERTIES_FIRST(
		tools::Property("Debugging", debugging),
		tools::Property("Debug Log", debugLog),
	)

};
}