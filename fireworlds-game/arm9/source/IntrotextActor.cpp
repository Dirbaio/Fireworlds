#include "IntrotextActor.h"
#include "textures.h"
#include "polyids.h"
#include "input.h"


IntrotextActor::IntrotextActor(Scene* world, const char* text, int offs, int levNum) : Actor(world)
{
    this->text = text;

    int len = 0;
    for(int i = 0; text[i] != 0; i++)
        len++;
    if(levNum != -1) len+=3;

    this->textLen = len;
    this->size = 10;
    this->touched = false;
    this->offs = offs;
    this->levNum = levNum;
    active = false;
}

void IntrotextActor::tick()
{
    x += vx;
    y += vy;

    if(x < sc->xCam - 260)
    {
        x = sc->xCam + 140;
        y = frand(70);
    }

    if(!onScreen()) return;

    if(levNum == -1)
    {
        Particle *p2 = sc->addParticle60();
        p2->vx = vx.tof5();
        p2->vy = (vy+frand(1)).tof5();
        p2->x = x.tof5()+irand(30*32);
        p2->y = y.tof5()+irand(5*32);

        p2->nTexture = 1;
        p2->r = 200;
        p2->g = 220;
        p2->b = 170;
        p2->a = 18;
        p2->sizePerLife = 4;
        p2->effect = FX_RED;
    }

    for(int i = 0; text[i] != 0; i++)
        if((sc->time - offs*7) % 100 == i*2)
        {
            for(int ang = 0; ang < DEGREES_IN_CIRCLE; ang+=DEGREES_IN_CIRCLE / 10)
            {
                Particle *p2 = sc->addParticle60();
                p2->vx = vx.tof5()+sinLerp(ang)/70+irand(10)+20;
                p2->vy = vy.tof5()+cosLerp(ang)/70+irand(10);
                p2->x = (x+size*i*2).tof5();
                p2->y = y.tof5()+sinLerp(sc->time*500+i*8000)/30+3*32;

                p2->nTexture = 1;
                p2->r = 100;
                p2->g = 199;
                p2->b = 255;
                p2->a = 18;
                p2->sizePerLife = 4;
                p2->effect = FX_RED;
            }
        }

    touched = false;

    if(inputKeysHeld & KEY_TOUCH)
    {
        if(x -size - 10<= sc->touchWorldX && y-size-10 <= sc->touchWorldY)
            if(x-size+size*textLen*2 + 10 >= sc->touchWorldX && y+size+10 >= sc->touchWorldY)
                touched = true;
    }
}

void IntrotextActor::explode()
{
    dead = true;

    if(!onScreenStrict()) return;
    int r = 1;

    for(int i = 0; i < textLen; i++)
        for(int ang = 0; ang < DEGREES_IN_CIRCLE; ang+=DEGREES_IN_CIRCLE / 10)
        {
            Particle *p2 = sc->addParticle60();
            p2->vx = vx.tof5()+sinLerp(ang)/30+irand(r);
            p2->vy = vy.tof5()+cosLerp(ang)/70+irand(r);
            p2->x = (x+size*i*2).tof5();
            p2->y = y.tof5()+sinLerp(sc->time*500+i*8000)/30+3*32;

            p2->nTexture = 1;

            if(active)
            {
                p2->r = 240;
                p2->g = 69;
                p2->b = 10;
            }
            else if(rand() % 2 == 0)
            {
                p2->r = 230;
                p2->g = 239;
                p2->b = 255;
            }
            else
            {
                p2->r = 220;
                p2->g = 255;
                p2->b = 180;
            }
            p2->a = 18;
            p2->sizePerLife = 6;
            p2->effect = FX_NONE;
        }

}
void IntrotextActor::render()
{
    if(!onScreen()) return;

    glPolyFmt(POLY_ALPHA(27+rand()%3) | POLY_ID(BG_FX2_POLYID) | POLY_CULL_NONE);

    if(touched || active)
        glColor3b(210, 0, 0);
    else
        glColor3b(210, 220, 255);

    int i;
    for(i = 0; text[i] != 0; i++)
        renderChar(text[i], x-sc->xCam+size*i*2+frand(1), y+frand(2)+sinLerp((sc->time+offs*13)*400+i*8000)/900-sc->yCam, size);

    if(levNum != -1)
    {
        i++;
        renderChar((char)((levNum/10)%10+'0'), x-sc->xCam+size*i*2+frand(1), y+frand(2)+sinLerp((sc->time+offs*13)*400+i*8000)/900-sc->yCam, size);
        i++;
        renderChar((char)((levNum)%10+'0'), x-sc->xCam+size*i*2+frand(1), y+frand(2)+sinLerp((sc->time+offs*13)*400+i*8000)/900-sc->yCam, size);

    }
}
