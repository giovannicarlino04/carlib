#include <stdio.h>
#include <stdlib.h>

#include "gc_network.h"
#include "gc_time.h"
#include "gc_xml.h"
#include "gc_memory.h"

// Function to test the network server
int network_serve(const char* file_path) {
    NetworkServer server;

    if (gc_network_init(&server, DEFAULT_PORT) != 0) {
        fprintf(stderr, "Failed to initialize network server.\n");
        return -1;
    }

    printf("Server running on port %d\n", DEFAULT_PORT);

    gc_network_host(&server, file_path);

    gc_network_cleanup(&server);
    return 0;
}


int main(int argc, char **argv) {
    int choice;
    char *file_path = argv[1];

    network_serve(file_path);

    printf("Exiting the program.\n");
    exit(0);
}
