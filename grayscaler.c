#include <stdio.h>

int main(int argc, char *argv[])
{
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <threads> <input.ppm> <output.ppm>\n", argv[0]);
        return 1;
    }

    return 0;
}