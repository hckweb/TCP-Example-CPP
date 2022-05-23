#include "TCPClient.h"

#include <iostream>
#include <thread>

#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define TAG "Client: "

int recvtimeout(int s, char* buf, int len, int timeout)
{
    fd_set fds;
    int n;
    struct timeval tv;

    FD_ZERO(&fds);
    FD_SET(s, &fds);

    tv.tv_sec = timeout;
    tv.tv_usec = 0;

    n = select(s + 1, &fds, NULL, NULL, &tv);
    if (n == 0)
        return -2; // timeout!
    if (n == -1)
        return -1; // error

    return recv(s, buf, len, 0);
}

std::string Client::read()
{
    int n;
    char buf[1024] = { 0 };
    n = recvtimeout(sock, buf, sizeof buf, 1);

    if (n == -1) {
        // Err
    } else if (n == -2) {
        // Timeout
    } else if (n == 0) {
        // Socket closed
        close();
    } else {
        return std::string(buf);
    }
    return std::string();
}

Client::Client(std::string server_ip, uint16_t port)
{
    struct sockaddr_in serv_addr;
    sock = 0;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cout << "Socket creation error" << std::endl;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, server_ip.c_str(), &serv_addr.sin_addr) <= 0) {
        std::cout << "Invalid address/Address not supported" << std::endl;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cout << "Connection Failed" << std::endl;
    }
}

void Client::send(std::string msg)
{
    int valread;
    const char* msg_c = msg.c_str();

    int ret_val = ::send(sock, msg_c, strlen(msg_c), 0);

    if (ret_val == -1)
        throw std::runtime_error { "error sending data" };

    std::cout << TAG << "Sent " << ret_val << " byte data : \"" << msg << "\"" << std::endl;
}

void Client::close()
{
    if (sock != -1) {
        ::close(sock);
        sock = -1;
    }
}

Client::~Client()
{
    close();
}