#include <nds.h>

u16 hdmaArray[SCREEN_HEIGHT+1];


int hdmaFrame = 0;

void hdma_start()
{
	
//	hdmaFrame++;
//	hdmaFrame%= 2;
	
	BG_PALETTE[0] = RGB15(7, 8, 14);

	WIN0_Y0 = 0;
	WIN0_Y1 = 192;
	WIN_IN = 0xFFFF;
	WIN_OUT = 0x0000;
	REG_DISPCNT |= DISPLAY_WIN0_ON;
	DMA_CR(0) = 0;
	DMA_SRC(0) = (vu32)hdmaArray;
	DMA_DEST(0) = (vu32)(&WIN0_X1);
	DMA_CR(0) = DMA_START_HBL | DMA_REPEAT | DMA_ENABLE |DMA_DST_RESET | 1;
	
}

void hdma_transition(int t)
{
	for(int i = 0; i <= SCREEN_HEIGHT/2; i++)
	{
		int left = t-i;
		if(left < 0) left = 0;
		if(left > 128) left = 128;
		if(left == 128)
			hdmaArray[i] = hdmaArray[SCREEN_HEIGHT-i] = 0;
		else
			hdmaArray[i] = hdmaArray[SCREEN_HEIGHT-i] = (left << 8) | (255-left);
	}
}