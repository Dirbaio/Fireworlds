#ifndef _PARTBUFFER_H
#define _PARTBUFFER_H

#include "Particle.h"

class Scene;

class ParticleBuffer
{
	public:
	
	Particle *pts;
	Particle fakeParticle;

	int ptsBegin, ptsEnd;
	int ptsCount;
	
	int bufferSize;
	int bufferLife;
	ParticleBuffer* backBuffer;
	
	ParticleBuffer(int size, int life, bool dtcm, ParticleBuffer* backBuffer);
	~ParticleBuffer();
	
	Particle* addParticle();
	void tick(Scene* sc);
	void render(Scene* sc, int& z);
};

#endif