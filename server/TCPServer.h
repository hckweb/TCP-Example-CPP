#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <cstdint>
#include <poll.h>
#include <string>
#include <thread>

class TCPServer {
public:
    TCPServer(uint16_t port, std::string (*read_callback)(std::string income_data));
    uint16_t get_port();
    ~TCPServer();

private:
    void loop();
    uint16_t port;
    std::string (*read_callback)(std::string);
    int listener;
    std::thread thread_object;
    std::unique_ptr<pollfd[]> up_pollfd;
};

#endif