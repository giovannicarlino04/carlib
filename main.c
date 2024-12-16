#include <stdio.h>
#include <stdlib.h>

#include "gc_network.h"
#include "gc_time.h"
#include "gc_xml.h"
#include "gc_memory.h"
#include "gc_iggy.h"


//MAINLY A TEST TO SEE IF FUNCTIONS WORK CORRECTLY

int main(int argc, char **argv) {
    int choice;
    if(argc != 2)
        return 1;
    char *file_path = argv[1];
    
    struct IGGYHeader header;

    gc_parse_iggy(file_path, header);

    getchar();

    printf("Exiting the program.\n");
    exit(0);
}
