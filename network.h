#pragma once

#include <stdio.h>
#include <string.h>

#include "time.h"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#endif

#define DEFAULT_PORT 8080

//Later on I'mma define my own Malloc()
//This should be moved in something like buffer.h
#define MAX_SIZE 2048

// Struct to manage the server
typedef struct {
#ifdef _WIN32
    SOCKET server_socket;
#else
    int server_socket;
#endif
    struct sockaddr_in server_addr;
    int port;
} NetworkServer;

// Functions for server management
int network_init(NetworkServer *server, int port);
int network_host(NetworkServer *server, const char* file_path);
int send_file_content(NetworkServer *server, int client_socket, const char *file_path);
void network_cleanup(NetworkServer *server);
