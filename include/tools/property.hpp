#pragma once
#include "NamedProperties/propertydefaults.hpp"

#define PROPERTIES(...) \
bool presentProperties(const acpl::tools::Property::Visitor& visitor) override{\
	using namespace acpl::tools;\
	return Property::Visitor::visit(visitor,{__VA_ARGS__});\
}\
bool presentProperties(const acpl::tools::Property::Visitor& visitor) const override{\
	using namespace acpl::tools;\
	return Property::Visitor::visit(visitor,{__VA_ARGS__});\
}


#define PROPERTIES_DECL \
bool presentProperties(const acpl::tools::Property::Visitor& visitor) override;\
bool presentProperties(const acpl::tools::Property::Visitor& visitor) const override;

#define PROPERTIES_IMPL(Super, ...) \
bool Super::presentProperties(const acpl::tools::Property::Visitor& visitor) {\
	using namespace acpl::tools;\
	return Property::Visitor::visit(visitor,{__VA_ARGS__});\
}\
bool Super::presentProperties(const acpl::tools::Property::Visitor& visitor) const {\
	using namespace acpl::tools;\
	return Property::Visitor::visit(visitor,{__VA_ARGS__});\
}


#define PROPERTIES_EXTEND(Parent, ...) \
bool presentProperties(const acpl::tools::Property::Visitor& visitor) override{\
   	using namespace acpl::tools;\
	if(!Parent::presentProperties(visitor)){\
		return false;\
	}\
	return Property::Visitor::visit(visitor,{__VA_ARGS__});\
}\
bool presentProperties(const acpl::tools::Property::Visitor& visitor) const override{\
	using namespace acpl::tools;\
	if(!Parent::presentProperties(visitor)){\
		return false;\
	}\
	return Property::Visitor::visit(visitor,{__VA_ARGS__});\
}

#define PROPERTIES_EXTEND_DECL \
bool presentProperties(const acpl::tools::Property::Visitor& visitor) override;\
bool presentProperties(const acpl::tools::Property::Visitor& visitor) const override;

#define PROPERTIES_EXTEND_IMPL(Super, Parent, ...) \
bool Super::presentProperties(const acpl::tools::Property::Visitor& visitor) override{\
   	using namespace acpl::tools;\
	if(!Parent::presentProperties(visitor)){\
		return false;\
	}\
	return Property::Visitor::visit(visitor,{__VA_ARGS__});\
}\
bool Super::presentProperties(const acpl::tools::Property::Visitor& visitor) const override{\
	using namespace acpl::tools;\
	if(!Parent::presentProperties(visitor)){\
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
	using PresentFunc_t = bool(*)(const Property::Visitor&, std::any);

	template<typename T>
	PropertiesHolderList(T& list) : anyList(std::make_any<T*>(&list)),
		present([](const Property::Visitor& visitor, std::any anyList){
			auto& list = *std::any_cast<T*>(anyList);
			for(auto& propHolder : list){
				if(!propHolder.presentProperties(visitor)){
					return false;
				}
			}
			return true;}
		)
	{}
	template<typename T>
	PropertiesHolderList(const T& list) : anyList(std::make_any<const T*>(&list)),
		present([](const Property::Visitor& visitor, std::any anyList){
			const auto& list = *std::any_cast<const T*>(anyList);
			for(const auto& propHolder : list){
				if(!propHolder.presentProperties(visitor)){
					return false;
				}
			}
			return true;}
		)
	{}
	
	template<typename T>
	PropertiesHolderList(T& list, PresentFunc_t presentFunc) : anyList(std::make_any<T*>(&list)),
		present(presentFunc)
	{}
	template<typename T>
	PropertiesHolderList(const T& list, PresentFunc_t presentFunc) : anyList(std::make_any<const T*>(&list)),
		present(presentFunc)
	{}

	bool presentProperties(const Property::Visitor& visitor) override{
		return present(visitor, anyList);
	}
	bool presentProperties(const Property::Visitor& visitor) const override{
		return present(visitor, anyList);
	}

	private:
	PresentFunc_t present;
	std::any anyList;
};

class PropertyList : public PropertiesHolder{
	public:
	using PresentFunc_t = bool(*)(const Property::Visitor&, std::any);

	template<typename T>
	PropertyList(T& list) : anyList(std::make_any<T*>(&list)),
		present([](const Property::Visitor& visitor, std::any anyList){
			auto& list = *std::any_cast<T*>(anyList);
			return Property::Visitor::visit(visitor, list);
		})
	{}
	template<typename T>
	PropertyList(const T& list) : anyList(std::make_any<const T*>(&list)),
		present([](const Property::Visitor& visitor, std::any anyList){
			const auto& list = *std::any_cast<const T*>(anyList);
			return Property::Visitor::visit(visitor, list);
		})
	{}

	PropertyList(std::initializer_list<Property>& ilProperties) : anyList(std::make_any<std::initializer_list<Property>*>(&ilProperties)),
		present([](const Property::Visitor& visitor, std::any anyList){
			auto& list = *std::any_cast<std::initializer_list<Property>*>(anyList);
			return Property::Visitor::visit(visitor, list);
		})
	{}

	PropertyList(const std::initializer_list<Property>& ilProperties) : anyList(std::make_any<const std::initializer_list<Property>*>(&ilProperties)),
		present([](const Property::Visitor& visitor, std::any anyList){
			const auto& list = *std::any_cast<const std::initializer_list<Property>*>(anyList);
			return Property::Visitor::visit(visitor, list);
		})
	{}
	

	bool presentProperties(const Property::Visitor& visitor) override{
		return present(visitor, anyList);
	}
	bool presentProperties(const Property::Visitor& visitor) const override{
		return present(visitor, anyList);
	}

	private:
	PresentFunc_t present;
	std::any anyList;
};

void PropertiesToString(const PropertiesHolder& propertyHolder, std::string& output, std::string separator = ", ");
}}