#include <unistd.h>
#include <sstream>
#include <chrono>

#include "zmq_server.hpp"
 
int main(int argc, char* argv[]) {
    if (argc != 2 && argc != 3) {
        throw std::runtime_error("Wrong amount of arguments for computing node");
    }
    int cur_id = std::atoi(argv[1]);
    int child_id = -1; // by default we look from the root node
    if (argc == 3) {
        child_id = std::atoi(argv[2]);
    }

    auto start_clock = std::chrono::high_resolution_clock::now();
    auto stop_clock = std::chrono::high_resolution_clock::now();
    auto time_clock = 0;
    bool flag_clock = false;

    zmq::context_t context;
    zmq::socket_t parent_socket(context, ZMQ_REP);
    connect(parent_socket, cur_id);

    zmq::socket_t child_socket(context, ZMQ_REQ);
    child_socket.setsockopt(ZMQ_SNDTIMEO, 5000);
    if (child_id != -1) {
        bind(child_socket, child_id);
    }
 
    std::string message;
    while (true) {
        message = receive_message(parent_socket);
        std::istringstream request(message);
        int dest_id;
        request >> dest_id;

        std::string cmd;
        request >> cmd;

        if (dest_id == cur_id) {

            if (cmd == "pid") { //if we need to get process id for the created node 
                send_message(parent_socket, "OK: " + std::to_string(getpid()));
            }

            else if (cmd == "create") {
                int new_child_id;
                request >> new_child_id;
                if (child_id != -1) {
                    unbind(child_socket, child_id);
                }
                bind(child_socket, new_child_id);
                pid_t pid = fork(); //create two processes
                if (pid < 0) { // if parent process noticed problem with creating child process
                    perror("Can't create new process");
                    return -1;
                }
                if (pid == 0) { // child process goes here
                    // replaces the current process image with a new process image
                    execl("./computing_node", "./computing_node", std::to_string(new_child_id).c_str(), std::to_string(child_id).c_str(), NULL);
                    perror("Can't execute new process");
                    return -2;
                }
                send_message(child_socket, std::to_string(new_child_id) + "pid");
                child_id = new_child_id;
                send_message(parent_socket, receive_message(child_socket));
            }

            else if (cmd == "exec") {
                std::string subcomand;
                request >> subcomand;
                std::string msg = "OK: " + std::to_string(cur_id);
                if (subcomand == "start") {
                    start_clock = std::chrono::high_resolution_clock::now();
                    flag_clock = true;
                } else if (subcomand == "stop") {
                    if (flag_clock) {
                        stop_clock = std::chrono::high_resolution_clock::now();
                        time_clock += std::chrono::duration_cast<std::chrono::milliseconds>(stop_clock - start_clock).count();
                        flag_clock = false;
                    }
                } else if (subcomand == "time") {
                    if (flag_clock == true) {
                        stop_clock = std::chrono::high_resolution_clock::now();
                        time_clock += std::chrono::duration_cast<std::chrono::milliseconds>(stop_clock - start_clock).count();
                        start_clock = stop_clock;
                    }
                    msg += ": " + std::to_string(time_clock);
                }
                send_message(parent_socket, msg);
            }             

            else if (cmd == "pingall") {
                std::string reply;
                if (child_id != -1) {
                    send_message(child_socket, std::to_string(child_id) + " pingall");
                    std::string msg = receive_message(child_socket);
                    reply += " " + msg;
                }
                send_message(parent_socket, std::to_string(cur_id) + reply);
            }

            else if (cmd == "kill") {
                if (child_id != -1) {
                    send_message(child_socket, std::to_string(child_id) + " kill");
                    std::string msg = receive_message(child_socket);
                    if (msg == "OK") {
                        send_message(parent_socket, "OK");
                    }
                    unbind(child_socket, child_id);
                    disconnect(parent_socket, cur_id);
                    break;
                }
                send_message(parent_socket, "OK");
                disconnect(parent_socket, cur_id);
                break;
            }
        }
        else if (child_id != -1) {
            send_message(child_socket, message);
            send_message(parent_socket, receive_message(child_socket));
            if (child_id == dest_id && cmd == "kill") {
                child_id = -1;
            }
        }
        else {
            send_message(parent_socket, "Error: node is unavailable");
        }
    }
}