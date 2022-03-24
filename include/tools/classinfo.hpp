#pragma once
#include <string_view>
namespace acpl{
namespace tools{
struct ClassInfo{
	virtual std::string_view className() const = 0;
	//virtual std::string toString() const = 0;
};
}}