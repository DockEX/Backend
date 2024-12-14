// VM Based Infrastructure

#include "crow_all.h"
#include "VmManager.h"

int main() {
    crow::SimpleApp app;

    CROW_ROUTE(app,"/")
    ([](){
        return crow::response(200,"C++ Server up and running!");
    });

    CROW_ROUTE(app, "/createvm")
    .methods(crow::HTTPMethod::POST)([](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body) {
            return crow::response(400, "Invalid JSON payload");
        }

        std::string userName = body["userName"].s();
        std::string repoName = body["repoName"].s();
        std::string vmName = userName + repoName;

        VMManager::createVM(vmName);
        return crow::response(200, "VM Created Successfully");
    });

    CROW_ROUTE(app, "/list-vms")
    ([]() {
        std::ostringstream output;
        output << "List of VMs:\n";
        std::string command = "sudo virsh list --all";
        FILE* stream = popen(command.c_str(), "r");
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), stream) != nullptr) {
            output << buffer;
        }
        pclose(stream);

        return crow::response(200, output.str());
    });

    app.port(8080).multithreaded().run();
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
