#ifndef VMMANAGER_H
#define VMMANAGER_H

#include <string>
#include <vector>
#include "crow_all.h"  


class VMManager {
public:
    static std::string createVM(const std::string& vmName);
    static void listVMs();
    // static std::string createVMWithUpdates(const std::string& vmName, crow::response& res);

private:
    static std::string getVMName();
    static std::string getVMIPAddress(const std::string& vmName);
    // static void generateSetupScript(const std::string& vmName);
    // static void executeSetupScript(const std::string& vmName);
    static void transferAndExecuteSSHSetupScript(const std::string& vmIP);
    // static void executePostInstallScript(const std::string& vmName,const std::string& vmIP, const std::string& postInstallScriptPath);
};

#endif // VMMANAGER_H