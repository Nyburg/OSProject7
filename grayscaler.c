#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "ppm.h"

struct thread_data {
    int thread_id;
    struct ppm *image;
    int start_y;
    int slice_height;
    long long sum_r;
    long long sum_g;
    long long sum_b;
};

static void *run_grayscale(void *arg)
{
    struct thread_data *td = (struct thread_data *)arg;

    td->sum_r = 0;
    td->sum_g = 0;
    td->sum_b = 0;

    int width = td->image->width;
    int end_y = td->start_y + td->slice_height;

    for (int y = td->start_y; y < end_y; y++) {
        for (int x = 0; x < width; x++) {
            int pixel = ppm_get_pixel(td->image, x, y);

            int r = PPM_PIXEL_R(pixel);
            int g = PPM_PIXEL_G(pixel);
            int b = PPM_PIXEL_B(pixel);

            td->sum_r += r;
            td->sum_g += g;
            td->sum_b += b;

            int gray = (299 * r + 587 * g + 114 * b) / 1000;
            int gray_pixel = PPM_PIXEL(gray, gray, gray);
            ppm_set_pixel(td->image, x, y, gray_pixel);
        }
    }

    return NULL;
}

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

    if (thread_count > image->height) {
        thread_count = image->height;
    }

    pthread_t *threads = malloc(sizeof(*threads) * (size_t)thread_count);
    struct thread_data *td = malloc(sizeof(*td) * (size_t)thread_count);

    if (threads == NULL || td == NULL) {
        fprintf(stderr, "Error: memory allocation failed.\n");
        free(threads);
        free(td);
        ppm_free(image);
        return 1;
    }

    int base_height = image->height / thread_count;
    int remainder = image->height % thread_count;

    for (int i = 0; i < thread_count; i++) {
        td[i].thread_id = i;
        td[i].image = image;
        td[i].start_y = i * base_height;
        td[i].slice_height = base_height;

        if (i == thread_count - 1) {
            td[i].slice_height += remainder;
        }

        printf("Thread %d: %d %d\n", i, td[i].start_y, td[i].slice_height);

        if (pthread_create(&threads[i], NULL, run_grayscale, &td[i]) != 0) {
            fprintf(stderr, "Error: failed to create thread %d.\n", i);

            for (int j = 0; j < i; j++) {
                (void)pthread_join(threads[j], NULL);
            }

            free(threads);
            free(td);
            ppm_free(image);
            return 1;
        }
    }

    long long total_r = 0;
    long long total_g = 0;
    long long total_b = 0;

    for (int i = 0; i < thread_count; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            fprintf(stderr, "Error: failed to join thread %d.\n", i);
            free(threads);
            free(td);
            ppm_free(image);
            return 1;
        }

        total_r += td[i].sum_r;
        total_g += td[i].sum_g;
        total_b += td[i].sum_b;
    }

    long long pixel_count = (long long)image->width * (long long)image->height;

    printf("Average R: %lld\n", total_r / pixel_count);
    printf("Average G: %lld\n", total_g / pixel_count);
    printf("Average B: %lld\n", total_b / pixel_count);

    if (ppm_write(image, argv[3]) != 0) {
        fprintf(stderr, "Error: could not write output file \"%s\".\n", argv[3]);
        free(threads);
        free(td);
        ppm_free(image);
        return 1;
    }

    free(threads);
    free(td);
    ppm_free(image);
    return 0;
}