#ifndef VMMANAGER_H
#define VMMANAGER_H

#include <string>
#include <vector>

class VMManager {
public:
    static void createVM();
    static void listVMs();

private:
    static std::string getVMName();
};

#endif // VMMANAGER_H