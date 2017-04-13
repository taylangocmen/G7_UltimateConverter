
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "uc_bmp.h"
#include "uc_image.h"
#include "uc_utils.h"

volatile int* IP = XPAR_CUSTOM_SPEED_UP_FINAL_0_S00_AXI_BASEADDR;

void read_bmp(UC_IMAGE* image) {
    unsigned i, j;

    printf("recieved image size: %u\n", image->fSize);

    i = 0;

    unsigned signature = get_bytes(0, 2, image->fBuffer);
    printf("signature: %08X\n", signature);
    if (signature != BMP_BM)
        abort_("File was to be a BMP but was not.");

    unsigned offsetPxArr = get_bytes(14, 10, image->fBuffer);
    unsigned dibHSize = get_bytes(18, 14, image->fBuffer);
    image->pxWidth = get_bytes(22, 18, image->fBuffer);
    image->pxHeight = get_bytes(26, 22, image->fBuffer);
    printf("offsetPxArr: %08X\n", offsetPxArr);
    printf("dibHSize: %08X\n", dibHSize);
    printf("image->pxWidth: %08X\n", image->pxWidth);
    printf("image->pxHeight: %08X\n", image->pxHeight);

    if (dibHSize != DIB_CORE && dibHSize != DIB_INFO && dibHSize != DIB_V4 && dibHSize != DIB_V5)
        abort_("File has an unsupported DIBHEADER size: %u ", dibHSize);

    assert(dibHSize >= DIB_CORE);
    if (dibHSize == DIB_CORE) {
        abort_("We don't know what to do with DIB_CORE yet file.");
        return;
    }

    unsigned bitsPPx = get_bytes(30, 28, image->fBuffer);
    unsigned imageSize = get_bytes(38, 34, image->fBuffer);
    unsigned colorsInTable = get_bytes(50, 46, image->fBuffer);
    printf("bitsPPx: %08X\n", bitsPPx);
    printf("imageSize: %08X\n", imageSize);
    printf("colorsInTable: %08X\n", colorsInTable);

    unsigned char colorTable[256][4];

    image->pxFormat = 3;

    if (bitsPPx == BMP_1BPP || bitsPPx == BMP_2BPP || bitsPPx == BMP_4BPP || bitsPPx == BMP_8BPP) {
        assert(colorsInTable == to_power(2, bitsPPx));
        printf("colors in table is true\n");

        unsigned index = BMP_HEADER_SIZE + dibHSize;
        for (j = 0; j < colorsInTable; index += 4, j++) {
            colorTable[j][2] = BOFF0(get_bytes(index + 1, index + 0, image->fBuffer)); // blue
            colorTable[j][1] = BOFF0(get_bytes(index + 2, index + 1, image->fBuffer)); // green
            colorTable[j][0] = BOFF0(get_bytes(index + 3, index + 2, image->fBuffer)); // red
            colorTable[j][3] = BOFF0(get_bytes(index + 4, index + 3, image->fBuffer)); // alpha
        }
    } else if (bitsPPx == BMP_16BPP || bitsPPx == BMP_24BPP || bitsPPx == BMP_32BPP) {
        abort_("We don't know what to do with this BPP yet file.");
    } else abort_("File has an unsupported BPP");

    printf("done with colors in table\n");

    unsigned endPxArr = offsetPxArr + imageSize;
    unsigned bytesPPx = bitsPPx / 8;
    unsigned lineSize = imageSize / image->pxHeight;
    unsigned paddingPLine = lineSize % bytesPPx;

    printf("lineSize: %08X\n", lineSize);
    printf("paddingPLine: %08X\n", paddingPLine);
    printf("image->pxWidth: %08X\n", image->pxWidth);
    printf("bytesPPx: %08X\n", bytesPPx);

    assert((lineSize - paddingPLine) == (image->pxWidth * bytesPPx));

    image->pxArr = (unsigned char***) malloc(image->pxHeight * sizeof (unsigned char**));
    for (i = 0; i < image->pxHeight; i++) {
        image->pxArr[i] = (unsigned char**) malloc(image->pxWidth * sizeof (unsigned char*));
        for (j = 0; j < image->pxWidth; j++) {
            image->pxArr[i][j] = (unsigned char*) malloc(4 * sizeof (unsigned char));
        }
    }
    printf("malloc for pxarr successful!\n");

    assert(dibHSize >= DIB_INFO);
    if (dibHSize == DIB_INFO) {
        unsigned rowIndex = image->pxHeight - 1;
        for (i = offsetPxArr; i < endPxArr; i += lineSize, rowIndex--) {
            unsigned colIndex = 0;
            for (j = i; j < i + lineSize - paddingPLine; j += bytesPPx, colIndex++) {
                unsigned pixel = get_bytes(j, j + bytesPPx, image->fBuffer);
                image->pxArr[rowIndex][colIndex][0] = colorTable[pixel][0];
                image->pxArr[rowIndex][colIndex][1] = colorTable[pixel][1];
                image->pxArr[rowIndex][colIndex][2] = colorTable[pixel][2];
                image->pxArr[rowIndex][colIndex][3] = colorTable[pixel][3];
            }
            assert(colIndex == image->pxWidth);
        }
        assert(rowIndex == 4294967295);
        printf("read_bmp successful!\n");
        return;
    }


    assert(bitsPPx == BMP_16BPP || bitsPPx == BMP_24BPP || bitsPPx == BMP_32BPP);

    assert(dibHSize >= DIB_V4);
    abort_("We don't know what to do with DIB_V4 yet file.");
    if (dibHSize == DIB_V4) {
        return;
    }

    assert(dibHSize == DIB_V5);
    abort_("We don't know what to do with DIB_V5 yet file");
    return;
}

