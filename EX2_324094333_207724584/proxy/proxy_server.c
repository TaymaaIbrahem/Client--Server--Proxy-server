// Don't forget to include "wsock32" in the library list.
#include <winsock2.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <dirent.h>
      /* declare constants as required */

const int TIME_PORT = 27015;
const int TERADATA_PORT = 3360;
// the checkForAnError function checks if there was an error and handles it accordingly
// @ params
// bytesResult = send/receive function result
// ErrorAt = string stating if it was receive or send function
// socket_1 & socket_2 = the sockets to close in case there was an error.
char *readFile(char *filename) {
 FILE *f = fopen(filename, "rt");
 assert(f);
 fseek(f, 0, SEEK_END);
 long length = ftell(f);
 fseek(f, 0, SEEK_SET);
 char *buffer = (char *) malloc(length + 1);
 buffer[length] = '\0';
 fread(buffer, 1, length, f);
 fclose(f);
 return buffer;
}

char* unionfiles(char * dirName){
                DIR *d;
                struct dirent *dir;
                char  filesBuff[] = "h";
                d = opendir(dirName);
                if (d)
                {
                    while ((dir = readdir(d)) != NULL)
                     {
                        strcat(filesBuff,dir->d_name);
                        strcat(filesBuff," ");
                        printf("%s\n", dir->d_name);
                     }
                    closedir(d);
                }


                return filesBuff;
        }

bool checkForAnError(int bytesResult, char* ErrorAt, SOCKET socket_1, SOCKET socket_2){
    if (SOCKET_ERROR == bytesResult) {
        printf("Time Server: Error at %s(): ",ErrorAt);
        printf("%d", WSAGetLastError());
        closesocket(socket_1);
        closesocket(socket_2);
        WSACleanup();
        return true;
    }
    return false;
}

