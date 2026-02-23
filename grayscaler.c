#include <stdio.h>
#include <stdlib.h>
#include "ppm.h"

static int parse_thread_count(char *s)
{
    char *endptr = NULL;
    long value = strtol(s, &endptr, 10);

    if (*s == '\0' || *endptr != '\0') {
        return -1;
    }

    if (value <= 0 || value > 1000000) {
        return -1;
    }

    return (int)value;
}

int main(int argc, char *argv[])
{
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <threads> <input.ppm> <output.ppm>\n", argv[0]);
        return 1;
    }

    int thread_count = parse_thread_count(argv[1]);
    if (thread_count <= 0) {
        fprintf(stderr, "Error: thread count must be a positive integer.\n");
        return 1;
    }

    struct ppm *image = ppm_read(argv[2]);
    if (image == NULL) {
        fprintf(stderr, "Error: could not read input file \"%s\".\n", argv[2]);
        return 1;
    }

    if (ppm_write(image, argv[3]) != 0) {
        fprintf(stderr, "Error: could not write output file \"%s\".\n", argv[3]);
        ppm_free(image);
        return 1;
    }

    ppm_free(image);
    return 0;
}