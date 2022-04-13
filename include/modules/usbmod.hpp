#pragma once

#include "shared/module.hpp"
#include <memory>
namespace acpl{
namespace modules{
class UsbModule : public shared::Module {
	public:
	MODULE_DEF(acpl::modules::UsbModule, "USB")

	UsbModule();

	private:
	class MainComponent;
};

}}