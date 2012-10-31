#include "Particle.h"
#include "textures.h"
#include "music.h"


extern Scene* sc;

Particle::Particle()
{
}

inline f32 tof32(int f5)
{
	f32 r = f5;
	r /= 32;
	return r;
}

void Particle::renderBlur()
{
	f32 vm = fsqrt((tof32(vx)-sc->vxCam)*(tof32(vx)-sc->vxCam) + (tof32(vy)-sc->vyCam)*(tof32(vy)-sc->vyCam));
	
	f32 size = 4; 
	f32 nvx = ((tof32(vx)-sc->vxCam)/ vm)*size;
	f32 nvy = ((tof32(vy)-sc->vyCam)/ vm)*size;

	int d = 2;
	f32 mvx = (tof32(vx)-sc->vxCam)*d+nvx;
	f32 mvy = (tof32(vy)-sc->vyCam)*d+nvy;
	
	glBegin(GL_QUAD);

		glColor3b(r,g,b);
		GFX_TEX_COORD = TEXTURE_PACK(inttot16(64), inttot16(0));
		glVertex3f32(tof32(x)+mvx-nvy-sc->xCam, tof32(y)+mvy+nvx-sc->yCam, 0);
		GFX_TEX_COORD = TEXTURE_PACK(inttot16(64), inttot16(64));
		glVertex3f32(tof32(x)+mvx+nvy-sc->xCam, tof32(y)+mvy-nvx-sc->yCam, 0);
		GFX_TEX_COORD = TEXTURE_PACK(inttot16(0), inttot16(64));
		glVertex3f32(tof32(x)-mvx+nvy-sc->xCam, tof32(y)-mvy-nvx-sc->yCam, 0);
		GFX_TEX_COORD = TEXTURE_PACK(inttot16(0), inttot16(0));
		glVertex3f32(tof32(x)-mvx-nvy-sc->xCam, tof32(y)-mvy+nvx-sc->yCam, 0);

	glEnd();
}

/*
void Particle::render(int z, Scene* sc)
{
	
	setTexture(nTexture);
	int xcam = sc->xCam.tof5();
	int ycam = sc->yCam.tof5();
	
	glPolyFmt(POLY_ALPHA(a) | POLY_ID(z % 60) | POLY_CULL_NONE);
	//draw the obj
	
	
	int size;
	if(sizePerLife < 0)
		size = -sizePerLife*32;
	else
		size = life*sizePerLife;
	
	if(effect == FX_ALPHA)
		a = life/64+2;
	
	if(effect == FX_BLUR)
	{
	}
	else if(type == 5)
	{
			glColor3b(r,g,b);

		for(int i = 0; txt[i] != 0; i++)
			renderChar(txt[i], x-xcam+size*i*2, y+sinLerp(sc->time*400+i*8000)/30-ycam, size);
	}
	else
	{
		glBegin(GL_QUAD);

			glColor3b(r,g,b);
			GFX_TEX_COORD = TEXTURE_PACK(inttot16(64), inttot16(0));
			glVertex3v16(x-size-xcam, y-size-ycam, 0);
			GFX_TEX_COORD = TEXTURE_PACK(inttot16(64), inttot16(64));
			glVertex3v16(x+size-xcam, y-size-ycam, 0);
			GFX_TEX_COORD = TEXTURE_PACK(inttot16(0), inttot16(64));
			glVertex3v16(x+size-xcam, y+size-ycam, 0);
			GFX_TEX_COORD = TEXTURE_PACK(inttot16(0), inttot16(0));
			glVertex3v16(x-size-xcam, y+size-ycam, 0);

		glEnd();
	}
}*/

void Particle::move(Scene* sc)
{
/*	
	//a = 30;
	
	if(type != 0)
	{
		x += vx;
		y += vy;
		if(type<4)
		{
			vx -= (vx+8)/16;
			vy -= (vy+8)/16;
		}
	}
	
	life--;
	
	//vy -= 1;
	switch(type)
	{
		case 0:
		{
			break;
		}
		case 1:
			r = 150;
			g = 100;
			b = 255;
			size = 80+irand(20);
			vx += vx2>>4;
			vy += vy2>>4;
			a = life*2+1;
			break;
		case 2:
			r += 3;
			if(r > 255) r = 255;
			g -= 4;
			if(g < 0) g = 0;
			b -= 1;
			if(b < 0) b = 0;

			size = 5*life;
			a = life+1;
			if(a > 31) a = 31;
			break;
		case 3:
			r = life/2+80;
			g = life/2+80;
			b = life/2+80;
			a = life/50+1;
			size = 500;
			
			break;
		case 4:
			a = life*2+1;
			if(a > 31) a = 31;
			break;
	}*/
	
}

bool Particle::out(int xc, int yc)
{
//	if(type == 0) return false;
	
	if(x < xc-150*32) return true;
	if(x > xc+150*32) return true;
	if(y < yc-150*32) return true;
	if(y > yc+150*32) return true;
	return false;
}
