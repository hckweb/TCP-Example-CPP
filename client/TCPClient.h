#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <string>
#include <thread>

class Client {
public:
    Client(std::string server_ip, uint16_t port);
    ~Client();
    void send(std::string msg);
    std::string read();
    void close();

private:
    int sock;
};

#endif