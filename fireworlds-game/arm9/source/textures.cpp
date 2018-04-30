#include "textures.h"
#include "introtext.h"
#include "texs.h"
#include <stdio.h>

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

    int texIndex = 64*1024/8 + c * 8;
    GFX_TEX_FORMAT = (TEXTURE_SIZE_8 << 20) | (TEXTURE_SIZE_8 << 23) | (GL_RGB8_A5 << 26) | ( texIndex & 0xFFFF );

    glBegin(GL_QUAD);

    GFX_TEX_COORD = TEXTURE_PACK(inttot16(0), inttot16(8)-1);
    glVertex3f32(x-sz, y-sz, 0);
    GFX_TEX_COORD = TEXTURE_PACK(inttot16(8)-1,inttot16(8)-1);
    glVertex3f32(x+sz, y-sz, 0);
    GFX_TEX_COORD = TEXTURE_PACK(inttot16(8)-1,inttot16(0));
    glVertex3f32(x+sz, y+sz, 0);
    GFX_TEX_COORD = TEXTURE_PACK(inttot16(0),inttot16(0));
    glVertex3f32(x-sz, y+sz, 0);

    glEnd();
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
            a = 0x1F;
            texdata[x+y*64] = (u8) (a)<< 3 | 1;
            texdata[x+y*64] = 31 << 3;
        }

    glBindTexture(0, textureIDS[n]);
    glTexImage2D(0, 0, GL_RGB8_A5, TEXTURE_SIZE_64, TEXTURE_SIZE_64, 0, 0, texdata);

    u16 paldata[16];
    for(int i = 0; i < 16; i++)
        paldata[i] = 0xFFFF;
    glColorTableEXT( 0, 0, 16, 0, 0, paldata );
}

void genTextures()
{
    u16 paldata[16];
    for(int i = 0; i < 16; i++)
        paldata[i] = i%2 ? 0x0000 : 0x001f;

    u8 texdata[64*64];

    for(int x = 0; x < 64; x++)
        for(int y = 0; y < 64; y++)
            texdata[x+y*64] =  1;

    glBindTexture(0, textureIDS[0]);
    glTexImage2D(0, 0, GL_RGB256, TEXTURE_SIZE_64, TEXTURE_SIZE_64, 0, 0, &texdata);
    glColorTableEXT( 0, 0, 16, 0, 0, paldata );
    iprintf("\x1b[0;0HA: %08x", glGetTexturePointer(textureIDS[0]));


    for(int x = 0; x < 64; x++)
        for(int y = 0; y < 64; y++)
            texdata[x+y*64] = 1 + (x+y)%2;
    paldata[0] = 0x7c00;
    paldata[1] = 0x001f;
    paldata[2] = 0x03e0;
    glBindTexture(0, textureIDS[1]);
    glTexImage2D(0, 0, GL_RGB256, TEXTURE_SIZE_64, TEXTURE_SIZE_64, 0, 0, &texdata);
    glColorTableEXT( 0, 0, 16, 0, 0, paldata );
    iprintf("\x1b[0;14HB: %08x", glGetTexturePointer(textureIDS[1]));

}

