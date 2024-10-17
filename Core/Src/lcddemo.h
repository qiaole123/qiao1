
#ifndef __LCDDEMO_H__
#define __LCDDEMO_H__

#define BLUE (0x1f)
#define GREEN (63<<5)
#define RED  (31<<11)
#define KEY_UP 100
#define KEY_DOWN 101
#define KEY_LEFT 102
#define KEY_RIGHT 103
#define KEY_PUSH 104
extern void lcdInit(void);

extern void Gui_DrawPoint(unsigned short  x,unsigned short y,unsigned short Data);
	
extern void Lcd_Clear(unsigned short Color);      // ÓÃÑÕÉ«Color Ìî³äÆÁÄ» 
extern const unsigned char Zk_ASCII8X16[];
extern void  dispAscii(uint16_t x, uint16_t y, char  chr, uint16_t bc, uint16_t fc);
extern const unsigned char gImage_phone[];

extern void Lcd_DispBmp(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const unsigned char  *p);
extern void lcdDispStr(uint16_t x, uint16_t y,char *str,uint16_t bc, uint16_t fc);
extern void lcdyemian();
extern void lcdcaidan();
extern void menu();
extern int getKey(unsigned int val);
extern void dht11get();
#endif
