#pragma once
#include "tools/property.hpp"
#include "tools/classinfo.hpp"
#include "tools/components.hpp"

#define COMPONENT_DEF(CLASS, PARENT)\
using Self = CLASS;\
using Super = PARENT;\
auto& self(){ return *this;}\
auto& super() {return *static_cast<Super*>(this);}\
std::string_view className() const override{return #CLASS;}

namespace acpl{
namespace modules{
namespace shared{
	
struct ModuleComponent : public tools::StateComponent, public tools::PropertiesHolder, public tools::ClassInfo {
	COMPONENT_DEF(acpl::modules::shared::ModuleComponent, tools::StateComponent)
	bool presentProperties(const tools::Property::Visitor&) override{return true;}
	bool presentProperties(const tools::Property::Visitor&) const override{return true;}
};

}}}