#include "IronActor.h"
#include "stdio.h"

#include "music.h"
#include "textures.h"
#include "polyids.h"
#include "PlayerActor.h"


IronActor::IronActor(Scene* world) : Actor(world)
{
    size = 24;
    touchedTime = -1;

    collisionType = COLLISION_SQUARE;
    collisionOnlyPlayer = false;
    bottomCollision = true;
    pushable = true;
    collisionPriority = -10; //This means that ice blocks can push the player, and the player cant push ice blocks
}

void IronActor::tick()
{
    
    calcGravityVec();
    vy += gvy*gvs;
    vx += gvx*gvs;
    vy *= f32(0.98);
    
    if(onGround())
        vx *= f32(0.8);
    else
        vx *= f32(0.99);
    
    if(inLava)
    {
        vx -= gvx*f32(0.6);
        vy -= gvy*f32(0.6);
    } 
    move(vx, vy);
    
    Actor::tick();

    if(!onScreen()) return;
    
    for(int i = 0; i < 2; i++)      
    {
        Particle* p2 = sc->addParticle60();
        p2->r = 255;
        p2->g = 170+sinLerp(sc->time*400+(x*100+y*100).toint())/64;
        p2->b = 14;
        p2->a = 18;
        p2->x = (x+frand(12)).tof5();
        p2->y = (y+frand(12)).tof5();

        p2->vx = (vx+frand(1)).tof5();
        p2->vy = (vy+frand(1)).tof5();

        p2->nTexture = 1;
        p2->sizePerLife = 6;
        p2->effect = FX_RED;
    }
}


void IronActor::render()
{
    if(!onScreen()) return;
    if(size == 0) return;
    
    f32 xcam = sc->xCam;
    f32 ycam = sc->yCam;
    
    setTexture(TEX_IRON);
    glPolyFmt(POLY_ALPHA(31) | POLY_ID(OBJ1_POLYID) | POLY_CULL_NONE);

    //draw the obj
    glColor3b(255, 170+sinLerp(sc->time*400+(x*100+y*100).toint())/64, 0);
    glBegin(GL_QUAD);

    GFX_TEX_COORD = TEXTURE_PACK(inttot16(64), inttot16(0));

    int r = 0;

    glVertex3f32(x-size-xcam+frand(r), y-size-ycam+frand(r), 0);
    GFX_TEX_COORD = TEXTURE_PACK(inttot16(64), inttot16(64));
    glVertex3f32(x-size-xcam+frand(r), y+size-ycam+frand(r), 0);
    GFX_TEX_COORD = TEXTURE_PACK(inttot16(0), inttot16(64));
    glVertex3f32(x+size-xcam+frand(r), y+size-ycam+frand(r), 0);
    GFX_TEX_COORD = TEXTURE_PACK(inttot16(0), inttot16(0));
    glVertex3f32(x+size-xcam+frand(r), y-size-ycam+frand(r), 0);
        
    glEnd();
}


void IronActor::collidedWithPlayer(PlayerActor* act)
{
}
