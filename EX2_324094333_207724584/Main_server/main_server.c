// Don't forget to include "wsock32" in the library list.
#include <winsock2.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include <assert.h>
const int TIME_PORT = 27015;

// the checkForAnError function checks if there was an error and handles it accordingly
// @ params
// bytesResult = send/receive function result
// ErrorAt = string stating if it was receive or send function
// socket_1 & socket_2 = the sockets to close in case there was an error.

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
                char * filesBuff= " ";
                d = opendir(dirName);
                if (d)
                {
                    while ((dir = readdir(d)) != NULL)
                     {
                        strcat(filesBuff,dir->d_name);
                        strcat(filesBuff," ");
                       // printf("%s\n", dir->d_name);
                     }
                    closedir(d);
                }
                return filesBuff;
                }

void main() {
	// Initialize Winsock (Windows Sockets).

	// Create a WSADATA object called wsaData.
	// The WSADATA structure contains information about the Windows
	// Sockets implementation.
	WSADATA wsaData;
	SOCKET listenSocket;
	struct sockaddr_in serverService;
	char timeBuff[26];
	char *filesBuff;
	char * filesNames;
	char* filesContents;
	int i;

	// Call WSAStartup and return its value as an integer and check for errors.
	// The WSAStartup function initiates the use of WS2_32.DLL by a process.
	// First parameter is the version number 2.2.
	// The WSACleanup function destructs the use of WS2_32.DLL by a process.
	if (NO_ERROR != WSAStartup(MAKEWORD(2, 0), & wsaData)) {
		printf("Time Server: Error at WSAStartup()\n");
		return;
	}
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
	serverService.sin_port = htons(TIME_PORT);

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



	while(1)
    {
        // Accept connections and handles them one by one.
        struct sockaddr_in from; // Address of sending partner
        int fromLen = sizeof(from);

        printf("Main Server: Wait for clients' requests.\n");

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
            printf("Time Server: Error at accept(): ");
            printf("%d", WSAGetLastError());
            closesocket(listenSocket);
            WSACleanup();
            return;
        }

        printf("Time Server: Client is connected.\n");
        while (1) {

            // Send and receive data.
            int bytesSent = 0;
            int bytesRecv = 0;
            char * sendBuff;
            char recvBuff[255];

            // Get client's requests and answer them.
            // The recv function receives data from a connected or bound socket.
            // The buffer for data to be received and its available size are
            // returned by recv. The last argument is an idicator specifying the way
            // in which the call is made (0 for default).


            bytesRecv = recv(msgSocket, recvBuff, 255, 0);
            if (checkForAnError(bytesRecv,"recv",listenSocket,msgSocket))
                return;

            // start processing the client requests.

            if (strncmp(recvBuff, "Get all the documents names which in (files) file.", 50) == 0) {
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



                sendBuff = files;

            }
            else if (strncmp(recvBuff, "Get the content of an document.", 31) == 0) {

                  /* char * p;
                    p=recvBuff;
                    char * name,p_name;
                    p_name=name;
                    while(*p!='\0'){
                        if(*p=='.'){
                            p++;

                            while(*p!='\0'){
                               *name=*p;
                               p++;
                               name++;
                            }

                            break;
                        }
                        p++;
                 }
                 char* current_fileNames;
                 current_fileNames = unionfiles("files");
                 boolean flag=false;
                 p=current_fileNames;
                 int p_nameLen=strlen(p_name);
                 while(*p!='/0'){
                    if(*p!='^'){
                        int x = strncmp(p,p_name,p_nameLen);
                        if(x==0){
                            flag=true;
                            break;
                        }
                    }
                    p++;
                 }
                 if(flag){
                    sendBuff = readFile(p_name);
                 }else{
                      sendBuff=("sorry the file you have asked is not exist");
                 }*/

            }
            else if (strncmp(recvBuff, "What's your name?", 17) == 0) {
                sendBuff = "My Name is Time Server, Nice to meet you.";
            }
            else{
                printf("Time Server: Closing Connection.\n");
                closesocket(msgSocket);
                break;
            }

            // after processing send answer to client
            printf("Sending: %s\n",sendBuff);
            bytesSent = send(msgSocket, sendBuff, (int) strlen(sendBuff), 0);
            if (checkForAnError(bytesRecv,"send",listenSocket,msgSocket))
                return;

            fflush(stdin);
            sendBuff="";
        }
    }
    closesocket(listenSocket);
    WSACleanup();
    return ;
}

