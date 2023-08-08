#include <iostream>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")
using namespace std;

void main() {
	//Initialize winsock

	WSAData wsData;
	WORD ver = MAKEWORD(2, 2);

	int wSock = WSAStartup(ver, &wsData);
	if (wSock != 0) {
		cerr << "Can't initialize winsock! \n";
		return;
	}
	//create socket

	SOCKET listener = socket(AF_INET, SOCK_STREAM, 0);
	if (listener == INVALID_SOCKET) {
		cerr << "Invalid socket!\n";
		return;
	}

	//bind socket to ip
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(6554);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;
	bind(listener, (sockaddr*)&hint, sizeof(hint));
	//socket is listening
	listen(listener, SOMAXCONN);
	//wait for connections
	sockaddr_in client;
	int sizeclient = sizeof(client);
	SOCKET clientSocket = accept(listener, (sockaddr*)&client, &sizeclient);
	if (clientSocket == INVALID_SOCKET) {
		cerr << "Invalid socket!\n";
		return;
	}
	char host[NI_MAXHOST]; //client remote name
	char service[NI_MAXHOST]; //service client is connected on

	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(service, NI_MAXHOST);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
		cout << host << " connected on port " << service << endl;
	}
	else {
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << host << " connected on port " << ntohs(client.sin_port) << endl;
	}
	//close listening socket
	closesocket(listener);

	//while loop: accept and echo message back to client
	char buf[4096];
	while(true){
		ZeroMemory(buf, 4096);
		// wait for client to send data
		//echo message back to client
		int bytesRecieved = recv(clientSocket, buf, 4096, 0);
		if (bytesRecieved == SOCKET_ERROR) {
			cerr << "Error recieving bytes\n";
			break;
		}
		if(bytesRecieved == 0) {
			cout << "Client disconnected\n";
			break;
		}
		cout << buf;
		send(clientSocket, buf, bytesRecieved + 1, 0);
	}

	//close socket
	closesocket(clientSocket);
	//cleanup winsock
	WSACleanup();
}