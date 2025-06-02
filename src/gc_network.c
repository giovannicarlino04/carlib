#include <winsock.h>

#include "common.h"

#define DEFAULT_PORT 8080
#define MAX_SIZE 4096

typedef int socklen_t;

// Struct to manage the server
typedef struct {
    SOCKET server_socket;
    struct sockaddr_in server_addr;
    int port;
} NetworkServer;


// Cleanup the server (close socket, etc.)
DLLEXPORT void gc_network_cleanup(NetworkServer *server) {
    closesocket(server->server_socket);
    WSACleanup();
    close(server->server_socket);
}


// Cross-platform network initialization
DLLEXPORT int gc_network_init(NetworkServer *server, int port) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        gc_MessageBox("WSAStartup failed", MB_ICONERROR);
        return -1;
    }

    server->port = port;
    server->server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server->server_socket == (unsigned int)-1) {
        gc_MessageBox("socket() failed", MB_ICONERROR);
        WSACleanup();
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

DLLEXPORT int gc_send_file_content(SOCKET client_socket, const char *file_path) {
    HANDLE file = CreateFileA(
        file_path,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (file == INVALID_HANDLE_VALUE) {
        gc_MessageBox("Could not open file", MB_ICONERROR);
        return -1;
    }

    const char response_header[] =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=UTF-8\r\n\r\n";

    send(client_socket, response_header, (int)strlen(response_header), 0);

    char buffer[MAX_SIZE];
    DWORD bytes_read;
    BOOL result;

    do {
        result = ReadFile(file, buffer, MAX_SIZE, &bytes_read, NULL);
        if (!result || bytes_read == 0)
            break;

        send(client_socket, buffer, bytes_read, 0);
    } while (bytes_read > 0);

    CloseHandle(file);
    return 0;
}

// Running the server to accept and handle connections
DLLEXPORT int gc_network_host(NetworkServer *server, const char* file_path) {
    int client_socket;
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[MAX_SIZE];

    while (1) {
        client_socket = accept(server->server_socket, (struct sockaddr *)&client_addr, &addr_len);

        int bytes_received = recv(client_socket, buffer, MAX_SIZE, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';  // Null-terminate the received data

            // Serve content from a file
            if (gc_send_file_content(client_socket, file_path) != 0) {
                 gc_MessageBox("Failed to send file content", MB_ICONERROR);
                break;
            }
        }
        closesocket(client_socket);
    }

    return 0;
}
