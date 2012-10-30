#include<nds.h>
#include "motionblur.h"
#include "polyids.h"
#include "textures.h"

using namespace std;

int blurBank = 0;
int darkenTime = 0;
void motionBlurFrame()
{

	if(blurBank == 1)
	{
		vramSetBankB(VRAM_B_TEXTURE_SLOT1);
		vramSetBankC(VRAM_C_LCD);
	}
	else
	{
		vramSetBankB(VRAM_B_LCD);
		vramSetBankC(VRAM_C_TEXTURE_SLOT1);
	}

	REG_DISPCAPCNT = 
			DCAP_MODE(DCAP_MODE_A) //blend source A and source B
		//|	DCAP_SRC_ADDR //this is not used since we are setting the display to render from VRAM
		|	DCAP_SRC_A(DCAP_SRC_A_3DONLY)
		|	DCAP_SIZE(DCAP_SIZE_256x192)
		|	DCAP_OFFSET(0) //where to write the captured data within our chosen VRAM bank
		|	DCAP_BANK(blurBank+1)
		;
		
	blurBank++;
	blurBank %= 2;

	if(darkenTime == 0) darkenTime = 2;
	darkenTime--;
}

void renderQuad()
{
	int zoom = 0;
//	if(darkenTime == 0)
		glColor3b(190, 190, 190);
//	else
//		glColor3b(255, 255, 255);
	
	glBegin(GL_QUAD);
	GFX_TEX_COORD = TEXTURE_PACK(inttot16(0), inttot16(0));
	glVertex3v16((-1<<12)-zoom, (1<<12)+zoom,0);
	GFX_TEX_COORD = TEXTURE_PACK(inttot16(0), inttot16(192));
	glVertex3v16((-1<<12)-zoom, (-1<<12)-zoom,0);
	GFX_TEX_COORD = TEXTURE_PACK(inttot16(256), inttot16(192));
	glVertex3v16((1<<12)+zoom, (-1<<12)-zoom,0);
	GFX_TEX_COORD = TEXTURE_PACK(inttot16(256), inttot16(0));
	glVertex3v16((1<<12)+zoom, (1<<12)+zoom,0);
}

void motionBlurRender()
{

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);

	setNoTexture();
	GFX_TEX_FORMAT = 0x4000 | 5<<20 | 5<<23 | 7<<26;
	glPolyFmt(POLY_ALPHA(28) | POLY_ID(MOTIONBLUR_POLYID) | POLY_CULL_NONE);
	renderQuad();
	setNoTexture();
}
