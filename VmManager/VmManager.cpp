#ifndef VM_MANAGER_H
#define VM_MANAGER_H
#include <iostream>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>

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
    std::string configFile = "/etc/libvirt/qemu/" + vmName + ".xml";
    std::ofstream file(configFile);
    if (!file.is_open()) {
        std::cerr << "Failed to create VM config file." << std::endl;
        return false;
    }

    // Writing a KVM configuration XML
    file << "<?xml version='1.0' encoding='UTF-8'?>\n"
         << "<domain type='kvm'>\n"
         << "  <name>" << vmName << "</name>\n"
         << "  <memory>" << memorySize << "</memory>\n"
         << "  <vcpu>1</vcpu>\n"
         << "  <devices>\n"
         << "    <disk type='file' device='disk'>\n"
         << "      <driver name='qemu' type='raw'/>\n"
         << "      <source file='" << diskImage << "'/>" << "\n"
         << "      <target dev='hda' bus='ide'/>" << "\n"
         << "    </disk>\n"
         << "    <interface type='bridge'>\n"
         << "      <source bridge='virbr0'/>" << "\n"
         << "    </interface>\n"
         << "  </devices>\n"
         << "  <graphics type='vnc' port='-1' autoport='yes'/>" << "\n"
         << "  <console type='pty' tty='/dev/pts/0'/>" << "\n"
         << "  <input type='tablet' model='usb-tablet'/>" << "\n"
         << "  <video>\n"
         << "    <model type='cirrus' vram='16384' heads='1'/>" << "\n"
         << "  </video>\n"
         << "</domain>\n";

    file.close();

    // Command to start the VM
    std::string cmd = "virsh start " + vmName;
    int result = system(cmd.c_str());
    return (result == 0);
}

bool VMManager::startVM(const std::string& vmName) {
    std::string cmd = "virsh start " + vmName;
    int result = system(cmd.c_str());
    return (result == 0);
}

bool VMManager::stopVM(const std::string& vmName) {
    std::string cmd = "virsh destroy " + vmName;
    int result = system(cmd.c_str());
    return (result == 0);
}

std::vector<std::string> VMManager::listVMs() {
    std::string cmd = "virsh list --all";
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
    std::string cmd = "journalctl -u libvirtd -f";
    return execCommand(cmd);
}

std::string VMManager::inspectVM(const std::string& vmName) {
    std::string cmd = "virsh dominfo " + vmName;
    return execCommand(cmd);
}

class VMManager {
public:
    VMManager() = default;
    
    bool createVM(const std::string& vmName, const std::string& repoContents);
    bool startVM(const std::string& vmName);
    bool stopVM(const std::string& vmName);
    std::vector<std::string> listVMs();
    std::string getVMLogs(const std::string& vmName);
    std::string inspectVM(const std::string& vmName);
};

#endif
