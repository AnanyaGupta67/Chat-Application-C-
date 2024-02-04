#include<iostream>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<thread>
using namespace std;

#pragma comment(lib , "ws2_32.lib")

bool Initialize()
{
	WSADATA data;
	return WSAStartup(MAKEWORD(2, 2), &data) == 0;
	return 1;
}

void SendMsg(SOCKET s)
{
	cout << endl << "Enter user name : ";
	string name;
	getline(cin, name);
	cout << endl;

	string send_msg;

	while (1) {
		cout << "\nEnter messege :" << endl;
		getline(cin, send_msg);
		string msg = name + " : " + send_msg;
		int bytesent = send(s, msg.c_str(), msg.length(), 0);
		if (bytesent == SOCKET_ERROR) {
			cout << "error sending messege " << endl;
			break;
		}


	}

	closesocket(s);
	WSACleanup();
}

void ReceiveMsg(SOCKET s)
{
	char buffer[4096];
	int recvlength;
	string msg_client;
	while (1) {
		recvlength = recv(s, buffer, sizeof(buffer), 0);
		if (recvlength <= 0) {
			cout << "\nDisconnected from server " << endl;
			break;
		}
		else {
			msg_client = string(buffer, recvlength);
			cout << msg_client<<endl;
		}
	}
	closesocket(s);
	WSACleanup();
}


int main()
{
	if (!Initialize()) {
		cout << "initialize winsock failed \n";
		return 1;
	}

	SOCKET s;
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET) {
		cout << "invalid socket \n";
		return 1;
	}

	int port = 12345;
	string serveraddress = "127.0.0.1";
	sockaddr_in serveraddr;

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);
	inet_pton(AF_INET, serveraddress.c_str(), &(serveraddr.sin_addr));
	if (connect(s, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR) {
		cout << "dont get connected to server \n";
		closesocket(s);
		WSACleanup();
		return 1;
	}

	cout << "successfully connected to server :)";

	thread sendthread(SendMsg, s);
	thread receivethread(ReceiveMsg, s);


	sendthread.join();
	receivethread.join();




	return 0;




}                              