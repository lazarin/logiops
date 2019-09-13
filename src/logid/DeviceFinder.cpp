#include <hid/DeviceMonitor.h>
#include <hidpp/SimpleDispatcher.h>
#include <hidpp/Device.h>
#include <hidpp10/Error.h>
#include <hidpp10/IReceiver.h>
#include <hidpp20/Error.h>
#include <cstring>
#include <unistd.h>
#include <thread>
#include <fstream>
#include <sstream>

#include "DeviceFinder.h"
#include "util.h"
#include "Device.h"

void DeviceFinder::addDevice(const char *path)
{
    std::string string_path(path);
    // Asynchronously scan device
    std::thread{[=]()
    {
        const int max_tries = 10;
        const int try_delay = 250000;

        //Check if device is an HID++ device and handle it accordingly
        try
        {
            HIDPP::SimpleDispatcher dispatcher(string_path.c_str());
            bool has_receiver_index = false;
            for(HIDPP::DeviceIndex index: {
                HIDPP::DefaultDevice, HIDPP::CordedDevice,
                HIDPP::WirelessDevice1, HIDPP::WirelessDevice2,
                HIDPP::WirelessDevice3, HIDPP::WirelessDevice4,
                HIDPP::WirelessDevice5, HIDPP::WirelessDevice6})
            {
                if(!has_receiver_index && index == HIDPP::WirelessDevice1)
                    break;
                for(int i = 0; i < max_tries; i++)
                {
                    try
                    {
                        HIDPP::Device d(&dispatcher, index);
                        auto version = d.protocolVersion();
                        uint major, minor;
                        std::tie(major, minor) = version;
                        if(index == HIDPP::DefaultDevice && version == std::make_tuple(1, 0))
                            has_receiver_index = true;
                        if(major > 1) // HID++ 2.0 devices only
                        {
                            auto dev = new Device(string_path, index);

							this->devicesMutex.lock();
								this->devices.insert({
									dev,
									std::thread{[dev]() {
										dev->start();
									}}
								});
							this->devicesMutex.unlock();

                            log_printf(INFO, "%s detected: device %d on %s", d.name().c_str(), index, string_path.c_str());
                        }
                        break;
                    }
                    catch(HIDPP10::Error &e)
                    {
                        if(e.errorCode() != HIDPP10::Error::UnknownDevice)
                        {
                            if(i == max_tries - 1)
                                log_printf(ERROR, "Error while querying %s, wireless device %d: %s", string_path.c_str(), index, e.what());
                            else usleep(try_delay);
                        }
                        else break;
                    }
                    catch(HIDPP20::Error &e)
                    {
                        if(e.errorCode() != HIDPP20::Error::UnknownDevice)
                        {
                            if(i == max_tries - 1)
                                log_printf(ERROR, "Error while querying %s, device %d: %s", string_path.c_str(), index, e.what());
                            else usleep(try_delay);
                        }
                        else break;
                    }
                    catch(HIDPP::Dispatcher::TimeoutError &e)
                    {
                        if(i == max_tries - 1)
                            log_printf(ERROR, "Device %s (index %d) timed out.", string_path.c_str(), index);
                        else usleep(try_delay);
                    }
                    catch(std::runtime_error &e)
                    {
                        if(i == max_tries - 1)
                            log_printf(ERROR, "Runtime error on device %d on %s: %s", index, string_path.c_str(), e.what());
                        else usleep(try_delay);
                    }
                }
            }
        }
        catch(HIDPP::Dispatcher::NoHIDPPReportException &e) { }
        catch(std::system_error &e) { log_printf(WARN, "Failed to open %s: %s", string_path.c_str(), e.what()); }

    }}.detach();
}

void DeviceFinder::removeDevice(const char* path)
{
	devicesMutex.lock();
		// Iterate through Devices, stop all in path
		for (auto it = devices.begin(); it != devices.end(); it++)
		{
			if(it->first->path == path)
			{
				log_printf(INFO, "%s on %s disconnected.", it->first->name.c_str(), path);
				it->first->stop();
				it->second.join();
				delete(it->first);
				devices.erase(it);
			}
		}
	devicesMutex.unlock();
}
