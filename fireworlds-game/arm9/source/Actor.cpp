#include "Actor.h"
#include<cstdio>
#include "PlayerActor.h"

Actor::Actor(Scene* sc)
{
    this->sc = sc;
    x = 0;
    y = 0;
    vx = 0;
    vy = 0;
    gx = 0;
    gy = 0;
    gvx = 0;
    gvy = 0;
    gvs = 0;
    inWater = false;
    inLava = false;

    size = 8;
    dead = false;

    collisionType = COLLISION_NONE;
    collisionOnlyPlayer = false;
    collisionOnlyNotify = false;
    isPlayer = false;
    bottomCollision = false;
    collisionPriority = 0;
    pushable = false;
}

void Actor::tick(){}
void Actor::render(){}
void Actor::renderPerspective(){}
void Actor::renderUnrotated(){}

void Actor::move(f32 xi, f32 yi)
{
    moveSub(xi, yi, 0, true);
}

void Actor::moveSub(f32 xi, f32 yi, int lev, bool pushback)
{
    x += xi;
    y += yi;
    
    if(!pushback) return;
    
    inWater = false;
    inLava = false;
    
    f32 oldgx = gx; //This helps onGround() be reliable :P
    f32 oldgy = gy;
    
    gx = 0;
    gy = 0;
    
    if(sc->lev)
    {
        if(bottomCollision)
        {
            y -= size;
            x -= size;
            sc->lev->fixCoords(this);
            x += size*2;
            sc->lev->fixCoords(this);
            y += size*2;
            sc->lev->fixCoords(this);
            x -= size*2;
            sc->lev->fixCoords(this);
            y -= size;
            x += size;
        }
        else
            sc->lev->fixCoords(this);
    }
    if(onGround()) groundTime = 0;
    groundTime++;
    if(!onGround() && groundTime < 10)
    {
        gx = oldgx;
        gy = oldgy;
    }

    sc->lev->liquidCollisions(this);
    sc->actorCollisions(this, lev);
}


void Actor::calcGravityVec()
{
    sc->calcGravityFor(this);
}

bool Actor::onGround()
{
    return gx != 0 || gy != 0;
}

bool Actor::onScreen()
{
    return sc->inCam(x, y);
}
bool Actor::onScreenStrict()
{
    return sc->inCamStrict(x, y);
}


bool Actor::closeTo(Actor* act, f32 sz)
{
    f32 dx = act->x - x;
    f32 dy = act->y - y;
    if(fabs(dx) > sz) return false;
    if(fabs(dy) > sz) return false;
    
    f32 dist_sq = dx*dx+dy*dy;
    return dist_sq <= sz*sz;
}

//A collides with B in no particular order.
void Actor::actorCollision(Actor* act)
{
    actorCollisionSub(act, 0);
}

void Actor::actorCollisionSub(Actor* act, int lev)
{
    if(act->collisionType == COLLISION_NONE) return;
    if(this->collisionType == COLLISION_NONE) return;
    
    if(this->collisionOnlyNotify || act->collisionOnlyNotify)
    {
        if(closeTo(act, size+act->size))
            notifyCollision(act);
    }
    else
    {
        if(collisionPriority == act->collisionPriority )
        {
            if(pushable)
                act->pushActor(this, lev, true);
            else
                this->pushActor(act, lev, false);
        }
        else if(collisionPriority <= act->collisionPriority )
            act->pushActor(this, lev, false);
        else
            this->pushActor(act, lev, true);
    }
}

void Actor::pushActor(Actor* act, int lev, bool pushback)
{
    if(lev > 7) return;
    
    bool collision = false;
    
    int collisionTypeOld = collisionType;
    collisionType = COLLISION_NONE;
    
    if(collisionTypeOld == COLLISION_ROUND && act->collisionType == COLLISION_ROUND)
        collision = roundCollision(act, size+act->size, lev+1, pushback);
    else
        collision = squareCollision(act, size+act->size, lev+1, pushback);
    
    collisionType = collisionTypeOld;
    if(collision)
    {
        //Okay. act has been pushed by me. 
        //Maybe I couldn't push him so I have to be pushed back.
        if(pushback)
            act->pushActor(this, lev+1, false);
        
        notifyCollision(act);
    }
}

void Actor::notifyCollision(Actor* act)
{
    collidedWithActor(act);
    act->collidedWithActor(this);
    
    if(act->isPlayer)
        collidedWithPlayer((PlayerActor*) act); //Shit unsafe cast, I know...
    if(isPlayer)
        act->collidedWithPlayer((PlayerActor*) this);
}

bool Actor::squareCollision(Actor* act, f32 size, int lev, bool pushback)
{
    if(act->x >= x+size) return false;
    if(act->x <= x-size) return false;
    if(act->y >= y+size) return false;
    if(act->y <= y-size) return false;
    f32 d1 = fabs(act->y - (y+size));
    f32 d2 = fabs(act->x - (x+size));
    f32 d3 = fabs(act->y - (y-size));
    f32 d4 = fabs(act->x - (x-size));

    int min = 0;
    f32 minv = d1;
    if(d2 < minv) minv = d2, min = 1;
    if(d3 < minv) minv = d3, min = 2;
    if(d4 < minv) minv = d4, min = 3;
    
    switch(min)
    {
        case 0: //Top
            act->moveSub(0, y+size-act->y, lev, pushback);
            act->vy = 0;
            act->gx = 0;
            act->gy = 1;
            break;
        case 1: //Top
            act->moveSub(x+size-act->x, 0, lev, pushback);
            act->vx = 0;
            act->gx = 1;
            act->gy = 0;
            break;
        case 2: //Top
            act->moveSub(0, y-size-act->y, lev, pushback);
            act->vy = 0;
            act->gx = 0;
            act->gy = -1;
            break;
        case 3: //Top
            act->moveSub(x-size-act->x, 0, lev, pushback);
            act->vx = 0;
            act->gx = -1;
            act->gy = 0;
            break;
    }

    act->groundTime = 0;
    return true;
}

bool Actor::roundCollision(Actor* act, f32 size, int lev, bool pushback)
{
    f32 dx = act->x - x;
    f32 dy = act->y - y;
    
    //To avoid overflows when ^2
    if(fabs(dx) > size) return false;
    if(fabs(dy) > size) return false;
    
    f32 dist_sq = dx*dx+dy*dy;
    if(dist_sq <= size*size)
    {
        vecNormalize(dx, dy);
        
        act->x = x+dx*size;
        act->y = y+dy*size;
        
        act->gx = dx;
        act->gy = dy;
        act->groundTime = 0;
        
        vecProject(act->vx, act->vy, act->gy, -act->gx);
        return true;
    }
    else return false;
}

void Actor::collidedWithPlayer(PlayerActor* act) {}
void Actor::collidedWithActor(Actor* act) {}
