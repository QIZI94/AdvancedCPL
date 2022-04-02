#include "modules/usbmod.hpp"
#include "modules/shared/modulecomponent.hpp"
#include "tools/commoncomponents.hpp"
#include "globalconfig.hpp"

#include <iostream>
#include <cstring>
#include <libudev.h>

namespace acpl{
namespace modules{

static GlobalConfig& GlobalCFG = GlobalConfig::Get();


class UsbModule::Manager : public tools::ComponentManager{};
 
struct UsbDeviceInfo : public tools::PropertiesHolder{
	std::string manufacturer;
	std::string product;
	char vendorID[5] = "";
	char productID[5] = "";
	char serialNumber[64] = "";
	UsbDeviceInfo(){/*delete me later*/}
	UsbDeviceInfo(const std::string){
		
	}
	PROPERTIES(
		Property("Manufacturer", 	Property::ReadOnly(manufacturer)),
		Property("Product", 		Property::ReadOnly(product)),
		Property("Vendor ID", 		std::string_view(vendorID, std::strlen(vendorID))),
		Property("Product ID", 		std::string_view(productID, std::strlen(vendorID))),
		Property("Serial", 			std::string_view(serialNumber, std::strlen(serialNumber))),
	)

};
struct Usb : public tools::PropertiesHolder{
	UsbDeviceInfo deviceInfo;
	PROPERTIES(
		Property("USB Info", (const tools::PropertiesHolder&)deviceInfo),
	)

};

struct UsbHub : public Usb{
	
	PROPERTIES_EXTEND(Usb,
		Property("USB Devices", usbListProperties)
	)

	UsbHub(){
		udev *udev;
		udev_enumerate *enumerate;
		udev_list_entry *devices, *dev_list_entry;
		udev_device *dev;
		udev = udev_new();
		if (!udev) {
			printf("Can't create udev\n");
			exit(1);
		}
		enumerate = udev_enumerate_new(udev);
		udev_enumerate_add_match_subsystem(enumerate, "hidraw");
		udev_enumerate_scan_devices(enumerate);
		devices = udev_enumerate_get_list_entry(enumerate);
		
		/* Create a list of the devices in the 'hidraw' subsystem. */
		udev_list_entry_foreach(dev_list_entry, devices) {
			const char *path;
			

			path = udev_list_entry_get_name(dev_list_entry);
			dev = udev_device_new_from_syspath(udev, path);

			/* usb_device_get_devnode() returns the path to the device node
				itself in /dev. */
			//printf("Device Node Path: %s\n", udev_device_get_devnode(dev));
			dev = udev_device_get_parent_with_subsystem_devtype(
					dev,
					"usb",
					"usb_device");
			if (!dev) {
				printf("Unable to find parent usb device.");
				exit(1);
			}
			Usb newDevice;
			const char* attribute = udev_device_get_sysattr_value(dev,"idVendor");
			
			std::strcpy(newDevice.deviceInfo.vendorID, attribute);

			attribute = udev_device_get_sysattr_value(dev, "idProduct");
			std::strcpy(newDevice.deviceInfo.productID, attribute);
			
			attribute = udev_device_get_sysattr_value(dev,"manufacturer");
			if(attribute){
				newDevice.deviceInfo.manufacturer = attribute;
			}

			attribute = udev_device_get_sysattr_value(dev,"product");
			if(attribute){
				newDevice.deviceInfo.product = attribute;
			}
			
			attribute = udev_device_get_sysattr_value(dev, "serial");
			if(attribute){
				std::strcpy(newDevice.deviceInfo.serialNumber, attribute);
			}
			usbList.push_back(newDevice);

			udev_device_unref(dev);
		}
	}
	std::vector<Usb> usbList= {};
	tools::PropertiesHolderList usbListProperties = tools::PropertiesHolderList(usbList);
	UsbDeviceInfo deviceInfo;
};

PROPERTIES_IMPL(UsbModule, 
	Property("Device Info", (const tools::PropertiesHolder&)UsbHub()),
)


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
		//self_ptr->restart();
		std::cout<<"DiscoverUsbDevicesComponent done\n";
		done();
	}
	void run() override {}

	void stop() override {
		done();
	}


	
};


struct UsbModule::MainComponent : public shared::ModuleComponentParented<UsbModule>{
	COMPONENT_DEF(acpl::modules::DiscoverUsbDevicesComponent, ModuleComponentParented)
	MainComponent(Parent_t& parent) : ModuleComponentParented(parent, ESSENTIAL){}
	void start() override {
		if(discoverUsbDevicesComponent.expired()){
			discoverUsbDevicesComponent = getParent().manager->addComponent<DiscoverUsbDevicesComponent>();
			discoverUsbReloadTimer = getParent().manager->addComponent<tools::TimerComponent>(
				discoverUsbDevicesComponent,
				DiscoverUsbDevicesComponent::timedReload,
				1000
			);

			
			//auto self_ptr = findSelf<MainComponent>(*getParent().manager).lock();
		}
		std::cout<<"MainComponent done\n";
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
	manager = std::make_unique<Manager>();
	manager->addComponent<MainComponent>(*this);
}
UsbModule::~UsbModule(){}


bool UsbModule::run(){
	return manager->handleComponents();
}



}}