#pragma once
#include <string_view>
#include <vector>

#include "tools/property.hpp"
#include "tools/classinfo.hpp"

#define MODULE_DEF(CLASS, DISPLAYNAME)\
	std::string_view className() const override{using TestType = CLASS;return #CLASS;}\
	std::string_view displayName() const override{return DISPLAYNAME;}

namespace acpl{
namespace modules{
namespace shared{
struct Module : public tools::PropertiesHolder, public tools::ClassInfo{
	bool presentProperties(const tools::Property::Visitor&) override{return true;}
	bool presentProperties(const tools::Property::Visitor&) const override{return true;}
    std::string_view className() const override{ return "Module";}
	virtual std::string_view displayName() const = 0;
	virtual bool run() = 0;

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

	bool isAskedToStop(){return m_askToStop;}
	void askToStop() { m_askToStop = true;}

	private:
	bool m_askToStop = false;
};

}}}