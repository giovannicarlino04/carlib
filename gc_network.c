#include "gc_network.h"
#include "gc_debug.h"
#include "gc_memory.h"

// Cross-platform network initialization
int gc_network_init(NetworkServer *server, int port) {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        gc_MessageBox("WSAStartup failed", MB_ICONERROR);
        return -1;
    }
#endif

    server->port = port;
    server->server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server->server_socket == (unsigned int)-1) {
        gc_MessageBox("socket() failed", MB_ICONERROR);
#ifdef _WIN32
        WSACleanup();
#endif
        return -1;
    }

    server->server_addr.sin_family = AF_INET;
    server->server_addr.sin_addr.s_addr = INADDR_ANY;
    server->server_addr.sin_port = htons(port);

    if (bind(server->server_socket, (struct sockaddr *)&server->server_addr, sizeof(server->server_addr)) == -1) {
        gc_MessageBox("bind() failed", MB_ICONERROR);
        gc_network_cleanup(server);
        return -1;
    }

    if (listen(server->server_socket, 5) == -1) {
        gc_MessageBox("bind() failed", MB_ICONERROR);
        gc_network_cleanup(server);
        return -1;
    }

    return 0;
}

// Sending file content to the client
int gc_send_file_content(int client_socket, const char *file_path) {
    FILE *file = fopen(file_path, "r");
    if (!file) {
        gc_MessageBox("Could not open file", MB_ICONERROR);
        return -1;
    }

    char response_header[] = "HTTP/1.1 200 OK\r\n"
                             "Content-Type: text/html; charset=UTF-8\r\n\r\n";
    send(client_socket, response_header, strlen(response_header), 0);

    char buffer[MAX_SIZE];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, MAX_SIZE, file)) > 0) {
        send(client_socket, buffer, bytes_read, 0);
    }

    fclose(file);
    return 0;
}

// Running the server to accept and handle connections
int gc_network_host(NetworkServer *server, const char* file_path) {
    int client_socket;
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[MAX_SIZE];

    while (1) {
        client_socket = accept(server->server_socket, (struct sockaddr *)&client_addr, &addr_len);

        int bytes_received = recv(client_socket, buffer, MAX_SIZE, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';  // Null-terminate the received data
            printf("Received: %s\n", buffer);

            // Serve content from a file
            if (gc_send_file_content(client_socket, file_path) != 0) {
                 gc_MessageBox("Failed to send file content", MB_ICONERROR);
                break;
            }
        }
#ifdef _WIN32
        closesocket(client_socket);
#else
        close(client_socket);
#endif        
    }

    return 0;
}

// Cleanup the server (close socket, etc.)
void gc_network_cleanup(NetworkServer *server) {
#ifdef _WIN32
    closesocket(server->server_socket);
    WSACleanup();
#else
    close(server->server_socket);
#endif
}
