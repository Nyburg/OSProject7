# Grayscaler

Convert a color PPM image to grayscale using multiple threads. The image is split into horizontal slices, and each thread processes one slice while also tracking RGB sums for average color reporting.

## Building

Command line:

* `make` to build. An executable called `grayscaler` will be produced.
* `make clean` to clean up object files.
* `make pristine` to clean up all build products, including the executable.

VS Code:

* The default build task can run `make` if configured.

## Files

* `grayscaler.c`: Main program logic, pthread worker, slice assignment, and average RGB computation
* `ppm.c`: Provided PPM image library implementation
* `ppm.h`: Provided PPM image library header/macros
* `goat.ppm`: Example input image for testing
* `Makefile`: Build rules for the project
* `README.md`: Project documentation

## Data

* `struct ppm`: Provided image structure containing width, height, metadata, and raw pixel data
* `struct thread_data`: Per-thread work description and RGB sums
  * `thread_id`
  * `image`
  * `start_y`
  * `slice_height`
  * `sum_r`
  * `sum_g`
  * `sum_b`

## Functions

* `main()`
  * `parse_thread_count()`: Parses and validates the requested thread count
  * `ppm_read()`: Loads the input image
  * Creates thread and thread-data arrays
  * Computes slice boundaries for each thread
  * `pthread_create()`: Launches one worker per slice
    * `run_grayscale()`: Converts pixels in assigned slice to grayscale and tracks RGB sums
      * `ppm_get_pixel()`: Reads a pixel
      * `ppm_set_pixel()`: Writes grayscale pixel
  * `pthread_join()`: Waits for workers and gathers sums
  * Computes average RGB from thread sums
  * `ppm_write()`: Saves output image
  * `ppm_free()`: Frees image memory

## Notes

* This program expects PPM type P6 images, as supported by the provided library.
* The last thread receives any remainder rows if the image height is not evenly divisible by the thread count.
* If the requested thread count is larger than the image height, the thread count is reduced to the image height.
* Grayscale conversion uses the BT.601 luma formula:
  * `gray = (299*r + 587*g + 114*b) / 1000`