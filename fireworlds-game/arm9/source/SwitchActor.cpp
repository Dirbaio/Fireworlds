#include "SwitchActor.h"
#include "stdio.h"

#include "music.h"
#include "textures.h"
#include "polyids.h"
#include "PlayerActor.h"


SwitchActor::SwitchActor(LevelScene* world, int num) : Actor(world)
{
    this->world = world;
    this->num = num;
    activeTime = 0;

    size = 35;
    collisionType = COLLISION_ROUND;
    collisionOnlyNotify = true;
    
    if(num == 0)
    {
        r = 100;
        g = 255;
        b = 20;
    }
    else if(num == 1)
    {
        r = 40;
        g = 100;
        b = 255;
    }
    else
    {
        r = 255;
        g = 100;
        b = 30;
    }
}

void SwitchActor::tick()
{
    Actor::tick();
    if(activeTime > 0) activeTime--;
    
    if(activeTime != 0)
        world->switchesActivated[num] = true;
        
/*
    if(t > 0) t--;
    if(winTime > 0) winTime--;
    
    if(musicTrackPlayed[2])
        t = 20;
    
    if(!onScreen()) return;
        
    int ang = (sc->time *(DEGREES_IN_CIRCLE/2 + 700)) % DEGREES_IN_CIRCLE;
    
    int d = ((sinLerp((sc->time * (world->energyCompleted()?140:60)) % DEGREES_IN_CIRCLE)) >> 9) + 8 - winTime/4;
    
    r = ((sinLerp((sc->time * 190) % DEGREES_IN_CIRCLE)) >> 6) + 192;
    g = ((sinLerp((sc->time * 150) % DEGREES_IN_CIRCLE +3000)) >> 6) + 190;
    b = ((sinLerp((sc->time * 240) % DEGREES_IN_CIRCLE +1000)) >> 6) + 190;

    r += winTime*6;
    g += winTime*6;
    b += winTime*6;
    
    if(r > 255) r = 255;
    if(g > 255) g = 255;
    if(b > 255) b = 255;

    if(r < 0) r = 0;
    if(g < 0) g = 0;
    if(b < 0) b = 0;
    
    if(!world->energyCompleted())
    {
        d /= 3;
        r /= 3;
        g /= 3;
        b /= 3;
    }
    
    */
    
    if(!onScreen()) return;
    for(int i = 0; i < 3; i++)
    {
        int ang = irand(DEGREES_IN_CIRCLE);

        int s = 28;
        Particle *p2 = sc->addParticle60();
        p2->vx = fsin(ang).tof5();
        p2->vy = -fcos(ang).tof5();
        p2->vx2 = -fcos(ang).tof5();
        p2->vy2 = -fsin(ang).tof5();

        if(activeTime != 0)
        {
            p2->vx *= 6;
            p2->vy *= 6;
            p2->vx2 *= 3;
            p2->vy2 *= 3;
        }
        p2->x = (x+fcos(ang)*s).tof5();
        p2->y = (y+fsin(ang)*s).tof5();

        p2->nTexture = 1;
        p2->r = r;
        p2->g = g;
        p2->b = b;
        if(num == 2)
            g += irand(40);
        else
            r += irand(90);

        p2->a = 18;
        p2->sizePerLife = 6;
        p2->effect = FX_NONE;
    }
    
}


void SwitchActor::render()
{
    if(!onScreen()) return;

    f32 size = activeTime != 0?35:32;

    setTexture(TEX_SWITCH);
    glPolyFmt(POLY_ALPHA(12+irand(6)) | POLY_ID(FIREGLOW_POLYID) | POLY_CULL_NONE);
    
    //draw the obj

    glColor3b(r, g, b);

    glBegin(GL_QUAD);

    GFX_TEX_COORD = TEXTURE_PACK(inttot16(64), inttot16(0));

    glVertex3f32(x-size-sc->xCam+frand(2), y-size-sc->yCam+frand(2), 0);
    GFX_TEX_COORD = TEXTURE_PACK(inttot16(64), inttot16(64));
    glVertex3f32(x-size-sc->xCam+frand(2), y+size-sc->yCam+frand(2), 0);
    GFX_TEX_COORD = TEXTURE_PACK(inttot16(0), inttot16(64));
    glVertex3f32(x+size-sc->xCam+frand(2), y+size-sc->yCam+frand(2), 0);
    GFX_TEX_COORD = TEXTURE_PACK(inttot16(0), inttot16(0));
    glVertex3f32(x+size-sc->xCam+frand(2), y-size-sc->yCam+frand(2), 0);
        
    glEnd();
    
}

void SwitchActor::collidedWithActor(Actor* act)
{
    if(activeTime == 0)
        for(int ang = 0; ang < DEGREES_IN_CIRCLE; ang += DEGREES_IN_CIRCLE/64)
        {
            int s = 28;
            Particle *p2 = sc->addParticle60();
            p2->vx = fcos(ang).tof5()*8+irand(2);
            p2->vy = fsin(ang).tof5()*8+irand(2);

            p2->x = (x+fcos(ang)*s).tof5();
            p2->y = (y+fsin(ang)*s).tof5();

            p2->nTexture = 1;
            p2->r = r+irand(90);
            p2->g = g;
            p2->b = b;
            p2->a = 18;
            p2->sizePerLife = 6;
            p2->effect = FX_NONE;
        }
    
    if(num == 2)
        activeTime = 120;
    else
        activeTime = 3;
}
