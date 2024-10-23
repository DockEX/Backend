#include "DockerManager.h"
#include <iostream>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
#include <sstream>

std::string execCommand(const std::string& cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

bool DockerManager::createContainer(const std::string& image, const std::string& containerName, 
                                    const std::string& mountPath, int hostPort, int containerPort) {
    std::string cmd = "docker run -d --name " + containerName + 
                      " -p " + std::to_string(hostPort) + ":" + std::to_string(containerPort) +
                      " -v " + mountPath + ":/app " + image;
    int result = system(cmd.c_str());
    return (result == 0);
}

bool DockerManager::startContainer(const std::string& containerName) {
    std::string cmd = "docker start " + containerName;
    int result = system(cmd.c_str());
    return (result == 0);
}

bool DockerManager::stopContainer(const std::string& containerName) {
    std::string cmd = "docker stop " + containerName;
    int result = system(cmd.c_str());
    return (result == 0);
}

std::vector<std::string> DockerManager::listContainers() {
    std::string cmd = "docker ps -a --format \"{{.Names}}\"";
    std::string output = execCommand(cmd);

    std::vector<std::string> containers;
    std::string line;
    std::istringstream stream(output);
    while (std::getline(stream, line)) {
        containers.push_back(line);
    }
    return containers;
}

std::string DockerManager::getContainerLogs(const std::string& containerName) {
    std::string cmd = "docker logs " + containerName;
    return execCommand(cmd);
}

std::string DockerManager::inspectContainer(const std::string& containerName) {
    std::string cmd = "docker inspect " + containerName;
    return execCommand(cmd);
}
