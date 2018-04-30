
#include<nds.h>
#include "LevelSelectScene.h"
#include "textures.h"
#include <cstdio>
#include <nds.h>
#include "input.h"
#include "music.h"
#include "LevelScene.h"
#include "saver.h"

#define camvel f32(4)



LevelSelectScene::LevelSelectScene() : Scene()
{
    for(int i = 0; i < 40; i++) levels[i] = NULL;

    levCount = levelsBeaten+1;
    if(levCount > 36) levCount = 36;
    for(int i = 1; i <= levCount; i++)
    {
        levels[i] = new IntrotextActor(this, "LEVEL", i, i);
        levels[i]->x = -90;
        levels[i]->y = -i*35;
        actors.push_back(levels[i]);
    }

    yCamAcc = 0;
    levSelected = 0;
    goTime = -1;
    goBack = false;
    topText = "Pick a level!;NOW!";
    musicNumber = 2;
    jumpToSelected = false;
}

LevelSelectScene::~LevelSelectScene()
{

}

void LevelSelectScene::tick()
{
    if(goTime > 0)
    {
        goTime--;
    }

    if(goTime == 49)
    {
        for(int i = 1; i <= levCount; i++)
            levels[i]->explode();
        for(int i = 0; i < 30; i++) levels[i] = NULL;
    }

    if(goTime == 0 && canChangeScene())
    {
        if(goBack)
            doSceneChange(new IntroScene());
        else
            doSceneChange(new LevelScene(levSelected));
    }

    if(time % 2 == 0)
    {
        Particle* star = addParticle1000();
        star->sizePerLife = -(2+rand() % 4);
        star->vy = star->sizePerLife*17+20;
        star->vx = 0;
        star->vx2 = 0;
        star->vy2 = 0;
        star->x = frand(128).tof5();
        star->y = (yCam + 126).tof5();
        star->nTexture = TEX_DIAMOND;
        star->effect = FX_NOFRICTION;


        star->r = 150+rand() % 100;
        star->g = 150+rand() % 100;
        star->b = 255;
        star->a = 10+rand() % 21;

    }

    Scene::tick();

    if(goTime != -1)
        return;

    /*
    for(int i = 0; i < 3; i++)
    {
        if(rand() % 90 == 0 || time == 0)
        {
            fireTargetX[i] = 70+irand(40);
            fireTargetY[i] = i*50 -50+irand(20);
//          if(rand() % 10 == 0) fireTargetY[i] = 300*32;
        }
        f32 dx = fires[i]->x-(fireTargetX[i]+xCam);
        f32 dy = fires[i]->y-(fireTargetY[i]+yCam);
        fires[i]->vx *= 0.6;
        fires[i]->vx -= dx/18;
        fires[i]->vy *= 0.6;
        fires[i]->vy -= dy/18;

    }*/

    for(int i = 1; i <= levCount; i++)
        if(levels[i]->touched)
            selectLevel(i);

    if(inputKeysRepeat & KEY_DOWN)
    {
        selectLevel(levSelected+1);
        jumpToSelected = true;
    }
    if(inputKeysRepeat & KEY_UP)
    {
        selectLevel(levSelected-1);
        jumpToSelected = true;
    }

    if(inputKeysDown & KEY_A && levSelected != -1)
        goTime = 50;

    if(inputKeysDown & KEY_B)
    {
        goBack = true;
        goTime = 50;
    }

    if(inputKeysDown & KEY_TOUCH)
    {
        jumpToSelected = false;
        touchDownY = touchWorldY;
        if(inputTouch.px >= 256-50 && inputTouch.py > 75 && inputTouch.py < 105 && levSelected != -1)
            goTime = 50;
    }
    else if(inputKeysHeld & KEY_TOUCH)
    {
        yCamAcc -= (touchWorldY - touchDownY) / 4;
        yCamAcc *= f32(0.6);
    }
    else
    {
        if(levSelected != -1 && jumpToSelected)
        {
            yCamAcc -= (yCam - (-levSelected*35)) / 66;
            yCamAcc *= f32(0.8);
        }
        else
            yCamAcc *= f32(0.9);
    }

    yCam += yCamAcc;
    if(yCam > 0) yCam = 0;
    if(yCam < -levCount*35) yCam = -levCount*35;
}

void LevelSelectScene::selectLevel(int i)
{
    if(i < 1) i = 1;
    if(i > levCount) i = levCount;

    if(levSelected != -1) levels[levSelected]->active = false;
    levSelected = i;
    levels[levSelected]->active = true;
}

void LevelSelectScene::render()
{
    Scene::render();

    if(goTime != -1) return;
    glColor3b(140+(fsin(time*400)*80).toint(), 220, 255);

    if(levSelected == -1) return;
    renderChar('G', 90+irand(3), irand(2), 10);
    renderChar('O', 110+irand(3), irand(2), 10);
}

void LevelSelectScene::renderPerspective()
{

}
