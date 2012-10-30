

#ifndef _ENDINGACTOR_H
#define _ENDINGACTOR_H

#include "Actor.h"
#include "Particle.h"

class EndingActor : public Actor
{
	public: 
	u8 t;
	int winTime;
	int r, g, b;
	LevelScene* world;
	
	EndingActor(LevelScene* world);
	
	virtual void tick();
	virtual void render();
	virtual void collidedWithPlayer(PlayerActor* act);
};

#endif