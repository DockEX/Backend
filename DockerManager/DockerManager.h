#ifndef DOCKER_MANAGER_H
#define DOCKER_MANAGER_H

#include <string>
#include <vector>

class DockerManager {
public:
    DockerManager() = default;

    bool createContainer(const std::string& image, const std::string& containerName, 
                         const std::string& mountPath, int hostPort, int containerPort);
    bool startContainer(const std::string& containerName);
    bool stopContainer(const std::string& containerName);
    std::vector<std::string> listContainers();
    std::string getContainerLogs(const std::string& containerName);
    std::string inspectContainer(const std::string& containerName);
};

#endif
