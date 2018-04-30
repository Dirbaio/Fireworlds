

#ifndef _SwitchActor_H
#define _SwitchActor_H

#include "Actor.h"
#include "Particle.h"

class SwitchActor : public Actor
{
    public: 
    int activeTime;
    int num;
    int r, g, b;
    LevelScene* world;
    
    SwitchActor(LevelScene* world, int num);
    
    virtual void tick();
    virtual void render();
    virtual void collidedWithActor(Actor* act);
};

#endif