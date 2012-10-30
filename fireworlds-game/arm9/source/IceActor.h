

#ifndef _IceActor_H
#define _IceActor_H

#include "Actor.h"
#include "Particle.h"

class IceActor : public Actor
{
	public: 
	
	int touchedTime;
	IceActor(Scene* world);
	
	virtual void tick();
	virtual void render();
	virtual void collidedWithPlayer(PlayerActor* act);
};

#endif