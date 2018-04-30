#include "CharActor.h"
#include "textures.h"
#include "polyids.h"
#include "input.h"


CharActor::CharActor(Scene* world, char c, f32 x, f32 y) : Actor(world)
{
    this->c = c;
    this->x = x;
    this->y = y;
    life = 120 + x.toint()/4;
}

void CharActor::tick()
{
    life--;
    if(life == 0) explode();
}

void CharActor::explode()
{
    dead = true;

    int r = 4;

    f32 xx, yy;
    
    sc->toWorldCoords(xx, yy, x, y);
    
    for(int ang = 0; ang < DEGREES_IN_CIRCLE; ang+=DEGREES_IN_CIRCLE / 12)
    {
        Particle *p2 = sc->addParticle60();
        p2->vx = (vx+sc->vxCam).tof5()+sinLerp(ang)/160+irand(r);
        p2->vy = (vy+sc->vyCam).tof5()+cosLerp(ang)/160+irand(r);
        p2->x = xx.tof5();
        p2->y = yy.tof5();
        
        p2->nTexture = 1;
        
        p2->r = 255;
        p2->g = 160+irand(80);
        p2->b = 0;
        
        p2->a = 18;
        p2->sizePerLife = 6;
        p2->effect = FX_NOFRICTION;
    }
}

void CharActor::renderUnrotated()
{
    glPolyFmt(POLY_ALPHA(27+rand()%3) | POLY_ID(BG_FX2_POLYID) | POLY_CULL_NONE);
    
    glColor3b(255, 160+irand(80), 0);
    
    renderChar(c, x+frand(1), y+frand(2), size);
}