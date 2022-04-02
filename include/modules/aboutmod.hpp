#pragma once
#include "shared/module.hpp"

namespace acpl{
namespace modules{
class AboutModule : public shared::Module{
	public:
	MODULE_DEF(acpl::modules::AboutModule, "About")
	bool run() override { return isAskedToStop();}

	PROPERTIES(
		Property("About Module here"),
		Property("Description: ", std::string_view("Description goes here"))
	)
	
};

}}