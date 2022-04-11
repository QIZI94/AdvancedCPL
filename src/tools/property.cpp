#include "tools/property.hpp"

#include <unordered_map>
#include <typeindex>
#include <inttypes.h>
#include <string>
namespace acpl{
namespace tools{

using any_t = Property::any_type;
using ToStringFunc_t = void(*)(Property::any_type& value, std::string& output);
using propint = Property::interface;

static std::unordered_map<std::type_index, ToStringFunc_t> matchAndParse{
	{typeid(propint::internal_type<const bool>), [](any_t& value, std::string& output){
		output.append(value.cast<bool>() ? "true" : "false");
	}},
	{typeid(propint::internal_type<const int8_t>), [](any_t& value, std::string& output){
		output.append(std::to_string(value.cast<int8_t>()));
	}},
	{typeid(propint::internal_type<const uint8_t>), [](any_t& value, std::string& output){
		output.append(std::to_string(value.cast<uint8_t>()));
	}},
	{typeid(propint::internal_type<const int16_t>), [](any_t& value, std::string& output){
		output.append(std::to_string(value.cast<int16_t>()));
	}},
	{typeid(propint::internal_type<const uint16_t>), [](any_t& value, std::string& output){
		output.append(std::to_string(value.cast<uint16_t>()));
	}},
	{typeid(propint::internal_type<const int32_t>), [](any_t& value, std::string& output){
		output.append(std::to_string(value.cast<int32_t>()));
	}},
	{typeid(propint::internal_type<const uint32_t>), [](any_t& value, std::string& output){
		output.append(std::to_string(value.cast<uint32_t>()));
	}},
	{typeid(propint::internal_type<const int64_t>), [](any_t& value, std::string& output){
		output.append(std::to_string(value.cast<int64_t>()));
	}},
	{typeid(propint::internal_type<const uint64_t>), [](any_t& value, std::string& output){
		output.append(std::to_string(value.cast<uint64_t>()));
	}},
	{typeid(propint::internal_type<const float>), [](any_t& value, std::string& output){
		output.append(std::to_string(value.cast<float>()));
	}},
	{typeid(propint::internal_type<const double>), [](any_t& value, std::string& output){
		output.append(std::to_string(value.cast<double>()));
	}},
	{typeid(propint::internal_type<const std::string_view>), [](any_t& value, std::string& output){
		output.append(value.cast<const std::string_view&>());
	}},
	{typeid(propint::internal_type<const std::string>), [](any_t& value, std::string& output){
		output.append(value.cast<const std::string&>());
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

	auto it = matchAndParse.find(value.any.type());
	if(it != matchAndParse.end()){
		it->second(value, output);
	}
	else if(value.is_type<const PropertiesHolderList>()){
		output.push_back('{');
		PropertiesToString(value.cast<const PropertiesHolderList&>(), output, separator);
		if(output.size() >=  separator.size()){
			output.erase(output.size() - separator.size(), separator.size());
		}
		output.append("}");
	}
	else if(value.is_type<const PropertyList>()){
		output.push_back('[');
		PropertiesToString(value.cast<const PropertyList&>(), output, separator);
		if(output.size() >=  separator.size()){
			output.erase(output.size() - separator.size(), separator.size());
		}
		output.append("]");
	}
	else if(value.is_type<const PropertiesHolder>()){
		output.push_back('{');
		PropertiesToString(value.cast<const PropertiesHolder&>(), output, separator);
		if(output.size() >=  separator.size()){
			output.erase(output.size() - separator.size(), separator.size());
		}
		output.append("}");
	}
	else if(value.is_type<Action&>()){

		/*Action& action = Property::cast_any<Action&>(value);
		action.activate();*/
		//do nothing
	}
	else{
		output.append("unknown");
	}
	output.append(separator);
}

void PropertiesToString(const PropertiesHolder& propertyHolder, std::string& output, std::string separator){
	Property::Visitor toStringVisitor(
		[&](const Property& property){
			ProeprtyToString(property, output, separator);
			return true;
		}
	);
	propertyHolder.presentProperties(toStringVisitor);
}

}}