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
	Property("Settings", (tools::PropertiesHolder&)GlobalCFG)
)

class SettingsModule::Manager : public tools::ComponentManager{};
 

struct SerializationComponent : public shared::ModuleComponent{
	SerializationComponent() : ModuleComponent(ESSENTIAL){}
	static void timedReload(tools::TimerComponent::ComponentWeak_t& serializationComponent, tools::TimerComponent& timer){
		if(serializationComponent.expired()){
			return;
		}

		if(auto This = std::dynamic_pointer_cast<SerializationComponent>(serializationComponent.lock())){
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
		
		
		//self_ptr->restart();
		std::cout<<"SerializationComponent done\n";
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
			serializationComponent = getParent().manager->addComponent<SerializationComponent>();
			serializationReloadTimer = getParent().manager->addComponent<tools::TimerComponent>(
				serializationComponent,
				SerializationComponent::timedReload,
				1000
			);

			
			//auto self_ptr = findSelf<MainComponent>(*getParent().manager).lock();
		}
		std::cout<<"MainComponent done\n";
		done();
	}
	protected:
	void run() override{}


	void stop() override {
		done();
	}

	std::weak_ptr<SerializationComponent> serializationComponent;
	std::weak_ptr<tools::TimerComponent> serializationReloadTimer;
};


SettingsModule::SettingsModule(){
	manager = std::make_unique<Manager>();
	manager->addComponent<MainComponent>(*this);
}
SettingsModule::~SettingsModule(){}


bool SettingsModule::run(){
	return manager->handleComponents();
}



}}