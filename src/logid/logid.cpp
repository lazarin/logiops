/*
 * Copyright 2019-2020 PixlOne
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <mutex>

#include "util/log.h"
#include "DeviceManager.h"
#include "logid.h"

#define evdev_name "logid"
#define DEFAULT_CONFIG_FILE "/etc/logid.cfg"

#ifndef LOGIOPS_VERSION
#define LOGIOPS_VERSION "null"
#warning Version is undefined!
#endif

using namespace logid;

std::string config_file = DEFAULT_CONFIG_FILE;

LogLevel logid::global_loglevel = INFO;
// Configuration* logid::global_config;
DeviceManager* logid::finder;

bool logid::kill_logid = false;
std::mutex logid::finder_reloading;

enum class Option
{
    None,
    Verbose,
    Config,
    Help,
    Version
};

/*
void logid::reload()
{
    log_printf(INFO, "Reloading logid...");
    finder_reloading.lock();
    finder->stop();
    Configuration* old_config = global_config;
    global_config = new Configuration(config_file.c_str());
    delete(old_config);
    delete(finder);
    finder = new DeviceMonitor();
    finder_reloading.unlock();
}
 */

void readCliOptions(int argc, char** argv)
{
    for(int i = 1; i < argc; i++) {
        Option option = Option::None;
        if(argv[i][0] == '-') {
            // This argument is an option
            switch(argv[i][1]) {
            case '-': {
                // Full option name
                std::string op_str = argv[i];
                if (op_str == "--verbose") option = Option::Verbose;
                if (op_str == "--config") option = Option::Config;
                if (op_str == "--help") option = Option::Help;
                if (op_str == "--version") option = Option::Version;
                break;
            }
            case 'v': // Verbosity
                option = Option::Verbose;
                break;
            case 'V': //Version
                    option = Option::Version;
                    break;
            case 'c': // Config file path
                option = Option::Config;
                break;
            case 'h': // Help
                option = Option::Help;
                break;
            default:
                logPrintf(WARN, "%s is not a valid option, ignoring.",
                        argv[i]);
            }

            switch(option) {
            case Option::Verbose: {
                if (++i >= argc) {
                    global_loglevel = DEBUG; // Assume debug verbosity
                    break;
                }
                std::string loglevel = argv[i];
                try {
                    global_loglevel = toLogLevel(argv[i]);
                } catch (std::invalid_argument &e) {
                    if (argv[i][0] == '-') {
                        global_loglevel = DEBUG; // Assume debug verbosity
                        i--; // Go back to last argument to continue loop.
                    } else {
                        logPrintf(WARN, e.what());
                        printf("Valid verbosity levels are: Debug, Info, "
                               "Warn/Warning, or Error.\n");
                        exit(EXIT_FAILURE);
                    }
                }
                    break;
            }
            case Option::Config: {
                if (++i >= argc) {
                    logPrintf(ERROR, "Config file is not specified.");
                    exit(EXIT_FAILURE);
                }
                config_file = argv[i];
                break;
            }
            case Option::Help:
                printf(R"(logid version %s
Usage: %s [options]
Possible options are:
    -v,--verbose [level]       Set log level to debug/info/warn/error (leave blank for debug)
    -V,--version               Print version number
    -c,--config [file path]    Change config file from default at %s
    -h,--help                  Print this message.
)", LOGIOPS_VERSION, argv[0], DEFAULT_CONFIG_FILE);
                exit(EXIT_SUCCESS);
            case Option::Version:
                printf("%s\n", LOGIOPS_VERSION);
                exit(EXIT_SUCCESS);
            case Option::None:
                break;
            }
        }
    }
}

int main(int argc, char** argv)
{
    readCliOptions(argc, argv);

    /*
    // Read config
    try { global_config = new Configuration(config_file.c_str()); }
    catch (std::exception &e) { global_config = new Configuration(); }


    //Create an evdev device called 'logid'
    try { global_evdev = new EvdevDevice(evdev_name); }
    catch(std::system_error& e)
    {
        log_printf(ERROR, "Could not create evdev device: %s", e.what());
        return EXIT_FAILURE;
    }

    */
    
    // Scan devices, create listeners, handlers, etc.
    finder = new DeviceManager();

    while(!kill_logid) {
        finder_reloading.lock();
        finder_reloading.unlock();
        finder->run();
    }

    return EXIT_SUCCESS;
}
