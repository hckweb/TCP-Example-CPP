#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <cstdint>
#include <string>

class TCPServer {
public:
    TCPServer(uint16_t port, std::string (*read_callback)(std::string income_data));
    uint16_t get_port();
    ~TCPServer();

private:
    uint16_t port;
    std::string (*read_callback)(std::string);
    int listener;
};

#endif