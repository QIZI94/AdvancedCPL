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

}}}