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

    (void)thread_count;  /* Real threading added next */

    struct ppm *image = ppm_read(argv[2]);
    if (image == NULL) {
        fprintf(stderr, "Error: could not read input file \"%s\".\n", argv[2]);
        return 1;
    }

    long long total_r = 0;
    long long total_g = 0;
    long long total_b = 0;

    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            int pixel = ppm_get_pixel(image, x, y);

            int r = PPM_PIXEL_R(pixel);
            int g = PPM_PIXEL_G(pixel);
            int b = PPM_PIXEL_B(pixel);

            total_r += r;
            total_g += g;
            total_b += b;

            int gray = (299 * r + 587 * g + 114 * b) / 1000;
            int gray_pixel = PPM_PIXEL(gray, gray, gray);

            ppm_set_pixel(image, x, y, gray_pixel);
        }
    }

    long long pixel_count = (long long)image->width * (long long)image->height;

    /* Placeholder thread output so formatting is visible during testing */
    printf("Thread %d: %d %d\n", 0, 0, image->height);
    printf("Average R: %lld\n", total_r / pixel_count);
    printf("Average G: %lld\n", total_g / pixel_count);
    printf("Average B: %lld\n", total_b / pixel_count);

    if (ppm_write(image, argv[3]) != 0) {
        fprintf(stderr, "Error: could not write output file \"%s\".\n", argv[3]);
        ppm_free(image);
        return 1;
    }

    ppm_free(image);
    return 0;
}