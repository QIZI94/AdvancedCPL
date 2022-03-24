#pragma once
#include <string_view>
#include <vector>

#include "tools/property.hpp"
#include "tools/classinfo.hpp"

#define MODULE_DEF(CLASS, DISPLAYNAME)\
	std::string_view className() const override{return #CLASS;}\
	std::string_view displayName() override{return DISPLAYNAME;}

namespace acpl{
namespace modules{
namespace shared{
struct Module : public tools::PropertiesHolder, public tools::ClassInfo{
	bool presentProperties(const tools::Property::Visitor&) override{return true;}
	bool presentProperties(const tools::Property::Visitor&) const override{return true;}
    std::string_view className() const override{ return "Module";}
	virtual std::string_view displayName() = 0;
//	virtual const std::vector<int>& getComponents() = 0;
	virtual bool run() = 0;

	bool isAskedToStop(){return m_askToStop;}
	void askToStop() { m_askToStop = true;}

	private:
	bool m_askToStop = false;
};

}}}