#ifndef LCD_H__    /* Guard against multiple inclusion */
#define LCD_H__

#define COLORON 0x0020  //Blue?
#define COLOROFF 0xDDDD  //Slightly off-white
#define COLORBAR 0xEEEE
#define COLORHIGH 0xAAAA
#define MAX_X 128
#define MAX_Y 160
#define MAX_STRLEN 50

void draw_loading_bar(unsigned short, unsigned short, unsigned int);
void draw_string(unsigned short, unsigned short, char*);
void draw_char(unsigned short, unsigned short, char);

#endif
