
#ifndef _LevelSelectScene_H
#define _LevelSelectScene_H

class LevelSelectScene;

#include "Scene.h"
#include "Actor.h"
#include <string>
#include "FireActor.h"
#include "IntrotextActor.h"
using namespace std;

class SeaActor;

class LevelSelectScene : public Scene
{
	public:
	
	f32 touchDownY;
	f32 yCamAcc;
	IntrotextActor* levels [40];
	int levCount;
	
	int goTime;
	int levSelected;
	bool goBack;
	bool jumpToSelected;
	
	LevelSelectScene();
	virtual ~LevelSelectScene();
	virtual void tick();
	virtual void render();
	virtual void renderPerspective();
	void selectLevel(int i);
	
};

#endif