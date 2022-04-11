#pragma once
#include "NamedProperties/propertydefaults.hpp"
#include <typeindex>

namespace acpl{
namespace tools{

namespace detail{
	struct AcplInterface : public nap::detail::DefaultInterface{
		struct any_type{
			any_type(){}
			any_type(DefaultInterface::any_type any) : any(any){}
			template<typename T>
			any_type(T& value) : any(make_any(value)){}
			template<typename T>
			any_type(const T& value) : any(make_any(value)){}
			template<typename T>
			static any_type make(T& value){
				return any_type(value);
			}
			template<typename T>
			static any_type make(const T& value){
				return any_type(value);
			}
			

			template<typename T>
			auto& cast(){
				return cast_any<T>(any);
			}
			template<typename T>
			const auto& cast() const{
				return cast_any<const T>(any);
			}
			template<typename T>
			bool is_type() const{
				return is_any<T>(any);
			}

			DefaultInterface::any_type any;
		};


		template<typename T>
		static auto read(T& value){
			return any_type(DefaultInterface::read<T>(value));
		}
		template<typename T>
		static auto read(const T& value){
			return any_type(DefaultInterface::read<T>(value));
		}
		template<typename T>
		static void write(T& value, any_type& anyType){
			DefaultInterface::write(value, anyType.any);
		}
	};
}

}}