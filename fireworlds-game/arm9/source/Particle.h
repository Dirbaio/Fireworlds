
#ifndef _PARTICLE_H
#define _PARTICLE_H

class Particle;

#include<nds.h>
class Scene;

#define FX_NONE 0
#define FX_BLUR 1
#define FX_RED 2
#define FX_ALPHA 3
#define FX_NOFRICTION 4
#define FX_ALPHA2 5

class Particle
{
	public:
	int x; //0x0
	int y; //0x4
	int vx; //0x8
	int vy; //0xC
	int vx2; //0x10
	int vy2; //0x14
	s8 r, g, b, a; //0x18
	s8 sizePerLife; //0x1C
	u8 effect;		//0x1D
	u8 empty;		//0x1E
	u8 nTexture;	//0x1F
	u16 life;		//0x20
	u16 empty2;		//0x22
	//0x24
	
	Particle();
	
	void renderBlur(Scene* sc);
	void move(Scene* sc);
	
	bool out(int x, int y);
};

#include "Scene.h"

#endif