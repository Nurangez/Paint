#include "flood_fill.h"

#include <stdbool.h>
#include <stdlib.h>

#include "util.h"

int checker(image_t* img, int ind, pixel_t* helper) {
    pixel_t h2 = img->img[ind];
    if (h2.r == helper->r && h2.g == helper->g && h2.b == helper->b) {
        return 1;
    } else {
        return 0;
    }
}

void flood1(image_t* img, int x, int y, pixel_t* target_color, pixel_t* helper) {
    int w = img->w;
    int h = img->h;
    img->img[y * w + x] = *target_color;

    if (x > 0 && checker(img, (y * w + x - 1), helper) == 1) {
        flood1(img, x - 1, y, target_color, helper);
    }
    if (y > 0 && checker(img, ((y - 1) * w + x), helper) == 1) {
        flood1(img, x, y - 1, target_color, helper);
    }
    if (y < h - 1 && checker(img, ((y + 1) * w + x), helper) == 1) {
        flood1(img, x, y + 1, target_color, helper);
    }
    if (x < w - 1 && checker(img, (y * w + x + 1), helper) == 1) {
        flood1(img, x + 1, y, target_color, helper);
    }
}

void flood(image_t* img, int x, int y, pixel_t* target_color) {
    if (x < 0 || y < 0 || x >= img->w || y >= img->h) return;
    int w = img->w;
    pixel_t helper = img->img[y * w + x];

    if (helper.r == target_color->r && helper.g == target_color->g && helper.b == target_color->b) {
        return;
    }

    flood1(img, x, y, target_color, &helper);
}
