
/* Don't forget to include "wsock32" in the library list. */

#include <winsock.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <windows.h>


const int Proxy_PORT = 3360;

// the checkForAnError function checks if there was an error and handles it accordingly
// @ params
// bytesResult = send/receive function result
// ErrorAt = string stating if it was receive or send function
// socket = the socket to close in case there was an error.

bool checkForAnError(int bytesResult, char* ErrorAt, SOCKET socket){
    if (SOCKET_ERROR == bytesResult) {
        printf("Time Client: Error at %s(): ",ErrorAt);
        printf("%d", WSAGetLastError());
        closesocket(socket);
        WSACleanup();
        return true;
    }
    return false;
}


void main() {

    /*
        Client Side:
        1. Create Socket.
        2. Create Server Address.
        3. Connect.
        4. Send.
        5. Recv.
    */

	// Initialize Winsock (Windows Sockets).
     DWORD dw1;
     DWORD dw2;
	WSADATA wsaData;
	if (NO_ERROR != WSAStartup(MAKEWORD(2, 0), & wsaData)) {
		printf("Time Client: Error at WSAStartup()\n");
		return;
	}

	// Client side:
	// Create a socket and connect to an internet address.

	SOCKET connSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == connSocket) {
		printf("Time Client: Error at socket(): ");
		printf("%d", WSAGetLastError());
		WSACleanup();
		return;
	}

	// For a client to communicate on a network, it must connect to a server.
	// Need to assemble the required data for connection in sockaddr structure.
	// Create a sockaddr_in object called server.
	// insert the server IP and port

	struct sockaddr_in server;
	memset( & server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(Proxy_PORT);

	// Connect to server.

	// The connect function establishes a connection to a specified network
	// address. The function uses the socket handler, the sockaddr structure
	// (which defines properties of the desired connection) and the length of
	// the sockaddr structure (in bytes).
	printf("\n   jj\n");
	if (SOCKET_ERROR == connect(connSocket, (SOCKADDR * ) & server, sizeof(server))) {
		printf("Time Client: Error at connect(): ");
		printf("%d", WSAGetLastError());
		closesocket(connSocket);
		WSACleanup();
		return;
	}
	printf("connection established successfully.");

	// Send and receive data.

	int bytesSent = 0;
	int bytesRecv = 0;
	char * sendBuff;
	char recvBuff[255];
	char option;
	char fileName [20]="";
    char temp[100]="";
	while (option != '4') {

        printf("\nPlease insert an option :\n");
        printf("\n 1 : Get all the documents names which in (files) file.");
        printf("\n 2 : Get the content of an document.");
        printf("\n 3 : Get the calculate result of Round Trip Time.");
        printf("\n 4 : Exit.");
        printf("\n Your option : ");
        scanf("%c", & option);

        switch(option){
            case '1':
                        sendBuff = "Get all the documents names which in (files) file.";
                        break;
            case '2':

                        printf("\n which file do you want : ");
                        scanf("%s",fileName);
                        strcat(temp,"Get the content of an document.");
                        strcat(temp,fileName);
                        printf("1%s",temp);
                        sendBuff="Get the content of an document.";
                        printf("\n hooon\n");
                        break;
            case '3':
                        sendBuff = "Get the content of an document.";
                        dw1 = GetTickCount();
                        bytesSent = send(connSocket, sendBuff, (int) strlen(sendBuff), 0);
                          if (checkForAnError(bytesSent,"send",connSocket))
                                return;
                        bytesRecv = recv(connSocket, recvBuff, 255, 0);
                            if (checkForAnError(bytesRecv,"recv",connSocket))
			                      return;
                        printf("\nRecieved from server: %s \n",recvBuff);
                        printf("\nRTT is : %d \n",GetTickCount()-dw1);

                        break;
            case '4':
                        sendBuff = "Exit";
                        break;
            default:
                        printf("\n *-*-* Please enter a valid option only. *-*-*\n");
                        fflush(stdin);
                        continue;
        }


        bytesSent = send(connSocket, sendBuff, (int) strlen(sendBuff), 0);
        if (checkForAnError(bytesSent,"send",connSocket))
            return;


        // if a request for exit was made, there is no need to recv a response from server, only send.
		if(option == '4'){
                printf("Closing connection.");
                break;
		}


		bytesRecv = recv(connSocket, recvBuff, 255, 0);
        if (checkForAnError(bytesRecv,"recv",connSocket)){
                printf("\n error");
			return;

	    }
		printf("\nRecieved from server: %s \n",recvBuff);

		fflush(stdin);
		sendBuff="";
        memset(recvBuff, 0, 255);
	}
}
