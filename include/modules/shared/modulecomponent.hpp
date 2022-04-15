#pragma once
#include "tools/property.hpp"
#include "tools/classinfo.hpp"
#include "tools/components.hpp"
#include <iostream>

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

	ModuleComponent() {}
	ModuleComponent(uint16_t stuckLimit) : tools::StateComponent(stuckLimit){}
	ModuleComponent(Essentiality_t essentiality) : tools::StateComponent(essentiality){}
	ModuleComponent(Essentiality_t essentiality, uint16_t stuckLimit) : tools::StateComponent(essentiality, stuckLimit){}

	COMPONENT_DEF(acpl::modules::shared::ModuleComponent, tools::StateComponent)
	bool presentProperties(const tools::Property::Visitor&) override{return true;}
	bool presentProperties(const tools::Property::Visitor&) const override{return true;}
		
	template <typename T>
	std::weak_ptr<T> findSelf(tools::ComponentManager& componentManager){
		auto matchingComponents = componentManager.getAllComponents<T>();
		auto it = std::find_if(matchingComponents.begin(), matchingComponents.end(), [this](tools::ComponentManager::ComponentBase_wptr_t& cmp){
			if(cmp.expired()){
				return false;
			}
			return (cmp.lock().get() == this);
		});
		if(it != matchingComponents.end()){
			auto shared = it->lock();
			return std::dynamic_pointer_cast<T>(shared);
		}

		return {};
	}
	template <typename T>
	std::weak_ptr<T> findSelf(const tools::ComponentManager& componentManager){
		auto matchingComponents = componentManager.getAllComponents<const T>();
		auto it = std::find_if(matchingComponents.begin(), matchingComponents.end(), [this](tools::ComponentManager::ConstComponentBase_wptr_t& cmp){
			if(cmp.expired()){
				return false;
			}
			return (cmp.lock().get() == this);
		});
		if(it != matchingComponents.end()){
			auto shared = it->lock();
			return std::dynamic_pointer_cast<const T>(shared);
		}

		return {};
	}

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

	void onStateFinish() override{

		if(isRestarting()){
			if(getLastResult() == DONE){
				std::cerr<<"\033[1;35mModuleComponent "<<className()<<" done restaring.\033[0m\n";
			}
			else if(getLastResult() == FAIL){
				std::cerr<<"\033[1;31mModuleComponent "<<className()<<" failed while restarting.\033[0m\n";
			}
		}
		else if(isStarting()){
			
			if(getLastResult() == DONE){
				std::cerr<<"\033[1;32mModuleComponent "<<className()<<" done starting.\033[0m\n";
			}
			else if(getLastResult() == FAIL){
				std::cerr<<"\033[1;31mModuleComponent "<<className()<<" failed while starting.\033[0m\n";
			}
		}
		else if(isRunning()){
			if(getLastResult() == DONE){
				std::cerr<<"\033[1;32mModuleComponent "<<className()<<" done running.\033[0m\n";
			}
			else if(getLastResult() == FAIL){
				std::cerr<<"\033[1;31mModuleComponent "<<className()<<" failed while running.\033[0m\n";
			}
		}
		else if(isStopping()){
			if(getLastResult() == DONE){
				std::cerr<<"\033[1;32mModuleComponent "<<className()<<" done stopping.\033[0m\n";
			}
			else if(getLastResult() == FAIL){
				std::cerr<<"\033[1;31mModuleComponent "<<className()<<" failed while stopping.\033[0m\n";
			}
		}
	}
};
template <typename Parent>
struct ModuleComponentParented : public ModuleComponent{
	public:
	using Parent_t = Parent;
	COMPONENT_DEF(acpl::modules::shared::ModuleComponentParented<Parent_t>, ModuleComponent)

	protected:
	constexpr ModuleComponentParented(Parent_t& parent) : parent(&parent){}
	constexpr ModuleComponentParented(Parent_t& parent, uint16_t stuckLimit) : parent(&parent), ModuleComponent(stuckLimit){}
	constexpr ModuleComponentParented(Parent_t& parent, Essentiality_t essentiality) : parent(&parent), ModuleComponent(essentiality){}
	constexpr ModuleComponentParented(Parent_t& parent, Essentiality_t essentiality, uint16_t stuckLimit) : parent(&parent), ModuleComponent(essentiality, stuckLimit){}
	
	Parent_t& getParent(){
		return *parent;
	}
	const Parent_t& getParent() const{
		return *parent;
	}

	private:
	Parent_t* parent;
	
	
};

class ComponentPropertiesList : public tools::PropertiesHolder{
	public:

	ComponentPropertiesList(tools::ComponentManager& list) : anyList(std::make_any<tools::ComponentManager*>(&list)),
		present([](const tools::Property::Visitor& visitor, std::any anyList){
			auto& list = *std::any_cast<tools::ComponentManager*>(anyList);
			bool visitReturn = true;

			list.visitComponents([&visitor, &visitReturn](tools::StateComponent& component, bool& bContinue){
					ModuleComponent* withProperties = dynamic_cast<ModuleComponent*>(&component);
					if(withProperties != nullptr){
						bContinue = visitReturn = withProperties->presentProperties(visitor);
					}
				}
			);
			return visitReturn;}
		)
	{}
	ComponentPropertiesList(const tools::ComponentManager& list) : anyList(std::make_any<const tools::ComponentManager*>(&list)),
		present([](const tools::Property::Visitor& visitor, std::any anyList){
			const auto& list = *std::any_cast<const tools::ComponentManager*>(anyList);
			bool visitReturn = true;

			list.visitComponents([&visitor, &visitReturn](tools::StateComponent& component, bool& bContinue){
					const ModuleComponent* withProperties = dynamic_cast<const ModuleComponent*>(&component);
					if(withProperties != nullptr){
						bContinue = visitReturn = withProperties->presentProperties(visitor);
					}
				}
			);
			return visitReturn;}
		)
	{}
	
	bool presentProperties(const tools::Property::Visitor& visitor) override{
		return present(visitor, anyList);
	}
	bool presentProperties(const tools::Property::Visitor& visitor) const override{
		return present(visitor, anyList);
	}

	private:
	using PresentFunc_t = bool(*)(const tools::Property::Visitor&, std::any);
	PresentFunc_t present;
	std::any anyList;
};

}}}