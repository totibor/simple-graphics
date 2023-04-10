#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "graphic.h"

#define SWAP(type, x, y)    do { type temp = x; x = y; y = temp; } while (0)
#define RED_CHAN(color)     (((color)&0x000000FF)>>(8*0))
#define GREEN_CHAN(color)   (((color)&0x0000FF00)>>(8*1))
#define BLUE_CHAN(color)    (((color)&0x00FF0000)>>(8*2))
#define ALPHA_CHAN(color)   (((color)&0xFF000000)>>(8*3))

typedef struct
{
    int x;
    int y;
} Point;

static int MAX_LINE_LENGTH;

/**
 * @brief Create a canvas object
 * 
 * @param pixels Pointer to the pixel array.
 * @param width Width of the canvas in pixels.
 * @param height Height of the canvas in pixels.
 * @param stride Number of pixels between each row.
 * @return Canvas object.
 */
Canvas create_canvas(uint32_t *pixels, size_t width, size_t height, size_t stride)
{
    Canvas canvas = {
        .pixels = pixels,
        .width  = width,
        .height = height,
        .stride = stride,
    };


    MAX_LINE_LENGTH = sqrt(pow(canvas.width, 2) + pow(canvas.height, 2));

    return canvas;
}

/**
 * @brief Interpolate a line between two points.
 * 
 * @param i0 First point's independent variable.
 * @param d0 First point's dependent variable.
 * @param i1 Second point's independent variable.
 * @param d1 Second point's dependent variable.
 * @return double* Array of dependent variables.
 */
double* interpolate(int i0, int d0, int i1, int d1)
{
    // TODO: calculate the exact line length.
    double* values = malloc(sizeof(double) * MAX_LINE_LENGTH);

    if (i0 == i1)
    {
        for (int i = 0; i <= i1; i++)
        {
            values[i] = d0;
        }
        return values;
    }

    double a = (double)(d1 - d0) / (double)(i1 - i0);

    double d = d0;

    for(int i = 0; i <= (i1 - i0); i++)
    {
        values[i] = d;
        d = d + a;
    }
    return values;
}

int* create_grid(Canvas canvas, int x_count, int y_count, int margin)
{
    int x1 = margin;
    int x2 = canvas.width - margin;
    int y1 = margin;
    int y2 = canvas.height - margin;

    double x_step = (double)(x2 - x1) / x_count;
    double y_step = (double)(y2 - y1) / y_count;

    // Multiply by 2 because each point has an X and Y coordinate.
    int* grid = malloc(sizeof(int) * (x_count + 1) * (y_count + 1) * 2);

    int i = 0;
    for(int y = 0; y <= y_count; y++)
    {
        for(int x = 0; x <= x_count; x++)
        {
            grid[i++] = x1 + x * x_step;
            grid[i++] = y1 + y * y_step;
        }
    }

    return grid;
}

/**
 * @brief Change the color of a specific pixel.
 * 
 * @param canvas Canvas to draw on.
 * @param x X coordinate of the pixel.
 * @param y Y coordinate of the pixel.
 * @param color Color to change the pixel to.
 */
void draw_pixel(Canvas canvas, int x, int y, uint32_t color)
{
    if (x >= canvas.width   || x < 0) return;   // x is outside of canvas
    if (y >= canvas.height  || y < 0) return;   // y is outside of canvas

    canvas.pixels[x + (y * canvas.stride)] = color;
}

void blend_pixel(uint32_t *dest, uint32_t src)
{
    uint32_t r1 = RED_CHAN(*dest);
    uint32_t g1 = GREEN_CHAN(*dest);
    uint32_t b1 = BLUE_CHAN(*dest);
    uint32_t a1 = ALPHA_CHAN(*dest);

    uint32_t r2 = RED_CHAN(src);
    uint32_t g2 = GREEN_CHAN(src);
    uint32_t b2 = BLUE_CHAN(src);
    uint32_t a2 = ALPHA_CHAN(src);
    
    r1 = (r1 * (255 - a2) + r2 * a2)/255; if (r1 > 255) r1 = 255;
    g1 = (g1 * (255 - a2) + g2 * a2)/255; if (g1 > 255) g1 = 255;
    b1 = (b1 * (255 - a2) + b2 * a2)/255; if (b1 > 255) b1 = 255;

    *dest = RGBA(r1, g1, b1, a1);
}

/**
 * @brief Draw a line between two points.
 * 
 * @param canvas Canvas to draw on.
 * @param x0 X coordinate of the first point.
 * @param y0 Y coordinate of the first point.
 * @param x1 X coordinate of the second point.
 * @param y1 Y coordinate of the second point.
 * @param color Color of the line.
 */
