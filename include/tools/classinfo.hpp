#pragma once
#include <string_view>
#include <functional>
#include <tools/report.hpp>
namespace acpl{
namespace tools{
struct ClassInfo{

	using ReprotVisitor = std::function<bool(const Report&)>;
	virtual std::string_view className() const = 0;
	std::string_view classNameShort() const{
		std::string_view longName = className();
		size_t idx = longName.rfind(":") + 1;
		if(idx != std::string_view::npos && idx < longName.size()){
			return std::string_view(&longName[idx], longName.size() - idx);
		}
		return longName;
	}
	//virtual std::string toString() const = 0;

	virtual void collectReprots(const ReprotVisitor&) const{}
	virtual void clearReprots(){}
	virtual void report(Report&&){}
	virtual void report(const Report&){}
};
}}