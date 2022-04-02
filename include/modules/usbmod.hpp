#pragma once

#include "shared/module.hpp"
#include <memory>
namespace acpl{
namespace modules{
class UsbModule : public shared::Module {
	public:
	MODULE_DEF(acpl::modules::UsbModule, "USB")
	PROPERTIES_DECL

		

	UsbModule();
	~UsbModule();
	bool run() override;

	private:
	class Manager;
	class MainComponent;
	std::unique_ptr<Manager> manager;


};

}}