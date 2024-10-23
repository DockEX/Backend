// #include <iostream>
// #include <string>
// #include "DockerManager/DockerManager.h"

// int main(int argc, char* argv[]) {
//     if (argc != 4) {
//         std::cerr << "Usage: DockerManager <userName> <repoName> <projectType>" << std::endl;
//         return 1;
//     }

//     std::string userName = argv[1];
//     std::string repoName = argv[2];
//     std::string projectType = argv[3];

//     DockerManager docker;

//     std::string repoPath = "/user-repos/" + userName + "/" + repoName;

//     std::string image;
//     int containerPort;
//     if (projectType == "node") {
//         image = "node:18-alpine";
//         containerPort = 3000;
//     } else if (projectType == "springboot") {
//         image = "openjdk:11-jre-slim";
//         containerPort = 8080;
//     }

//     std::string containerName = userName + "-" + repoName;
//     int hostPort = 5000 + (std::hash<std::string>{}(userName) % 1000);

//     if (docker.createContainer(image, containerName, repoPath, hostPort, containerPort)) {
//         std::cout << "Container created successfully! Access it at: http://localhost:" << hostPort << std::endl;
//         return 0;
//     } else {
//         std::cerr << "Failed to create container." << std::endl;
//         return 1;
//     }
// }


// /*
// VM Based Infrastructure

// #include <iostream>
// #include <string>
// #include "VMManager.h"

// int main(int argc, char* argv[]) {
//     if (argc != 4) {
//         std::cerr << "Usage: VMManager <userName> <repoName> <projectType>" << std::endl;
//         return 1;
//     }

//     std::string userName = argv[1];
//     std::string repoName = argv[2];
//     std::string projectType = argv[3];

//     VMManager vmManager;

//     // VM config: disk image and memory based on project type
//     std::string diskImage;
//     std::string memorySize;
//     int guestPort;
    
//     if (projectType == "node") {
//         diskImage = "/path/to/node-vm.img"; // Node.js VM image
//         memorySize = "512"; // 512 MB memory
//         guestPort = 3000; // Guest VM port for Node.js
//     } else if (projectType == "springboot") {
//         diskImage = "/path/to/springboot-vm.img"; // Spring Boot VM image
//         memorySize = "1024"; // 1 GB memory
//         guestPort = 8080; // Guest VM port for Spring Boot
//     }

//     std::string vmName = userName + "-" + repoName;
//     int hostPort = 5000 + (std::hash<std::string>{}(userName) % 1000); // Unique host port

//     // Create and start the VM
//     if (vmManager.createVM(vmName, diskImage, memorySize, hostPort, guestPort)) {
//         std::cout << "VM created and started successfully! Access it at: http://localhost:" << hostPort << std::endl;
//         return 0;
//     } else {
//         std::cerr << "Failed to create VM." << std::endl;
//         return 1;
//     }
// }


// */


#include <iostream>
#include <string>
#include "DockerManager/DockerManager.h"

int main() {
    DockerManager dm;

    std::cout << "Welcome to the Docker Container Creator!" << std::endl;
    
    while (true) {
        std::cout << "Enter the name of the container to create (or 'exit' to quit): ";
        std::string containerName;
        std::getline(std::cin, containerName);

        if (containerName == "exit") {
            break;
        }

        // Ask for image name
        std::cout << "Enter the Docker image name: ";
        std::string imageName;
        std::getline(std::cin, imageName);

        // Ask for host port
        int hostPort;
        std::cout << "Enter the host port to map: ";
        std::cin >> hostPort;

        // Ask for container port
        int containerPort;
        std::cout << "Enter the container port to expose: ";
        std::cin >> containerPort;

        // Ask for mount path
        std::cout << "Enter the mount path (leave empty for no volume mounting): ";
        std::string mountPath;
        std::getline(std::cin, mountPath); // Consume newline left-over

        try {
            // Create the container
            bool success = dm.createContainer(imageName, containerName, mountPath, hostPort, containerPort);
            
            if (success) {
                std::cout << "Container '" << containerName << "' created successfully." << std::endl;
            } else {
                std::cout << "Failed to create container." << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error creating container: " << e.what() << std::endl;
        }
    }

    return 0;
}