# Communucation over TCP Server and Client Example

This project is done for a real example of TCP communucation in C++.

Unix standart socket library is used for comminucation to make it faster and native.

## Server side

Server
* is able to handle multiple clients. Dynamic allocation is done with unique pointer.
* works with a callback that carry income data.
* is able to open any avalible port.
* is able to open multiple port to listen.
* works with a thread that polls for every device that is listened.

## Client side
Client
* has a light and basic structure.
* is able to send and receive string data.

## Build
1. Type `make` from the top-level directory.
2. Type `./server/main_server` to run the `server` program.
3. Type `./client/main_client` to run the `client` program.