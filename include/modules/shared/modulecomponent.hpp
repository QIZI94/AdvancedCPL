#pragma once
#include "tools/property.hpp"
#include "tools/classinfo.hpp"
#include "tools/components.hpp"

#define COMPONENT_DEF(CLASS, PARENT)\
using Self = CLASS;\
using Super = PARENT;\
auto& self(){ return *this;}\
auto& super() {return *static_cast<Super*>(this);}\
std::string_view className() const override{using TestType = CLASS;return #CLASS;}

namespace acpl{
namespace modules{
namespace shared{
	
struct ModuleComponent : public tools::StateComponent, public tools::PropertiesHolder, public tools::ClassInfo {
	COMPONENT_DEF(acpl::modules::shared::ModuleComponent, tools::StateComponent)
	bool presentProperties(const tools::Property::Visitor&) override{return true;}
	bool presentProperties(const tools::Property::Visitor&) const override{return true;}

	virtual std::string toString(std::string separator = ", ") const {
		std::string ret;
		ret.append(className());
		
		ret.append(" {");
		tools::PropertiesToString(*this, ret, separator);
		if(ret.size() >=  separator.size()){
			ret.erase(ret.size() - separator.size(), separator.size());
		}
		ret.push_back('}');
		return ret;
	}
};

}}}