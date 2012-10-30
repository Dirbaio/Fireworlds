#include "EndingActor.h"
#include "stdio.h"

#include "music.h"
#include "textures.h"
#include "polyids.h"
#include "PlayerActor.h"


EndingActor::EndingActor(LevelScene* world) : Actor(world)
{
	this->world = world;
	t = 0;
	winTime = 0;
	size = 40;
	collisionType = COLLISION_ROUND;
	collisionOnlyPlayer = true;
	collisionOnlyNotify = true;

}

void EndingActor::tick()
{
	Actor::tick();

	if(t > 0) t--;
	if(winTime > 0) winTime--;
	
	if(musicTrackPlayed[2])
		t = 20;
	
	if(!onScreen()) return;
		
	int ang = (sc->time *(DEGREES_IN_CIRCLE/2 + 700)) % DEGREES_IN_CIRCLE;
	
	int d = ((sinLerp((sc->time * (world->energyCompleted()?140:60)) % DEGREES_IN_CIRCLE)) >> 9) + 8 - winTime/4;
	
	r = ((sinLerp((sc->time * 190) % DEGREES_IN_CIRCLE)) >> 6) + 192;
	g = ((sinLerp((sc->time * 150) % DEGREES_IN_CIRCLE +3000)) >> 6) + 190;
	b = ((sinLerp((sc->time * 240) % DEGREES_IN_CIRCLE +1000)) >> 6) + 190;

	r += winTime*6;
	g += winTime*6;
	b += winTime*6;
	
	if(r > 255) r = 255;
	if(g > 255) g = 255;
	if(b > 255) b = 255;

	if(r < 0) r = 0;
	if(g < 0) g = 0;
	if(b < 0) b = 0;
	
	if(!world->energyCompleted())
	{
		d /= 3;
		r /= 3;
		g /= 3;
		b /= 3;
	}
	
	for(int i = 0; i < 3; i++)
	{
		ang += DEGREES_IN_CIRCLE /3;


		Particle *p2 = sc->addParticle60();
		p2->vx = -vx.tof5()+sinLerp(ang)/30+irand(10);
		p2->vy = -vy.tof5()+cosLerp(ang)/30+irand(10);
		p2->x = x.tof5()-p2->vx*d;
		p2->y = y.tof5()-p2->vy*d;

		p2->nTexture = 1;
		p2->r = r;
		p2->g = g;
		p2->b = b;
		p2->a = 18;
		p2->sizePerLife = 7;
		p2->effect = FX_NONE;
	}
	
}


void EndingActor::render()
{
	
}

void EndingActor::collidedWithPlayer(PlayerActor* act)
{
	if(act->win())
		winTime = 60;
}