unsigned write_bmp(UC_IMAGE* image, volatile int *toAddr){
    unsigned pxWidth = image->pxWidth;
    unsigned pxHeight = image->pxHeight;
    unsigned pxFormat = image->pxFormat;
    assert(pxFormat == 3);

    unsigned offsetPxArr = BMP_HEADER_SIZE + DIB_INFO + (4 * BMP_256);
    unsigned padding = (4 - (pxWidth % 4));
    if(padding == 4) padding = 0;
    unsigned lineSize = (pxWidth + padding);
    unsigned imageSize = (pxHeight * lineSize);
    unsigned fileSize = offsetPxArr + imageSize;
    unsigned char* bitmapStream = (unsigned char *)toAddr;
    unsigned i, j;
    
    // bitmapStream signature
    bitmapStream[0] = 'B';
    bitmapStream[1] = 'M';
    
    // file fileSize
    bitmapStream[2] = BOFF0(fileSize);
    bitmapStream[3] = BOFF1(fileSize);
    bitmapStream[4] = BOFF2(fileSize);
    bitmapStream[5] = BOFF3(fileSize);
    
    // reserved field (in hex. 00 00 00 00)
    for(i = 6; i < 10; i++) bitmapStream[i] = 0;
    
    // offset of pixel data inside the image
    bitmapStream[10] = BOFF0(offsetPxArr);
    bitmapStream[11] = BOFF1(offsetPxArr);
    bitmapStream[12] = BOFF2(offsetPxArr);
    bitmapStream[13] = BOFF3(offsetPxArr);

    // -- BITMAP HEADER -- // // header fileSize
    bitmapStream[14] = BOFF0(DIB_INFO);
    bitmapStream[15] = BOFF1(DIB_INFO);
    bitmapStream[16] = BOFF2(DIB_INFO);
    bitmapStream[17] = BOFF3(DIB_INFO);

    // width of the image
    bitmapStream[18] = BOFF0(pxWidth);
    bitmapStream[19] = BOFF1(pxWidth);
    bitmapStream[20] = BOFF2(pxWidth);
    bitmapStream[21] = BOFF3(pxWidth);

    // height of the image
    bitmapStream[22] = BOFF0(pxHeight);
    bitmapStream[23] = BOFF1(pxHeight);
    bitmapStream[24] = BOFF2(pxHeight);
    bitmapStream[25] = BOFF3(pxHeight);

    // Planes
    bitmapStream[26] = 1;
    bitmapStream[27] = 0;

    // number of bits per pixel
    bitmapStream[28] = 8; // 3 byte
    bitmapStream[29] = 0;

    // compression method (no compression)
    for(i = 30; i < 34; i++) bitmapStream[i] = 0;

    // fileSize of pixel data
    bitmapStream[34] = BOFF0(imageSize);
    bitmapStream[35] = BOFF1(imageSize);
    bitmapStream[36] = BOFF2(imageSize);
    bitmapStream[37] = BOFF3(imageSize);

    // horizontal resolution of the image - pixels per meter (2835)
    bitmapStream[38] = BOFF0(BMP_PPM);
    bitmapStream[39] = BOFF1(BMP_PPM);
    bitmapStream[40] = BOFF2(BMP_PPM);
    bitmapStream[41] = BOFF3(BMP_PPM);

    // vertical resolution of the image - pixels per meter (2835)
    bitmapStream[42] = BOFF0(BMP_PPM);
    bitmapStream[43] = BOFF1(BMP_PPM);
    bitmapStream[44] = BOFF2(BMP_PPM);
    bitmapStream[45] = BOFF3(BMP_PPM);

    // color pallette information
    bitmapStream[46] = BOFF0(BMP_256);
    bitmapStream[47] = BOFF1(BMP_256);
    bitmapStream[48] = BOFF2(BMP_256);
    bitmapStream[49] = BOFF3(BMP_256);

    // number of important colors
    for(i = 50; i < 54; i++) bitmapStream[i] = 0;

    // -- color table -- //
    for(i = 54, j = 0; j < BMP_256; i+=4, j++){
        unsigned color = bmp256_color_table_index_to_color(j);
        bitmapStream[i] = BOFF0(color);
        bitmapStream[i+1] = BOFF1(color);
        bitmapStream[i+2] = BOFF2(color);
        bitmapStream[i+3] = BOFF3(color);
    }

    unsigned a, b;
    
    for(a = pxHeight; a > 0; a--){
        for(b = 0; b < pxWidth; b++, i++) {
            unsigned char r = image->pxArr[a-1][b][0];
            unsigned char g = image->pxArr[a-1][b][1];
            unsigned char b = image->pxArr[a-1][b][2];
            unsigned index = bmp256_color_table_color_to_index(r, g, b);
            bitmapStream[i] = index;
        }
        for(b = 0; b < padding; b++, i++)
            bitmapStream[i] = 0;
    }
    
    return fileSize;
}


unsigned bmp256_color_table_index_to_color(unsigned i){
    *(IP) = i;
    while(*(IP+1) == 0)
        xil_printf("waiting bmp256_color_table_index_to_color: %u \n", *(IP+1));
    unsigned color = *(IP+2);
    return color;
}

unsigned bmp256_color_table_color_to_index(unsigned char r, unsigned char g, unsigned char b){
    unsigned color = (b | (g << 8) | (r << 16));
    *(IP+3) = color;
    while(*(IP+4)==0)
        xil_printf("waiting bmp256_color_table_color_to_index: %u \n", *(IP+4));
    unsigned index = *(IP+5);
    return index;
}
