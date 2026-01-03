#ifndef PAINTER_H
#define PAINTER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint-gcc.h>
#include <stdbool.h>
#include <malloc.h>
#include <math.h>


#define SWAP(a, b, t) t = a; a = b; b = t;

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

#define DEFAULT_IMAGE_WIDTH 1920
#define DEFAULT_IMAGE_HEIGHT 1080

typedef struct{
    int width;
    int height;
    Color* buffer;
} Image;



#define PAINTER_IMPLEMENTATION
#ifdef PAINTER_IMPLEMENTATION

void createImage(Image* img, size_t w, size_t h){
    img->buffer = (Color*)malloc(w * h * sizeof(Color));
    img->width = w;
    img->height = h;
}

void deleteImage(Image* img){
    free(img->buffer);
}




void fillImage(Image* img, Color color){
    for(int i = 0; i < img->height; i++)
    for(int j = 0; j < img->width; j++){
        img->buffer[i * img->width + j] = color;
    }
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

Rect correctRect(Image* img, Rect rect){
    // x, y correction
    if(rect.x < 0) rect.x = 0;
    else if(rect.x > img->width) rect.x = img->width;
    if(rect.y < 0) rect.y = 0;
    else if(rect.y > img->height) rect.y = img->height;

    // w, h correction
    if(rect.x + rect.w > img->width)
        rect.w = img->width - rect.x;
    if(rect.y + rect.h > img->height)
        rect.h = img->height - rect.y;

    return rect;
}


void drawCircle(Image* img, Point p, int radius, Color color){
    Rect rect = {
        .x = p.x - radius,
        .y = p.y - radius,
        .w = radius*2,
        .h = radius*2
    };

    rect = correctRect(img, rect);

    for(int i = rect.y; i < rect.h + rect.y; i++)
    for(int j = rect.x; j < rect.w + rect.x; j++){
        int dx = j - p.x;
        int dy = i - p.y;
        if(dx*dx + dy*dy <= radius*radius)
            img->buffer[i * img->width + j] = color;
    }
}

void drawRect(Image* img, Rect rect, Color color){

    rect = correctRect(img, rect);

    for(int i = rect.y; i < rect.h + rect.y; i++)
    for(int j = rect.x; j < rect.w + rect.x; j++){
        img->buffer[i * img->width + j] = color;
    }
}

// Correct line endpoints when they are out of frame size
Point correctLineEnd(int x, int y, int dx, int dy, int width, int height){
    if(x > width - 1){
        y -= dy*(x - (width - 1))/dx;
        x = width - 1;
    } else if(x < 0){
        y -= dy*x/dx;
        x = 0;
    }
    if(y > height - 1){
        x -= dx*(y - (height - 1))/dy;
        y = height - 1;
    } else if(y < 0){
        x -= dx*y/dy;
        y = 0;
    }

    Point p = {
        .x = x,
        .y = y,
    };

    return p;
}


void bresenhamHorizontal(Image* img, Color c, int x0, int y0, int x1, int y1){
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    
    if(x0 > x1){
        int temp;
        SWAP(x0, x1, temp);
        SWAP(y0, y1, temp);
    }
    
    int D = dx;
    int move = -1;
    if(y0 < y1){
        move = 1;
    }
    int k = 2 * dy;
    int y = y0;
    
    for(int x = x0; x < x1; x++){
        D += k;
        if(D > 2 * dx){
            y += move;
            D -= 2 * dx;
        }
        img->buffer[y*img->width + x] = c;
    }
}

void bresenhamVertical(Image* img, Color c, int x0, int y0, int x1, int y1){
    if(y0 > y1){
        int temp;
        SWAP(x0, x1, temp);
        SWAP(y0, y1, temp);
    }
    
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    
    
    int D = dy;
    int move = -1;
    if(x0 < x1){
        move = 1;
    }
    int k = 2 * dx;
    int x = x0;
    
    for(int y = y0; y < y1; y++){
        D += k;
        if(D > 2 * dy){
            x += move;
            D -= 2 * dy;
        }
        img->buffer[y*img->width + x] = c;
    }
}

// Bresenham
void drawLine(Image* img, Color c, int x0, int y0, int x1, int y1){
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);

    Point p0 = correctLineEnd(x0, y0, dx, dy, img->width, img->height);
    Point p1 = correctLineEnd(x1, y1, dx, dy, img->width, img->height);
    x0 = p0.x;
    y0 = p0.y;
    x1 = p1.x;
    y1 = p1.y;

    if(dx > dy){
        bresenhamHorizontal(img, c, x0, y0, x1, y1);
    } else {
        bresenhamVertical(img, c, x0, y0, x1, y1);
    }
}


void drawLineAAHorizontal(Image* img, Color c, int x0, int y0, int x1, int y1){
    if(x0 > x1){
        int temp;
        SWAP(x0, x1, temp);
        SWAP(y0, y1, temp);
    }
    
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    
    
    float k = (float)dy / (float)dx;
    if(y0 < y1) k = -k;
    
    float y = (float)y0;
    
    for(int x = x0; x < x1; x++){
        y -= k;
        
        float ry = floorf(y);
        float dist = y - ry;
        img->buffer[((int)y)*img->width + x] = c;
        img->buffer[((int)y+1)*img->width + x] = c;
        img->buffer[((int)y)*img->width + x].a = (uint8_t)(floor((1 - dist)*255.0f));
        img->buffer[((int)y+1)*img->width + x].a = (uint8_t)(floor(dist*255.0f));
    }
}

void drawLineAAVertical(Image* img, Color c, int x0, int y0, int x1, int y1){
    if(y0 > y1){
        int temp;
        SWAP(x0, x1, temp);
        SWAP(y0, y1, temp);
    }
    
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    
    
    float k = (float)dx / (float)dy;
    if(x0 < x1) k = -k;
    
    float x = (float)x0;
    
    for(int y = y0; y < y1; y++){
        x -= k;
        
        float rx = floorf(x);
        float dist = x - rx;
        img->buffer[(y)*img->width + (int)x] = c;
        img->buffer[(y+1)*img->width + (int)(x + 1)] = c;
        img->buffer[(y)*img->width + (int)x].a = (uint8_t)(floor((1 - dist)*255.0f));
        img->buffer[(y+1)*img->width + (int)(x + 1)].a = (uint8_t)(floor(dist*255.0f));
    }
}

// Xiaolin Wu
void drawLineAntiAliased(Image* img, Color c, int x0, int y0, int x1, int y1){
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);

    Point p0 = correctLineEnd(x0, y0, dx, dy, img->width, img->height);
    Point p1 = correctLineEnd(x1, y1, dx, dy, img->width, img->height);
    x0 = p0.x;
    y0 = p0.y;
    x1 = p1.x;
    y1 = p1.y;

    if(dx > dy){
        drawLineAAHorizontal(img, c, x0, y0, x1, y1);
    } else {
        drawLineAAVertical(img, c, x0, y0, x1, y1);
    }
}


#endif
#endif