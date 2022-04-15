#include "modules/usbmod.hpp"
#include "modules/shared/modulecomponent.hpp"
#include "tools/commoncomponents.hpp"
#include "globalconfig.hpp"

#include <iostream>
#include <map>
#include <cstring>
#include <libudev.h>

namespace acpl{
namespace modules{

static GlobalConfig& GlobalCFG = GlobalConfig::Get();
struct UsbDeviceInfo : public tools::PropertiesHolder{

	struct Attribute{
		Attribute(const std::string& name, const std::string value) : 
		name(name), value(value)
		{}
		operator tools::Property() const{
			return tools::Property(name, value);
		}
		const std::string name;
		const std::string value;
	};
	void addAttribute(const Attribute& attribute){
		m_attributes.emplace_back(attribute);
	}
	void addAttribute(Attribute&& attribute){
		m_attributes.emplace_back(attribute);
	}
	void addAttribute(const std::string& name, const std::string& value){
		m_attributes.emplace_back(name, value);
	}
	
	void clear(){
		m_attributes.clear();
	}


	UsbDeviceInfo(){}
	bool presentProperties(const acpl::tools::Property::Visitor& visitor) override{
		const UsbDeviceInfo* constThis = this;
		return constThis->presentProperties(visitor);
	}
	bool presentProperties(const acpl::tools::Property::Visitor& visitor) const override{
		return tools::Property::Visitor::visit(visitor, m_attributes);
	}
	

	private:
	std::vector<Attribute> m_attributes;
	
};
struct Usb : public tools::PropertiesHolder{
	
	UsbDeviceInfo deviceInfo;
	PROPERTIES(
		Property("Device Info", (const tools::PropertiesHolder&)deviceInfo),
	)

};

struct UsbHub : public Usb{
	using UsbList = std::vector<Usb>;
	using HubMap = std::map<std::string, UsbHub>;
	PROPERTIES_EXTEND(Usb,
		Property("USB Devices", usbListProperties),
		Property("HUB Devices", hubListProperties),
	)
	UsbList usbList;
	HubMap usbHubMap;
	tools::PropertiesHolderList usbListProperties = tools::PropertiesHolderList(usbList);
	tools::PropertiesHolderList hubListProperties = tools::PropertiesHolderList(usbHubMap, UsbHub::presentHubs);
	
	static bool presentHubs(const tools::Property::Visitor& visitor, std::any anyList){
		if(anyList.type() == typeid(const HubMap*)){
			const auto& list = *std::any_cast<const HubMap*>(anyList);
			for(auto& pair : list){
				if(!pair.second.presentProperties(visitor)){
					return false;
				}
			}
		}
		else{
			auto& list = *std::any_cast<HubMap*>(anyList);
			for(auto& pair : list){
				if(!pair.second.presentProperties(visitor)){
					return false;
				}
			}
		}
		return true;
	}
};

struct DiscoverUsbDevicesComponent : public shared::ModuleComponent{
	DiscoverUsbDevicesComponent() : ModuleComponent(ESSENTIAL){}
	COMPONENT_DEF(acpl::modules::DiscoverUsbDevicesComponent, ModuleComponent)

	static void timedReload(tools::TimerComponent::ComponentWeak_t& discoverUsbDevices, tools::TimerComponent& timer){
		if(discoverUsbDevices.expired()){
			return;
		}

		if(auto This = std::dynamic_pointer_cast<DiscoverUsbDevicesComponent>(discoverUsbDevices.lock())){
			This->restart();
			timer.restart();
		}
	}
	protected:
	void start() override {
		discoverDevices();
		done();
	}
	void run() override {}

	PROPERTIES(
		Property("HUB Devices", tools::PropertiesHolderList(m_usbHubs, UsbHub::presentHubs)),
	)

	private:
	struct HubNode{
		udev_device* dev = nullptr;
		HubNode* child = nullptr;
	};

	UsbHub& buildHierarchyFromParents(HubNode* hub){

		HubNode parentHub;
		parentHub.child = hub;
		parentHub.dev = udev_device_get_parent_with_subsystem_devtype(
			hub->dev,
			"usb",
			"usb_device"
		);
		if(parentHub.dev != nullptr){
			return buildHierarchyFromParents(&parentHub);
		}
		else{
			HubNode* currentHub = hub;
			UsbHub::HubMap* currentHubMap = &m_usbHubs;
			
			do{
				const char* hubPath = udev_device_get_devnode(currentHub->dev);
				auto it = currentHubMap->find(hubPath);
				UsbHub& usbHub = (*currentHubMap)[hubPath];
				if(it == currentHubMap->end()){
					gatherDeviceInfo(hub->dev, usbHub.deviceInfo);
				}
				
				currentHubMap = &usbHub.usbHubMap;
				currentHub = currentHub->child;
				
				if(currentHub == nullptr){
					return usbHub;
				}
			}while(true);
		}
	}

