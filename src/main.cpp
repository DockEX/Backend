// VM Based Infrastructure

#include <iostream>
#include <string>
#include <vector>
#include "VmManager.h"

int main() {
    std::cout << "Welcome to the VM Manager!" << std::endl;
    
    while (true) {
        std::cout << "\n1. Create VM" << std::endl;
        std::cout << "2. List VMs" << std::endl;
        std::cout << "3. Exit" << std::endl;
        
        int choice;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1:
                VMManager::createVM();
                break;
            case 2:
                VMManager::listVMs();
                break;
            case 3:
                std::cout << "Exiting..." << std::endl;
                return 0;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
        }
    }

    return 0;
}



// Docker Based Infrastructure

// #include <iostream>
// #include <string>
// #include "DockerManager/DockerManager.h"

// int main() {
//     DockerManager dm;

//     std::cout << "Welcome to the Docker Container Creator!" << std::endl;

//     while (true) {
//         std::cout << "Enter the name of the container to create (or 'exit' to quit): ";
//         std::string containerName;
//         std::getline(std::cin, containerName);

//         if (containerName == "exit") {
//             break;
//         }

//         // Ask for image name
//         std::cout << "Enter the Docker image name: ";
//         std::string imageName;
//         std::getline(std::cin, imageName);

//         // Ask for host port
//         int hostPort;
//         std::cout << "Enter the host port to map: ";
//         std::cin >> hostPort;

//         // Ask for container port
//         int containerPort;
//         std::cout << "Enter the container port to expose: ";
//         std::cin >> containerPort;

//         // Ask for mount path
//         std::cout << "Enter the mount path (leave empty for no volume mounting): ";
//         std::string mountPath;
//         std::getline(std::cin, mountPath);

//         // Trim whitespace
//         mountPath.erase(0, mountPath.find_first_not_of(" \t"));
//         mountPath.erase(mountPath.find_last_not_of(" \t") + 1);

//         try {
//             // Create the container
//             bool success = dm.createContainer(imageName, containerName, mountPath, hostPort, containerPort);

//             if (success) {
//                 std::cout << "Container '" << containerName << "' created successfully." << std::endl;
//             } else {
//                 std::cout << "Failed to create container." << std::endl;
//             }
//         } catch (const std::exception& e) {
//             std::cerr << "Error creating container: " << e.what() << std::endl;
//         }
//     }

//     return 0;
// }
