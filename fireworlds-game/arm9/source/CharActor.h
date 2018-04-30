

#ifndef _CHARACTOR_H
#define _CHARACTOR_H

class CharActor;
#include "Actor.h"
#include "Particle.h"
#include "IntroScene.h"

class CharActor : public Actor
{
    public: 
    
    char c;
    int life;
    
    CharActor(Scene* world, char c, f32 x, f32 y);
    
    virtual void tick();
    virtual void renderUnrotated();
    void explode();
};

#endif