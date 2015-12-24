#include "stdio.h"
#include "string.h"
#include "NUC100Series.h"
#include "SYS.h"
#include "SPI.h"
#include "GPIO.h"	
#include "LCD.h"
#include "Font5x7.h"
#include "Font8x16.h"

extern SPI_T * SPI_PORT[4]={SPI0, SPI1, SPI2, SPI3};

char DisplayBuffer[128*8];

void init_SPI3(void)
{
	DrvGPIO_InitFunction(E_FUNC_SPI3);
	/* Configure SPI3 as a master, Type1 waveform, 32-bit transaction */
	DrvSPI_Open(eDRVSPI_PORT3, eDRVSPI_MASTER, eDRVSPI_TYPE1, 9);
	/* MSB first */
	DrvSPI_SetEndian(eDRVSPI_PORT3, eDRVSPI_MSB_FIRST);
	/* Disable the automatic slave select function of SS0. */
	DrvSPI_DisableAutoSS(eDRVSPI_PORT3);
	/* Set the active level of slave select. */
	DrvSPI_SetSlaveSelectActiveLevel(eDRVSPI_PORT3, eDRVSPI_ACTIVE_LOW_FALLING);
	/* SPI clock rate 1MHz */
	DrvSPI_SetClockFreq(eDRVSPI_PORT3, 25000000, 0);
}

void lcdWriteCommand(unsigned char temp)
{
 	// Write Data
	SPI_PORT[eDRVSPI_PORT3]->SSR.SSR=1;	      //chip select
	SPI_PORT[eDRVSPI_PORT3]->TX[0]=temp;    	//write command
	SPI_PORT[eDRVSPI_PORT3]->CNTRL.GO_BUSY = 1;
  while ( SPI_PORT[eDRVSPI_PORT3]->CNTRL.GO_BUSY == 1 ); //check data out?
	SPI_PORT[eDRVSPI_PORT3]->SSR.SSR=0;	
	while(DrvSPI_IsBusy(eDRVSPI_PORT3) != 0); 
}

// Wrtie data to LCD 
void lcdWriteData(unsigned char temp)
{
	// Write Data
	SPI_PORT[eDRVSPI_PORT3]->SSR.SSR=1;	   //chip select
	SPI_PORT[eDRVSPI_PORT3]->TX[0] =0x100 | temp;    	//write data
	SPI_PORT[eDRVSPI_PORT3]->CNTRL.GO_BUSY = 1;
  while ( SPI_PORT[eDRVSPI_PORT3]->CNTRL.GO_BUSY == 1 ); //check data out?
	SPI_PORT[eDRVSPI_PORT3]->SSR.SSR=0;
}

// Set Address to LCD
void lcdSetAddr(unsigned char PAs, unsigned char CA)
{
	// Set PA

	SPI_PORT[eDRVSPI_PORT3]->SSR.SSR=1; 	
	SPI_PORT[eDRVSPI_PORT3]->TX[0] = 0xB0 | PAs;	
	SPI_PORT[eDRVSPI_PORT3]->CNTRL.GO_BUSY = 1;
  while ( SPI_PORT[eDRVSPI_PORT3]->CNTRL.GO_BUSY == 1 );	 //check data out?
	// Set CA MSB
	SPI_PORT[eDRVSPI_PORT3]->SSR.SSR=0;

	SPI_PORT[eDRVSPI_PORT3]->SSR.SSR=1;	
	SPI_PORT[eDRVSPI_PORT3]->TX[0] =0x10 |((CA>>4)&0xF);
	SPI_PORT[eDRVSPI_PORT3]->CNTRL.GO_BUSY = 1;
  while ( SPI_PORT[eDRVSPI_PORT3]->CNTRL.GO_BUSY == 1 );	  //check data out?
 	// Set CA LSB
	SPI_PORT[eDRVSPI_PORT3]->SSR.SSR=0;

	SPI_PORT[eDRVSPI_PORT3]->SSR.SSR=1;
	SPI_PORT[eDRVSPI_PORT3]->TX[0] =0x00 | (CA & 0xF);		
	SPI_PORT[eDRVSPI_PORT3]->CNTRL.GO_BUSY = 1;
  while ( SPI_PORT[eDRVSPI_PORT3]->CNTRL.GO_BUSY == 1 );	  //check data out?
	SPI_PORT[eDRVSPI_PORT3]->SSR.SSR=0;
}

void init_LCD(void)
{
	init_SPI3();
	lcdWriteCommand(0xEB); 
	lcdWriteCommand(0x81); 
	lcdWriteCommand(0xA0);  
	lcdWriteCommand(0xC0);  
	lcdWriteCommand(0xAF); // Set Display Enable 
}

void clear_LCD(void)
{
	int16_t i,j;
	for (j=0;j<LCD_Ymax;j++)
	  for (i=0;i<LCD_Xmax;i++)
	     DisplayBuffer[i+j/8*LCD_Xmax]=0;
	
	lcdSetAddr(0x0, 0x0);			  								  
	for (i = 0; i < 132 *8; i++)
	{
		lcdWriteData(0x00);
	}
	lcdWriteData(0x0f);
}

void printC(int16_t x, int16_t y, unsigned char ascii_code)
{
  int8_t i;
  unsigned char temp;	    
  for(i=0;i<8;i++) {
	lcdSetAddr((y/8),(LCD_Xmax+1-x-i));
	temp=Font8x16[(ascii_code-0x20)*16+i];	 
	lcdWriteData(temp);
    }

  for(i=0;i<8;i++) {
	lcdSetAddr((y/8)+1,(LCD_Xmax+1-x-i));	 
	temp=Font8x16[(ascii_code-0x20)*16+i+8];
	lcdWriteData(temp);
    }
}

