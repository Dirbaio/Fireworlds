
#include<nds.h>
#include "IntroScene.h"
#include "textures.h"
#include <cstdio>
#include <nds.h>
#include "input.h"
#include "music.h"

#include "LevelScene.h"
#include "CreditsScene.h"
#include "LevelSelectScene.h"
#define camvel f32(4)


IntroScene::IntroScene() : Scene()
{

	fires[0] = NULL;
	texts[0] = new IntrotextActor(this, "PLAY", 0, -1);
	texts[1] = new IntrotextActor(this, "CREDITS", 0, -1);
	
	texts[0]->vx = camvel - camvel/3;
	texts[1]->vx = camvel - camvel/3;
	texts[0]->y = 60;
	texts[1]->y = -60;
	texts[0]->x = 330;
	texts[1]->x = 450;

	actors.push_back(texts[0]);
	actors.push_back(texts[1]);
	
	
	for(int i = 0; i < 3; i++)
	{
		fireTargetX[i] = 0;
		fireTargetY[i] = -60 + 60*i;
	}
	
	videoPath = "/intro.vds";
	musicNumber = 0;
	topText = "Hello Fireworlds!;;Select an option!";
}

IntroScene::~IntroScene()
{
}


void IntroScene::tick()
{
	if(time == 29)
	{
		fires[0] = new FireActor(this);
		fires[0]->x = xCam;
		fires[0]->vx = camvel;
		fires[0]->y = -60;
		fires[0]->colorShift = 0;
		actors.push_back(fires[0]);
	}
	if(time > 30 && time < 40)
		fires[0]->activated();

	if(time < 10)
		for(int ang = 0; ang < DEGREES_IN_CIRCLE; ang+=DEGREES_IN_CIRCLE / 30)
		{
			Particle *p2 = addParticle60();
			p2->vx = camvel.tof5()+sinLerp(ang)/30+irand(10);
			p2->vy = cosLerp(ang)/30+irand(10);
			p2->x = (xCam+70+time).tof5()-p2->vx*20;
			p2->y = (yCam).tof5()-p2->vy*20;

			p2->nTexture = 1;
			p2->r = 255;
			p2->g = 120-time*10;
			p2->b = 20;
			p2->a = 18;
			p2->sizePerLife = 7;
			p2->effect = FX_NOFRICTION;
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
		int ny = sinLerp(time*100) + sinLerp(time*240) + sinLerp(time*174)/2;
		if(time < 250)
			ny = sinLerp(time*440)*2;
		if(time < 150)
			ny = sinLerp(time*440)*3;
		f32 dy = fires[0]->y-(f32(ny) / (1<<7));
		fires[0]->vy *= 0.7;
		fires[0]->vy -= dy/24;
	}
	
	if((texts[0]->touched || inputKeysDown & KEY_A) && canChangeScene())
	{
		texts[0]->explode();
		doSceneChange(new LevelSelectScene());
	}
	if((texts[1]->touched) && canChangeScene())
	{
		texts[1]->explode();
		doSceneChange(new CreditsScene(false));
	}
		
	Scene::tick();
}

void IntroScene::render()
{
	Scene::render();
}

void IntroScene::renderPerspective()
{
}

f32 IntroScene::getNextTextY()
{
	int y = irand(70);
	while(abs(y-texty)<40)
		y = irand(70);
	
	texty = y;
	return y;
}