
#include <iostream>
#include <string>
#include "modules/shared/moduleinitialization.hpp"


using namespace acpl;

int main(){
	acpl::modules::shared::ModuleUniqueList modulesList = acpl::modules::shared::InitializeModules();

	for(auto& module : modulesList){
		module->run();


		std::cout<<'\n'<<module->classNameShort()<<": "<<'\n';
/*
		tools::Property::Visitor printProperties([](const tools::Property& prop){

			if(prop.isNameOnly()){
				std::cout<<'\t'<<prop.name()<<'\n';
				return true;
			}
			std::cout<<'\t'<<prop.name()<<": ";
			tools::Property::any_type value;
			prop.read(value);
			if(tools::Property::is_any<const std::string_view&>(value)){
				
				std::cout<<tools::Property::cast_any<const std::string_view&>(value);
			}


			std::cout<<'\n';

			return true;
		});*/

		/*std::string output;
		tools::PropertiesToString(*module, output);*/
		std::cout<<module->toString(",\n\t")<<'\n';
		//module->presentProperties(printProperties);

	}
}