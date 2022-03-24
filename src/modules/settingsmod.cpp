#include "modules/settingsmod.hpp"
#include "globalconfig.hpp"


namespace acpl{
namespace modules{

GlobalConfig& GlobalCFG = GlobalConfig::Get();
PROPERTIES_FIRST_IMPL(SettingsModule, 
	Property("FirstTest", GlobalCFG.firstTest)

)


SettingsModule::SettingsModule(){
	
}


bool SettingsModule::run(){
	return isAskedToStop();
}



}}