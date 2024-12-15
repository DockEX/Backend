#include "VmManager.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <thread>
#include <chrono>

using namespace std;

string VMManager::createVM(const std::string& vmName) {
    cout << "Creating VM: " << vmName << endl;

    // Construct the virt-install script to create the VM
    string scriptContent =
        "#!/bin/bash\n"
        "sudo virt-install --name=" + vmName + " \\\n"
        "--memory=1024 \\\n"
        "--vcpus=1 \\\n"
        "--disk size=3,bus=virtio,device=disk,format=qcow2,sparse=true \\\n"
        "--os-variant ubuntu20.04 \\\n"
        "--graphics spice,listen=127.0.0.1 \\\n"
        "--noautoconsole \\\n"
        "--location /var/lib/libvirt/images/ubuntu-20.04.3-desktop-amd64.iso\n";

    // Write script to a file
    ofstream scriptFile("/tmp/create_vm.sh");
    scriptFile << scriptContent;
    scriptFile.close();

    // Execute the script to create the VM
    int retCode = system("chmod +x /tmp/create_vm.sh && /tmp/create_vm.sh");
    if (retCode != 0) {
        cerr << "Error executing VM creation script" << endl;
        return "";
    }

    // Sleep for 20 seconds to allow the VM to boot and acquire an IP
    cout << "Waiting for the VM to boot and acquire an IP address..." << endl;
    std::this_thread::sleep_for(std::chrono::seconds(60));

    // Fetch the VM's IP address
    string vmIP = getVMIPAddress(vmName);
    if (vmIP.empty()) {
        cerr << "Failed to retrieve the IP address of the VM" << endl;
        return "";
    } else {
        cout << "VM " << vmName << " created successfully with IP: " << vmIP << endl;
    }

    // Transfer and execute the post-installation script
    // string postInstallScriptPath = "/tmp/post_install_" + vmName + ".sh";
    // executePostInstallScript(vmName, vmIP, postInstallScriptPath);

    return vmIP;
}

void VMManager::executePostInstallScript(const std::string& vmName, const std::string& vmIP, const std::string& postInstallScriptPath) {
    // Create a post-installation script to set up SSH and configure firewall
    string postInstallScript =
        "#!/bin/bash\n"
        "sudo apt update\n"
        "sudo apt install -y net-tools\n"
        "sudo apt install -y openssh-server\n"
        "sudo systemctl start ssh\n"
        "sudo systemctl enable ssh\n"
        "sudo ufw allow ssh\n"
        "sudo ufw enable\n";

    // Save the post-installation script to a local file
    ofstream postInstallFile(postInstallScriptPath);
    postInstallFile << postInstallScript;
    postInstallFile.close();

    // Use virt-copy-in to copy the script to the VM's root directory
    string copyCommand = "sudo virt-copy-in -a /var/lib/libvirt/images/" + vmName + ".img " + postInstallScriptPath + " /tmp/";
    int retCode = system(copyCommand.c_str());
    if (retCode != 0) {
        cerr << "Error copying the post-installation script to the VM" << endl;
        return;
    }

    // Execute the script inside the VM via SSH
    string sshCommand = "ssh -o StrictHostKeyChecking=no ubuntu@" + vmIP + " 'bash -s' < /tmp/" + postInstallScriptPath;
    retCode = system(sshCommand.c_str());
    if (retCode != 0) {
        cerr << "Error executing post-installation script on the VM" << endl;
    } else {
        cout << "Post-installation setup completed successfully on VM " << vmName << endl;
    }
}

string VMManager::getVMIPAddress(const std::string& vmName) {
    cout << "Fetching IP address for VM: " << vmName << endl;

    // Command to fetch the MAC address of the VM
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
    macAddress.erase(macAddress.find_last_not_of(" \n\r\t") + 1);  // Trim whitespace

    if (macAddress.empty()) {
        cerr << "MAC address not found for VM: " << vmName << endl;
        return "";
    }

    // Command to fetch the IP address using the MAC address
    string ipCommand = "arp -n | grep " + macAddress + " | awk '{print $1}'";
    FILE* ipPipe = popen(ipCommand.c_str(), "r");
    if (!ipPipe) {
        cerr << "Error retrieving IP address" << endl;
        return "";
    }

    char ipBuffer[128];
    fgets(ipBuffer, sizeof(ipBuffer), ipPipe);
    pclose(ipPipe);

    string ipAddress = string(ipBuffer);
    ipAddress.erase(ipAddress.find_last_not_of(" \n\r\t") + 1);  // Trim whitespace

    return ipAddress;
}


