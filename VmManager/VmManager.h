#ifndef VM_MANAGER_H
#define VM_MANAGER_H

#include <string>
#include <vector>

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