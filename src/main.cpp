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

        int ram = body["ram"].i();       // Dynamic RAM in MB
        int vcpus = body["vcpus"].i();  // Dynamic vCPUs
        int diskSize = body["diskSize"].i(); // Disk size in GB

        std::string vmIP = VMManager::createVM(vmName, ram, vcpus, diskSize);
        if (vmIP.empty()) {
            return crow::response(500, "Failed to create VM or retrieve IP");
        }

        return crow::response(vmIP); // Returning the response with the IP
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

