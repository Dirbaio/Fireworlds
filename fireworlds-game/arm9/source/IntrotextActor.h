

#ifndef _INTROTEXTACTOR_H
#define _INTROTEXTACTOR_H

class IntrotextActor;
#include "Actor.h"
#include "Particle.h"
#include "IntroScene.h"

class IntrotextActor : public Actor
{
	public: 
	
	const char* text;
	int textLen;
	bool touched;
	int offs;
	int levNum;
	bool active;
	IntrotextActor(Scene* world, const char* text, int offs, int levNum);
	
	virtual void tick();
	virtual void render();
	void explode();
};

#endif