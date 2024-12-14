#include "network.h"
#include "time.h"

int network_test(const char* file_path) {
    NetworkServer server;

    if (network_init(&server, DEFAULT_PORT) != 0) {
        fprintf(stderr, "Failed to initialize network server.\n");
        return -1;
    }

    printf("Server running on port %d\n", DEFAULT_PORT);

    network_host(&server, file_path);

    network_cleanup(&server);
    return 0;
}

int main(int argc, char* argv[]){
    if(argc >= 2)
        network_test(argv[1]);
}
