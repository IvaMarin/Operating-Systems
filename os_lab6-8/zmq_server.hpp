#include <zmq.hpp>
#include <iostream>
 
const int MAIN_PORT = 4040;
 
void send_message(zmq::socket_t& socket, const std::string& msg) {
    zmq::message_t message(msg.size());
    memcpy(message.data(), msg.c_str(), msg.size());
    socket.send(message);
}
 
std::string receive_message(zmq::socket_t& socket) {
    zmq::message_t message;
    int chars_read;
    try {
        chars_read = (int)socket.recv(&message);
    }
    catch (...) {
        chars_read = 0;
    }
    if (chars_read == 0) {
        return "Error: Node is not available";
    }
    std::string received_msg(static_cast<char*>(message.data()), message.size());
    return received_msg;
}
// connect client to the socket
void connect(zmq::socket_t& socket, int id) { // note: 127.0.0.1 is an address for machine to connect to itself via network
    std::string address = "tcp://127.0.0.1:" + std::to_string(MAIN_PORT + id);
    socket.connect(address);
}
// disconnect client from the socket
void disconnect(zmq::socket_t& socket, int id) {
    std::string address = "tcp://127.0.0.1:" + std::to_string(MAIN_PORT + id);
    socket.disconnect(address);
}

// connect socket to ip address and port and start accepting connections
void bind(zmq::socket_t& socket, int id) {
    std::string address = "tcp://127.0.0.1:" + std::to_string(MAIN_PORT + id);
    socket.bind(address);
}

// disconnect socket from ip address and port and stop accepting connections
void unbind(zmq::socket_t& socket, int id) {
    std::string address = "tcp://127.0.0.1:" + std::to_string(MAIN_PORT + id);
    socket.unbind(address);
}