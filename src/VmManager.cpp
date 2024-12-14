#include "VmManager.h"
#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;

void VMManager::createVM() {
    string vmName = getVMName();

    // Correctly formatted virt-install script with line continuation
    string scriptContent = 
        "#!/bin/bash\n"
        "sudo virt-install --name=" + vmName + " \\\n"
        "--memory=2048 \\\n"
        "--vcpus=1 \\\n"
        "--disk size=5,bus=virtio,device=disk,format=qcow2,sparse=true \\\n"
        "--os-variant ubuntu20.04 \\\n"
        "--graphics spice,listen=127.0.0.1 \\\n"
        "--noautoconsole \\\n"
        "--location /var/lib/libvirt/images/ubuntu-20.04.3-desktop-amd64.iso";

    // Write the script to a file
    ofstream scriptFile("/tmp/create_vm.sh");
    scriptFile << scriptContent;
    scriptFile.close();

    // Make the script executable and execute it
    system("chmod +x /tmp/create_vm.sh && /tmp/create_vm.sh");
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