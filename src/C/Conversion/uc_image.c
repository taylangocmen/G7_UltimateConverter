
#include <stdio.h>
#include <stdlib.h>

#include "uc_bmp.h"
#include "uc_png.h"
#include "uc_image.h"
#include "uc_utils.h"

unsigned convert_image(volatile int *fromAddr, size_t fileSize, unsigned fromFmt, volatile int *toAddr, unsigned toFmt) {
    UC_IMAGE* temp;
    temp = open_uc_image(fromAddr, fileSize, fromFmt);
    unsigned size = write_uc_image(temp, toAddr, toFmt);
    close_uc_image(temp);
    free(temp);
    return size;
}

UC_IMAGE* open_uc_image(volatile int *memAddr, size_t fileSize, unsigned fmt) {
    UC_IMAGE* image;
    image = (UC_IMAGE*) malloc(sizeof (UC_IMAGE));
    image->fSize = fileSize;
    image->fBuffer = (unsigned char*)memAddr;
    image->fFormat = fmt;

    if (fmt == bmp) read_bmp(image);
    else if (fmt == png) read_png(image);

    return image;
}

unsigned write_uc_image(UC_IMAGE* image, volatile int *toAddr, unsigned fmt) {
    if (fmt == bmp) 
        return write_bmp(image, toAddr);
    else if (fmt == png) 
        return write_png(image, toAddr);
    else
        return 0;
}

void close_uc_image(UC_IMAGE* image) {
    unsigned i, j;

    if (image->pxArr != NULL) {
        for (i = 0; i < image->pxHeight; i++) {
            for (j = 0; j < image->pxWidth; j++) {
                if (image->pxArr[i][j] != NULL)
                    free(image->pxArr[i][j]);
            }
            if (image->pxArr[i] != NULL)
                free(image->pxArr[i]);
        }
        if (image->pxArr != NULL)
            free(image->pxArr);
        image->pxArr = NULL;
    }
}

