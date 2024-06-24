/* image magic cli : mogrify -format bmp original.jpg */

/*
https://stackoverflow.com/questions/17967480/parse-read-bitmap-file-in-c
https://github.com/Richardds/BitmapGrayscale/blob/master/main.cpp
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include <time.h>

#define NTHREADS 4

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

struct thread_data
{
    IMAGEPIXEL **pixels;
    int width;
    int	from;
    int	to;
};

void *calc_grayscale(void *ptr)
{
    struct thread_data *my_data = (struct thread_data *) ptr;

    IMAGEPIXEL **my_pixels = my_data->pixels;
    int my_from  = my_data->from;
    int my_to    = my_data->to;
    int my_width = my_data->width;

    for (int y = my_from; y < my_to; y++)
    {
        for (int x = 0; x < my_width; x++)
        {
            IMAGEPIXEL *pixel = &my_pixels[y][x];
            unsigned char gray = pixel->r * RED_COEFFICIENT + pixel->g * GREEN_COEFFICIENT + pixel->b * BLUE_COEFFICIENT;
            memset(pixel, gray, sizeof(IMAGEPIXEL));
        }
    }

    pthread_exit(NULL);
}

int main()
{
    /* open file descriptors */
    FILE *fIn = fopen("original.bmp", "rb");
    FILE *fOut = fopen("new_grayscale.bmp", "wb");
    if (!fIn || !fOut)
    {
        printf("File error.\n");
        return 0;
    }

    /* read headers */
    FILEHEADER fh;
    fread(&fh, sizeof(FILEHEADER), 1, fIn);
    
    INFOHEADER ih;
    fread(&ih, sizeof(INFOHEADER), 1, fIn);

    /* get important valus */
    int width = ih.width;
    int height = abs(ih.height);
    int stride = (width * 3 + 3) & ~3;
    int padding = stride - width * 3;
    
    printf("width: %d (%d)\n", width, width * 3);
    printf("height: %d\n", height);
    printf("stride: %d\n", stride);
    printf("padding: %d\n", padding);

    /* setup headers */
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

    /* write headers */
    fwrite(&fh, sizeof(FILEHEADER), 1, fOut);
    fwrite(&ih, sizeof(INFOHEADER), 1, fOut);
	
    /* define pixels array */
    IMAGEPIXEL **pixels = (IMAGEPIXEL **) malloc(height * sizeof(IMAGEPIXEL *));
    for (int y = 0; y < height; y++)
        pixels[y] = (IMAGEPIXEL *) malloc(width * sizeof(IMAGEPIXEL));

    /* read pixels */
    for (int y = 0; y < height; y++)
        fread(pixels[y], sizeof(IMAGEPIXEL), width, fIn);

    pthread_t threads[NTHREADS];
    struct thread_data thread_args[NTHREADS];

    int SIZE = height;
    int block = SIZE / NTHREADS;

    /* begin time */
    struct timespec begin, end; 
    clock_gettime(CLOCK_REALTIME, &begin);

    /* thread stuff */
    for (int i = 0; i < NTHREADS; i++)
    {
        thread_args[i].from = i * block;
        thread_args[i].to = thread_args[i].from + block;
        if (i == NTHREADS - 1) thread_args[i].to = SIZE;
        thread_args[i].pixels = pixels;
        thread_args[i].width = width;
        pthread_create(&threads[i], NULL, calc_grayscale, (void *) &thread_args[i]);
    }

    for (int i = 0; i < NTHREADS; i++)
        pthread_join(threads[i], NULL);
    
    /* end time */
    clock_gettime(CLOCK_REALTIME, &end);
    long seconds = end.tv_sec - begin.tv_sec;
    long nanoseconds = end.tv_nsec - begin.tv_nsec;
    double elapsed = seconds + nanoseconds*1e-9;
    printf("Time measured: %.3f seconds.\n", elapsed);

    /* write 2d pixels array to file*/
    for (int y = 0; y < height; y++)
        fwrite(pixels[y], sizeof(IMAGEPIXEL), width, fOut);

    /* free allocated heap chunks */
    for (int y = 0; y < height; y++)
    {
        free(pixels[y]);
        pixels[y] = NULL;
    }
    free(pixels);
    pixels = NULL;

    fclose(fOut);
    fclose(fIn);
    
    return 0;
}