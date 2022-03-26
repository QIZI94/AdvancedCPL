#pragma once
#include "NamedProperties/propertydefaults.hpp"

#define PROPERTIES_FIRST(...) \
bool presentProperties(const acpl::tools::Property::Visitor& visitor) override{\
	using namespace acpl::tools;\
	return Property::Visitor::visit(visitor,{__VA_ARGS__});\
}\
bool presentProperties(const acpl::tools::Property::Visitor& visitor) const override{\
	using namespace acpl::tools;\
	return Property::Visitor::visit(visitor,{__VA_ARGS__});\
}


#define PROPERTIES_FIRST_DECL \
bool presentProperties(const acpl::tools::Property::Visitor& visitor) override;\
bool presentProperties(const acpl::tools::Property::Visitor& visitor) const override;

#define PROPERTIES_FIRST_IMPL(Super, ...) \
bool Super::presentProperties(const acpl::tools::Property::Visitor& visitor) {\
	using namespace acpl::tools;\
	return Property::Visitor::visit(visitor,{__VA_ARGS__});\
}\
bool Super::presentProperties(const acpl::tools::Property::Visitor& visitor) const {\
	using namespace acpl::tools;\
	return Property::Visitor::visit(visitor,{__VA_ARGS__});\
}


#define PROPERTIES(Parent, ...) \
bool presentProperties(const acpl::tools::Property::Visitor& visitor) override{\
   	using namespace acpl::tools;\
	if(!Parent::presentProeprties()){\
		return false;\
	}\
	return Property::Visitor::visit(visitor,{__VA_ARGS__});\
}\
bool presentProperties(const acpl::tools::Property::Visitor& visitor) const override{\
	using namespace acpl::tools;\
	if(!Parent::presentProeprties()){\
		return false;\
	}\
	return Property::Visitor::visit(visitor,{__VA_ARGS__});\
}

namespace acpl{
namespace tools{
using Property = nap::Property;
struct PropertiesHolder{
	virtual bool presentProperties(const Property::Visitor&) = 0;
	virtual bool presentProperties(const Property::Visitor&) const = 0;
};

class PropertiesHolderList : public PropertiesHolder{
	public:
	template<typename T>
	PropertiesHolderList(T& list){
		anyList = std::make_any<T*>(&list);
		present = [](const Property::Visitor& visitor, std::any anyList){
			auto& list = *std::any_cast<T*>(anyList);
			for(auto propHolder : list){
				if(!propHolder.presentProperties(visitor)){
					return false;
				}
			}
			return true;
		};
	}
	template<typename T>
	PropertiesHolderList(const T& list){
		list = std::make_any<const T*>(&list);
		present = [](const Property::Visitor& visitor, std::any anyList){
			auto& list = *std::any_cast<const T*>(anyList);
			for(auto propHolder : list){
				if(!propHolder.presentProperties(visitor)){
					return false;
				}
			}
			return true;
		};
	}

	bool presentProperties(const Property::Visitor& visitor) override{
		return present(visitor, anyList);
	}
	bool presentProperties(const Property::Visitor& visitor) const override{
		return present(visitor, anyList);
	}

	private:
	using PresentFunc_t = bool(*)(const Property::Visitor&, std::any);
	PresentFunc_t present;
	std::any anyList;
};

void PropertiesToString(const PropertiesHolder& propertyHolder, std::string& output, std::string separator = ", ");
}}