void draw_line(Canvas canvas, int x0, int y0, int x1, int y1, uint32_t color)
{
    if(abs(x1 - x0) > abs(y1 - y0))
    {
        // Line is horizontal-ish
        if (x0 > x1)
        {
            SWAP(int, x0, x1);
            SWAP(int, y0, y1);
        }
        
        // Array returned by fn interpolate is allocated in heap memory.
        // TODO: Check if this memory is still allocated after fn draw_line returns
        double *ys = interpolate(x0, y0, x1, y1);
        for(int x = x0; x <= x1; x++)
        {
            // draw_pixel(canvas, x, ys[x - x0], color);
            int y = ys[x - x0];
            blend_pixel(&PIXEL(canvas, x, y), color);
        }
    }
    else
    {
        // Line is vertical-ish
        if(y0 > y1)
        {
            SWAP(int, x0, x1);
            SWAP(int, y0, y1);
        }

        // Array returned by fn interpolate is allocated in heap memory.
        // TODO: Check if this memory is still allocated after fn draw_line returns
        double *xs = interpolate(y0, x0, y1, x1);
        for(int y = y0; y <= y1; y++)
        {
            // draw_pixel(canvas, xs[y - y0], y, color);
            int x = xs[y - y0];
            blend_pixel(&PIXEL(canvas, x, y), color);
        }
    }
    
}

/**
 * @brief Draw a wireframe triangle.
 * 
 * @param canvas Canvas to draw on.
 * @param x0 X coordinate of the first point.
 * @param y0 Y coordinate of the first point.
 * @param x1 X coordinate of the second point.
 * @param y1 Y coordinate of the second point.
 * @param x2 X coordinate of the third point.
 * @param y2 Y coordinate of the third point.
 * @param color Color of the triangle.
 */
void draw_triangle(Canvas canvas, int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color)
{
    draw_line(canvas, x0, y0, x1, y1, color);
    draw_line(canvas, x1, y1, x2, y2, color);
    draw_line(canvas, x2, y2, x0, y0, color);
}

/**
 * @brief Draw a filled triangle.
 * 
 * @param canvas Canvas to draw on.
 * @param x0 X coordinate of the first point.
 * @param y0 Y coordinate of the first point.
 * @param x1 X coordinate of the second point.
 * @param y1 Y coordinate of the second point.
 * @param x2 X coordinate of the third point.
 * @param y2 Y coordinate of the third point.
 * @param color Color of the triangle.
 */
void draw_filled_triangle(Canvas canvas, int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color)
{
    // Sort the points so that y0 <= y1 <= y2
    if (y1 < y0)
    {
        SWAP(int, x1, x0);
        SWAP(int, y1, y0);
    }
    if (y2 < y0)
    {
        SWAP(int, x2, x0);
        SWAP(int, y2, y0);
    }
    if (y2 < y1)
    {
        SWAP(int, x2, x1);
        SWAP(int, y2, y1);
    }

    // Compute the x coordinates of the triangle edges
    double* x01 = interpolate(y0, x0, y1, x1);
    double* x12 = interpolate(y1, x1, y2, x2);
    double* x02 = interpolate(y0, x0, y2, x2);

    // Concatenate x01 and x12
    double* x012 = malloc(sizeof(double) * (y2 - y0));
    memcpy(x012, x01, sizeof(double) * (y1 - y0));
    memcpy(x012 + (y1 - y0), x12, sizeof(double) * (y2 - y1));

    // Determine which is the left and right edge
    double* left_edge;
    double* right_edge;
    int middle = (y2 - y0) / 2;
    if (x02[middle] < x012[middle])
    {
        left_edge = x02;
        right_edge = x012;
    }
    else
    {
        left_edge = x012;
        right_edge = x02;
    }

    // Draw the triangle
    for (int y = y0; y < y2; y++)
    {
        for (int x = left_edge[y - y0]; x <= right_edge[y - y0]; x++)
        {
            // draw_pixel(canvas, x, y, color);
            blend_pixel(&PIXEL(canvas, x, y), color);
        }
    }

    free(x01);
    free(x12);
    free(x02);
    free(x012);
}

void draw_rect(Canvas canvas, int x1, int y1, int width, int height, uint32_t color)
{
    // TODO: use normalised rectange.
    // Rectangles can have negative widths/heights, which means that the starting location
    // will not always be at the top left corner. Normalizing the rectangle will ensure
    // that the X and Y co-ordinates of the rectangle are at the top left.

    int x2 = x1 + width;
    int y2 = y1 + height;

    // If width and/or height is negative
    // if (x1 > x2) SWAP(int, x1, x2);
    // if (y1 > y2) SWAP(int, y1, y2);

    for(int y = y1; y <= y2; y++)
    {
        for(int x = x1; x <= x2; x++)
        {
            blend_pixel(&PIXEL(canvas, x, y), color);
        }
    }
}