// string VMManager::getVMIPAddress(const std::string& vmName) {
//     cout << "Fetching IP address for VM: " << vmName << endl;

//     // Command to fetch the MAC address of the VM
//     string macCommand = "virsh domiflist " + vmName + " | grep -oE '([0-9a-f]{2}:){5}[0-9a-f]{2}'";
//     FILE* macPipe = popen(macCommand.c_str(), "r");
//     if (!macPipe) {
//         cerr << "Error retrieving MAC address" << endl;
//         return "";
//     }

//     char macBuffer[128];
//     fgets(macBuffer, sizeof(macBuffer), macPipe);
//     pclose(macPipe);

//     string macAddress = string(macBuffer);
//     macAddress.erase(macAddress.find_last_not_of(" \n\r\t") + 1);

//     if (macAddress.empty()) {
//         cerr << "MAC address not found for VM: " << vmName << endl;
//         return "";
//     }

//     // Command to fetch the IP address using the MAC address
//     string ipCommand = "arp -n | grep " + macAddress + " | awk '{print $1}'";
//     FILE* ipPipe = popen(ipCommand.c_str(), "r");
//     if (!ipPipe) {
//         cerr << "Error retrieving IP address" << endl;
//         return "";
//     }

//     char ipBuffer[128];
//     fgets(ipBuffer, sizeof(ipBuffer), ipPipe);
//     pclose(ipPipe);

//     string ipAddress = string(ipBuffer);
//     ipAddress.erase(ipAddress.find_last_not_of(" \n\r\t") + 1);  

//     return ipAddress;
// }

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


/* 
#include "VmManager.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>

using namespace std;

void VMManager::createVM() {
    string vmName = getVMName();
    
    // Create the VM using virt-install
    string scriptContent = 
        "#!/bin/bash\n"
        "sudo virt-install --name=" + vmName + "\n"
        "--memory=1024\n"
        "--vcpus=1\n"
        "--disk size=5,bus=virtio,device=disk,format=qcow2,sparse=true\n"
        "--os-variant ubuntu20.04\n"
        "--graphics spice,listen=127.0.0.1\n"
        "--noautoconsole\n"
        "--location /var/lib/libvirt/images/ubuntu-20.04-desktop-amd64.iso\n"
        "--extra-args='console=ttyS0,115200'\n";

    ofstream scriptFile("/tmp/create_vm.sh");
    scriptFile << scriptContent;
    scriptFile.close();

    system("chmod +x /tmp/create_vm.sh && /tmp/create_vm.sh");

    // Wait for the VM to start up (adjust the sleep time as needed)
    sleep(30);

    // Install SSH server
    system("sudo virsh console " + vmName);
    cout << "Press Enter when the VM console opens..." << endl;
    cin.get();

    // Send commands to install SSH server
    cout << "Installing SSH server..." << endl;
    system("echo 'sudo apt update' | sudo virsh send-key " + vmName);
    system("echo 'sudo apt install -y openssh-server' | sudo virsh send-key " + vmName);
    system("echo 'sudo systemctl enable ssh' | sudo virsh send-key " + vmName);
    system("echo 'sudo systemctl start ssh' | sudo virsh send-key " + vmName);

    // Retrieve IP address
    string ipCommand = "sudo virsh domifaddr " + vmName;
    string ipAddress;
    FILE* pipe = popen(ipCommand.c_str(), "r");
    if (!pipe) throw runtime_error("popen() failed!");
    char buffer[128];
    while (fgets(buffer, 128, pipe) != NULL)
        ipAddress += buffer;
    pclose(pipe);

    // Close the VM console
    system("sudo virsh detach-console " + vmName);

    // Display IP address to user
    cout << "\nVM " << vmName << " IP Address: " << ipAddress << endl;
}

// ... rest of the file remains unchanged ...
*/