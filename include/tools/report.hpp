#pragma once
#include <string>
#include <string_view>


namespace acpl{
namespace tools{
struct Report{
	enum class Type{
		Notification,
		Warning,
		Error
	};
	Report(Type reportType, std::string_view sourceDescription, std::string_view sourceClassName = {}, std::string_view sourceCallstack = {}) : m_reportType(reportType){
		constexpr size_t SeparatorsAmount = 2;
		m_message.reserve(sourceDescription.size() + sourceClassName.size() + sourceCallstack.size() + SeparatorsAmount);
		if(!sourceDescription.empty()){
			m_message.append(sourceDescription.begin(), sourceDescription.end());
			description = std::string_view((&m_message.back())  - sourceDescription.size() + 1, sourceDescription.size());
			
		}
		if(!sourceClassName.empty()){
			m_message.push_back(' ');
			m_message.append(sourceClassName.begin(), sourceClassName.end());
			className = std::string_view((&m_message.back())  - sourceClassName.size() + 1, sourceClassName.size());
		}
		if(!sourceCallstack.empty()){
			m_message.push_back('\n');
			m_message.append(sourceCallstack.begin(), sourceCallstack.end());
			callstack = std::string_view((&m_message.back())  - sourceCallstack.size() + 1, sourceCallstack.size());
		}
	}

	Report(const Report& other) : Report(other.m_reportType, other.description, other.className, other.callstack){}
	Report(Report&& other) : m_message(std::move(other.m_message)), m_reportType(other.m_reportType), description(other.description), className(other.className), callstack(other.callstack){
		other.description 	= {};
		other.className 	= {};
		other.callstack 	= {};
	}

	Report& operator = (const Report& other) {
		return *this = Report(other.m_reportType, other.description, other.className, other.callstack);
	}
	Report& operator = (Report&& other) {
		m_message 			= std::move(other.m_message);
		m_reportType 		= other.m_reportType;
		description 		= other.description;
		className 			= other.className;
		callstack 			= other.callstack;
		other.description 	= {};
		other.className 	= {};
		other.callstack 	= {};

		return *this;
	}
	

	static Report Notification(std::string_view sourceDescription, std::string_view sourceClassName = {}, std::string_view sourceCallstack = {}){
		return Report(Type::Notification, sourceDescription, sourceClassName, sourceCallstack);
	}

	static Report Warning(std::string_view sourceDescription, std::string_view sourceClassName = {}, std::string_view sourceCallstack = {}){
		return Report(Type::Warning, sourceDescription, sourceClassName, sourceCallstack);
	}

	static Report Error(std::string_view sourceDescription, std::string_view sourceClassName = {}, std::string_view sourceCallstack = {}){
		return Report(Type::Error, sourceDescription, sourceClassName, sourceCallstack);
	}

	const std::string& wholeMessage() const{return m_message;}
	Type getReprotType() const {return m_reportType;}

	std::string_view description;
	std::string_view className;
	std::string_view callstack;

	private: // members
	std::string m_message;
	Type m_reportType;
};


}}