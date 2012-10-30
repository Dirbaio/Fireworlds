
#ifndef _CreditsScene_H
#define _CreditsScene_H

class CreditsScene;

#include "Scene.h"
#include "Actor.h"
#include <string>
#include "FireActor.h"
#include "IntrotextActor.h"
using namespace std;

class SeaActor;

class CreditsScene : public Scene
{
	public:
	bool playVideo;
	CreditsScene(bool playVideo);
	virtual ~CreditsScene();
	virtual void tick();
	virtual void render();
};

#endif