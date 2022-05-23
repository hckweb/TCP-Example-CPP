#include "TCPServer.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>

#define TAG "Server: "

static int get_listener_socket(uint16_t _port)
{
    int listener;
    int yes = 1;
    int rv;

    struct addrinfo hints, *ai, *p;
    char port_string[5 + 1];
    sprintf(port_string, "%d", _port);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, port_string, &hints, &ai)) != 0) {
        throw std::runtime_error { "error selectserver" };
    }

    for (p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) {
            continue;
        }

        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }

        break;
    }

    if (p == NULL) {
        return -1;
    }

    freeaddrinfo(ai);

    if (listen(listener, 10) == -1) {
        return -1;
    }

    return listener;
}

TCPServer::TCPServer(uint16_t _port, std::string (*_read_callback)(std::string))
{
    listener = get_listener_socket(_port);

    if (listener == -1)
        throw std::runtime_error { "error getting listening socket" };

    port = _port;
    std::cout << TAG << "Starting TCP server on port: " << port << std::endl;
    read_callback = _read_callback;
}

TCPServer::~TCPServer()
{
    std::cout << TAG << "TCP server on port: " << port << " closed" << std::endl;
}

uint16_t TCPServer::get_port()
{
    return port;
}