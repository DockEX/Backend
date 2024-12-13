// // #include "VmManager.h"
// // #include <iostream>
// // #include <fstream>
// // #include <cstdlib>
// // #include <cstring> 
// // #include <chrono>
// // #include <thread>

// // bool VMManager::createVM(const std::string& vmName) {
// //     // Create a simple cloud-init script
// //     std::string cloudInitScript = "/tmp/cloud-init-" + vmName + ".cfg";
// //     std::ofstream cis(cloudInitScript);
// //     if (!cis.is_open()) {
// //         std::cerr << "Failed to open cloud-init script file." << std::endl;
// //         return false;
// //     }

// //     // Write basic cloud-init script
// //     cis << "#cloud-config\n";
// //     cis << "users:\n";
// //     cis << "- name: ubuntu\n";
// //     cis << "  sudo: ['ALL=(ALL) NOPASSWD:ALL']\n";
// //     cis << "  shell: /bin/bash\n";
// //     cis.close();

// //     // Create an ISO file from the cloud-init script
// //     std::string isoFile = "/tmp/cloud-init-" + vmName + ".iso";
// //     std::string mkisofsCmd = "genisoimage -output " + isoFile + " -volid cidata -joliet -rock " + cloudInitScript;
// //     int mkisofsResult = system(mkisofsCmd.c_str());
// //     if (mkisofsResult != 0) {
// //         std::cerr << "Failed to create ISO file for cloud-init." << std::endl;
// //         remove(cloudInitScript.c_str());
// //         return false;
// //     }

// //     std::string volCheckCmd = "virsh vol-dumpxml default/" + vmName;
// //     FILE* pipe = popen(volCheckCmd.c_str(), "r");
// //     if (!pipe) {
// //         std::cerr << "Error checking volume existence" << std::endl;
// //         return false;
// //     }
    
// //     char buffer[128];
// //     while (fgets(buffer, 128, pipe) != NULL);
// //     pclose(pipe);

// //     if (strcmp(buffer, "<volume><name>") == 0) {
// //         std::cout << "Volume '" << vmName << "' already exists." << std::endl;
// //         return true; // Return early if volume already exists
// //     }

// //     std::string volExistsCmd = "virsh vol-info --pool default " + vmName;
// //     int volExistsResult = system(volExistsCmd.c_str());
    
// //     if (volExistsResult == 0) {
// //         std::cout << "Volume '" << vmName << "' already exists. Deleting..." << std::endl;
// //         std::string deleteVolCmd = "sudo virsh vol-delete default " + vmName;
// //         int deleteResult = system(deleteVolCmd.c_str());
// //         if (deleteResult != 0) {
// //             std::cerr << "Error deleting volume: " << deleteResult << std::endl;
// //         } else {
// //             std::cout << "Successfully deleted volume '" << vmName << "'" << std::endl;
// //         }
// //     }

// //     // Create the disk image
// //     std::string imgCmd = "qemu-img create -f qcow2 /var/lib/libvirt/images/" + vmName + ".img 20G";
// //     int imgResult = system(imgCmd.c_str());
// //     if (imgResult != 0) {
// //         std::cerr << "Error creating disk image: " << imgResult << std::endl;
// //         remove((const char*)("/var/lib/libvirt/images/" + vmName + ".img").c_str());
// //         return false;
// //     }

// //     // Create the volume
// //     std::string volCmd = "virsh vol-create-as default " + vmName + " " + std::to_string(20 * 1024 * 1024); // 20GB
// //     int volResult = system(volCmd.c_str());
// //     if (volResult != 0) {
// //         std::cerr << "Error creating volume: " << volResult << std::endl;
// //         remove((const char*)("/var/lib/libvirt/images/" + vmName + ".img").c_str());
// //         return false;
// //     }

// //     // Create the VM using virt-install
// //     std::string cmd = "sudo virt-install --name=" + vmName +
// //                   " --memory=2048 --vcpus=2 --disk vol=default/" + vmName + ",device=disk,bus=scsi,format=qcow2" +
// //                   " --network bridge=virbr0 --graphics none --os-variant u20.04 --os-type linux" +
// //                   " --location " + isoFile; // Changed from --cdrom to --location

// //     // Execute the command
// //     int result = system(cmd.c_str());
    
// //     if (result != 0) {
// //         std::cerr << "Error creating VM: Command failed with exit status " << result << std::endl;
        
// //         // Stop the VM if it's running
// //         std::string stopCmd = "virsh shutdown --force " + vmName;
// //         system(stopCmd.c_str());
        
// //         remove((const char*)("/var/lib/libvirt/images/" + vmName + ".img").c_str());
// //         return false;
// //     }

// //     // Wait for up to 30 seconds for the installation to complete
// //     auto start = std::chrono::steady_clock::now();
// //     while (true) {
// //         auto end = std::chrono::steady_clock::now();
// //         auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
// //         if (duration.count() > 30) break; // Exit after 30 seconds
        
// //         // Check if the VM is still running
// //         std::string statusCmd = "virsh dominfo " + vmName + " | grep 'State:'";
// //         FILE* pipe = popen(statusCmd.c_str(), "r");
// //         if (!pipe) continue;
// //         char buffer[128];
// //         while (fgets(buffer, 128, pipe)) {
// //             if (strstr(buffer, "running") == NULL) {
// //                 pclose(pipe);
// //                 break;
// //             }
// //         }
// //         pclose(pipe);
// //         std::this_thread::sleep_for(std::chrono::seconds(1));
// //     }

// //     // Clean up temporary files
// //     remove((const char*)cloudInitScript.c_str());
// //     remove((const char*)isoFile.c_str());