void plot_points(Canvas canvas, int x0, int y0, int x, int y, uint32_t color) {
    blend_pixel(&PIXEL(canvas, x0 + x, y0 + y), color);
    blend_pixel(&PIXEL(canvas, x0 - x, y0 + y), color);
    blend_pixel(&PIXEL(canvas, x0 + x, y0 - y), color);
    blend_pixel(&PIXEL(canvas, x0 - x, y0 - y), color);
    blend_pixel(&PIXEL(canvas, x0 + y, y0 + x), color);
    blend_pixel(&PIXEL(canvas, x0 - y, y0 + x), color);
    blend_pixel(&PIXEL(canvas, x0 + y, y0 - x), color);
    blend_pixel(&PIXEL(canvas, x0 - y, y0 - x), color);
}

void draw_circle(Canvas canvas, int x0, int y0, int radius, uint32_t color) {
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;

    while (x <= y) {
        plot_points(canvas, x0, y0, x, y, color);
        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

void add_grain(Canvas canvas, int amount)
{
    for(int x = 0; x < canvas.width; x++)
    {
        for(int y = 0; y < canvas.height; y++)
        {
            uint32_t r1 = RED_CHAN(PIXEL(canvas, x, y));
            uint32_t g1 = GREEN_CHAN(PIXEL(canvas, x, y));
            uint32_t b1 = BLUE_CHAN(PIXEL(canvas, x, y));
            uint32_t a1 = ALPHA_CHAN(PIXEL(canvas, x, y));

            int grain = rand() % amount;
            int sign = rand() % 2;

            if (sign == 0)
            {
                grain = -grain;
            }

            int r2 = r1 + grain; if (r2 > 255) r2 = 255; if(r2 < 0) r2 = 0;
            int g2 = g1 + grain; if (g2 > 255) g2 = 255; if(g2 < 0) g2 = 0;
            int b2 = b1 + grain; if (b2 > 255) b2 = 255; if(b2 < 0) b2 = 0;

            PIXEL(canvas, x, y) = RGBA(r2, g2, b2, a1);

        }
    }
}


void draw_filled_circle(Canvas canvas, int x, int y, int radius, uint32_t color)
{
    int x1 = 0;
    int y1 = radius;
    int d = 3 - 2 * radius;

    while (y1 >= x1)
    {
        draw_line(canvas, x - x1, y - y1, x + x1, y - y1, color);
        draw_line(canvas, x - y1, y - x1, x + y1, y - x1, color);
        draw_line(canvas, x - y1, y + x1, x + y1, y + x1, color);
        draw_line(canvas, x - x1, y + y1, x + x1, y + y1, color);

        if (d < 0)
            d += 4 * x1 + 6;
        else
        {
            d += 4 * (x1 - y1) + 10;
            y1--;
        }
        x1++;
    }
}

void draw_grid(Canvas canvas, int x_count, int y_count, int margin, uint32_t color)
{
    int x1 = margin;
    int x2 = canvas.width - margin;
    int y1 = margin;
    int y2 = canvas.height - margin;

    double x_step = (double)(x2 - x1) / x_count;
    double y_step = (double)(y2 - y1) / y_count;

    for(int i = 0; i <= x_count; i++)
    {
        draw_line(canvas, x1 + i * x_step, y1, x1 + i * x_step, y2, color);
    }

    for(int i = 0; i <= y_count; i++)
    {
        draw_line(canvas, x1, y1 + i * y_step, x2, y1 + i * y_step, color);
    }
}

void fill_canvas(Canvas canvas, uint32_t color)
{
    for(size_t i = 0; i < canvas.width * canvas.height; i++)
    {
        canvas.pixels[i] = color;
    }
}

void bresenham_line(Canvas canvas, int x0, int y0, int x1, int y1, u_int32_t color)
{
    if (x0 > x1 || y0 > y1)
    {
        SWAP(int, x0, x1);
        SWAP(int, y0, y1);
    }
    
    int xi = 1;
    int dx = x1 - x0;
    if (dx < 0)
    {
        xi = -1;
        dx = -dx;
    }

    int dy = y1 - y0;
    int yi = 1;
    if (dy < 0 )
    {
        yi = -1;
        dy = -dy;
    }

    int x = x0;
    int y = y0;

    
    if (abs(dx) > abs(dy))
    {
        int P = 2 * dy - dx;
        while (x != x1)
        {
            draw_pixel(canvas, x, y, color);
            x += xi;
            if(P < 0)
            {
                P = P + (2 * dy);
            }
            else
            {
                P = P + (2 * dy) - (2 * dx);
                y = y + yi;
            }
        }
    }
    else
    {
        int P = 2 * dx - dy;
        while(y != y1)
        {
            draw_pixel(canvas, x, y, color);
            y += yi;
            if(P < 0)
            {
                P = P + (2 * dx);
            }
            else
            {
                P = P + (2 * dx) - (2 * dy);
                x = x + xi;
            }
        }
    }
}