	UsbHub& findDeviceParents(udev_device* dev){

		HubNode first;

		first.dev = udev_device_get_parent_with_subsystem_devtype(
			dev,
			"usb",
			"usb_device"
		);
		return buildHierarchyFromParents(&first);

	}
	void discoverDevices(){
		udev *udev;
		udev_enumerate *enumerate;
		udev_list_entry *devices, *dev_entry;
		udev_device *dev;

		enumerate = udev_enumerate_new(udev);
		udev_enumerate_add_match_subsystem(enumerate, "hidraw");

		udev_enumerate_scan_devices(enumerate);
 		devices = udev_enumerate_get_list_entry(enumerate);
		m_usbHubs.clear();
		udev_list_entry_foreach(dev_entry, devices) {
			const char *path = udev_list_entry_get_name(dev_entry);
			dev = udev_device_new_from_syspath(udev, path);
	
			dev = udev_device_get_parent_with_subsystem_devtype(
				dev,
				"usb",
				"usb_device"
			);
			if(dev == nullptr){
				continue;
			}

			Usb& newUsb = findDeviceParents(dev).usbList.emplace_back(); //usbHub.usbList.emplace_back();
			gatherDeviceInfo(dev, newUsb.deviceInfo);			
		}
	}


	static void gatherDeviceInfo(udev_device* device, UsbDeviceInfo& deviceInfo){
		auto isAttributeIgnored = [](std::string_view attributeName){
			constexpr auto ignoredAttributes = std::array{
				std::string_view("product"),
				std::string_view("manufacturer"),
				std::string_view("subsystem"),
				std::string_view("uevent"),
				std::string_view("descriptors"),
			};
			for(const auto& entry : ignoredAttributes){
				if(entry == attributeName){
					return true;
				}
			}
			return false;
		};

		const char* attributeValue = udev_device_get_sysattr_value(device, "product");
		if(attributeValue != nullptr){
			deviceInfo.addAttribute("Product",attributeValue);
		}
		attributeValue = udev_device_get_sysattr_value(device, "manufacturer");
		if(attributeValue != nullptr){
			deviceInfo.addAttribute("Manufacturer", attributeValue);
		}

		udev_list_entry* attributeEntry = nullptr;
		udev_list_entry_foreach(attributeEntry, udev_device_get_sysattr_list_entry(device)){
			const char* attributeName = udev_list_entry_get_name(attributeEntry);
			if(isAttributeIgnored(attributeName)){
				continue;
			}
			attributeValue 	= udev_device_get_sysattr_value(device, attributeName);
			if(attributeName != nullptr && attributeValue != nullptr){
				deviceInfo.addAttribute(attributeName, attributeValue);
			}
		}
	}

	private:

	UsbHub::HubMap m_usbHubs;
	
};


struct UsbModule::MainComponent : public shared::ModuleComponentParented<UsbModule>{
	COMPONENT_DEF(acpl::modules::UsbModule::MainComponent, ModuleComponentParented)
	MainComponent(Parent_t& parent) : ModuleComponentParented(parent, ESSENTIAL){}
	void start() override {
		if(discoverUsbDevicesComponent.expired()){
			discoverUsbDevicesComponent = getParent().getComponentManager().addComponent<DiscoverUsbDevicesComponent>();
			discoverUsbReloadTimer = getParent().getComponentManager().addComponent<tools::TimerComponent>(
				discoverUsbDevicesComponent,
				DiscoverUsbDevicesComponent::timedReload,
				1000
			);

			//auto self_ptr = findSelf<MainComponent>(*getParent().manager).lock();
		}
		done();
	}
	void run() override{}


	void stop() override {
		done();
	}

	std::weak_ptr<DiscoverUsbDevicesComponent> discoverUsbDevicesComponent;
	std::weak_ptr<tools::TimerComponent> discoverUsbReloadTimer;
};


UsbModule::UsbModule(){
	getComponentManager().addComponent<MainComponent>(*this);
}

}}