

#ifndef _CreditsTextActor_H
#define _CreditsTextActor_H

class CreditsTextActor;
#include "Actor.h"
#include "Particle.h"
#include "IntroScene.h"

class CreditsTextActor : public Actor
{
    public: 
    
    const char* text;
    int textLen;
    int time;
    bool special;
    
    CreditsTextActor(Scene* world, f32 x, f32 y, bool special, const char* text);
    
    virtual void tick();
    virtual void render();
    void explode();
};

#endif