#include "VMManager.h"
#include <iostream>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
#include <sstream>
#include <fstream>

std::string execCommand(const std::string& cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

bool VMManager::createVM(const std::string& vmName, const std::string& diskImage, 
                         const std::string& memorySize, int hostPort, int guestPort) {
    // Create a config file for the VM
    std::string configFile = "/etc/xen/" + vmName + ".cfg";
    std::ofstream file(configFile);
    if (!file.is_open()) {
        std::cerr << "Failed to create VM config file." << std::endl;
        return false;
    }

    // Writing a simple VM configuration for Xen
    file << "name = \"" << vmName << "\"\n";
    file << "memory = " << memorySize << "\n";
    file << "vcpus = 1\n"; // Number of virtual CPUs
    file << "disk = [\"file:" << diskImage << ",xvda,w\"]\n";
    file << "vif = [\"bridge=br0\"]\n";
    file << "boot = 'c'\n"; // Boot from disk
    file << "extra = \"root=/dev/xvda1 console=hvc0\"\n"; // Extra boot options for Linux guests
    file.close();

    // Command to start the VM
    std::string cmd = "xl create " + configFile;
    int result = system(cmd.c_str());
    return (result == 0);
}

bool VMManager::startVM(const std::string& vmName) {
    std::string cmd = "xl start " + vmName;
    int result = system(cmd.c_str());
    return (result == 0);
}

bool VMManager::stopVM(const std::string& vmName) {
    std::string cmd = "xl destroy " + vmName;
    int result = system(cmd.c_str());
    return (result == 0);
}

std::vector<std::string> VMManager::listVMs() {
    std::string cmd = "xl list --minimal";
    std::string output = execCommand(cmd);

    std::vector<std::string> vms;
    std::string line;
    std::istringstream stream(output);
    while (std::getline(stream, line)) {
        vms.push_back(line);
    }
    return vms;
}

std::string VMManager::getVMLogs(const std::string& vmName) {
    // Xen VMs typically log in syslog or Xen log files, you can modify this as needed
    std::string cmd = "xl dmesg";
    return execCommand(cmd);
}

std::string VMManager::inspectVM(const std::string& vmName) {
    std::string cmd = "xl info " + vmName;
    return execCommand(cmd);
}
