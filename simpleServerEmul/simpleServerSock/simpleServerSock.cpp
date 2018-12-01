// simpleServerSock.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>
#include <thread>
#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "26000"

void threadTest1() {
	printf("Inside threadTest 1 ----- ");
}

void threadTest2() {
	printf("Inside threadTest 2 ----- ");
}


void listenToClient(SOCKET clientSocket, char* buffer, int buffLength, int iResult) {
	bool first = false;
	do {

		iResult = recv(clientSocket, buffer, buffLength, 0);

		//Receive data
		if (iResult > 0) {
			printf("Bytes received: %d\n", iResult);
			printf("Input received : %s\n", recvbuf);
			/*const char* response = "Hello from key/mouse emulator app server";
			iSendResult = send(clientSocket, response, sizeof(char) * (strlen(response)+1), 0);
			if (iSendResult == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(clientSocket);
				WSACleanup();
				return 1;
			}
			printf("Bytes sent: %d\n", iSendResult);*/
			if (first == false) {
				first = true;
			}
			else {
				//We consider only the first character since we're sending single key, later on it will be standardized messages
				emulKey(recvbuf[0]);
			}
		}
		else if (iResult == 0)
			printf("Connection closing...\n");
		else {
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(clientSocket);
			WSACleanup();
			return 1;
		}

	} while (iResult > 0);

	// shutdown the connection since we're done
	iResult = shutdown(clientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(clientSocket);
		WSACleanup();
	}
}

int main()
{	
	WSADATA wsaData;
	int iResult;

	SOCKET listenSocket = INVALID_SOCKET;
	SOCKET clientSocket = INVALID_SOCKET;

	struct addrinfo *result = NULL;
	struct addrinfo hints; 

	int iSendResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	char recvbufThread2[DEFAULT_BUFLEN];
	int recvbuflenThread2 = DEFAULT_BUFLEN;

	//Initialization
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}
	else {
		printf("WSAStartup success !\n");
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	//Resolve the server adress and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	//Create a SOCKET for connecting to server
	listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (listenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	//Setup the TCP listening socket
	iResult = bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	iResult = listen(listenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	clientSocket = accept(listenSocket, NULL, NULL);
	if (clientSocket == INVALID_SOCKET) {
		printf("accept failed with error: %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}
	printf("Found a connection\n");
	//No longer need server socket
	closesocket(listenSocket);
	/*
	bool first = false;
	// Receive until the peer shuts down the connection

	do {
		iResult = recv(clientSocket, recvbuf, recvbuflen, 0);
		
		//Receive data
		if (iResult > 0) {
			printf("Bytes received: %d\n", iResult);
			printf("Input received : %s\n", recvbuf);
			const char* response = "Hello from key/mouse emulator app server";
			iSendResult = send(clientSocket, response, sizeof(char) * (strlen(response)+1), 0);
			if (iSendResult == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(clientSocket);
				WSACleanup();
				return 1;
			}
			printf("Bytes sent: %d\n", iSendResult);
			if (first == false) {
				first = true;
			}
			else {
				Print buffer
				printf("Buffer length = %d", recvbuflen);
				for(int i = 0; i < recvbuflen; i++) {
					printf("%c", recvbuf[i]);
				}
				2 threads test with join
				std::thread t1(threadTest1);
				std::thread t2(threadTest2);
				t1.join();
				t2.join();
				
				std::thread(listenToClient, recvbuf[0]).detach();
				//emulKey(recvbuf[0]);
			}
		}
		else if (iResult == 0)
			printf("Connection closing...\n");
		else {
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(clientSocket);
			WSACleanup();
			return 1;
		}

	} while (iResult > 0);

	// shutdown the connection since we're done
	iResult = shutdown(clientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}
	*/

	std::thread t1(listenToClient, clientSocket, recvbuf, recvbuflen, iResult);
	std::thread t2(listenToClient, clientSocket, recvbufThread2, recvbuflenThread2, iResult);
	t1.join();
	t2.join();
	// cleanup
	closesocket(clientSocket);
	WSACleanup();
	system("PAUSE");
	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
