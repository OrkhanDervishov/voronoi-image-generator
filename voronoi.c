#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint-gcc.h>
#include <malloc.h>
#include <math.h>
#include <time.h>

#define PAINTER_IMPLEMENTATION
#include "painter.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


#define POINT_COUNT 200


typedef enum{
    EUCLIDIAN = 0,
    MANHATTAN = 1,
} DistanceType;



int saveImagePPM(Image* img, char* filename){

    char path[64];
    sprintf(path, "images/%s", filename);

    FILE* file = fopen(path, "wb");

    if(file == NULL){
        perror("File opening failed\n");
        return 1;
    }

    // File configs
    fprintf(file, "P6\n%d %d\n255\n", img->width, img->height);

    // Save image data
    for(int i = 0; i < img->height; i++)
    for(int j = 0; j < img->width; j++){
        uint8_t bytes[3] = {
            (img->buffer[i * img->width + j].b),
            (img->buffer[i * img->width + j].g),
            (img->buffer[i * img->width + j].r)
        };

        fwrite(bytes, sizeof(bytes), 1, file);
    }

    fclose(file);
    return 0;
}

int saveImagePNG(Image* img, char* filename){
    char path[64];
    sprintf(path, "images/%s", filename);
    return stbi_write_png(path, img->width, img->height, 4, &img->buffer[0], 4*img->width);
}


Point getRandomPoint(int w, int h){
    Point p = {
        .x = rand() % w,
        .y = rand() % h
    };

    return p;
}


void generateRandomPoints(Point* arr, int count, int image_w, int image_h){
    for(int i = 0; i < count; i++){
        arr[i] = getRandomPoint(image_w, image_h);
    }
}

void generateRandomColors(Color* colors, int count){
    for(int i = 0; i < count; i++){
        colors[i] = getRandomColor();
    }
}

#define RECT_POINT_SIZE 5
#define RADIUS 2

void drawPoints(Image* img, Point* pArray, int count, Color color){
    for(int i = 0; i < count; i++){
        drawCircle(img, pArray[i], RADIUS, color);
    }
}

#define EUCLIDIAN_DISTANCE(x0, y0, x1, y1) (x1-x0)*(x1-x0)*100 + (y1-y0)*(y1-y0)*100  
#define MANHATTAN_DISTANCE(x0, y0, x1, y1) abs(x1-x0) + abs(y1-y0)

void voronoiColoring(Image* img, Point* pArray, int count, DistanceType type){
    Color* colors = (Color*)malloc(sizeof(Color)*count);
    generateRandomColors(colors, count);

    for(int i = 0; i < img->height; i++)
    for(int j = 0; j < img->width; j++){
        Color closestColor;
        int minDistance = 1e+9;
        for(int k = 0; k < count; k++){
            Point p = pArray[k];
            int distance;
            if(type == EUCLIDIAN)
                distance = EUCLIDIAN_DISTANCE(j, i, p.x, p.y);
            else if(type == MANHATTAN)
                distance = MANHATTAN_DISTANCE(j, i, p.x, p.y);
            if(distance < minDistance){
                minDistance = minDistance > distance ? distance : minDistance;
                closestColor = colors[k];
            }
        }
        img->buffer[i * img->width + j] = closestColor;
    }
    free(colors);
}


#define BACKGROUND_COLOR 0xFF505050
#define POINT_COLOR 0xFF000000

#define VORONOI
// #define PAINTER_TEST


int main(int argc, char* argv[]){

    // Default inputs
    DistanceType type = EUCLIDIAN;
    int point_count = POINT_COUNT;
    int width = DEFAULT_IMAGE_WIDTH;
    int height = DEFAULT_IMAGE_HEIGHT;
    int seed = time(NULL);

    // User inputs
    if(argc > 1) type = atoi(argv[1]);
    if(argc > 2) point_count = atoi(argv[2]);
    if(argc > 3) width = atoi(argv[3]);
    if(argc > 4) height = atoi(argv[4]);
    if(argc > 5) seed = atoi(argv[5]);

    printf("distance formula:%s count:%d width:%d height:%d seed:%d\n", 
        type ? "Manhattan" : "Euclidian",
        point_count, 
        width, 
        height, 
        seed);
    
    srand(seed);
    Image img; 
    Color line_color = {.r=255, .g=0, .b=0, .a=255}; 
    Color bc;
    bc.rgba = BACKGROUND_COLOR;
    
    createImage(&img, width, height);
    fillImage(&img, bc);

#ifdef VORONOI
    Point* pArray = (Point*)malloc(sizeof(Point)*point_count);
    Color c;
    c.rgba = POINT_COLOR;

    generateRandomPoints(pArray, point_count, img.width, img.height);
    voronoiColoring(&img, pArray, point_count, type);
    drawPoints(&img, pArray, point_count, c);

    free(pArray);
#endif

// Under construction. Nothing to do with voronoi diagrams
#ifdef PAINTER_TEST
    // drawLine(&img, line_color, 10, 10, 2000, 10);
    drawLineAntiAliased(&img, line_color, -10, -300, 2000, 400);
#endif

    // saveImagePPM(&img, "image.ppm");
    saveImagePNG(&img, "image.png");

    deleteImage(&img);
    return 0;
}