void genTextures2()
{
//  int pal_addr = gluTexLoadPal( &paldata[0], 8, GL_RGB8_A5 );

    u16 paldata[16];
    for(int i = 0; i < 16; i++)
        paldata[i] = 0xFFFF;

    u8 texdata[64*64];

    for(int x = 0; x < 64; x++)
        for(int y = 0; y < 64; y++)
        {
            texdata[x+y*64] = (u8)  1;
        }

    glBindTexture(0, textureIDS[0]);
    glTexImage2D(0, 0, GL_RGB256, TEXTURE_SIZE_64, TEXTURE_SIZE_64, 0, 0, texdata);
    glColorTableEXT( 0, 0, 16, 0, 0, paldata );


    genTextureFromImg(1, 0);
    genTextureFromImg(2, 1);
    genTextureFromImg(3, 2);


    u8* texdata2 = new u8[256*64];
    u8* text = (u8*) introtextTiles;
    for(int x = 0; x < 256; x++)
        for(int y = 0; y < 64; y++)
        {
            u8 val = 0;
            if(text[x+y*256] == 0x01)
                val = 31<<3;
            texdata2[x+y*256] = val;
        }


    glBindTexture(0, textureIDS[4]);
    glTexImage2D(0, 0, GL_RGB8_A5, TEXTURE_SIZE_256, TEXTURE_SIZE_64, 0, 0, texdata2);
    glColorTableEXT( 0, 0, 16, 0, 0, paldata );

    genTextureFromImg(6, 3);
    genTextureFromImg(8, 4);


    for(int x = 0; x < 64; x++)
        for(int y = 0; y < 64; y++)
            texdata[x+y*64] = (u8) (31)<< 3 | 1;

    glBindTexture(0, textureIDS[7]);
    glTexImage2D(0, 0, GL_RGB8_A5, TEXTURE_SIZE_64, TEXTURE_SIZE_64, 0, 0, texdata);
    glColorTableEXT( 0, 0, 16, 0, 0, paldata );

    for(int x = 0; x < 64; x++)
        for(int y = 0; y < 64; y++)
        {
            int dist = (x-32)*(x-32)+(y-32)*(y-32);
            dist = sqrt32(dist);
//          dist /= 32;
            if(dist>31) dist = 31;
            if(dist<0) dist = 0;
            if(dist !=31)
                texdata[x+y*64] = (u8) 31<< 3 | 1;
            else
                texdata[x+y*64] = (u8) 0;

        }

    glBindTexture(0, textureIDS[9]);
    glTexImage2D(0, 0, GL_RGB8_A5, TEXTURE_SIZE_64, TEXTURE_SIZE_64, 0, TEXGEN_TEXCOORD, texdata);
    glColorTableEXT( 0, 0, 16, 0, 0, paldata );

    genTextureFromImg(10, 5);
    genTextureFromImg(11, 6);
    genTextureFromImg(12, 7);
    genTextureFromImg(13, 8);

    delete[] texdata2;
}

void copy16(void* dest, void* src, int len) {
    uint16 *src16 = (uint16*)src;
    uint16 *dest16 = (uint16*)dest;
    for(int i = 0; i < len/2; i++)
        dest16[i] = src16[i];
}

void setNoTexture()
{
    setTexture(0);
}

void loadTextures()
{
    u8 texdata[64*64];

    vramSetBankA(VRAM_A_LCD);

    for(int i = 0; i < 14; i++) {
        u8* tx = (u8*) texsBitmap;
        tx += i*64*64;

        for(int x = 0; x < 64; x++)
            for(int y = 0; y < 64; y++)
                texdata[x+y*64] = (texsPal[tx[x+y*64]] & 0x1F) << 3;

        copy16(VRAM_A + i*64*64/2, texdata, sizeof(texdata));
    }

    u8* texdata2 = new u8[introtextTilesLen];
    u8* text = (u8*) introtextTiles;
    for(int x = 0; x < introtextTilesLen; x++)
        texdata2[x] = text[x] == 0x01 ? (31 << 3) : 0;
    copy16(VRAM_A + 64*1024/2, texdata2, introtextTilesLen);

    vramSetBankA(VRAM_A_TEXTURE);

    u16 paldata[16];
    for(int i = 0; i < 16; i++)
        paldata[i] = 0xFFFF;

    vramSetBankF(VRAM_F_LCD);
    copy16(VRAM_F, paldata, sizeof(paldata));
    vramSetBankF(VRAM_F_TEX_PALETTE_SLOT0);
}

void setTexture(int nTexture)
{
    int texIndex = nTexture * 64 * 64 / 8;
    GFX_TEX_FORMAT = (TEXTURE_SIZE_64 << 20) | (TEXTURE_SIZE_64 << 23) | (GL_RGB8_A5 << 26) | ( texIndex & 0xFFFF );

    int palIndex = 0;
    GFX_PAL_FORMAT = palIndex;
}
