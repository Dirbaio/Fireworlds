#include <cstdio>
#include "Scene.h"
#include "Actor.h"
#include "input.h"
#include "sub.h"
#include "music.h"

#include "polyids.h"
#include "textures.h"

Scene::Scene()
{
	xCam = 0;
	yCam = 0;

	buffer10 = new ParticleBuffer(800, 10, false, NULL);
	buffer20 = new ParticleBuffer(800, 20, false, NULL);
	buffer60b = new ParticleBuffer(1000, 60, false, NULL);
	buffer60 = new ParticleBuffer(300, 60, true, buffer60b);
	buffer180 = new ParticleBuffer(200, 180, false, NULL);
	buffer1000 = new ParticleBuffer(400, 1000, false, NULL);

	lev = NULL;
	newScene = NULL;
	time = 0;

	actors = list<Actor*>();
	camRotX = 0;
	camRotY = -1;

	topText = "  D1RB41O;    H4Z;   TR0LL";
	textChar = 0;
	textTime = 40;

	curTextX = 10;
	curTextY = 120;
	cursorTime = 100;

	musicNumber = 0;

	zoom = 1;
	videoPath = NULL;

	for(int i = 0; i < 32; i++)
	{
		switchesActive[i] = false;
		switchesActivated[i] = false;
	}
}

Scene::~Scene()
{
	delete buffer10;
	delete buffer20;
	delete buffer60;
	delete buffer60b;
	delete buffer180;
	delete buffer1000;
	for(list<Actor*>::iterator it = actors.begin(); it != actors.end(); it++)
	{
		Actor* a = *it;
		delete a;
	}
	if(lev != NULL)
		delete lev;
}

bool Scene::inCam(f32 x, f32 y)
{
	return x>xCam-256 && x < xCam+256 && y > yCam-192 && y < yCam+192;
}
bool Scene::inCamStrict(f32 x, f32 y)
{
	return x>xCam-128 && x < xCam+128 && y > yCam-96 && y < yCam+96;
}

void Scene::onSceneBegin()
{
	playMusic(musicNumber);
}


void Scene::tick()
{
	if(cursorTime != 0) cursorTime--;

	if(textTime != -1)
	{
		if(textTime != 0)
			textTime--;
		else if(topText[textChar] == 0)
			textTime = -1;
		else
		{
			if(topText[textChar] == ';')
			{
				curTextY += 12;
				curTextX = 10;
			}
			else
			{
				if(topText[textChar] == 'M' || topText[textChar]=='m' || topText[textChar]=='W'||topText[textChar]=='w')
					curTextX += 2;
				subRenderPixelChar(curTextX, curTextY, topText[textChar]);
				curTextX += 11;
			}
			textChar++;
			cursorTime = 199;
			textTime = 1+rand()%5;
			if(topText[textChar] == ' ' || topText[textChar]==';')
				textTime = 8;
		}
	}

	if(camRelaxTime > 0) camRelaxTime--;

	for(list<Actor*>::iterator it = actors.begin(); it != actors.end(); it++)
	{
//		if(inCam((*it)->x, (*it)->y))
			(*it)->tick();
	}

	for(list<Actor*>::iterator it = actors.begin(); it != actors.end(); )
	{
		if((*it)->dead)
		{
			list<Actor*>::iterator it2 = it;
			it2++;
			delete *it;
			actors.erase(it);
			it = it2;
		}
		else it++;
	}

	tickParticles();

	vxCam = xCam-oxCam;
	vyCam = yCam-oyCam;
	oxCam = xCam;
	oyCam = yCam;

	toWorldCoords(touchWorldX, touchWorldY, -128 + inputTouch.px,  96-inputTouch.py);
}

void Scene::actorCollisions(Actor* act, int lev)
{
	//Calculate collisions pairwise
	for(list<Actor*>::iterator it = actors.begin(); it != actors.end(); it++)
		if(*it != act)
			(*it)->actorCollisionSub(act, lev);
}

void Scene::toWorldCoords(f32& dx, f32& dy, f32 x, f32 y)
{
	x /= zoom;
	y /= zoom;
	f32 nvxt = -x*camRotY + -y*camRotX;
	f32 nvyt = x*camRotX + -y*camRotY;

	dx = xCam +nvxt;
	dy = yCam +nvyt;

}

void Scene::tickParticles()
{
	buffer10->tick(this);
	buffer20->tick(this);
	buffer60->tick(this);
	buffer60b->tick(this);
	buffer180->tick(this);
	buffer1000->tick(this);

	time++;
}


void Scene::render()
{
	int z = 0;

	buffer1000->render(this, z);
	buffer10->render(this, z);
	buffer20->render(this, z);
	buffer60->render(this, z);
	buffer60b->render(this, z);
	buffer180->render(this, z);
	for(list<Actor*>::iterator it = actors.begin(); it != actors.end(); it++)
		(*it)->render();

	if((cursorTime / 15) % 2 == 1)
		subRenderObjChar(curTextX, curTextY, '.');
}

void Scene::renderPerspective()
{
	for(list<Actor*>::iterator it = actors.begin(); it != actors.end(); it++)
		(*it)->renderPerspective();

}

void Scene::renderUnrotated()
{
	for(list<Actor*>::iterator it = actors.begin(); it != actors.end(); it++)
		(*it)->renderUnrotated();
}


Particle* Scene::addParticle10()
{
	return buffer10->addParticle();
}
Particle* Scene::addParticle20()
{
	return buffer20->addParticle();
}
Particle* Scene::addParticle60()
{
	return buffer60->addParticle();
}
Particle* Scene::addParticle180()
{
	return buffer180->addParticle();
}
Particle* Scene::addParticle1000()
{
	return buffer1000->addParticle();
}

bool Scene::canChangeScene()
{
	return newScene == NULL;
}

void Scene::doSceneChange(Scene* nScene)
{
	newScene = nScene;
}

void Scene::calcGravityFor(Actor* act)
{
	act->gvx = 0;
	act->gvy = -1;
	act->gvs = 0.17;
}

void Scene::renderString(const char* str, f32 x, f32 y, int r, int g, int b)
{
	int l = 0;
	while(str[l] != 0) l++;

	x -= (l*25) / 2;

	int i = 0;
	while(str[i] != 0)
	{
		glPolyFmt(POLY_ALPHA(27+rand()%3) | POLY_ID(BG_FX2_POLYID) | POLY_CULL_NONE);
		int rr = r+irand(80); if(rr < 0) rr = 0; if(rr > 255) rr = 255;
		int gg = g+irand(80); if(gg < 0) gg = 0; if(gg > 255) gg = 255;
		int bb = b+irand(80); if(bb < 0) bb = 0; if(bb > 255) bb = 255;

		glColor3b(rr, gg, bb);
		renderChar(str[i], x+frand(1), y+frand(2), 10);
		x += 25;
		i++;
	}
}
