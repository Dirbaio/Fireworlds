#include "ParticleBuffer.h"
#include "input.h"

class Scene;

DTCM_DATA Particle dtcm_pts[300];
#define GFX_RDLINES              (*(vu32*) 0x04000320)

ParticleBuffer::ParticleBuffer(int size, int life, bool dtcm, ParticleBuffer* backBuffer)
{
    bufferSize = size;
    bufferLife = life;
    
    ptsBegin = 0;
    ptsEnd = 0;
    ptsCount = 0;
    this->backBuffer = backBuffer;
    
    if(dtcm)
        pts = dtcm_pts;
    else
        pts = new Particle[size];

}

ParticleBuffer::~ParticleBuffer()
{
    if(pts != dtcm_pts)
        delete[] pts;
}


extern "C" {
    void particleMove(Particle* begin, Particle* end);
    void particleRender(Particle* begin, Particle* end);
}

int xParticleOffs;
int yParticleOffs;
int particlePolyID = 0;


void ParticleBuffer::tick(Scene* sc)
{
}


void ParticleBuffer::render(Scene* sc, int& z)
{
    xParticleOffs = sc->xCam.tof5();
    yParticleOffs = sc->yCam.tof5();
    if(ptsBegin > ptsEnd)
    {
        particleRender(&pts[ptsBegin], &pts[bufferSize]);
        particleRender(&pts[0], &pts[ptsEnd]);
    }
    else
        particleRender(&pts[ptsBegin], &pts[ptsEnd]);

    while(ptsBegin != ptsEnd && pts[ptsBegin].life <= 0)
    {
        ptsBegin++;
        ptsCount--;
        if(ptsBegin == bufferSize) ptsBegin = 0;
    }
}

Particle* ParticleBuffer::addParticle()
{
    int newEnd = ptsEnd+1;
    if(newEnd == bufferSize) newEnd = 0;
    if(GFX_RDLINES < 22 && !runningInEmu) return &fakeParticle;
    if(newEnd == ptsBegin) //Circular buffer full :(
    {
        if(backBuffer != NULL) 
            return backBuffer->addParticle();
        else
            return &fakeParticle;
    }
    else
    {
        Particle* res = &pts[ptsEnd];
        ptsEnd = newEnd;
        ptsCount++;
        res->life = bufferLife;
        res->vx2 = 0;
        res->vy2 = 0;
        return res;
    }
}
