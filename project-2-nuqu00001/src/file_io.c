#include "file_io.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

// fscnf returns the number of extracted elements
int image_read(image_t* img, FILE* fin) {
    char format[3];
    int check = fscanf(fin, "%2s", format);
    if (check != 1) return -1;
    int checkP = strcmp(format, "P3");
    if (checkP != 0) return -1;

    int w, h;
    if (fscanf(fin, "%d %d", &w, &h) != 2) return -1;

    img->w = w;
    img->h = h;
    pixel_t* helper = img->img;
    img->img = malloc(sizeof(pixel_t) * w * h);
    free(helper);
    if (img->img == NULL) return -1;

    int max;
    if (fscanf(fin, "%d", &max) != 1) return -1;
    if (max != 255) return -1;

    if (img->img == NULL) return -1;

    for (int i = 0; i < w * h; i++) {
        int r, g, b;

        if (fscanf(fin, "%d %d %d", &r, &g, &b) != 3) return -1;

        img->img[i].r = r;
        img->img[i].g = g;
        img->img[i].b = b;
    }

    return 0;
}

void image_write(const image_t* img, FILE* fout) {
    int w = img->w;
    int h = img->h;
    fprintf(fout, "P3\n");
    fprintf(fout, "%d %d\n", w, h);
    fprintf(fout, "255\n");

    for (int i = 0; i < w * h; i++) {
        if ((i + 1) % w == 0) {
            fprintf(fout, "\n");
        }
        fprintf(fout, "%d %d %d ", img->img[i].r, img->img[i].g, img->img[i].b);
    }
}

void image_free(image_t* img) {
    if (img->img != NULL) {
        free(img->img);
    }
    img->w = img->h = 0;
    img->img = NULL;
}
