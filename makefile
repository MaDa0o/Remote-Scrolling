CC = g++

server.exe: Server.cpp
	${CC} Server.cpp -o server.exe