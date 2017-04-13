
#ifndef UC_IMAGE_H
#define UC_IMAGE_H

#include "uc_utils.h"

typedef struct UC_IMAGE {
    unsigned char*** pxArr;
    unsigned pxHeight;
    unsigned pxWidth;
    unsigned pxFormat;

    unsigned char* fBuffer;
    size_t fSize;
    unsigned fFormat;
} UC_IMAGE;

UC_IMAGE* open_uc_image(volatile int *memAddr, size_t fileSize, unsigned fmt);

unsigned write_uc_image(UC_IMAGE* image, volatile int *toAddr, unsigned fmt);

void close_uc_image(UC_IMAGE* image);

unsigned convert_image(volatile int *fromAddr, size_t fileSize, unsigned fromFmt, volatile int *toAddr, unsigned toFmt);

#endif /* UC_IMAGE_H */

