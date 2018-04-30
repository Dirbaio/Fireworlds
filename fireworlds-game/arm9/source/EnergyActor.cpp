#include "EnergyActor.h"
#include "music.h"
#include "textures.h"
#include "polyids.h"
#include "PlayerActor.h"


EnergyActor::EnergyActor(Scene* world) : Actor(world)
{
    t = 0;
    size = 30;
    collisionType = COLLISION_ROUND;
    collisionOnlyPlayer = true;
    collisionOnlyNotify = true;
}

void EnergyActor::tick()
{
    Actor::tick();

    if(t > 0) t--;
    
    if(musicTrackPlayed[2])
        t = 20;
    
    if(!onScreen()) return;

    b = ((sinLerp((sc->time * 1420) % DEGREES_IN_CIRCLE)) >> 4) + 152;
    if(b > 255) b = 255;
    if(b < 0) b = 0;
//  for(int i = 0; i < 2; i++)
    {
        Particle *p2 = sc->addParticle20();
        p2->vx = irand(35);
        p2->vy = irand(35);
        p2->x = x.tof5()+irand(25);
        p2->y = y.tof5()+irand(25);



        p2->nTexture = TEX_DIAMOND;
        p2->r = b;
        p2->g = 220+irand(10);
        p2->b = 240+irand(10);
        p2->a = 20;
        p2->sizePerLife = 26;
        p2->effect = FX_NONE;
        if(rand() % 5 == 0)
        {
            p2->vx = vx.tof5()+irand(128*2);
            p2->vy = vy.tof5()+irand(128*2);
            p2->effect = FX_BLUR;
        }
    }
    
}


void EnergyActor::render()
{

}

void EnergyActor::collidedWithPlayer(PlayerActor* act)
{
    act->world->getEnergy();
    dead = true;

    for(int ang = 0; ang < DEGREES_IN_CIRCLE; ang+=DEGREES_IN_CIRCLE / 15)
    {
        Particle *p2 = sc->addParticle20();
        p2->vx = vx.tof5()+sinLerp(ang)/30+irand(20);
        p2->vy = vy.tof5()+cosLerp(ang)/30+irand(20);
        p2->x = x.tof5()+p2->vx*3;
        p2->y = y.tof5()+p2->vy*3;

        p2->nTexture = TEX_DIAMOND;
        p2->r = 210+irand(10);
        p2->g = 220+irand(10);
        p2->b = 240+irand(10);
        p2->a = 20;
        p2->sizePerLife = 30;
        p2->effect = FX_NONE;
    }

}
