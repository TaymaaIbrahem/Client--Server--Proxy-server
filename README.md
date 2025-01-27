# Client-Server Application with Proxy and Main Server

This project implements a client-server system with a proxy server and a main server, developed in C using Code::Blocks. The system handles specific client requests while managing communication between the proxy and the main server for efficient file handling and error resolution. All communication is based on TCP (Transmission Control Protocol) to ensure reliable data transfer between components.

## Overview
- **Client:**
  
    The client can send four types of service requests:
  
      1. List Files: Returns a list of all files in the files folder.
      2. File Contents: Returns the contents of a specific file.
      3. Round-Trip Time Measurement: Measures the time it takes for a request and response to complete.
      4. Close Connection: Terminates the connection with the server.
    
- **Proxy Server:**
  
    The proxy server supports all client-requested services and:

      1. Handles Missing Files: If the requested file is not found in the proxy, it checks the main server for the file.
      2. Error Handling: Returns an error message for unsupported service requests.
  
- **Main Server:**
  
    The main server:

      1. Supports all the services requested by the proxy server.
      2. Acts as the fallback source for missing files requested by the client via the proxy server.

  
## Key Features
1. TCP Communication: Ensures reliable and ordered data transfer between client, proxy server, and main server.
2. Error handling for unsupported requests.
3. Efficient file fetching mechanism between proxy and main server.
4. Round-trip time measurement for request-response cycles.

   
## Requirements
1. Code::Blocks: Development environment for C programs.
2. C Compiler: GCC or compatible compiler.

   
## Instructions to Run the App
1. Download the Repository:
   - Clone or download the project files from this repository.
2. Open in Code::Blocks:
   - Launch Code::Blocks and open the project file (.cbp).
3. Build the Project:
   - Build the project using the Build menu or by pressing Ctrl+F9.
4. Run the Application:
   
   Start the main server, proxy server, and client in the following order:
   - Run the Main Server project.
   - Run the Proxy Server project.
   - Run the Client project.
5. Client Commands:
   
   Use the client interface to send requests(by the order number of the request 1,2,3,4). Supported requests:
     1. LIST: Lists all files in the files folder.
     2. GET <filename>: Returns the contents of a specific file. The files in the folder are homeWork1, homeWork2, homeWork3, homeWork4.
     3. PING: Measures round-trip time.
     4. EXIT: Closes the connection.

