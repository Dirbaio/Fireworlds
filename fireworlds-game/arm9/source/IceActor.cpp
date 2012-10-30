#include "IceActor.h"
#include "stdio.h"

#include "music.h"
#include "textures.h"
#include "polyids.h"
#include "PlayerActor.h"


IceActor::IceActor(Scene* world) : Actor(world)
{
	size = 32;
	touchedTime = -1;

	collisionType = COLLISION_SQUARE;
	collisionOnlyPlayer = false;
	bottomCollision = true;
	collisionPriority = 10; //This means that ice blocks can push the player, and the player cant push ice blocks
}

void IceActor::tick()
{
	if(touchedTime != -1)
	{
		touchedTime++;
		size = 32-touchedTime/3;
		if(size < 0) size = 0;
		
		if(size < 4)
		{
			dead = true;
			for(int ang = 0; ang < DEGREES_IN_CIRCLE; ang+=DEGREES_IN_CIRCLE / 15)
			{
				Particle *p2 = sc->addParticle60();
				p2->vx = vx.tof5()+sinLerp(ang)/30+irand(10);
				p2->vy = vy.tof5()+cosLerp(ang)/30+irand(10);

				p2->r = 220;
				p2->g = 240;
				p2->b = 255;
				p2->a = 21;
				p2->x = x.tof5();
				p2->y = y.tof5();

				p2->nTexture = 10;
				p2->sizePerLife = 7;
				p2->effect = FX_NONE;	
			}
		}
	}
	
	if(size > 32) size = 32;

	
	if(size == 32 )
	{
		if((rand() % 60) == 0)
		{
			Particle* p2 = sc->addParticle180();
			p2->r = 220;
			p2->g = 240;
			p2->b = 255;
			p2->a = 25;
			p2->x = (x+frand(size*2)).tof5();
			p2->y = (y+frand(size*2)).tof5();

			p2->vx = (vx+frand(1)).tof5();
			p2->vy = (vy+frand(1)).tof5();

			p2->nTexture = 10;
			p2->sizePerLife = -16;
			p2->effect = FX_ALPHA2;
		}
	}
	else if (size != 0 && sc->time % 2 == 0)
	{
		Particle* p2 = sc->addParticle60();
		p2->r = 220;
		p2->g = 240;
		p2->b = 255;
		p2->a = 21;
		p2->x = x.tof5();
		p2->y = y.tof5();

		p2->vx = (vx+frand(4)).tof5();
		p2->vy = (vy+frand(4)).tof5();

		p2->nTexture = 10;
		p2->sizePerLife = 7;
		p2->effect = FX_NONE;		
	}
	
	calcGravityVec();
	vy += gvy*gvs;
	vx += gvx*gvs;
	vy *= f32(0.97);
	
	if(inWater) vy += 0.4;
	
	if(inLava && touchedTime == -1)
	{
		touchedTime = 1;
		vy = 6;
	}

	move(vx, vy);
	
	Actor::tick();
}


void IceActor::render()
{
	if(!onScreen()) return;
	if(size == 0) return;
	
	f32 xcam = sc->xCam;
	f32 ycam = sc->yCam;
	
	setTexture(TEX_ICE);
	glPolyFmt(POLY_ALPHA(31) | POLY_ID(OBJ1_POLYID) | POLY_CULL_NONE);

	//draw the obj
	glColor3b(70+sinLerp(sc->time*400)/64, 180, 255);
	glBegin(GL_QUAD);

	GFX_TEX_COORD = TEXTURE_PACK(inttot16(64), inttot16(0));

	int r = 0;
	if(touchedTime != -1) r = 4;
	
	glVertex3f32(x-size-xcam+frand(r), y-size-ycam+frand(r), 0);
	GFX_TEX_COORD = TEXTURE_PACK(inttot16(64), inttot16(64));
	glVertex3f32(x-size-xcam+frand(r), y+size-ycam+frand(r), 0);
	GFX_TEX_COORD = TEXTURE_PACK(inttot16(0), inttot16(64));
	glVertex3f32(x+size-xcam+frand(r), y+size-ycam+frand(r), 0);
	GFX_TEX_COORD = TEXTURE_PACK(inttot16(0), inttot16(0));
	glVertex3f32(x+size-xcam+frand(r), y-size-ycam+frand(r), 0);
	
	glEnd();
}


void IceActor::collidedWithPlayer(PlayerActor* act)
{
	if(touchedTime == -1)
		touchedTime = 1;
}
