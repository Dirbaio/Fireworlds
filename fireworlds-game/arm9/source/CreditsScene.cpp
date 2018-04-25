
#include<nds.h>
#include "CreditsScene.h"
#include "textures.h"
#include <cstdio>
#include <nds.h>
#include "input.h"
#include "music.h"

#include "LevelScene.h"
#include "LevelSelectScene.h"
#include "CreditsTextActor.h"
#include "SeaActor.h"

CreditsScene::CreditsScene(bool playVideo) : Scene()
{
//	videoPath = "/ending.vds";
	musicNumber = playVideo?7:1;
	topText = "";
	this->playVideo = playVideo;
	actors.push_back(new SeaActor(this));
}

CreditsScene::~CreditsScene()
{
}


void CreditsScene::tick()
{
	int s = 60*9;
	if(time == s*0)		actors.push_back(new CreditsTextActor(this, 0, 0, true, 	"CREDITS"));

	if(time == s*1)		actors.push_back(new CreditsTextActor(this, 0, 20, true, 	"PROGRAMMING"));
	if(time == s*1)		actors.push_back(new CreditsTextActor(this, 0, -20, false, "DIRBAIO"));

	if(time == s*2)		actors.push_back(new CreditsTextActor(this, 0, 20, true, 	"MUSIC"));
	if(time == s*2)		actors.push_back(new CreditsTextActor(this, 0, -20, false, "SAGA MUSIX"));
	if(time == s*2)		actors.push_back(new CreditsTextActor(this, 0, -50, true, 	"SAGAMUSIX.DE"));

	if(time == s*3)		actors.push_back(new CreditsTextActor(this, 0, 20, true, 	"INTRO VIDEO"));
	if(time == s*3)		actors.push_back(new CreditsTextActor(this, 0, -20, false, "SORINGTON"));

	if(time == s*4)	 	actors.push_back(new CreditsTextActor(this, 0, 60, true, 	"ADDITIONAL"));
	if(time == s*4)	 	actors.push_back(new CreditsTextActor(this, 0, 40, true, 	"LEVEL MAKERS"));
	if(time == s*4)	 	actors.push_back(new CreditsTextActor(this, 0, 00, false, 	"MARC B."));
	if(time == s*4)		actors.push_back(new CreditsTextActor(this, 0, -30, false, "SIR_VOE"));
	
	if(time == s*5)	 	actors.push_back(new CreditsTextActor(this, 0, 30, true, 	"LIBRARIES"));
	if(time == s*5)	 	actors.push_back(new CreditsTextActor(this, 0, -10, false, "LIBNDS"));
	if(time == s*5)		actors.push_back(new CreditsTextActor(this, 0, -40, false, "ASLIB"));

	if(time == s*6)	 	actors.push_back(new CreditsTextActor(this, 0, 60, true, 	"MANY THANKS"));
	if(time == s*6)	 	actors.push_back(new CreditsTextActor(this, 0, 0, false, 	"GBATEMP.NET"));
	if(time == s*6)		actors.push_back(new CreditsTextActor(this, 0, -30, false, "THE HB BOUNTY"));
	if(time == s*6)		actors.push_back(new CreditsTextActor(this, 0, -60, false, "DEVKITPRO"));

	if(time == s*7)	 	actors.push_back(new CreditsTextActor(this, 0, 0, false, 	"DIRBAIO"));
	if(time == s*7)		actors.push_back(new CreditsTextActor(this, 0, -30, true, "JUNE 2011"));

	if(time == s*8 && playVideo) videoPath = "/ending.vds";
	if(time == s*8+1 && canChangeScene()) doSceneChange(new IntroScene());
	
	
	Scene::tick();
}

void CreditsScene::render()
{
	Scene::render();
}