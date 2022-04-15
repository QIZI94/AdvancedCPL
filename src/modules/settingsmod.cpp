#include "modules/settingsmod.hpp"
#include "modules/shared/modulecomponent.hpp"
#include "tools/commoncomponents.hpp"
#include "globalconfig.hpp"

#include <iostream>
#include <fstream>

#include <libgen.h>         // dirname
#include <unistd.h>         // readlink
#include <linux/limits.h>   // PATH_MAX

namespace acpl{
namespace modules{

static GlobalConfig& GlobalCFG = GlobalConfig::Get();
PROPERTIES_IMPL(SettingsModule, 
	Property("Settings", (tools::PropertiesHolder&)GlobalCFG),
	Action("Action", [this](){
		tools::Property::ExecWhenNotConst([](SettingsModule* This){
			std::cout<<"heyo\n";
			This->askToStop();
		},this);
	})
)

 

struct SettingsSerializationComponent : public shared::ModuleComponent{
	COMPONENT_DEF(acpl::modules::SettingsSerializationComponent, ModuleComponent)
	SettingsSerializationComponent() : ModuleComponent(ESSENTIAL){}
	static void timedReload(tools::TimerComponent::ComponentWeak_t& serializationComponent, tools::TimerComponent& timer){
		if(serializationComponent.expired()){
			return;
		}

		if(auto This = std::dynamic_pointer_cast<SettingsSerializationComponent>(serializationComponent.lock())){
			This->restart();
			timer.restart();
		}
	}
	protected:
	void start() override {
		if(!isRestarting()){
			char result[PATH_MAX];
			ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
			const char *path;
			if (count != -1) {
				path = dirname(result);
			}		
			settingsPath = path;
			settingsPath.append("/settings.json");
			std::cout<<"Path to settings: "<<settingsPath<<'\n';
		}
		std::string content;
		tools::PropertiesToString(GlobalCFG,content);
		std::ofstream(settingsPath)<<content<<(counter++);
		
		done();
	}
	void run() override {}

	void stop() override {
		done();
	}

	int counter = 0;
	std::string settingsPath;
};


struct SettingsModule::MainComponent : public shared::ModuleComponentParented<SettingsModule>{
	COMPONENT_DEF(acpl::modules::SettingsModule::MainComponent, ModuleComponentParented)
	MainComponent(Parent_t& parent) : ModuleComponentParented(parent, ESSENTIAL){}
	void start() override {
		if(serializationComponent.expired()){
			serializationComponent = getParent().getComponentManager().addComponent<SettingsSerializationComponent>();
			serializationReloadTimer = getParent().getComponentManager().addComponent<tools::TimerComponent>(
				serializationComponent,
				SettingsSerializationComponent::timedReload,
				1000
			);

			
			//auto self_ptr = findSelf<MainComponent>(*getParent().manager).lock();
		}
		done();
	}
	protected:
	void run() override{}


	void stop() override {
		done();
	}

	std::weak_ptr<SettingsSerializationComponent> serializationComponent;
	std::weak_ptr<tools::TimerComponent> serializationReloadTimer;
};


SettingsModule::SettingsModule(){
	
	getComponentManager().addComponent<MainComponent>(*this);
}




}}