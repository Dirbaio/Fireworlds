
#include<nds.h>
#include "TestScene.h"
#include "textures.h"
#include <cstdio>
#include <nds.h>
#include "input.h"
#include "music.h"

#include "LevelScene.h"
#include "CreditsScene.h"
#include "LevelSelectScene.h"
#define camvel f32(4)


TestScene::TestScene() : Scene()
{

	fires[0] = NULL;
	
	
	for(int i = 0; i < 3; i++)
	{
		fireTargetX[i] = 0;
		fireTargetY[i] = -60 + 60*i;
	}
	
//	videoPath = "/intro.vds";
	musicNumber = 0;
	topText = "Hello Fireworlds!;;Testing fireball!";
}

TestScene::~TestScene()
{
}


void TestScene::tick()
{
	if(time == 1)
	{
		fires[0] = new FireActor(this);
		fires[0]->x = xCam;
		fires[0]->vx = camvel;
		fires[0]->y = -60;
		fires[0]->colorShift = 0;
		actors.push_back(fires[0]);
	}
	
	
	if(time % 5 == 0)
	{
		Particle* star = addParticle1000();
		star->sizePerLife = -(1+rand() % 4);
		if(rand() % 6 == 0)
			star->sizePerLife -= 7+rand() % 4;
		star->vx = camvel.tof5()+star->sizePerLife*17-10;
		star->vy = 0;
		star->vx2 = 0;
		star->vy2 = 0;
		star->x = xCam.tof5()+230*32;
//		star->x = rand() % (192*32) - (192*32/16);
		star->y = rand() % (192*32) - (192*32/2);
		star->nTexture = TEX_DIAMOND;
		star->effect = FX_NOFRICTION;
		
		
		star->r = 150+rand() % 100;
		star->g = 150+rand() % 100;
		star->b = 255;
		star->a = 10+rand() % 21;
	}
	xCam += camvel;
	
	if(fires[0])
	{
//		int ny = sinLerp(time*100) + sinLerp(time*240) + sinLerp(time*174)/2;
//		if(time < 250)
			int ny = sinLerp(time*440)*4;
//		if(time < 150)
//			ny = sinLerp(time*440)*3;
		f32 dy = fires[0]->y-(f32(ny) / (1<<7));
		fires[0]->vy *= 0.7;
		fires[0]->vy -= dy/24;
	}
	
		
	Scene::tick();
}

void TestScene::render()
{
	Scene::render();
}

void TestScene::renderPerspective()
{
}

f32 TestScene::getNextTextY()
{
	int y = irand(70);
	while(abs(y-texty)<40)
		y = irand(70);
	
	texty = y;
	return y;
}
