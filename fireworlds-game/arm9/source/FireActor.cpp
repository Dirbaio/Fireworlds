#include "FireActor.h"
#include "stdio.h"
#include "music.h"
#include "textures.h"
#include "polyids.h"

u16 colors[][3] = 
{
    {255, 76, 14},
    {130, 255, 14},
    {14, 208, 255},
    {111, 169, 243},
    {71, 255, 171},
    {210, 141, 255},
    {214, 255, 71},
    {255, 192, 147},
};

FireActor::FireActor(Scene* world) : Actor(world)
{
    r = 255;
    g = 150;
    b = 50;
    
    t = 0;
    c = 0;
    colorShift = 0;
    activeFire = true;
    activePlayer = true;
    firesize = 3;
}

void FireActor::tick()
{
    Actor::tick();
    x += vx;
    y += vy;
    
    if(!onScreen()) return;
    
    if(t > 0) t--;
    
    if(musicTrackPlayed[1])
    {
        t = 20;
        c = musicTrackPlayed[1];
        
    }
    int c2 = (c+colorShift) % 8;
    r = colors[c2][0];
    g = colors[c2][1];
    b = colors[c2][2];
    
    if(t > 0) t--;
    if(activeFire)
    {
        int rad = 15+t;
        for(int i = 0; i < 10; i++)
        {
            int ang = irand(DEGREES_IN_CIRCLE);
            f32 cos = fcos(ang);
            f32 sin = fsin(ang);
            cos -= 0.5;
            cos += cos/2;
            f32 xx = vx-gx;
            f32 yy = vy-gy;
            vecNormalize(xx, yy);
            xx *= rad;
            yy *= rad;
            f32 dx = xx*cos-yy*sin;
            f32 dy = xx*sin+yy*cos;
            f32 pos = -vecProjectionLen(vx, vy, dx, dy)/rad+0.1;
//          if(pos<0) continue;
            Particle *p2 = sc->addParticle10();
            p2->vx = (vx - gx*2 + yy*sin*pos + frand(0.4)).tof5();
            p2->vy = (vy - gy*2 - xx*sin*pos + frand(0.4)).tof5();
            p2->x = (x+dx).tof5();
            p2->y = (y+dy).tof5();
//          p2->vx2 = (-vx - yy*sin/16).tof5();
//          p2->vy2 = (-vy + xx*sin/16).tof5(); 
        
            p2->nTexture = 1;
            p2->r = 255;
            p2->g = 0;
            p2->b = 100;
            p2->a = 18;
            p2->sizePerLife = 45;
            p2->effect = FX_NONE;
        }
        
        for(int i = 0; i < 4; i++)      
        {
            Particle* p2 = sc->addParticle20();
            p2->r = 255;
            p2->g = 200;
            p2->b = 100;
            p2->a = 18;
            f32 dx = frand(firesize*2);
            f32 dy = frand(firesize*2);
            p2->x = (x+dx).tof5();
            p2->y = (y+dy).tof5();

            p2->vx = (vx/2 - gx - dx/8 + frand(0.4)).tof5();
            p2->vy = (vy/2 - gy - dy/8 + frand(0.4)).tof5();

            if(t > 15)
            {
                p2->vx += (p2->x-x.tof5()) /50;
                p2->vy += (p2->y-y.tof5()) /50;
            }
            p2->nTexture = 1;
            p2->sizePerLife = 20+irand(15);
            p2->effect = FX_RED;
        }
    }
    
    
    if(activePlayer)
    {
        if(musicTrackPlayed[1])
            activated();

        int ang = rand() % DEGREES_IN_CIRCLE;
        if(musicTrackPlaying[0])
            for(int i = 0; i < 2; i++)
            {
                Particle *p2 = sc->addParticle20();
                p2->x = x.tof5();
                p2->y = y.tof5();
                p2->vx = vx.tof5()+sinLerp(ang)/25;
                p2->vy = vy.tof5()+cosLerp(ang)/25;
                int extraang = DEGREES_IN_CIRCLE/4;
                
                extraang *= musicTrackPlaying[0]%4;
                if(extraang == 0)
                    extraang = DEGREES_IN_CIRCLE/2;
                
                p2->vx2 = sinLerp(ang+extraang)/25;
                p2->vy2 = cosLerp(ang+extraang)/25;
                p2->x += sinLerp(ang)/10;
                p2->y += cosLerp(ang)/10;
                p2->nTexture = 1;
                p2->sizePerLife = -3;
                p2->r = 150;
                p2->g = 100;
                p2->b = 255;
                p2->a = 22;
                p2->effect = FX_BLUR;
            }

        if(sc->time%4 == 0)
        {
            Particle *p2 = sc->addParticle180();
            p2->x = x.tof5();
            p2->y = y.tof5();
            p2->vx = irand(140);
            p2->vy = irand(140);
            p2->nTexture = 1;
            p2->sizePerLife = -20;
            p2->r = 200;
            p2->g = 200;
            p2->b = 200;
            p2->effect = FX_ALPHA;
        }
    }

//  move(vx, vy);
    
    
}



void FireActor::activated()
{
    for(int ang = 0; ang < DEGREES_IN_CIRCLE; ang+=DEGREES_IN_CIRCLE / 48)
    {
        Particle *p2 = sc->addParticle60();
        p2->vx = vx.tof5()+sinLerp(ang)/30+irand(10);
        p2->vy = vy.tof5()+cosLerp(ang)/30+irand(10);
        p2->x = x.tof5()+p2->vx*3;
        p2->y = y.tof5()+p2->vy*3;
        if(rand() % 10 == 0)
        {
            p2->vx = vx.tof5()+(rand() % 256*4) - 128*4;
            p2->vy = vy.tof5()+(rand() % 256*4) - 128*4;
        }
        p2->nTexture = 1;
        p2->r = 200;
        p2->g = 100;
        p2->b = 230;
        p2->a = 10;
        p2->sizePerLife = 4;
        p2->effect = FX_RED;
    }
}

void FireActor::render()
{

    f32 size = 4*14 + t*5;
    f32 xcam = sc->xCam;
    f32 ycam = sc->yCam;
    
    if(!activeFire) return;
    if(!activePlayer) return;
    if(!onScreen()) return;

    setTexture(1);
    glPolyFmt(POLY_ALPHA(6+irand(3)) | POLY_ID(FIREGLOW_POLYID+colorShift) | POLY_CULL_NONE);
    //draw the obj
    glColor3b(200, 150, 100);
    glBegin(GL_QUAD);

    GFX_TEX_COORD = TEXTURE_PACK(inttot16(64), inttot16(0));

    glVertex3f32(x-size-xcam+frand(9), y-size-ycam+frand(9), 0);
    GFX_TEX_COORD = TEXTURE_PACK(inttot16(64), inttot16(64));
    glVertex3f32(x-size-xcam+frand(9), y+size-ycam+frand(9), 0);
    GFX_TEX_COORD = TEXTURE_PACK(inttot16(0), inttot16(64));
    glVertex3f32(x+size-xcam+frand(9), y+size-ycam+frand(9), 0);
    GFX_TEX_COORD = TEXTURE_PACK(inttot16(0), inttot16(0));
    glVertex3f32(x+size-xcam+frand(9), y-size-ycam+frand(9), 0);
        
    glEnd();

}
