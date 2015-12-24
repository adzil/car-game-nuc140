#include <inttypes.h> 

#define LCD_Xmax 128
#define LCD_Ymax 64
#define FG_COLOR 0xFFFF
#define BG_COLOR 0x0000

extern void init_LCD(void);

extern void clear_LCD(void);

extern void printC(int16_t x, int16_t y, unsigned char ascii_code);

extern void printC_5x7(int16_t x, int16_t y, unsigned char ascii_code);

extern void print_Line(int8_t line, char text[]);

extern void printS(int16_t x, int16_t y, char text[]);

extern void printS_5x7(int16_t x, int16_t y, char text[]);

extern void draw_Rect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color);
extern void draw_Recti(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color);
void draw_Image(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t *buffer);
void draw_Imagei(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t *buffer);

extern void draw_Pixel(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor);

extern void draw_LCD(unsigned char *buffer);

void create_Pixel(int16_t x, int16_t y, uint8_t color);
void commit_Pixel(int16_t x, int16_t y, int16_t xf, int16_t yf);


