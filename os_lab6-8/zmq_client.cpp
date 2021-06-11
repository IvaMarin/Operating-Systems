#include <unistd.h>
#include <sstream>
#include <set>
#include <chrono>

#include "zmq_server.hpp"
#include "topology.hpp"

int main() {
    topology network;
    std::vector<zmq::socket_t> branches;
    zmq::context_t context;

    std::string cmd;
    while (std::cin >> cmd) {

        if (cmd == "create") {
            int node_id, parent_id;
            std::cin >> node_id >> parent_id;

            if (network.find(node_id) != -1) {
                std::cout << "Error: already exists" << std::endl;
            }
            
            else if (parent_id == -1) { //if we're creating root node
                pid_t pid = fork(); //create two processes
                if (pid < 0) { // if parent process noticed problem with creating child process
                    perror("Can't create new process");
                    return -1;
                }
                if (pid == 0) { // child process goes here
                    // replaces the current process image with a new process image
                    execl("./computing_node", "./computing_node", std::to_string(node_id).c_str(), NULL);
                    perror("Can't execute new process");
                    return -2;
                }
                
                branches.emplace_back(context, ZMQ_REQ);
                branches[branches.size() - 1].setsockopt(ZMQ_SNDTIMEO, 5000);
                bind(branches[branches.size() - 1], node_id);
                send_message(branches[branches.size() - 1], std::to_string(node_id) + "pid");
                
                std::string reply = receive_message(branches[branches.size() - 1]);
                std::cout << reply << std::endl;
                network.insert(node_id, parent_id);
            }
            else if (network.find(parent_id) == -1) {
                std::cout << "Error: parent not found" << std::endl;
            }
            else { // if we're creating a child node from existing parent node
                int branch = network.find(parent_id);
                send_message(branches[branch], std::to_string(parent_id) + "create " + std::to_string(node_id));

                std::string reply = receive_message(branches[branch]);
                std::cout << reply << std::endl;
                network.insert(node_id, parent_id);
            }
        }
        else if (cmd == "exec") {
            int dest_id;
            std::string subcomand;
            std::cin >> dest_id >> subcomand;
            int branch = network.find(dest_id);
            if (branch == -1) {
                std::cout << "ERROR: incorrect node id" << std::endl;
            }
            else {
                if (subcomand == "start") {
                    send_message(branches[branch], std::to_string(dest_id)  + "exec " + " start");
                } else if (subcomand == "stop") {
                    send_message(branches[branch], std::to_string(dest_id)  +  "exec " + " stop");
                } else if (subcomand == "time") {
                    send_message(branches[branch], std::to_string(dest_id)  + "exec " + " time");
                }

                std::string reply = receive_message(branches[branch]);
                std::cout << reply << std::endl;
            }
        }
        else if (cmd == "kill") {
            int id;
            std::cin >> id;
            int branch = network.find(id);
            if (branch == -1) {
                std::cout << "ERROR: incorrect node id" << std::endl;
            }
            else {
                bool is_first = (network.get_first_id(branch) == id);
                send_message(branches[branch], std::to_string(id) + " kill");

                std::string reply = receive_message(branches[branch]);
                std::cout << reply << std::endl;
                network.erase(id);
                if (is_first) { // check if id node is the first in the list
                    unbind(branches[branch], id);
                    branches.erase(branches.begin() + branch);
                }
            }
        }
        else if (cmd == "pingall") {
            std::set<int> available_nodes;
            for (size_t i = 0; i < branches.size(); ++i) {
                int first_node_id = network.get_first_id(i);
                send_message(branches[i], std::to_string(first_node_id) + " pingall");

                std::string received_message = receive_message(branches[i]);
                std::istringstream reply(received_message);
                int node;
                while(reply >> node) {
                    available_nodes.insert(node);
                }
            }
            std::cout << "OK: ";
            if (available_nodes.empty()) {
                std::cout << "no available nodes" << std::endl;
            }
            else {
                for (auto v : available_nodes) {
                    std::cout << v << " ";
                }
                std::cout << std::endl;
            }
        }
        else if (cmd == "exit") {
            for (size_t i = 0; i < branches.size(); ++i) {
                int first_node_id = network.get_first_id(i);
                send_message(branches[i], std::to_string(first_node_id) + " kill");
                
                std::string reply = receive_message(branches[i]);
                if (reply != "OK") {
                    std::cout << reply << std::endl;
                }
                else {
                    unbind(branches[i], first_node_id);
                }
            }
            exit(0);
        }
        else {
            std::cout << "I don't know this command" << std::endl;
        }
    }
}