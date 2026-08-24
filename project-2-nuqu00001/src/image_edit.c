#include "image_edit.h"

#include <stdlib.h>

#include "util.h"

// counterclockwise: (x, y) -> (y, w-x-1)

void rotate_counterclockwise(image_t* img) {
    int w = img->w;
    int h = img->h;
    int newW = h;
    // (x, y)= (y, w-1-x)
    pixel_t* nimp = malloc(sizeof(pixel_t) * w * h);  // nimp as a pointer
    for (int i = 0; i < w * h; i++) {
        int x = i % w;
        int y = i / w;
        int newx = y;
        int newy = w - 1 - x;
        int newI = newy * newW + newx;

        nimp[newI] = img->img[i];
    }
    pixel_t* helper = img->img;
    img->img = nimp;
    img->w = h;
    img->h = w;
    free(helper);
}

// clockwise :
// for simplicity use the (x, y) -> (h-y-1, x)

void rotate_clockwise(image_t* img) {
    int w = img->w;
    int h = img->h;
    int newW = h;

    pixel_t* nimp = malloc(sizeof(pixel_t) * w * h);  // nimp as a pointer
    for (int i = 0; i < w * h; i++) {
        int x = i % w;
        int y = i / w;
        int newx = h - 1 - y;
        int newy = x;
        // new (x, y)=(h-1-y, x)
        pixel_t helper = img->img[i];
        if ((helper.r < 0 || helper.r > 255) ||
            ((helper.g < 0 || helper.g > 255) || (helper.b < 0 || helper.b > 255))) {
            printf("Pixel out of the bound\n");
            return;
        }

        nimp[newy * newW + newx] = helper;
    }
    pixel_t* helpertofree = img->img;
    img->img = nimp;
    img->w = h;
    img->h = w;
    free(helpertofree);
}

// my goal is to directly swap ie it will take max (x/2, (x+1)/2)  rt/ efficient
// divide by 2
void mirror_horizontal(image_t* img) {
    int w = img->w;
    int h = img->h;

    for (int y = 0; y < h; y++) {
        int newW = w / 2;  // since it takes the min as int

        for (int x = 0; x < newW; x++) {  // newX := i
            int currI = (y * w + x);

            int nextX = (w - x - 1);
            int nextI = (y * w + nextX);

            // store values by swapping indexes
            pixel_t helper = img->img[nextI];

            img->img[nextI] = img->img[currI];
            img->img[currI] = helper;
        }
    }
}

// newY= h-y-1;
// same logic as the horizontal, but with the y achse

void mirror_vertical(image_t* img) {
    int w = img->w;
    int h = img->h;

    for (int y = 0; y < h / 2; y++) {
        int nextY = h - y - 1;

        for (int x = 0; x < w; x++) {
            // since there is no need to change x
            int currI = y * w + x;
            // next are the one which we switch with

            int nextI = nextY * w + x;

            pixel_t helper = img->img[currI];
            img->img[currI] = img->img[nextI];
            img->img[nextI] = helper;  // helper has the current index
        }
    }
}

// error found with buffer overflow: one index for diff size of arrays!!!!
void resize(image_t* img, int new_width, int new_height) {
    int w = img->w;
    int h = img->h;
    if (new_width <= 0 || new_height <= 0) {
        printf("inacceptable width/height\n");
        return;
    }

    pixel_t* pixel2 = calloc(new_width * new_height, sizeof(pixel_t));
    if (pixel2 == NULL) {
        printf("Memory allocation failed\n");
        return;
    }

    for (int j = 0; j < new_height; j++) {
        for (int i = 0; i < new_width; i++) {
            int ind = new_width * j + i;  // index for new image
            if (w <= i || h <= j) {       // case distinction if size of the old image is smaller than the new one
                pixel2[ind].r = 0;
                pixel2[ind].g = 0;
                pixel2[ind].b = 0;
            } else {
                int idx = w * j + i;  // index for old image
                pixel2[ind].r = img->img[idx].r;
                pixel2[ind].g = img->img[idx].g;
                pixel2[ind].b = img->img[idx].b;
            }
        }
    }

    pixel_t* helper = img->img;
    img->img = pixel2;
    free(helper);
    img->w = new_width;
    img->h = new_height;
}
