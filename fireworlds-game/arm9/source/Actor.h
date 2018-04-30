
#ifndef _ACTOR_H
#define _ACTOR_H

#include<nds.h>
class Actor;
#include "Level.h"
#include "LevelScene.h"

using namespace std;
#include "behaviors.h"

class PlayerActor;

#define COLLISION_NONE 0
#define COLLISION_SQUARE 1
#define COLLISION_ROUND 2

class Actor
{
    public: 
    
    //Position, velocity, ground, gravity, etc.
    f32 x, y; //Posicion obj
    f32 vx, vy; //Velocidad
    f32 gx, gy; //Vector unit perpendicular al suelo.
    f32 gvx, gvy; //Vector gravedad.
    f32 gvs;
    
    //Collision stuff!!
    f32 size;
    int collisionType;
    bool collisionOnlyNotify;
    bool collisionOnlyPlayer; //Doesn't collide with the player, just notifies. 
    bool isPlayer; //Is true if this is a playeractor.
    bool bottomCollision; //Makes the level collision point the bottom instead of the center. It breaks with weird gravities.
    int groundTime; //Time since last onGround.
    int collisionPriority; //Higher means it can push other objects. Default/player is 0.
    bool pushable;
        
    bool inWater;
    bool inLava;

    //Scene stuff.
    Scene* sc;
    bool dead; //When set to true, the scene will automatically delete this actor.
    
    
    
    Actor(Scene* world);
    
    virtual void tick();
    virtual void render();
    virtual void renderPerspective();
    virtual void renderUnrotated();
    void move(f32 xi, f32 yi);
    void moveSub(f32 xi, f32 yi, int lev, bool pushback);
//  virtual bool levelCollide(int x, int y);
    void calcGravityVec();
    
    bool onGround();
    bool onScreen();
    bool onScreenStrict();
    bool closeTo(Actor* act, f32 dist);
    
    //a.actorCollision(b) means the actor B is moving, and we have to chech whether B
    //                    collides with A. The default implementation uses fields size and collisionType
    
    virtual void actorCollision(Actor* act);
    virtual void actorCollisionSub(Actor* act, int lev);
    void pushActor(Actor* act, int lev, bool pushback);
    
//  virtual void playerCollision(PlayerActor* act);
    
    virtual void collidedWithPlayer(PlayerActor* act);
    virtual void collidedWithActor(Actor* act);
    
    void notifyCollision(Actor* act);
    
    //These are "standard" collision types that can be reused
    bool squareCollision(Actor* act, f32 size, int lev, bool pushback);
    bool roundCollision(Actor* act, f32 size, int lev, bool pushback);
};

#endif