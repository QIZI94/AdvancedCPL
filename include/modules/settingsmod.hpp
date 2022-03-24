#pragma once

#include "shared/module.hpp"
namespace acpl{
namespace modules{
class SettingsModule : public shared::Module {
	public:
	MODULE_DEF(acpl::modules::SettingsModule, "Settings")
	PROPERTIES_FIRST_DECL
	SettingsModule();
	bool run() override;
};

}}