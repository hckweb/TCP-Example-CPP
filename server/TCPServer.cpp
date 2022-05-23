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

static void* get_client_address(struct sockaddr* sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

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

    thread_object = std::thread(&TCPServer::loop, this);
}

TCPServer::~TCPServer()
{
    pthread_cancel(thread_object.native_handle());
    thread_object.join();
    std::cout << TAG << "TCP server on port: " << port << " closed" << std::endl;
}

uint16_t TCPServer::get_port()
{
    return port;
}

void TCPServer::loop()
{
    int fd_count = 1;
    socklen_t addrlen;
    char buf[256];
    char remoteIP[INET6_ADDRSTRLEN];
    int newfd;
    struct sockaddr_storage remoteaddr;

    up_pollfd = std::unique_ptr<pollfd[]> { new pollfd[fd_count] };
    up_pollfd[0].fd = listener;
    up_pollfd[0].events = POLLIN;

    for (;;) {
        int poll_count = poll(up_pollfd.get(), fd_count, -1);

        if (poll_count == -1)
            throw std::runtime_error { "poll error" };

        for (int i = 0; i < fd_count; i++) {

            if (up_pollfd[i].revents & POLLIN) {
                if (up_pollfd[i].fd == listener) {
                    addrlen = sizeof remoteaddr;
                    newfd = accept(listener, (struct sockaddr*)&remoteaddr, &addrlen);

                    if (newfd == -1) {
                        throw std::runtime_error { "accept error" };
                    } else {
                        ++fd_count;

                        pollfd _temp[fd_count];
                        for (size_t i = 0; i < fd_count - 1; ++i) {
                            _temp[i].fd = up_pollfd[i].fd;
                            _temp[i].events = up_pollfd[i].events;
                        }

                        up_pollfd.release();
                        up_pollfd = std::unique_ptr<pollfd[]> { new pollfd[fd_count] };

                        for (size_t i = 0; i < fd_count; ++i) {
                            up_pollfd[i].fd = _temp[i].fd;
                            up_pollfd[i].events = _temp[i].events;
                        }

                        up_pollfd[fd_count - 1].fd = newfd;
                        up_pollfd[fd_count - 1].events = POLLIN;

                        std::cout << TAG << "New connection from " << inet_ntop(remoteaddr.ss_family, get_client_address((struct sockaddr*)&remoteaddr), remoteIP, INET6_ADDRSTRLEN) << " on socket " << newfd << '\n';
                    }
                } else {
                    memset(buf, 0, sizeof(buf));
                    int nbytes = recv(up_pollfd[i].fd, buf, sizeof buf, 0);
                    int sender_fd = up_pollfd[i].fd;

                    if (nbytes <= 0) {
                        if (nbytes == 0) {
                            std::cout << TAG << "Socket closed " << sender_fd << '\n';
                        } else {
                            throw std::runtime_error { "recv error" };
                        }

                        close(up_pollfd[i].fd);

                        up_pollfd[i] = up_pollfd[fd_count - 1];

                        --fd_count;

                        pollfd _temp[fd_count];
                        for (size_t i = 0; i < fd_count; ++i) {
                            _temp[i].fd = up_pollfd[i].fd;
                            _temp[i].events = up_pollfd[i].events;
                        }

                        up_pollfd.release();
                        up_pollfd = std::unique_ptr<pollfd[]> { new pollfd[fd_count] };

                        for (size_t i = 0; i < fd_count; ++i) {
                            up_pollfd[i].fd = _temp[i].fd;
                            up_pollfd[i].events = _temp[i].events;
                        }
                    } else {
                        if (read_callback) {
                            std::string temp_string = read_callback(std::string(buf));
                            send(sender_fd, temp_string.c_str(), temp_string.length(), 0);
                        }
                    }
                }
            }
        }
    }
}