bool checkForAnErrorClient(int bytesResult, char* ErrorAt, SOCKET socket){
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
	// Initialize Winsock (Windows Sockets).

	// Create a WSADATA object called wsaData.
	// The WSADATA structure contains information about the Windows
	// Sockets implementation.
	WSADATA wsaData;
	SOCKET listenSocket;

	struct sockaddr_in serverService;
	char *files_requests[5];
	char *files_content[5];
	char filesBuff[200];
	int i,j;
	for(i=0;i<5;i++){
        files_requests[i]=" ";
	}
	for(i=0;i<5;i++){
        files_content[i]=" ";
	}
	// Call WSAStartup and return its value as an integer and check for errors.
	// The WSAStartup function initiates the use of WS2_32.DLL by a process.
	// First parameter is the version number 2.2.
	// The WSACleanup function destructs the use of WS2_32.DLL by a process.
	if (NO_ERROR != WSAStartup(MAKEWORD(2, 0), & wsaData)) {
		printf("Time Proxy Server: Error at WSAStartup()\n");
		return;
	}


	/////////////////////////////////////////////////////////////////////////////////////


    // Create the listen socket
	listenSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (INVALID_SOCKET == listenSocket) {
		printf("Time Server: Error at socket(): ");
		printf("%d", WSAGetLastError());
		WSACleanup();
		return;
	}

	memset( & serverService, 0, sizeof(serverService));
	// Address family (must be AF_INET - Internet address family).
	serverService.sin_family = AF_INET;
	// IP address. The sin_addr is a union (s_addr is a unsigned long
	// (4 bytes) data type).
	// inet_addr (Iternet address) is used to convert a string (char *)
	// into unsigned long.
	// The IP address is INADDR_ANY to accept connections on all interfaces.
	serverService.sin_addr.s_addr = htonl(INADDR_ANY);
	// IP Port. The htons (host to network - short) function converts an
	// unsigned short from host to TCP/IP network byte order
	// (which is big-endian).
	serverService.sin_port = htons(TERADATA_PORT);

	// Bind the socket for client's requests.

	// The bind function establishes a connection to a specified socket.
	// The function uses the socket handler, the sockaddr structure (which
	// defines properties of the desired connection) and the length of the
	// sockaddr structure (in bytes).
	if (SOCKET_ERROR == bind(listenSocket, (SOCKADDR * ) & serverService, sizeof(serverService))) {
		printf("Time Server: Error at bind(): ");
		printf("%d", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return;
	}

	// Listen on the Socket for incoming connections.
	// This socket accepts only one connection (no pending connections
	// from other clients). This sets the backlog parameter.

	//defines the maximum length to which the
    //  queue of pending connections for listenSocket may grow
	if (SOCKET_ERROR == listen(listenSocket, 5)) {
		printf("Time Server: Error at listen(): ");
		printf("%d", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return;
	}

	 SOCKET connSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	                 if (INVALID_SOCKET == connSocket) {
	                     	printf("proxy as  Client soket: Error at socket(): ");
	                     	printf("%d", WSAGetLastError());
		                    WSACleanup();
		                    return;
	                 }
	struct sockaddr_in server;
	memset( & server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(TIME_PORT);
	if (SOCKET_ERROR == connect(connSocket, (SOCKADDR * ) & server, sizeof(server))) {
		printf("Proxy Client: Error at connect(): ");
		printf("%d", WSAGetLastError());
		closesocket(connSocket);
		WSACleanup();
		return;
	}
	printf("connection established successfully with main server.");

	while(1)
    {
        // Accept connections and handles them one by one.
        struct sockaddr_in from; // Address of sending partner
        int fromLen = sizeof(from);

        printf("Proxy Server: Wait for clients' requests.\n");

        // The accept function permits an incoming connection
        // attempt on another socket (msgSocket).
        // The first argument is a bounded-listening socket that
        // receives connections.
        // The second argument is an optional pointer to buffer
        // that receives the internet address of the connecting enrity.
        // the third one is a pointer to the length of the network address
        // (given in the second argument).
        SOCKET msgSocket = accept(listenSocket, (struct sockaddr * ) & from, & fromLen);
        if (INVALID_SOCKET == msgSocket) {
            printf("Proxy Server: Error at accept(): ");
            printf("%d", WSAGetLastError());
            closesocket(listenSocket);
            WSACleanup();
            return;
        }

        printf("Proxy Server: Client is connected.\n");
        while (1) {

            // Send and receive data.
            int bytesSentToClient= 0;
            int bytesSentToServer= 0;
            int bytesRecvFromClient = 0;
            int bytesRecvFromServer = 0;
            char * sendBuffToClient=" ";
            char * sendBuffToServer= " ";
            char* sendBuff;
            char recvBuffFromClient[255];
            char recvBuffFromServer[500];

            // Get client's requests and answer them.
            // The recv function receives data from a connected or bound socket.
            // The buffer for data to be received and its available size are
            // returned by recv. The last argument is an idicator specifying the way
            // in which the call is made (0 for default).


            bytesRecvFromClient = recv(msgSocket, recvBuffFromClient, 255, 0);
            if (checkForAnError(bytesRecvFromClient,"recv",listenSocket,msgSocket))
                return;


            // start processing the client requests.
            if (strncmp(recvBuffFromClient, "Get all the documents names which in (files) file.", 50) == 0) {
                   DIR *d;
                   struct dirent *dir;
                   char  files[]=" ";
                   d = opendir("y");
                   if (d)
                    {
                      while ((dir = readdir(d)) != NULL)
                          {
                            strcat(files,dir->d_name);
                            strcat(files," ");
                            // printf("%s\n", dir->d_name);
                          }
                      closedir(d);
                    }


                sendBuffToClient = files;
            }
            else if (strncmp(recvBuffFromClient, "Get the content of an document.", 31) == 0) {
                    printf("\n srrtttthoon\n");
                     for(i=0;i<5;i++){
                        if(strcmp(recvBuffFromClient,files_requests)==0){
                           sendBuffToClient = files_content[i];
                        }
                     }
                     if(strcmp(sendBuffToClient," ") == 0){
                        sendBuffToServer = recvBuffFromClient;
                         if(strcmp(sendBuffToServer," ")){
                             bytesSentToServer = send(connSocket, sendBuffToServer, (int) strlen(sendBuffToServer), 0);
                             if (checkForAnErrorClient(bytesSentToServer,"send",connSocket))
                                  return;

                             bytesRecvFromServer = recv(connSocket, recvBuffFromServer, 255, 0);
                             if (checkForAnErrorClient(bytesRecvFromServer,"recv",connSocket)){
                                  printf("\n error");
			                      return;


	                         }

                         }
                     }
                   /* char fileName[10]="";
                    j=0;
                    for(i=32;i<strlen(recvBuffFromClient);i++){
                        fileName[j++]=recvBuffFromClient[i];
                        printf("\n%c",fileName[j]);
                    }
                    /*char * p;
                    p=recvBuffFromClient;
                    while(*p!='\0'){
                        if(*p=='.'){
                            p++;
                            i=0;
                            while(*p!='\0'){
                               *name=*p;
                               p++;
                               name++;
                            }
                            i++;
                            break;
                        }
                        p++;
                 }
                 printf("\n %s \n ",fileName);

                 boolean flag = false;
                 /*char* content;
                 while(*p!='\0'){
                    if(*p!='^'){
                        int x = strncmp(p,p_name,strlen(p_name));
                        if(x==0){
                            flag = true;
                            content = readFile(p_name);
                        }
                    }
                    p++;
                 }
                 if(flag){
                    //sendBuffToClient = content;
                 }else{

                     sendBuffToServer=strcat("Get the content of an document.",fileName);
                     //reate a socket and connect to an Internet address.(socket to connect with the main server)
                     bytesSentToServer = send(connSocket, sendBuffToServer, (int) strlen(sendBuffToServer), 0);
                      if (checkForAnErrorClient(bytesSentToServer,"send",connSocket))
                             return;
                     bytesRecvFromServer = recv(connSocket, recvBuffFromServer, 255, 0);
                     if (checkForAnErrorClient(bytesRecvFromServer,"recv",connSocket)){

			              return;

                      }



               }*/
            }
            else if (strncmp(recvBuffFromClient, "the calculate result of Round Trip Time.", 17) == 0) {



            }
            else{
                printf("Time Server: Closing Connection.\n");
                closesocket(msgSocket);
                break;
            }

            if(strcmp(sendBuffToClient," ")){
            // after processing send answer to client
            printf("Sending: %s\n",sendBuffToClient);
            bytesSentToClient = send(msgSocket, sendBuffToClient, (int) strlen(sendBuffToClient), 0);
            if (checkForAnError(bytesSentToClient,"send",listenSocket,msgSocket)){
                return;

            }
            }
            if(strcmp(sendBuffToServer," ")){
                 bytesSentToServer = send(connSocket, sendBuffToServer, (int) strlen(sendBuffToServer), 0);
                 if (checkForAnErrorClient(bytesSentToServer,"send",connSocket))
                  return;
            }
            fflush(stdin);
            sendBuffToClient="";
            sendBuffToServer="";
        }
    }
    closesocket(listenSocket);
    WSACleanup();
    return;
}

