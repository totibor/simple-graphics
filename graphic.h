#ifndef _TT_GRAPHIC_H_
#define _TT_GRAPHIC_H_

#define RGBA(r, g, b, a) ((((r)&0xFF)<<(8*0)) | (((g)&0xFF)<<(8*1)) | (((b)&0xFF)<<(8*2)) | (((a)&0xFF)<<(8*3)))

typedef struct
{
    uint32_t *pixels;
    size_t width;
    size_t height;
    size_t stride;
} Canvas;

Canvas create_canvas(uint32_t *pixels, size_t width, size_t height, size_t stride);
int* create_grid(Canvas canvas, int x_count, int y_count, int margin);
void draw_pixel(Canvas canvas, int x, int y, uint32_t color);
void draw_line(Canvas canvas, int x0, int y0, int x1, int y1, uint32_t color);
void draw_triangle(Canvas canvas, int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
void draw_filled_triangle(Canvas canvas, int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
void draw_rect(Canvas canvas, int x, int y, int width, int height, uint32_t color);
void draw_grid(Canvas canvas, int x_count, int y_count, int margin, uint32_t color);
void fill_canvas(Canvas canvas, uint32_t color);

#endif /* _TT_GRAPHIC_H_ */