// print char function using Font5x7
void printC_5x7 (int16_t x, int16_t y, unsigned char ascii_code) 
{
	uint8_t i;
	if (x<(LCD_Xmax-5) && y<(LCD_Ymax-7)) {
	   if      (ascii_code<0x20) ascii_code=0x20;
     else if (ascii_code>0x7F) ascii_code=0x20;
	   else           ascii_code=ascii_code-0x20;
	   for (i=0;i<5;i++) {
			  lcdSetAddr((y/8),(LCD_Xmax+1-x-i)); 
        lcdWriteData(Font5x7[ascii_code*5+i]);
		 }
	}
}

void print_Line(int8_t line, char text[])
{
	int8_t i;
	for (i=0;i<strlen(text);i++) 
		printC(i*8,line*16,text[i]);
}

void printS(int16_t x, int16_t y, char text[])
{
	int8_t i;
	for (i=0;i<strlen(text);i++) 
		printC(x+i*8, y,text[i]);
}

void printS_5x7(int16_t x, int16_t y, char text[])
{
	uint8_t i;
	for (i=0;i<strlen(text);i++) {
		printC_5x7(x,y,text[i]);
	  x=x+5;
	}
}

void draw_Rect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color) {
	uint8_t xi, yi, xf, yf;
    
    xf = x + w;
    yf = y + h;
    
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (xf > LCD_Xmax) xf = LCD_Xmax;
    if (yf > LCD_Ymax) yf = LCD_Ymax;
    
    for (yi = y; yi < yf; yi++)
        for (xi = x; xi < xf; xi++)
            create_Pixel(xi, yi, color);
    
    commit_Pixel(x, y, xf, yf);
}

void draw_Recti(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color) {
	uint8_t xi, yi, xf, yf;
    
    xf = x + w + 1;
    yf = y + h;
    
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (xf > LCD_Xmax) xf = LCD_Xmax;
    if (yf > LCD_Ymax) yf = LCD_Ymax;
    
    for (yi = y; yi < yf; yi++)
        for (xi = x; xi < xf; xi++)
            if (xi == xf - 1)
                create_Pixel(xi, yi, 0);
            else
                create_Pixel(xi, yi, color);
    
    commit_Pixel(x, y, xf, yf);
}

void draw_Image(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t *buffer) {
    int16_t xi, yi, xf, yf, ctr, bufctr;
    
    xf = x + w;
    yf = y + h;
    ctr = 0;
    bufctr = 0;
    
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (xf > LCD_Xmax) xf = LCD_Xmax;
    if (yf > LCD_Ymax) yf = LCD_Ymax;
    
    for (yi = y; yi < yf; yi++) {
        for (xi = x; xi < xf; xi++) {
            create_Pixel(xi, yi, (buffer[bufctr] >> ctr++) & 1);
            if (ctr >= 8) {
                ctr = 0;
                bufctr++;
            }
        }
    }
    
    commit_Pixel(x, y, xf, yf);
}

void draw_Imagei(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t *buffer) {
    int16_t xi, yi, xf, yf, ctr, bufctr;
    
    xf = x + w + 1;
    yf = y + h;
    ctr = 0;
    bufctr = 0;
    
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (xf > LCD_Xmax) xf = LCD_Xmax;
    if (yf > LCD_Ymax) yf = LCD_Ymax;
    
    for (yi = y; yi < yf; yi++) {
        for (xi = x; xi < xf; xi++) {
            if (xi == xf - 1) {
                create_Pixel(xi, yi, 0);
            } else {
                create_Pixel(xi, yi, (buffer[bufctr] >> ctr++) & 1);
                if (ctr >= 8) {
                    ctr = 0;
                    bufctr++;
                }
            }
        }
    }
    
    commit_Pixel(x, y, xf, yf);
}

void create_Pixel(int16_t x, int16_t y, uint8_t color) {
    int16_t pos;
    
    pos = y & 7;
    y = y >> 3;
    
    if (color!=0) 
		DisplayBuffer[x+y*LCD_Xmax] |= (1<<pos);
	else 
		DisplayBuffer[x+y*LCD_Xmax] &= ~(1<<pos);
}

void commit_Pixel(int16_t x, int16_t y, int16_t xf, int16_t yf) {
    int16_t xi, yi;
    
    y = y >> 3;
    yf = yf >> 3;
    
    for (xi = x; xi <= xf; xi++) {
        for (yi = y; yi <= yf; yi++) {
            lcdSetAddr(yi,(LCD_Xmax+1-xi));
            lcdWriteData(DisplayBuffer[xi+yi*LCD_Xmax]);
        }
    }
}

void draw_Pixel(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor)
{
	if (fgColor!=0) 
		DisplayBuffer[x+y/8*LCD_Xmax] |= (0x01<<(y%8));
	else 
		DisplayBuffer[x+y/8*LCD_Xmax] &= (0xFE<<(y%8));

	lcdSetAddr(y/8,(LCD_Xmax+1-x));
	lcdWriteData(DisplayBuffer[x+y/8*LCD_Xmax]);
}

void draw_LCD(unsigned char *buffer)
{
  uint8_t x,y;
    for (x=0; x<LCD_Xmax; x++) {
        for (y=0; y<(LCD_Ymax/8); y++) {
           lcdSetAddr(y,(LCD_Xmax+1-x));				
           lcdWriteData(buffer[y+x*(LCD_Ymax/8)]);
        }
    }			
}
