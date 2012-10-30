#ifndef _TEXTURES_H
#define _TEXTURES_H

#include<nds.h>
#include "f32.h"


struct TextureEntry
{
	int format, pal_addr;
	u32 size;
};

#define TEX_BALL 1
#define TEX_VERTGRAD 2
#define TEX_DIAMOND 3
#define TEX_TEXT1 4
#define TEX_TEXT2 5
#define TEX_INVBALL 6
#define TEX_SOLID 7
#define TEX_ICE 8
#define TEX_CIRCLE 9
#define TEX_ICECRYSTAL 10
#define TEX_SWITCH 11
#define TEX_IRON 12


void renderChar(char c, f32 x, f32 y, f32 sz);
void setTexture(int nTexture);
void setNoTexture();

void loadTextures();

#endif