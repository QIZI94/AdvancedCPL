#pragma once
#include <string_view>
#include <vector>

#include "tools/components.hpp"
#include "tools/property.hpp"
#include "tools/classinfo.hpp"

#define MODULE_DEF(CLASS, DISPLAYNAME)\
	std::string_view className() const override{using TestType = CLASS;return #CLASS;}\
	std::string_view displayName() const override{return DISPLAYNAME;}

namespace acpl{
namespace modules{
namespace shared{
struct Module : public tools::PropertiesHolder, public tools::ClassInfo{

    std::string_view className() const override{ return "Module";}
	virtual std::string_view displayName() const = 0;

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

	virtual bool run() {
		return m_componentManager.handleComponents();
	}

	bool presentProperties(const tools::Property::Visitor& visitor) override{
		bool visitReturn = true;
		m_componentManager.visitComponents([&visitor, &visitReturn](tools::StateComponent& component, bool& bContinue){
				tools::PropertiesHolder* withProperties = dynamic_cast<tools::PropertiesHolder*>(&component);
				if(withProperties != nullptr){
					bContinue = visitReturn = withProperties->presentProperties(visitor);
				}
			}
		);
		return visitReturn;
	}
	bool presentProperties(const tools::Property::Visitor& visitor) const override{
		bool visitReturn = true;
		m_componentManager.visitComponents([&visitor, &visitReturn](const tools::StateComponent& component, bool& bContinue){
				const tools::PropertiesHolder* withProperties = dynamic_cast<const tools::PropertiesHolder*>(&component);
				if(withProperties != nullptr){
					bContinue = visitReturn = withProperties->presentProperties(visitor);
				}
			}
		);
		return visitReturn;
	}
	tools::ComponentManager& getComponentManager(){
		return m_componentManager;
	}
	const tools::ComponentManager& getComponentManager() const{
		return m_componentManager;
	}

	private:
	bool m_askToStop = false;
	tools::ComponentManager m_componentManager;
};

}}}