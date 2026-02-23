#ifndef PPM_H
#define PPM_H

#define PPM_PIXEL_R(p) (((p)>>16)&0xff)
#define PPM_PIXEL_G(p) (((p)>>8)&0xff)
#define PPM_PIXEL_B(p) ((p)&0xff)

#define PPM_PIXEL(r,g,b) ((((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff))

struct ppm {
    int type;
    int width;
    int height;
    int maxval;
    char *data;
};

struct ppm *ppm_read(char *filename);
int ppm_write(struct ppm *ppm, char *filename);
void ppm_free(struct ppm *ppm);
int ppm_get_pixel(struct ppm *ppm, int x, int y);
void ppm_set_pixel(struct ppm *ppm, int x, int y, int pixel);

#endif
