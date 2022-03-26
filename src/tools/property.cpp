#include "tools/property.hpp"

#include <unordered_map>
#include <typeindex>
#include <inttypes.h>
#include <string>
namespace acpl{
namespace tools{

using any_t = Property::any_type;
using ToStringFunc_t = void(*)(Property::any_type& value, std::string& output);

static std::unordered_map<std::type_index, ToStringFunc_t> matchAndParse{
	{typeid(bool), [](any_t& value, std::string& output){
		output.append(Property::cast_any<bool>(value) ? "true" : "false");
	}},
	{typeid(int8_t), [](any_t& value, std::string& output){
		output.append(std::to_string(Property::cast_any<int8_t>(value)));
	}},
	{typeid(uint8_t), [](any_t& value, std::string& output){
		output.append(std::to_string(Property::cast_any<uint8_t>(value)));
	}},
	{typeid(int16_t), [](any_t& value, std::string& output){
		output.append(std::to_string(Property::cast_any<int16_t>(value)));
	}},
	{typeid(uint16_t), [](any_t& value, std::string& output){
		output.append(std::to_string(Property::cast_any<uint16_t>(value)));
	}},
	{typeid(int32_t), [](any_t& value, std::string& output){
		output.append(std::to_string(Property::cast_any<int32_t>(value)));
	}},
	{typeid(uint32_t), [](any_t& value, std::string& output){
		output.append(std::to_string(Property::cast_any<uint32_t>(value)));
	}},
	{typeid(int64_t), [](any_t& value, std::string& output){
		output.append(std::to_string(Property::cast_any<int64_t>(value)));
	}},
	{typeid(uint64_t), [](any_t& value, std::string& output){
		output.append(std::to_string(Property::cast_any<uint64_t>(value)));
	}},
	{typeid(float), [](any_t& value, std::string& output){
		output.append(std::to_string(Property::cast_any<float>(value)));
	}},
	{typeid(double), [](any_t& value, std::string& output){
		output.append(std::to_string(Property::cast_any<double>(value)));
	}},
	{typeid(const std::string_view*), [](any_t& value, std::string& output){
		output.append(Property::cast_any<const std::string_view&>(value));
	}},
	{typeid(const std::string*), [](any_t& value, std::string& output){
		output.append(Property::cast_any<const std::string&>(value));
	}},
};

static void ProeprtyToString(const Property& property, std::string& output, const std::string& separator = "\n"){
	if(property.isNameOnly()){
		output.append(property.name());
		output.append(separator);
		return;
	}
	if(!property.isReadable()){
		return; //skip
	}
	output.append(property.name());
	output.append(": ");

	Property::any_type value;
	property.read(value);

	auto it = matchAndParse.find(value.type());
	if(it != matchAndParse.end()){
		it->second(value, output);
	}
	else if(Property::is_any<const PropertiesHolderList>(value)){
		output.push_back('[');
		PropertiesToString(Property::cast_any<const PropertiesHolderList&>(value), output, separator);
		if(output.size() >=  separator.size()){
			output.erase(output.size() - separator.size(), separator.size());
		}
		output.append("]");
	}
	else{
		output.append("unknown");
	}
	output.append(separator);
}

void PropertiesToString(const PropertiesHolder& propertyHolder, std::string& output, std::string separator){
	nap::Property::Visitor toStringVisitor(
		[&](const nap::Property& property){
			ProeprtyToString(property, output, separator);
			return true;
		}
	);
	propertyHolder.presentProperties(toStringVisitor);
}

}}