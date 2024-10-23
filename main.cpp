#include <iostream>
#include <string>
#include "DockerManager.h"

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: DockerManager <userName> <repoName> <projectType>" << std::endl;
        return 1;
    }

    std::string userName = argv[1];
    std::string repoName = argv[2];
    std::string projectType = argv[3];

    DockerManager docker;

    std::string repoPath = "/user-repos/" + userName + "/" + repoName;

    std::string image;
    int containerPort;
    if (projectType == "node") {
        image = "node:18-alpine";
        containerPort = 3000;
    } else if (projectType == "springboot") {
        image = "openjdk:11-jre-slim";
        containerPort = 8080;
    }

    std::string containerName = userName + "-" + repoName;
    int hostPort = 5000 + (std::hash<std::string>{}(userName) % 1000);

    if (docker.createContainer(image, containerName, repoPath, hostPort, containerPort)) {
        std::cout << "Container created successfully! Access it at: http://localhost:" << hostPort << std::endl;
        return 0;
    } else {
        std::cerr << "Failed to create container." << std::endl;
        return 1;
    }
}


/*
VM Based Infrastructure

#include <iostream>
#include <string>
#include "VMManager.h"

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: VMManager <userName> <repoName> <projectType>" << std::endl;
        return 1;
    }

    std::string userName = argv[1];
    std::string repoName = argv[2];
    std::string projectType = argv[3];

    VMManager vmManager;

    // VM config: disk image and memory based on project type
    std::string diskImage;
    std::string memorySize;
    int guestPort;
    
    if (projectType == "node") {
        diskImage = "/path/to/node-vm.img"; // Node.js VM image
        memorySize = "512"; // 512 MB memory
        guestPort = 3000; // Guest VM port for Node.js
    } else if (projectType == "springboot") {
        diskImage = "/path/to/springboot-vm.img"; // Spring Boot VM image
        memorySize = "1024"; // 1 GB memory
        guestPort = 8080; // Guest VM port for Spring Boot
    }

    std::string vmName = userName + "-" + repoName;
    int hostPort = 5000 + (std::hash<std::string>{}(userName) % 1000); // Unique host port

    // Create and start the VM
    if (vmManager.createVM(vmName, diskImage, memorySize, hostPort, guestPort)) {
        std::cout << "VM created and started successfully! Access it at: http://localhost:" << hostPort << std::endl;
        return 0;
    } else {
        std::cerr << "Failed to create VM." << std::endl;
        return 1;
    }
}


*/