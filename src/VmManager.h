#ifndef VMMANAGER_H
#define VMMANAGER_H

#include <string>
#include <vector>

class VMManager {
public:
    static std::string createVM(const std::string& vmName);
    static void listVMs();

private:
    static std::string getVMName();
    static std::string getVMIPAddress(const std::string& vmName);
    static void executePostInstallScript(const std::string& vmName,const std::string& vmIP, const std::string& postInstallScriptPath);
};

#endif // VMMANAGER_H