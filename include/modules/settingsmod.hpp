#pragma once

#include "shared/module.hpp"
#include <memory>
namespace acpl{
namespace modules{
class SettingsModule : public shared::Module {
	public:
	MODULE_DEF(acpl::modules::SettingsModule, "Settings")
	PROPERTIES_FIRST_DECL
	SettingsModule();
	~SettingsModule();
	bool run() override;
	
	private:
	class Manager;
	class MainComponent;
	std::unique_ptr<Manager> manager;


};

}}