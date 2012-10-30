#include "SeaActor.h"
#include "textures.h"
#include "perlin.h"
#include <nds.h>
#include "polyids.h"

SeaActor::SeaActor(Scene* world) : Actor(world)
{
}

void SeaActor::tick()
{
}

void SeaActor::render()
{
}

int ydelta;

inline int SeaActor::gety(int x, int z)
{
	return (sinLerp(x*5000+sc->time*400)+sinLerp(x*2415-sc->time*300)+sinLerp(z*3000+sc->time*100))/15+ydelta;
}

inline int SeaActor::getc(int x, int z)
{
	return ((sinLerp(x*4000+sc->time*100)+sinLerp(x*2555-sc->time*140)+sinLerp(z*2000+sc->time*700))>>7)+100;
}

void SeaActor::renderPerspective()
{


	glPolyFmt(POLY_ALPHA(20) | POLY_ID(BG_FX1_POLYID) | POLY_CULL_NONE);
	glColor3b(10, 50, 255);

	setTexture(13);

	ydelta = 0;
	renderSea(-6, 6, -10, -5, 1, false);
	renderSea(-10, 10, -5, -2, 1, true);
	renderSea(-18, 18, -2, 5, 2, true);
	renderSea(-20, 22, 6, 12, 4, true);
	
}

void SeaActor::renderSea(int xMin, int xMax, int zMin, int zMax, int inc, bool trick)
{
	int s = 900;

	int vel = 7;
	int zstart = sc->time / vel + zMin; //6 quad/s
	int zend= sc->time / vel + zMax; //6 quad/s
	int ofs = sc->time * s / vel;

	int r = 80;
	int b = 240;
	for(int z = zstart; z < zend; z+=inc)
	{
		glBegin(GL_QUAD_STRIP);
		int old1 = gety(xMin, z);
		int old2 = gety(xMin, z+inc);
		int oldc1 = getc(xMin, z);
		int oldc2 = getc(xMin, z+inc);

		GFX_TEX_COORD = TEXTURE_PACK(inttot16(64*inc), inttot16(0));
		glColor3b(r, oldc1, b);
		glVertex3v16((xMin+0)*s, old1, -(z+0)*s+ofs);
		GFX_TEX_COORD = TEXTURE_PACK(inttot16(64*inc), inttot16(64*inc));
		glColor3b(r, oldc2, b);
		glVertex3v16((xMin+0)*s, old2, -(z+inc)*s+ofs);
		bool tex = false;
		for(int x = xMin; x < xMax; x+=inc)
		{
			int new1 = gety(x+inc, z);
			int new2;
			
			if(trick && z == zend-1 && (x/inc)%2 == 0)
				new2 = (old2+gety(x+inc*2, z+inc))/2;
			else
				new2 = gety(x+inc, z+inc);
			int newc1 = getc(x+inc, z);
			int newc2 = getc(x+inc, z+inc);
			
			GFX_TEX_COORD = TEXTURE_PACK(inttot16(64*tex*inc), inttot16(0));
			glColor3b(r, newc1, b);
			glVertex3v16((x+inc)*s, new1, -(z+0)*s+ofs);		
			GFX_TEX_COORD = TEXTURE_PACK(inttot16(64*tex*inc), inttot16(64*inc));
			glColor3b(r, newc2, b);
			glVertex3v16((x+inc)*s, new2, -(z+inc)*s+ofs);

			tex = !tex;
			old1 = new1;
			old2 = new2;
			oldc1 = newc1;
			oldc2 = newc2;
		}
		glEnd();
	}

}