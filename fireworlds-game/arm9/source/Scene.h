
#ifndef _SCENE_H
#define _SCENE_H

class Scene;

#include<nds.h>
#include<list>
#include "Particle.h"
#include "ParticleBuffer.h"
#include "f32.h"

using namespace std;

class ParticleBuffer;
class Actor;
class Level;

class Scene
{
    public:
    list<Actor*> actors;
    
    ParticleBuffer *buffer10;
    ParticleBuffer *buffer20;
    ParticleBuffer *buffer60;
    ParticleBuffer *buffer60b;
    ParticleBuffer *buffer180;
    ParticleBuffer *buffer1000;
    Level* lev;
    
    f32 xCam, yCam;
    f32 oxCam, oyCam;
    f32 vxCam, vyCam;
    int camRelaxTime;
    int cursorTime;
    
    f32 camRotX, camRotY; //Rotation vec.
    
    int time;
    
    f32 touchWorldX, touchWorldY;
    
    Scene* newScene;
    
    const char* topText;
    const char* videoPath;
    
    int textChar;
    int textTime;
    
    int curTextX, curTextY;
    
    int musicNumber;
    
    f32 zoom;
    bool switchesActivated[32];
    bool switchesActive[32];
    
    Scene();
    virtual ~Scene();
    
    virtual void tick();
    void tickParticles();
    virtual void render();
    virtual void renderPerspective();
    virtual void renderUnrotated();
    virtual void onSceneBegin();
    
    Particle* addParticle10();
    Particle* addParticle20();
    Particle* addParticle60();
    Particle* addParticle180();
    Particle* addParticle1000();
    
    void doSceneChange(Scene* nScene);
    bool canChangeScene();
    
    void actorCollisions(Actor* act, int lev);
    
    bool inCam(f32 x, f32 y);
    bool inCamStrict(f32 x, f32 y);
    virtual void calcGravityFor(Actor* act);
    void toWorldCoords(f32& dx, f32& dy, f32 x, f32 y);
    void renderString(const char* str, f32 x, f32 y, int r, int g, int b);
};

#endif
