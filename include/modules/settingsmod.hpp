#pragma once

#include "shared/module.hpp"
#include <memory>
namespace acpl{
namespace modules{
class SettingsModule : public shared::Module {
	public:
	MODULE_DEF(acpl::modules::SettingsModule, "Settings")
	PROPERTIES_DECL
	SettingsModule();
	
	private:
	class Manager;
	class MainComponent;

};

}}