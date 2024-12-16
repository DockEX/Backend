#include "VmManager.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <thread>
#include <chrono>

using namespace std;

string VMManager::createVM(const std::string& vmName) {
    cout << "Creating VM: " << vmName << endl;

    int retCode = system("chmod +x /tmp/fetch_public_key.sh && /tmp/fetch_public_key.sh");
    if (retCode != 0) {
        cerr << "Error fetching public key from host" << endl;
        return "";
    }

    string scriptContent =
        "#!/bin/bash\n"
        "sudo virt-install --name=" + vmName + " \\\n"
        "--memory=2048 \\\n"
        "--vcpus=1 \\\n"
        "--disk size=4,bus=virtio,device=disk,format=qcow2,sparse=true \\\n"
        "--os-variant ubuntu20.04 \\\n"
        "--graphics spice,listen=127.0.0.1 \\\n"
        "--noautoconsole \\\n"
        "--location /var/lib/libvirt/images/ubuntu-20.04.6-desktop-amd64.iso\n";

    ofstream scriptFile("/tmp/create_vm.sh");
    scriptFile << scriptContent;
    scriptFile.close();

    retCode = system("chmod +x /tmp/create_vm.sh && /tmp/create_vm.sh");
    if (retCode != 0) {
        cerr << "Error executing VM creation script" << endl;
        return "";
    }

    cout << "Waiting for the VM to boot and acquire an IP address..." << endl;

    string vmIP;
    int maxRetries = 20; 
    int retryInterval = 5;  

    for (int i = 0; i < maxRetries; ++i) {
        vmIP = getVMIPAddress(vmName);
        if (!vmIP.empty()) {
            cout << "VM " << vmName << " created successfully with IP: " << vmIP << endl;

            // transferAndExecuteSSHSetupScript(vmIP);
            
            return vmIP;
        }

        cout << "Retrying to fetch IP address (" << i + 1 << "/" << maxRetries << ")..." << endl;
        std::this_thread::sleep_for(std::chrono::seconds(retryInterval));
    }

    cerr << "Failed to retrieve the IP address of the VM after multiple retries" << endl;
    return "";
}

void VMManager::transferAndExecuteSSHSetupScript(const string& vmIP) {
    cout << "Transferring SSH setup script to VM: " << vmIP << endl;

    string scpCommand = "scp /tmp/setup_vm_ssh.sh ubuntu@" + vmIP + ":/tmp/setup_vm_ssh.sh";
    int retCode = system(scpCommand.c_str());
    if (retCode != 0) {
        cerr << "Error transferring SSH setup script to the VM" << endl;
        return;
    }

    string sshCommand = "ssh ubuntu@" + vmIP + " 'chmod +x /tmp/setup_vm_ssh.sh && /tmp/setup_vm_ssh.sh'";
    retCode = system(sshCommand.c_str());
    if (retCode != 0) {
        cerr << "Error executing SSH setup script on the VM" << endl;
    } else {
        cout << "SSH key has been successfully set up on VM: " << vmIP << endl;
    }
}

string VMManager::getVMIPAddress(const std::string& vmName) {
    cout << "Fetching IP address for VM: " << vmName << endl;

    string macCommand = "virsh domiflist " + vmName + " | grep -oE '([0-9a-f]{2}:){5}[0-9a-f]{2}'";
    FILE* macPipe = popen(macCommand.c_str(), "r");
    if (!macPipe) {
        cerr << "Error retrieving MAC address" << endl;
        return "";
    }

    char macBuffer[128];
    fgets(macBuffer, sizeof(macBuffer), macPipe);
    pclose(macPipe);

    string macAddress = string(macBuffer);
    macAddress.erase(macAddress.find_last_not_of(" \n\r\t") + 1);  
    cout << "MAC Address: " << macAddress << endl;

    if (macAddress.empty()) {
        cerr << "MAC address not found for VM: " << vmName << endl;
        return "";
    }

    string leaseCommand = "virsh net-dhcp-leases default | grep " + macAddress;
    FILE* leasePipe = popen(leaseCommand.c_str(), "r");
    if (!leasePipe) {
        cerr << "Error retrieving DHCP lease information" << endl;
        return "";
    }

    char leaseBuffer[256];
    if (fgets(leaseBuffer, sizeof(leaseBuffer), leasePipe) == nullptr) {
        cerr << "No lease found for VM: " << vmName << endl;
        pclose(leasePipe);
        return "";
    }

    pclose(leasePipe);

    string leaseOutput = string(leaseBuffer);

    size_t firstSpacePos = leaseOutput.find(' ');
    if (firstSpacePos == string::npos) {
        cerr << "Failed to extract IP address from lease output" << endl;
        return "";
    }

    size_t secondSpacePos = leaseOutput.find(' ', firstSpacePos + 1);
    if (secondSpacePos == string::npos) {
        cerr << "Failed to extract IP address from lease output" << endl;
        return "";
    }
    
    size_t ipPos = leaseOutput.find("ipv4");
    if (ipPos == std::string::npos) {
        return ""; 
    }
    std::string ipAddress = leaseOutput.substr(ipPos + 5, leaseOutput.find("/", ipPos) - ipPos - 5);

    ipAddress.erase(0, ipAddress.find_first_not_of(" \n\r\t"));
    ipAddress.erase(ipAddress.find_last_not_of(" \n\r\t") + 1);

    return ipAddress;
}

void VMManager::listVMs() {
    string command = "sudo virsh list --all";
    system(command.c_str());
}

string VMManager::getVMName() {
    cout << "Enter VM name: ";
    string vmName;
    cin >> vmName;
    return vmName;
}