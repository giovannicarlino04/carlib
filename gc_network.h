#ifndef GC_NETWORK_H
#define GC_NETWORK_H

#include <winsock2.h>
#include <ws2tcpip.h>

#define DEFAULT_PORT 8080


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
int gc_network_init(NetworkServer *server, int port);
int gc_network_host(NetworkServer *server, const char* file_path);
int gc_send_file_content(SOCKET client_socket, const char *file_path);
void gc_network_cleanup(NetworkServer *server);
#endif // GC_NETWORK_H
