#include "LevelScene.h"
#include "FireActor.h"
#include "SeaActor.h"
#include "PlayerActor.h"
#include "IntroScene.h"
#include <cstdio>
#include "input.h"
#include "EndingActor.h"
#include "EnergyActor.h"
#include "IceActor.h"
#include "SwitchActor.h"
#include "IronActor.h"
#include "polyids.h"
#include "textures.h"

#include "sub.h"
#include "music.h"
#include "LevelSelectScene.h"
#include "CharActor.h"
#include "saver.h"
#include "CreditsScene.h"

//ar levName [60] = "0123456789012345678901";
char levName [30] = "fat:/data/fworlds/01.dgl\0\0";

LevelScene::LevelScene(int levelNum) : Scene()
{
	//I hate C strings. They are EVIL.
	levName[18] = (char) ('0' + (levelNum/10)%10);
	levName[19] = (char) ('0' + (levelNum)%10);
	lev = new Level(levName, this);
	this->levelNum = levelNum;
	
	topText = lev->levelText;
	cvx = 0;
	cvy = 0;
	
	energyCount = 0;
	energyNeeded = 0;
	playerExitCount = 0;
	playerExitNeeded = 0;
	gravType = 0;

	spawnActors();
	
//	actors.push_back(new SeaActor(this));
	player = NULL;
	activatePlayer();
	go(player->x, player->y);
	
	actors.push_back(new CharActor(this, 'L', -40, 60));
	actors.push_back(new CharActor(this, 'E', -20, 60));
	actors.push_back(new CharActor(this, 'V', 0,   60));
	actors.push_back(new CharActor(this, 'E', 20,  60));
	actors.push_back(new CharActor(this, 'L', 40,  60));

	actors.push_back(new CharActor(this, (char) ('0' + (levelNum/10)%10), -10, 30));
	actors.push_back(new CharActor(this, (char) ('0' + (levelNum)%10), 10,   30));

	zoom = 0.7;
	paused = false;
	oldSwitch2 = false;
	
	if(levelNum < 8) musicNumber = 4; //Late night
	else if(levelNum < 17) musicNumber = 5; //Hardtech
	else if(levelNum < 28) musicNumber = 3; //Escaping the Orbit
	else if(levelNum < 99) musicNumber = 6; //Happy	
}

void LevelScene::activatePlayer()
{
	if(players.size() == 0) return;
	if(player == players.front()) return;
	player = players.front();
	players.pop_front();
	players.push_back(player);
	player->activated();
	camRelaxTime = 60;
}

void LevelScene::playerDied(PlayerActor* p)
{
	players.remove(p);
	if(players.size() < playerExitNeeded - playerExitCount) //If cant complete level :P
	{
		exitLevel(false);
		player = NULL;
	}
	else 
		if(p == player) activatePlayer();
}

void LevelScene::playerWon(PlayerActor* p)
{
	players.remove(p);
	playerExitCount++;
	if(playerExitNeeded <= playerExitCount)
	{
		exitLevel(true);
		player = NULL;
	}
	else 
		if(p == player) activatePlayer();
}


bool LevelScene::energyCompleted()
{
	return energyNeeded <= energyCount;
}

void LevelScene::getEnergy()
{
	energyCount++;
}

void LevelScene::exitLevel(bool win)
{
	if(!canChangeScene()) return;
	if(win)
	{
		if(levelsBeaten < levelNum)
		{
			levelsBeaten = levelNum;
			writeSaver();
		}
		
		if(levelNum < 36)
			doSceneChange(new LevelScene(levelNum+1));
		else
			doSceneChange(new CreditsScene(true));
	}
	else
		doSceneChange(new LevelScene(levelNum));
}
	
void LevelScene::spawnActors()
{
	for(int i = 0; i < lev->acts->actCount; i++)
	{
		LevelAct* act = lev->acts->acts + i;
		
		Actor* spawn = NULL;
		switch(act->type)
		{
			case 0:
			case 1:
				spawn = new PlayerActor(this);
				players.push_back((PlayerActor*) spawn);
				playerExitNeeded++;
				break;
			case 2:
				spawn = new EndingActor(this);
				break;
			case 3:
				gravCenterX = act->x;
				gravCenterY = act->y;
				gravType = act->data;
				break;
			case 4:
				spawn = new EnergyActor(this);
				energyNeeded++;
				break;
			case 5:
//				musicNumber = act->data;
				break;
			case 6:
				spawn = new IceActor(this);
				break;
			case 7:
				spawn = new SwitchActor(this, act->data);
				break;
			case 8:
				spawn = new IronActor(this);
				break;
		}
		
		if(spawn == NULL) continue;
		
		spawn->x = act->x;
		spawn->y = act->y;
		actors.push_back(spawn);
	}
}

void LevelScene::go(f32 x, f32 y)
{
	xCam = x;
	yCam = y;
	oxCam = x;
	oyCam = y;
	
	transitionTime = -1;
}

LevelScene::~LevelScene()
{

//	if(lev != NULL)
//		delete lev;
}

int irand2(int n)
{
	return rand()%(2*n) - n;
}

