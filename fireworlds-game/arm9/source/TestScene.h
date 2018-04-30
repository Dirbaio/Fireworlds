
#ifndef _TestScene_H
#define _TestScene_H

class TestScene;

#include "Scene.h"
#include "Actor.h"
#include <string>
#include "FireActor.h"
using namespace std;

class SeaActor;

class TestScene : public Scene
{
    public:
    int timeToSpawn;
    FireActor* fires[3];
    f32 fireTargetX[3];
    f32 fireTargetY[3];
    
    TestScene();
    virtual ~TestScene();
    virtual void tick();
    virtual void render();
    virtual void renderPerspective();

    int texty;
    f32 getNextTextY();
};

#endif
