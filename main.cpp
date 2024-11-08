// VM Based Infrastructure

#include <iostream>
#include <string>
#include <curl/curl.h>
#include "VMManager.h"

size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

std::string fetchGithubRepo(const std::string &owner, const std::string &repo)
{
    CURL *curl;
    CURLcode res;
    std::string readBuffer;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl)
    {
        std::string url = "https://api.github.com/repos/" + owner + "/" + repo + "/contents";
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            std::cerr << "cURL error: " << curl_easy_strerror(res) << std::endl;
        }
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();

    return readBuffer;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <owner> <repo>" << std::endl;
        return 1;
    }

    std::string owner = argv[1];
    std::string repo = argv[2];

    VMManager vmManager;

    // Fetch GitHub repository contents
    std::string repoContents = fetchGithubRepo(owner, repo);

    // Create VM
    std::string vmName = owner + "-" + repo;
    if (!vmManager.createVM(vmName, repoContents))
    {
        std::cerr << "Failed to create VM." << std::endl;
        return 1;
    }

    // Start VM
    if (!vmManager.startVM(vmName))
    {
        std::cerr << "Failed to start VM." << std::endl;
        return 1;
    }

    std::cout << "VM created and started successfully!" << std::endl;
    std::cout << "VM Name: " << vmName << std::endl;

    // List VMs
    std::vector<std::string> vms = vmManager.listVMs();
    std::cout << "\nCurrent VMs:" << std::endl;
    for (const auto &vm : vms)
    {
        std::cout << vm;
    }

    // Get VM logs
    std::string logs = vmManager.getVMLogs(vmName);
    std::cout << "\n\nVM Logs:" << std::endl;
    std::cout << logs;

    // Inspect VM
    std::string info = vmManager.inspectVM(vmName);
    std::cout << "\n\nVM Info:" << std::endl;
    std::cout << info;

    return 0;
}

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