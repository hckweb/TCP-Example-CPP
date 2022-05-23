PROJECT=main_server

$(PROJECT): $(PROJECT).cpp
	g++ -std=c++17 -pthread $(PROJECT).cpp TCPServer.cpp -o $(PROJECT)

clean:
	rm -rf $(PROJECT)