#include "textures.h"
#include "introtext.h"
#include "texs.h"

int textureIDS[20];

//texture coordinates
const u32 uv[] =
{
	
	TEXTURE_PACK(inttot16(64), 0),
	TEXTURE_PACK(inttot16(64),inttot16(64)),
	TEXTURE_PACK(0, inttot16(64)),
	TEXTURE_PACK(0,0)
};


void renderChar(char c, f32 x, f32 y, f32 sz)
{
	sz += sz/2;
	int xc = (c % 32) * 8;
	int yc = (c / 32) * 8;
	setTexture(4);
	glBegin(GL_QUAD);

		GFX_TEX_COORD = TEXTURE_PACK(inttot16(xc),inttot16(yc+8)-1);
		glVertex3f32(x-sz, y-sz, 0);
		GFX_TEX_COORD = TEXTURE_PACK(inttot16(xc+8)-1,inttot16(yc+8)-1);
		glVertex3f32(x+sz, y-sz, 0);
		GFX_TEX_COORD = TEXTURE_PACK(inttot16(xc+8)-1,inttot16(yc));
		glVertex3f32(x+sz, y+sz, 0);
		GFX_TEX_COORD = TEXTURE_PACK(inttot16(xc),inttot16(yc));
		glVertex3f32(x-sz, y+sz, 0);

	glEnd();
}

int oldTex = -1;
void setTexture(int nTexture)
{
	if(nTexture != oldTex)
		glBindTexture(0, textureIDS[nTexture]);
	oldTex = nTexture;
	
//	if( textures[nTexture].format != GL_RGB )
//		glColorTable(textures[nTexture].format, textures[nTexture].pal_addr);
}

void genTextureFromImg(int n, int offs)
{
	u8* tx = (u8*) texsBitmap;
	tx += offs*64*64;
	
	u8 texdata[64*64];

	for(int x = 0; x < 64; x++)
		for(int y = 0; y < 64; y++)
		{
			int c = texsPal[tx[x+y*64]];			
			int a = c & 0x1F;
			
			texdata[x+y*64] = (u8) (a)<< 3 | 1;
		}

	glBindTexture(n, textureIDS[n]);
	glTexImage2D(0, 0, GL_RGB8_A5, TEXTURE_SIZE_64, TEXTURE_SIZE_64, 0, TEXGEN_TEXCOORD | GL_TEXTURE_WRAP_T | GL_TEXTURE_WRAP_S, &texdata);

	u16 paldata[8];
	for(int i = 0; i <8; i++)
		paldata[i] = 0xFFFF;
	glColorTableEXT( 0, 0, 8, 0, 0, paldata );


}

void genTextures()
{


//	int pal_addr = gluTexLoadPal( &paldata[0], 8, GL_RGB8_A5 );

	u16 paldata[8];
	for(int i = 0; i <8; i++)
		paldata[i] = 0xFFFF;

	u8 texdata[64*64];



	for(int x = 0; x < 64; x++)
		for(int y = 0; y < 64; y++)
		{
			texdata[x+y*64] = (u8)  1;
		}

	glBindTexture(0, textureIDS[0]);
	glTexImage2D(0, 0, GL_RGB256, TEXTURE_SIZE_64, TEXTURE_SIZE_64, 0, TEXGEN_TEXCOORD, &texdata);
	glColorTableEXT( 0, 0, 8, 0, 0, paldata );
	
	
	genTextureFromImg(1, 0);
	genTextureFromImg(2, 1);
	genTextureFromImg(3, 2);


	u8* texdata2 = new u8[256*64];
	u8* text = (u8*) introtextBitmap;
	for(int x = 0; x < 256; x++)
		for(int y = 0; y < 64; y++)
		{
			u8 val = 0;
			if(text[x+y*256] == 0x01)
				val = 31<<3;
			texdata2[x+y*256] = val;
		}
	
	
	glBindTexture(4, textureIDS[4]);
	glTexImage2D(0, 0, GL_RGB8_A5, TEXTURE_SIZE_256, TEXTURE_SIZE_64, 0, TEXGEN_TEXCOORD, texdata2);
	glColorTableEXT( 0, 0, 8, 0, 0, paldata );

	genTextureFromImg(6, 3);
	genTextureFromImg(8, 4);


	for(int x = 0; x < 64; x++)
		for(int y = 0; y < 64; y++)
			texdata[x+y*64] = (u8) (31)<< 3 | 1;

	glBindTexture(7, textureIDS[7]);
	glTexImage2D(0, 0, GL_RGB8_A5, TEXTURE_SIZE_64, TEXTURE_SIZE_64, 0, TEXGEN_TEXCOORD, &texdata);
	glColorTableEXT( 0, 0, 8, 0, 0, paldata );

	for(int x = 0; x < 64; x++)
		for(int y = 0; y < 64; y++)
		{
			int dist = (x-32)*(x-32)+(y-32)*(y-32);
			dist = sqrt32(dist);
//			dist /= 32;
			if(dist>31) dist = 31;
			if(dist<0) dist = 0;
			if(dist !=31)
				texdata[x+y*64] = (u8) 31<< 3 | 1;
			else
				texdata[x+y*64] = (u8) 0;

		}

	glBindTexture(9, textureIDS[9]);
	glTexImage2D(0, 0, GL_RGB8_A5, TEXTURE_SIZE_64, TEXTURE_SIZE_64, 0, TEXGEN_TEXCOORD, &texdata);
	glColorTableEXT( 0, 0, 9, 0, 0, paldata );

	genTextureFromImg(10, 5);
	genTextureFromImg(11, 6);
	genTextureFromImg(12, 7);
	genTextureFromImg(13, 8);
	
	delete[] texdata2;
}


void setNoTexture()
{
	setTexture(0);
}

void loadTextures()
{
	vramSetBankA(VRAM_A_TEXTURE);
	vramSetBankF(VRAM_F_TEX_PALETTE_SLOT0);
	vramSetBankG(VRAM_G_TEX_PALETTE_SLOT5);
	glGenTextures(20, textureIDS);
	genTextures();
}
