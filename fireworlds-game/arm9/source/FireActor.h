

#ifndef _FIREACTOR_H
#define _FIREACTOR_H

#include "Actor.h"
#include "Particle.h"

class FireActor : public Actor
{
    public: 
    int color;

    s16 r, g, b, a;
    int c;
    u8 t;
    int colorShift;
    bool activeFire;
    bool activePlayer;
    f32 firesize;
    
    FireActor(Scene* world);
    
    virtual void tick();
    virtual void render();
    void activated();
    
};

#endif