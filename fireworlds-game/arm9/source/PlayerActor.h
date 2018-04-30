#include "FireActor.h"

class PlayerActor : public FireActor
{
    public:
    LevelScene* world;
    bool playerDead;
    int life;
    
    int timeToDie;
    bool dieWin;
    int boostTime;
    bool canBoost;
    PlayerActor(LevelScene* world);
    
    virtual void tick();
    virtual void render();
    virtual void actorCollisionSub(Actor* act, int lev);
    void die();
    bool win();
};