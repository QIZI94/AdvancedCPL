#include "modules/shared/moduleinitialization.hpp"

#include "modules/aboutmod.hpp"
#include "modules/settingsmod.hpp"
#include "modules/usbmod.hpp"
namespace acpl{
namespace modules{
namespace shared{

ModuleUniqueList InitializeModules(){
    ModuleUniqueList ret;
	ret.emplace_back(std::make_unique<UsbModule>());
	ret.emplace_back(std::make_unique<SettingsModule>());
    ret.emplace_back(std::make_unique<AboutModule>());
	
    return ret;
}

}}}