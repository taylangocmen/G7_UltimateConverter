
#ifndef UC_BMP_H
#define UC_BMP_H

#include "uc_utils.h"
#include "uc_image.h"

#define BMP_HEADER_SIZE 14

#define BMP_BM 'BM'

#define DIB_CORE 12
#define DIB_INFO 40
#define DIB_V4 108
#define DIB_V5 124

#define BMP_1BPP 1
#define BMP_2BPP 2
#define BMP_4BPP 4
#define BMP_8BPP 8
#define BMP_16BPP 16
#define BMP_24BPP 24
#define BMP_32BPP 32

#define BMP_PPM 2835

#define BMP_256 256
#define BMP_BLUE256(x)  ((unsigned)(((float)(x & 0b11) / (float)3.0) * (float)255.0))
#define BMP_GREEN256(x) ((unsigned)(((float)((x >> 2) & 0b111) / (float)7.0) * (float)255.0))
#define BMP_RED256(x)   ((unsigned)(((float)((x >> 5) & 0b111) / (float)7.0) * (float)255.0))

#define BMP_BLUEINDEX256(x)  (((float)x / (float)255.0) * (float)0b11)
#define BMP_GREENINDEX256(x) (((float)x / (float)255.0) * (float)0b111)
#define BMP_REDINDEX256(x)   (((float)x / (float)255.0) * (float)0b111)

void read_bmp(UC_IMAGE* image);

unsigned write_bmp(UC_IMAGE* image, volatile int *toAddr);

unsigned bmp256_color_table_index_to_color(unsigned i);
unsigned bmp256_color_table_color_to_index(unsigned char r, unsigned char g, unsigned char b);

#endif /* UC_BMP_H */
