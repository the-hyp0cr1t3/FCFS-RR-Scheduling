#include "utils.h"

#include <stdio.h>
#include <stdlib.h>

void check_file(char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "ERROR: File could not be created (%s)\n", filename);
        exit(EXIT_FAILURE);
    }

    fclose(file);
}