// //     std::cout << "VM '" << vmName << "' created successfully!" << std::endl;
// //     return true;
// // }

// #include "VmManager.h"
// #include <iostream>
// #include <fstream>
// #include <cstdlib>
// #include <cstring> 
// #include <chrono>
// #include <thread>

// // Function to create a VM
// bool VMManager::createVM(const std::string& vmName) {
//     // Path for the cloud-init script
//     std::string cloudInitScript = "/tmp/cloud-init-" + vmName + ".cfg";
    
//     // Create and write to the cloud-init script
//     std::ofstream cis(cloudInitScript);
//     if (!cis.is_open()) {
//         std::cerr << "Failed to open cloud-init script file at " << cloudInitScript << std::endl;
//         return false;
//     }

//     // Write basic cloud-init configuration
//     cis << "#cloud-config\n";
//     cis << "users:\n";
//     cis << "- name: ubuntu\n";
//     cis << "  sudo: ['ALL=(ALL) NOPASSWD:ALL']\n";
//     cis << "  shell: /bin/bash\n";
//     cis.close();
//     std::cout << "Cloud-init script created at: " << cloudInitScript << std::endl;

//     // Path for the ISO file
//     std::string isoFile = "/tmp/cloud-init-" + vmName + ".iso";

//     // Create the ISO file using xorriso
//     std::string mkisofsCmd = "xorriso -as genisoimage "
//                          "-o " + isoFile +
//                          " -volid CIDATA -joliet -rock "
//                          + cloudInitScript + "=user-data";

//     int mkisofsResult = system(mkisofsCmd.c_str());
//     if (mkisofsResult != 0) {
//         std::cerr << "Failed to create ISO file for cloud-init. Command: " << mkisofsCmd << std::endl;
//         remove(cloudInitScript.c_str());
//         return false;
//     }
//     std::cout << "ISO file created successfully: " << isoFile << std::endl;

//     // Check if volume exists
//     std::string volExistsCmd = "virsh vol-info --pool default " + vmName;
//     int volExistsResult = system(volExistsCmd.c_str());
//     if (volExistsResult == 0) {
//         std::cout << "Volume '" << vmName << "' already exists. Deleting..." << std::endl;
//         std::string deleteVolCmd = "sudo virsh vol-delete --pool default " + vmName;
//         int deleteResult = system(deleteVolCmd.c_str());
//         if (deleteResult != 0) {
//             std::cerr << "Error deleting volume: " << deleteResult << std::endl;
//             return false;
//         }
//         std::cout << "Successfully deleted volume '" << vmName << "'" << std::endl;
//     }

//     // Create the disk image
//     std::string imgCmd = "qemu-img create -f qcow2 /var/lib/libvirt/images/" + vmName + ".img 20G";
//     int imgResult = system(imgCmd.c_str());
//     if (imgResult != 0) {
//         std::cerr << "Error creating disk image: " << imgResult << std::endl;
//         remove((const char*)("/var/lib/libvirt/images/" + vmName + ".img").c_str());
//         return false;
//     }
//     std::cout << "Disk image created: " << "/var/lib/libvirt/images/" + vmName + ".img" << std::endl;

//     // Create the volume
//     std::string volCmd = "virsh vol-create-as default " + vmName + " 20G --format qcow2";
//     int volResult = system(volCmd.c_str());
//     if (volResult != 0) {
//         std::cerr << "Error creating volume: " << volResult << std::endl;
//         remove((const char*)("/var/lib/libvirt/images/" + vmName + ".img").c_str());
//         return false;
//     }
//     std::cout << "Volume created: " << vmName << std::endl;

//     // Create the VM using virt-install
//     std::string cmd = "sudo virt-install --name=" + vmName +
//                       " --memory=512 --vcpus=1 --disk vol=default/" + vmName + ",device=disk,bus=scsi,format=qcow2" +
//                       " --network bridge=virbr0 --graphics none --os-variant ubuntu20.04 --os-type linux" +
//                       " --location " + isoFile;
//     int result = system(cmd.c_str());
//     if (result != 0) {
//         std::cerr << "Error creating VM: Command failed with exit status " << result << std::endl;
        
//         // Cleanup on failure
//         std::string stopCmd = "virsh destroy " + vmName;
//         system(stopCmd.c_str());
//         remove((const char*)("/var/lib/libvirt/images/" + vmName + ".img").c_str());
//         return false;
//     }
//     std::cout << "VM created successfully: " << vmName << std::endl;

//     // Wait for VM to start (max 30 seconds)
//     auto start = std::chrono::steady_clock::now();
//     while (true) {
//         auto end = std::chrono::steady_clock::now();
//         auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
//         if (duration.count() > 30) {
//             std::cerr << "VM creation timed out." << std::endl;
//             break;
//         }

//         // Check if the VM is running
//         std::string statusCmd = "virsh dominfo " + vmName + " | grep 'State:'";
//         FILE* pipe = popen(statusCmd.c_str(), "r");
//         if (!pipe) continue;
//         char buffer[128];
//         while (fgets(buffer, 128, pipe)) {
//             if (strstr(buffer, "running") != NULL) {
//                 std::cout << "VM is running: " << vmName << std::endl;
//                 pclose(pipe);
//                 goto cleanup; // Exit the loop if VM is running
//             }
//         }
//         pclose(pipe);
//         std::this_thread::sleep_for(std::chrono::seconds(1));
//     }

// cleanup:
//     // Clean up temporary files
//     remove(cloudInitScript.c_str());
//     remove(isoFile.c_str());
//     std::cout << "Temporary files cleaned up." << std::endl;

//     return true;
// }


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