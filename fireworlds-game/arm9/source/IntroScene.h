
#ifndef _INTROSCENE_H
#define _INTROSCENE_H

class IntroScene;

#include "Scene.h"
#include "Actor.h"
#include <string>
#include "FireActor.h"
#include "IntrotextActor.h"
using namespace std;

class SeaActor;

class IntroScene : public Scene
{
	public:
	int timeToSpawn;
	FireActor* fires[3];
	f32	fireTargetX[3];
	f32	fireTargetY[3];
	IntrotextActor* texts[2];
	
	IntroScene();
	virtual ~IntroScene();
	virtual void tick();
	virtual void render();
	virtual void renderPerspective();

	int texty;
	f32 getNextTextY();
};

#endif