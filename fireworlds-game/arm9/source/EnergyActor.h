

#ifndef _ENERGYACTOR_H
#define _ENERGYACTOR_H

#include "Actor.h"
#include "Particle.h"

class EnergyActor : public Actor
{
	public: 
	u8 t;
	int winTime;
	int r, g, b;
	
	EnergyActor(Scene* world);
	
	virtual void tick();
	virtual void render();
	virtual void collidedWithPlayer(PlayerActor* act);
};

#endif