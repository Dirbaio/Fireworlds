#include<nds.h>
#include<map>
#include "introtext.h"

using namespace std;


map<char,int> anchos;

void initSub()
{
	anchos['A'] = 12;
	anchos['B'] = 10;
	anchos['C'] = 12;
	anchos['D'] = 12;
	anchos['E'] = 10;
	anchos['F'] = 11;
	anchos['G'] = 12;
	anchos['H'] = 12;
	anchos['I'] = 7;
	anchos['J'] = 10;
	anchos['K'] = 10;
	anchos['L'] = 10;
	anchos['M'] = 16;
	anchos['N'] = 14;
	anchos['O'] = 14;
	anchos['P'] = 10;
	anchos['Q'] = 14;
	anchos['R'] = 10;
	anchos['S'] = 12;
	anchos['T'] = 11;
	anchos['U'] = 11;
	anchos['V'] = 11;
	anchos['W'] = 11;
	anchos['X'] = 14;
	anchos['Y'] = 12;
	anchos['Z'] = 12;
	anchos['0'] = 9;
	anchos['1'] = 9;
	anchos['2'] = 11;
	anchos['3'] = 11;
	anchos['4'] = 11;
	anchos['5'] = 11;
	anchos['6'] = 11;
	anchos['7'] = 12;
	anchos['8'] = 11;
	anchos['9'] = 11;
	anchos['.'] = 7;
	anchos[','] = 7;
	anchos[' '] = 7;
	anchos['!'] = 7;
	anchos[':'] = 7;
}


int oamId = 0, oamIdSub = 0;

void subFrameStart()
{
	oamId = 20;
	oamIdSub = 24;
}

void renderSpriteSub(int x, int y, int t)
{
	oamSet(&oamSub, oamIdSub, x, y, 0, 0, SpriteSize_16x16, SpriteColorFormat_256Color,
	oamGetGfxPtr(&oamSub, t)	, -1, false, false, false, false, false);
//
	oamIdSub++;
}

void subClear()
{
	u16* ptr = (u16*)(0x06610000);
	for(int i = 0; i < 256*192/2; i++)
		ptr[i] = 0;
}

void subSetPixel(int x, int y, int shit)
{
	//Complicated tiling stuff.
	//Mother of god!

	u16* ptr = (u16*)(0x06610000);

	int bx = x / 64;
	int by = y / 64;
	int b = bx + by*4;
	ptr += b*8*8*64/2;

	x %= 64;
	y %= 64;
	int tx = x/8;
	int ty = y/8;
	int t = tx+ty*8;
	ptr += t*8*8/2;

	int px = x%8 + (y%8) * 8;
	ptr += px/2;

	shit &= 0xF;

	u16 v = *ptr;
	if(px % 2 == 0)
		v = (v & 0xFF00) | shit;
	else
		v = (v & 0x00FF) | shit<<8;

	*ptr = v;
}

void renderBg()
{
	//God, how I hate sprites!!
	for(int i = 0; i < 4*3; i++)
		oamSet(&oamSub, i, 64*(i%4), 64*(i/4), 3, 1, SpriteSize_64x64, SpriteColorFormat_256Color, (void*)(0x06604000 + i*8*8*64), -1, false, false, false, false, false);
	for(int i = 0; i < 4*3; i++)
		oamSet(&oamSub, i+12, 64*(i%4), 64*(i/4), 2, 2, SpriteSize_64x64, SpriteColorFormat_256Color, (void*)(0x06610000 + i*8*8*64) , -1, false, false, false, false, false);
}

void subRenderObjChar(int x, int y, char c)
{
	int num = -1;
	if(c >= 'A' && c <= 'Z')
		num = c - 'A';
	if(c >= '0' && c <= '9')
		num = c - '0' + 32;
	if(c >= 'a' && c <= 'z')
		num = c - 'a';
	if(c == '!') num = 42;
	if(c == '.') num = 43;
	if(c == ',') num = 44;
	if(c == ':') num = 45;

	if(num != -1)
		renderSpriteSub(x, y, num);
}

char toupper(char c)
{
	if(c <= 'z' && c >= 'a')
		return c - 'a' + 'A';
	return c;
}


int charWidth(char c)
{
	c = toupper(c);
	return anchos[c];
}

void subRenderObjString(int x, int y, const char* str)
{
	int i = 0;
	while(str[i] != 0)
	{
		subRenderObjChar(x, y, str[i]);
		x+= anchos[toupper(str[i])];
		i++;
	}
}

void subRenderObjNum(int x, int y, int n)
{
	char c;
	if (n == 0) subRenderObjChar(x, y, '0');
	while (n != 0){
		c = n%10+'0';
		n/=10;
		subRenderObjChar(x, y, c);
		x-=12;
	}
}

void subRenderPixelChar(int xp, int yp, char c)
{
	c = toupper(c);
	u8* text = (u8*) introtextTiles;
    text += c * 8 * 8;
	for(int x = 0; x < 8; x++)
		for(int y = 0; y < 8; y++)
			if(text[x + y*8] == 0x01)
			{
				subSetPixel(xp+x*2, yp+y*2, 1);
				subSetPixel(xp+x*2, yp+y*2+1, 1);
				subSetPixel(xp+x*2+1, yp+y*2+1, 1);
				subSetPixel(xp+x*2+1, yp+y*2, 1);
			}
}

void subRenderPixelString(int xp, int yp, const char* str)
{
	for(int i = 0; str[i] != 0; i++)
	{
		if(str[i] == 'M' || str[i]=='m' || str[i]=='W'|| str[i]=='w')
			xp += 2;

		subRenderPixelChar(xp, yp, str[i]);
		xp += 11;
	}
}
