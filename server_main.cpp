#include<iostream>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<tchar.h>
#include<thread>
#include<vector>

//sever code --->
using namespace std;
#pragma comment(lib , "ws2_32.lib")
struct sockaddr_in srv;



bool Initialize()
{
	WSADATA data;
	return WSAStartup(MAKEWORD(2, 2), &data) == 0;
	return 1;
}

void InteractWithClient(SOCKET clientSocket , vector<SOCKET>& clients) {
	//send and recv client 
	char buffer[4096];

	while (1) {

		int byterecvd = recv(clientSocket, buffer, sizeof(buffer), 0);

		if (byterecvd <= 0) {
			cout << "\nClient disconnected " << endl;
			break;
		}
		string msg(buffer, byterecvd);
		cout << "messege from client : " << msg << endl;

		for (auto client : clients) {
			if (client != clientSocket) {
				send(clientSocket, msg.c_str(), msg.length(), 0);
			}
		}
	}

	auto it = find(clients.begin(), clients.end(), clientSocket);
	if (it != clients.end()) {
		clients.erase(it);
	}

	closesocket(clientSocket);
}

int main()
{
	int nRet = 0;
	if (!Initialize())
	{
		cout << "winsock initialization failed " << endl;
	}
	else {
		cout << "winsock initialization successfully " << endl;
	}

	//create socket
	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET) {
		cout << "socket creation falied " << endl;
		return 1;
	}
	int port = 12345;
	sockaddr_in serveraddr;


	//convert the ip address (0.0.0.0)
	if (InetPton(AF_INET, _T("0.0.0.0"), &serveraddr.sin_addr) != 1) {
		cout << endl << "setting address structure failed " << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	//before binding..you have to initialize struct members-

	srv.sin_family = AF_INET;
	srv.sin_addr.s_addr = INADDR_ANY;
	srv.sin_port = htons(port);
	memset(&(srv.sin_zero), 0, 8);

	//bind
	nRet = bind(listenSocket, (sockaddr*)(&srv), sizeof(sockaddr));
	if (nRet < 0)
	{
		cout << "bind failed \n";
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	//listen
	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
		cout << "listen failed " << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}
	cout << "server has started listening on port : " << port << endl;

	vector<SOCKET> clients;       //to save all the socket clients are coming

	//accept
	while (1) {
		SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
		if (clientSocket == INVALID_SOCKET) {
			cout << "invalid client socket " << endl;

		}
		clients.push_back(clientSocket);
		thread t1(InteractWithClient, clientSocket, std::ref(clients));
		t1.detach();
	}
	

	
	closesocket(listenSocket);
	WSACleanup();



	return 0;
}