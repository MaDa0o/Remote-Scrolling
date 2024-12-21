#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>			//parent header for sys/type.h
#include <netdb.h>			//have macros related to the highest reserved ip port number
#include <arpa/inet.h>		//defind uint32_t and uint16_t types
#include <string>
#include <string.h>			//for memset because it is c function
// Steps to setup Server
// 	-> Create TCP socket
// 	-> Bind socket to server address
// 	-> listen to connection requests from a client in a passive mode
// 	-> Accept the request from client 
//  -> receive and display messages
//  -> close socket

int main(int argc, char* argv[]){

	// Creating a socket
	int sock = socket(AF_INET, SOCK_STREAM, 0);				//int socket(int domain, int type, int protocol)

	if(sock == -1){
		std::cerr<<"Can't create the socket!! Something Bad happened";
		return 1;
	}

	//Bind the socket to server address and port
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);

	int bound = bind(sock, (sockaddr*)&hint, sizeof(hint));
	if(bound == -1){
		std::cerr<<"Can't bind to IP/port";
		return 2;
	}

	//listen to connections
	if(listen(sock, SOMAXCONN) == -1){
		std::cerr<<"Can't listen to connections";
		return 3;
	}

	//accept connection
	sockaddr_in client;
	socklen_t clientSize = sizeof(client);
	char host[NI_MAXHOST];
	char svc[NI_MAXSERV];

	int clientSocket = accept(sock, (sockaddr*)&client, &clientSize);

	if(clientSocket == -1){
		std::cerr<<"Problem with client connection";
		return 4;
	}

	close(sock);

	//cleaning memory
	memset(host, 0, NI_MAXHOST);
	memset(svc, 0, NI_MAXSERV);

	int result = getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, svc, NI_MAXSERV, 0);

	if(result){
		std::cout<<host<<" Connected on "<<svc<<std::endl;
	}else{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		std::cout<<host<<" Connected on "<<ntohs(client.sin_port)<<std::endl;
	}

	//Receive and display messages
	char buff[4096];
	while(true){
		//clear buffer
		memset(buff, 0 , 4096);
		//wait for a message
		int bytesRecv = recv(clientSocket, buff, 4096, 0);
		if(bytesRecv == -1){
			std::cerr<<"issue in receiving"<<std::endl;
			break;
		}

		if(bytesRecv == 0){
			std::cout<<"Client Disconnected."<<std::endl;
			break;
		}
		//display message
		std::cout<< "Received: "<< std::string(buff, 0, bytesRecv)<<std::endl;
	}

	//close socket
	close(clientSocket);

	return 0;
}