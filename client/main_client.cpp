#include "TCPClient.h"

#include <iostream>

int main()
{
    Client c("127.0.0.1", 3642);

    c.send("DATA");
    std::cout << c.read() << "\n";

    while (1)
        ;

    return 0;
}