void LevelScene::tick()
{
	if(inputKeysDown & KEY_START)
	{
		paused = !paused;
		pauseOption = 0;
	}
	
	if(paused)
	{
		if(inputKeysRepeat & KEY_DOWN)
			pauseOption++;
		if(inputKeysRepeat & KEY_UP)
			pauseOption--;
			
		pauseOption = ((pauseOption%3)+3)%3;
		
		
		if(inputKeysRepeat & KEY_A && canChangeScene())
		{
			if(pauseOption == 0) paused = false;
			if(pauseOption == 1) doSceneChange(new LevelScene(levelNum));
			if(pauseOption == 2) doSceneChange(new LevelSelectScene());
			
		}
	
		return;
	}
	
	for(int i = 0; i < 6; i++)
	{
		switchesActive[i] = switchesActivated[i];
		switchesActivated[i] = false;
	}
	
	
	if(player)
	{
		printf("\x1b[9;1HPlayer:  %d %d      ", player->x.toint(), player->y.toint());

//		if(player->onGround())
//			subRenderObjChar(200, 50, 'G');

		if(camRelaxTime != 0)
		{
			xCam = (xCam*30 + player->x+player->vx*10) / 31;
			yCam = (yCam*30 + player->y+player->vy*10) / 31;
		}
		else
		{
			xCam = (xCam*20 + player->x+player->vx*10) / 21;
			yCam = (yCam*20 + player->y+player->vy*10) / 21;

			if(xCam < player->x - 80) xCam = player->x - 80;
			if(xCam > player->x + 80) xCam = player->x + 80;
			if(yCam < player->y - 60) yCam = player->y - 60;
			if(yCam > player->y + 60) yCam = player->y + 60;
		}
		
		if(gravType == 0)
		{
			camRotX = 0;
			camRotY = -1;
		}
		else
		{
			camRotX = player->gvx;
			camRotY = player->gvy;
		}
	}
	
	lev->tick(xCam, yCam);


	if(transitionTime != -1)
	{
		Scene::tickParticles();
		transitionTime--;
		if(transitionTime == 0 && canChangeScene())
			doSceneChange(new IntroScene());
		
		return;
	}
	
	Scene::tick();
	
/*	//Player collisions.
	//What's this? I dont even know.
	for(list<Actor*>::iterator it = actors.begin(); it != actors.end(); it++)
		for(list<PlayerActor*>::iterator it2 = players.begin(); it2 != players.end(); it2++)
			(*it) -> playerCollision(*it2);
	*/
	
	if(inputKeysDown & KEY_X)
		activatePlayer();
	
	if((inputKeysDown & KEY_SELECT) && canChangeScene()) 
		doSceneChange(new LevelScene(levelNum));

	/*
	//Looks distracting.
	if(switchesActive[2] != oldSwitch2)
	{
		oldSwitch2 = switchesActive[2];
		actors.push_back(new CharActor(this, oldSwitch2?'a':'b', 0, -50));
		
	}*/
	
}

void LevelScene::render()
{
	if(paused) return;
	
	Scene::render();
	
	lev->render(xCam, yCam);
	
	subRenderObjNum(120, 80, energyCount);
	subRenderObjNum(190, 80, energyNeeded);	
}


void LevelScene::renderPerspective()
{
	if(paused) return;
	Scene::renderPerspective();
}

void LevelScene::renderUnrotated()
{
	if(paused)
	{
		renderString("PAUSE", 0, 70, 255, 100, 0);

		renderString("CONTINUE", 0, 10, pauseOption==0?255:80, 150, 255);
		renderString("RESTART", 0, -30, pauseOption==1?255:80, 150, 255);
		renderString("EXIT", 0, -70, pauseOption==2?255:80, 150, 255);
	}
	else
		Scene::renderUnrotated();
}

void LevelScene::calcGravityFor(Actor* act)
{
	f32 dx = act->x - gravCenterX;
	f32 dy = act->y - gravCenterY;
	
	f32 div = (fabs(dx)+fabs(dy))/2;
	dx /= div;
	dy /= div;
	vecNormalize(dx, dy);
	
	act->gvs = 0.17;

	switch(gravType)
	{
		case 0: //Vertical grav.
			act->gvx = 0;
			act->gvy = -1;
			if(switchesActive[2]) act->gvy = 1;
			break;
		case 1: //inwards grav
			act->gvx = -dx;
			act->gvy = -dy;
			break;
		case 2: //outwards grav
			act->gvx = dx;
			act->gvy = dy;
			break;
		case 3: //clockwise grav
			act->gvx = -dy;
			act->gvy = dx;
/*			act->gvx -= dx/2;
			act->gvy -= dy/2;*/
			vecNormalize(act->gvx, act->gvy);
			break;
		case 4: //counterclockwise grav
			act->gvx = dy;
			act->gvy = -dx;/*
			act->gvx -= dx/3;
			act->gvy -= dy/3;*/
			vecNormalize(act->gvx, act->gvy);
			break;
	}
}


void LevelScene::onSceneBegin()
{
	Scene::onSceneBegin();
	subRenderPixelString(20, 82, "Energy");
	subRenderPixelString(140, 82, "of");
}