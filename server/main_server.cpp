#include "TCPServer.h"

#include <iostream>
#include <string>

std::string callback(std::string data)
{
    std::cout << "Income data: " << data << std::endl;
    return std::string("TEST");
}

int main()
{
    TCPServer a(3642, callback);

    while (1)
        ;

    return 0;
}