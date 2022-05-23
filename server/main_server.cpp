#include "TCPServer.h"

#include <iostream>
#include <string>

constexpr char json_data[] = "{\"Temperature\":27.5,\"Date\":\"2022-06-12T23:35:36.000Z\",\"GPS\":{\"latitude\":38.423733,\"longitude\":27.142826},\"Accelerometer\":{\"x\":1,\"y\":-0.7,\"z\":1.5},\"Gyroscope\":{\"x\":4.1,\"y\":-1.7,\"z\":-1.5}}";

std::string callback(std::string data)
{
    std::cout << "Income data: " << data << std::endl;
    return std::string(json_data);
}

int main()
{
    TCPServer a(3642, callback);

    while (1)
        ;

    return 0;
}