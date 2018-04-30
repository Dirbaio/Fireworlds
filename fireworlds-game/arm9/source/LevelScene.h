
#ifndef _LEVELSCENE_H
#define _LEVELSCENE_H

class LevelScene;

#include "Scene.h"
#include "Actor.h"
#include <string>
#include "Level.h"

using namespace std;

class PlayerActor;

class LevelScene : public Scene
{
    public:
    
    PlayerActor* player;
    list<PlayerActor*> players;
    int cvx, cvy;
    int transitionTime;
    
    int energyCount;
    int energyNeeded;
    int playerExitCount;
    int playerExitNeeded;
    
    
    int gravType;
    f32 gravCenterX;
    f32 gravCenterY;
    
    int levelNum;
    
    
    bool paused;
    int pauseOption;
    
    bool oldSwitch2;
    
    void go(f32 x, f32 y);
    LevelScene(int levelNum);
    virtual ~LevelScene();
    virtual void tick();
    virtual void render();
    virtual void renderUnrotated();
    virtual void renderPerspective();
    void spawnActors();
    void activatePlayer();
    void playerDied(PlayerActor* p);
    void playerWon(PlayerActor* p);
    void exitLevel(bool win);
    void getEnergy();
    bool energyCompleted();

    virtual void onSceneBegin();
    virtual void calcGravityFor(Actor* act);
};

#endif
