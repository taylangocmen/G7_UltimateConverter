
#include <stdio.h>
#include <stdlib.h>

#include "uc_png.h"
#include "uc_image.h"
#include "uc_utils.h"
#include "lodepng.h"

void read_png(UC_IMAGE* image) {
    unsigned error;
    unsigned char* rawImage;
    unsigned width, height, format;
    unsigned char* png = 0;
    size_t pngsize;
    LodePNGState state;

    lodepng_state_init(&state);

    png = (unsigned char *)image->fBuffer;
    pngsize = image->fSize;
    error = 0;
    
    if (!error)
        error = lodepng_decode(&rawImage, &width, &height, &state, png, pngsize);
    else
        abort_("Could not decode png file - error %u: %s", error, lodepng_error_text(error));

    /*use rawImage here*/
    unsigned i, j;
    format = 3;
    image->pxHeight = height;
    image->pxWidth = width;
    image->pxFormat = format;
    image->pxArr = (unsigned char***) malloc(height * sizeof (unsigned char**));
    for (i = 0; i < height; i++) {
        image->pxArr[i] = (unsigned char**) malloc(width * sizeof (unsigned char*));
        for (j = 0; j < width; j++) {
            image->pxArr[i][j] = (unsigned char*) malloc(4 * sizeof (unsigned char));
            image->pxArr[i][j][0] = rawImage[4 * width * i + 4 * j + 0];
            image->pxArr[i][j][1] = rawImage[4 * width * i + 4 * j + 1];
            image->pxArr[i][j][2] = rawImage[4 * width * i + 4 * j + 2];
            image->pxArr[i][j][3] = 255;
        }
    }

    lodepng_state_cleanup(&state);
    free(rawImage);
}

unsigned write_png(UC_IMAGE* image, volatile int *toAddr) {
    unsigned pxHeight = image->pxHeight;
    unsigned pxWidth = image->pxWidth;
    unsigned pxFormat = image->pxFormat;

    unsigned x, y, z;
    unsigned char* rawImage;
    rawImage  = (unsigned char*)malloc(pxWidth * pxHeight * pxFormat * sizeof(unsigned char));
    for (y = 0; y < pxHeight; y++)
        for (x = 0; x < pxWidth; x++)
            for (z = 0; z < pxFormat; z++)
                rawImage[pxFormat * pxWidth * y + pxFormat * x + z] = BOFF0(image->pxArr[y][x][z]);

    unsigned err = 1;
    size_t buffersize;
	unsigned char* buffer;
    unsigned char* writebuffer = (unsigned char*) toAddr;

    if (pxFormat == 3){
        err = lodepng_encode_memory(&buffer, &buffersize, rawImage, pxWidth, pxHeight, LCT_RGB, 8);
    }
    if (err){
        printf("Could not encode png file pxFormat: %u, err: %u\n", pxFormat, err);
    }
    
    unsigned i;
    unsigned buffersizeunsigned = (unsigned)buffersize;
    for(i = 0; i < buffersizeunsigned; i++){
        writebuffer[i] = buffer[i];
    }

    free(buffer);
    free(rawImage);
    return buffersizeunsigned;
}
