/* image magic cli : mogrify -format bmp original.jpg */

/*
https://stackoverflow.com/questions/17967480/parse-read-bitmap-file-in-c
https://github.com/Richardds/BitmapGrayscale/blob/master/main.cpp
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#define RED_COEFFICIENT   0.299
#define GREEN_COEFFICIENT 0.587
#define BLUE_COEFFICIENT  0.114

typedef struct __attribute__((__packed__)) {                                                                                                                                                                                                                             
    uint8_t  magic1;
    uint8_t  magic2;
    uint32_t total_size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t bitmap_offset;                                                                                                                                                         
} FILEHEADER;                                                                                                                                                                                                                                

typedef struct __attribute__((__packed__)) {                                                                                                                                                                                                                             
    uint32_t size;
    uint32_t width;
    uint32_t height;
    uint16_t color_planes;
    uint16_t color_depth;
    uint32_t compression;
    uint32_t bitmap_size;
    uint32_t x_resolution;
    uint32_t y_resolution;
    uint32_t colors;
    uint32_t important_colors;                                                                                                                                                                                                        
} INFOHEADER;                                                                                                                                                                                                                                

typedef struct __attribute__((__packed__)) {                                                                                                                                                                                                                             
    uint8_t  r;
    uint8_t  g;
    uint8_t  b;                                                                                                                                                                                                                      
} IMAGEPIXEL;

int main()
{
    FILE *fIn = fopen("original.bmp", "rb");
    FILE *fOut = fopen("new_grayscale.bmp", "wb");
    if (!fIn || !fOut)
    {
        printf("File error.\n");
        return 0;
    }

    FILEHEADER fh;
    fread(&fh, sizeof(unsigned char), sizeof(FILEHEADER), fIn);
    
    INFOHEADER ih;
    fread(&ih, sizeof(unsigned char), sizeof(INFOHEADER), fIn);

    int width = ih.width;
    int height = abs(ih.height);
    int stride = (width * 3 + 3) & ~3;
    int padding = stride - width * 3;
    
    printf("width: %d (%d)\n", width, width * 3);
    printf("height: %d\n", height);
    printf("stride: %d\n", stride);
    printf("padding: %d\n", padding);

    // need to take padding into consideration for file size
    fh.total_size = sizeof(FILEHEADER) + sizeof(INFOHEADER) + (3 * ih.width * ih.height) + padding * ih.height;
    fh.bitmap_offset = sizeof(FILEHEADER) + sizeof(INFOHEADER);
    ih.size = sizeof(INFOHEADER);
    ih.color_planes = 1;
    ih.compression = 0; // BI_RGB
    ih.bitmap_size = (3 * ih.width * ih.height) + padding * ih.height;
    ih.x_resolution = 0;
    ih.y_resolution = 0;
    ih.colors = 0;
    ih.important_colors = 0;

    fwrite(&fh, sizeof(unsigned char), sizeof(FILEHEADER), fOut);
    fwrite(&ih, sizeof(unsigned char), sizeof(INFOHEADER), fOut);

    struct timespec begin, end; 
    clock_gettime(CLOCK_REALTIME, &begin);

    IMAGEPIXEL pixel;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            fread(&pixel, sizeof(IMAGEPIXEL), 1, fIn);
            unsigned char gray = pixel.r * RED_COEFFICIENT + pixel.g * GREEN_COEFFICIENT + pixel.b * BLUE_COEFFICIENT;
            //pixel.r = gray;
            //pixel.g = gray;
            //pixel.b = gray;
            memset(&pixel, gray, sizeof(IMAGEPIXEL));
            fwrite(&pixel, sizeof(IMAGEPIXEL), 1, fOut);
        }
        fread(&pixel, padding, 1, fIn);
        fwrite(&pixel, padding, 1, fOut);
    }

    clock_gettime(CLOCK_REALTIME, &end);
    long seconds = end.tv_sec - begin.tv_sec;
    long nanoseconds = end.tv_nsec - begin.tv_nsec;
    double elapsed = seconds + nanoseconds*1e-9;
    printf("Time measured: %.3f seconds.\n", elapsed);

    fclose(fOut);
    fclose(fIn);
    
    return 0;
}