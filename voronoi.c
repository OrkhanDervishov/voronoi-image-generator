#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint-gcc.h>
#include <malloc.h>
#include <math.h>
#include <time.h>

typedef union{
    struct{
        uint8_t r, g, b, a;
    };
    uint32_t rgba;
} Color;

typedef struct
{
    int x, y;
} Point;

typedef struct
{
    int x, y;
    int w, h;
} Rect;

#define IMAGE_WIDTH 720
#define IMAGE_HEIGHT 480

typedef struct{
    int width;
    int height;
    Color buffer[IMAGE_HEIGHT][IMAGE_WIDTH];
} Image;

#define POINT_COUNT 25



int saveImage(Image* img, char* filename){

    char path[64];
    sprintf(path, "images/%s", filename);

    FILE* file = fopen(path, "w");

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
            (img->buffer[i][j].b),
            (img->buffer[i][j].g),
            (img->buffer[i][j].r)
        };

        fwrite(bytes, sizeof(bytes), 1, file);
    }

    fclose(file);
    return 0;
}

void createImage(Image* img, size_t w, size_t h){
    img->width = w;
    img->height = h;
}





void fillImage(Image* img, uint32_t color){
    for(int i = 0; i < img->height; i++)
    for(int j = 0; j < img->width; j++){
        img->buffer[i][j].rgba = color;
    }
}

void FillImage(Image* img, Color color){
    for(int i = 0; i < img->height; i++)
    for(int j = 0; j < img->width; j++){
        img->buffer[i][j].rgba = color.rgba;
    }
}


Point getRandomPoint(int w, int h){
    Point p = {
        .x = rand() % w,
        .y = rand() % h
    };

    printf("x:%d y:%d\n", p.x, p.y);

    return p;
}

Color getRandomColor(){
    Color c = {
        .r = rand() % 256,
        .g = rand() % 256,
        .b = rand() % 256,
        .a = 255
    };

    return c;
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

void drawRect(Image* img, Rect rect, Color color){
    if(rect.x < 0) rect.x = 0;
    else if(rect.x > img->width) rect.x = img->width;
    if(rect.y < 0) rect.y = 0;
    else if(rect.y > img->height) rect.y = img->height;
    
    if(rect.x + rect.w > img->width)
        rect.w = img->width - rect.x;
    if(rect.y + rect.h > img->height)
        rect.h = img->height - rect.y;

    // printf("works\n");
    // printf("x:%d y:%d w:%d h:%d ", rect.x, rect.y, rect.w, rect.h);
    for(int i = rect.y; i < rect.h + rect.y; i++)
    for(int j = rect.x; j < rect.w + rect.x; j++){
        img->buffer[i][j] = color;
    }
}

void drawCircle(Image* img, Point p, int radius, Color color){
    Rect rect = {
        .x = p.x - radius,
        .y = p.y - radius,
        .w = radius*2,
        .h = radius*2
    };

    if(rect.x < 0) rect.x = 0;
    else if(rect.x > img->width) rect.x = img->width;
    if(rect.y < 0) rect.y = 0;
    else if(rect.y > img->height) rect.y = img->height;

    if(rect.x + rect.w > img->width)
        rect.w = img->width - rect.x;
    if(rect.y + rect.h > img->height)
        rect.h = img->height - rect.y;

    for(int i = rect.y; i < rect.h + rect.y; i++)
    for(int j = rect.x; j < rect.w + rect.x; j++){
        int dx = j - p.x;
        int dy = i - p.y;
        if(dx*dx + dy*dy <=radius*radius)
            img->buffer[i][j] = color;
    }
}


#define RECT_POINT_SIZE 5
#define RADIUS 5

void drawPoints(Image* img, Point* pArray, int count, Color color){
    for(int i = 0; i < count; i++){
        // Rect r = {
        //     .x = pArray[i].x - RECT_POINT_SIZE/2,
        //     .y = pArray[i].y - RECT_POINT_SIZE/2,
        //     .w = RECT_POINT_SIZE,
        //     .h = RECT_POINT_SIZE
        // };
        // drawRect(img, r, color);
        drawCircle(img, pArray[i], RADIUS, color);
    }
}

#define GET_DISTANCE(x0, y0, x1, y1) (x1-x0)*(x1-x0) + (y1-y0)*(y1-y0)

void voronoiColoring(Image* img, Point* pArray, int count){
    Color colors[POINT_COUNT];
    generateRandomColors(colors, count);

    for(int i = 0; i < img->height; i++)
    for(int j = 0; j < img->width; j++){
        Color closestColor;
        int minDistance = 1e+9;
        for(int k = 0; k < count; k++){
            Point p = pArray[k];
            int distance = GET_DISTANCE(j, i, p.x, p.y);
            if(distance < minDistance){
                minDistance = minDistance > distance ? distance : minDistance;
                closestColor = colors[k];
            }
        }
        img->buffer[i][j] = closestColor;
    }
}


#define BACKGROUND_COLOR 0xFF202020
#define POINT_COLOR 0xFFFFFFFF

int main(void){
    srand(time(NULL));
    Image img;  
    Point pArray[POINT_COUNT];
    Color c;
    c.rgba = POINT_COLOR;
    // printf("%d %d %d %d", c.r, c.g, c.b, c.a);
    createImage(&img, IMAGE_WIDTH, IMAGE_HEIGHT);
    fillImage(&img, BACKGROUND_COLOR);

    generateRandomPoints(pArray, POINT_COUNT, img.width, img.height);
    voronoiColoring(&img, pArray, POINT_COUNT);
    drawPoints(&img, pArray, POINT_COUNT, c);

    // FillImage(&img, c);
    saveImage(&img, "image.ppm");

    return 0;
}
