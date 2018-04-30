

#ifndef _IronActor_H
#define _IronActor_H

#include "Actor.h"
#include "Particle.h"

class IronActor : public Actor
{
    public: 
    
    int touchedTime;
    IronActor(Scene* world);
    
    virtual void tick();
    virtual void render();
    virtual void collidedWithPlayer(PlayerActor* act);
};

#endif