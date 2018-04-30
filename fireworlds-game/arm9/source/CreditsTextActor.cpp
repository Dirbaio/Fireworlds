#include "CreditsTextActor.h"
#include "textures.h"
#include "polyids.h"
#include "input.h"


CreditsTextActor::CreditsTextActor(Scene* world, f32 x, f32 y, bool special, const char* text) : Actor(world)
{
    this->text = text;
    
    int len = 0;
    for(int i = 0; text[i] != 0; i++)
        len++;
    
    this->textLen = len;
    this->size = special?8:10;
    this->special = special;
    x -= size*textLen;
    x += size;
    this->x = x;
    this->y = y;
    time = 0;
}

#define tr 8
void CreditsTextActor::tick()
{
    time++;
    if(time >= 60*tr-20)
        x -= special?-10:10;
    if(time >= 60*tr) dead = true;
    
    x += vx;
    y += vy;
    
    if(x < sc->xCam - 260)
    {
        x = sc->xCam + 140;
        y = frand(70);
    }
    
    if(!onScreen()) return;
    
    Particle *p2 = sc->addParticle60();
    p2->vx = vx.tof5();
    p2->vy = (vy+frand(1)).tof5();
    if(time >= 60*tr-40) p2->vx += special?-200:200;
    p2->x = x.tof5()+rand() % (size*64*textLen-size*2).toint();
    p2->y = y.tof5()+irand(5*32);
    
    p2->nTexture = 1;
    p2->r = 170;
    p2->g = 180;
    p2->b = 20;
    p2->a = 18;
    p2->sizePerLife = 4;
    p2->effect = FX_RED;

    for(int i = 0; text[i] != 0; i++)
        if(time == i*4)
        {
            for(int ang = 0; ang < DEGREES_IN_CIRCLE; ang+=DEGREES_IN_CIRCLE / 10)
            {
                Particle *p2 = sc->addParticle60();
                p2->vx = vx.tof5()+sinLerp(ang)/70+irand(10)+20;
                p2->vy = vy.tof5()+cosLerp(ang)/70+irand(10);
                p2->x = (x+size*i*2).tof5();
                p2->y = y.tof5()+sinLerp(sc->time*500+i*8000)/30+3*32;
                
                p2->nTexture = 1;
                p2->r = 220;
                p2->g = 199;
                p2->b = 40;
                p2->a = 18;
                p2->sizePerLife = 4;
                p2->effect = FX_RED;
            }               
        }

}

void CreditsTextActor::render()
{
    glPolyFmt(POLY_ALPHA(26+rand()%3) | POLY_ID(BG_FX2_POLYID) | POLY_CULL_NONE);
    
    if(special)
        glColor3b(255, 160, 10);
    else
        glColor3b(255, 240, 150);
    
    int i;
    for(i = 0; text[i] != 0 && i < time/4; i++)
        renderChar(text[i], x-sc->xCam+size*i*2, y+sinLerp((sc->time)*400+i*8000)/1500-sc->yCam, size);

}