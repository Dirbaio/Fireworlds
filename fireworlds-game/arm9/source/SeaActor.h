
#ifndef _SEAACTOR_H
#define _SEAACTOR_H

#include "Actor.h"
#include "LevelScene.h"

class SeaActor: public Actor
{
    public:
    SeaActor(Scene* world);
    
    virtual void tick();
    virtual void render();
    virtual void renderPerspective();
    int gety(int x, int z);
    int getc(int x, int z);
    void renderSea(int xMin, int xMax, int zMin, int zMax, int inc, bool trick);
};

#endif