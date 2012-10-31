#include "PlayerActor.h"
#include <cstdio>
#include "input.h"


#define MAX_LIFE 28

PlayerActor::PlayerActor(LevelScene* world) : FireActor(world)
{
	this->world = world;
	playerDead = false;
	activePlayer = false;
	timeToDie = -1;
	life = MAX_LIFE;
	
	collisionType = COLLISION_ROUND;
	collisionOnlyPlayer = false;
	isPlayer = true;
	size = 12;
	boostTime = 0;
	canBoost = true;
}


void PlayerActor::tick()
{
	
	if(timeToDie != -1)
	{
		if(timeToDie == 0)
		{
			if(playerDead) return;
			
			playerDead = true;
			dead = true;
			if(dieWin)
				world->playerWon(this);
			else
				world->playerDied(this);
		}
		timeToDie--;
		collisionType = COLLISION_NONE;
		return;
	}
	
	if(boostTime > 0)
	{
//		if(sc->time % 5 == 0) activated();
		boostTime--;
	}

	FireActor::tick();
	
	activePlayer = world->player == this;
	colorShift = activePlayer?1:0;
	calcGravityVec();
	f32 vy2 = vecProjectionLen(vx, vy, gvx, gvy);
	f32 vx2 = vecProjectionLen(vx, vy, gvy, -gvx);


	//Gravedad.
	if(inWater || inLava)
		vy2 *= f32(0.9);
	
	if((inputKeysHeld & KEY_A) && activePlayer) 
		vy2 += gvs/2;
	else
		vy2 += gvs;

	f32 accel;
	
	if(boostTime != 0)
	{
//		vx2 *= f32(0.999);
		accel = 0.2;
	}
	else if(onGround())
	{
		vx2 *= f32(0.8);
		accel = 0.4;
	}
	else
	{
		vx2 *= f32(0.94);
		accel = 0.08;
	}
	
	vy2 *= f32(0.97);

	f32 nvx = 0;
	f32 nvy = 0;
	if(activePlayer)
	{
		if(inputKeysHeld & KEY_LEFT) nvx -= accel;
		if(inputKeysHeld & KEY_RIGHT) nvx += accel;
		if(inputKeysHeld & KEY_DOWN) nvy -= accel;
		if(inputKeysHeld & KEY_UP) nvy += accel;
	}
	
	//z = -roty + i*rotx
	//r = nvx +i*nvy
	//zr = -nvx*roty - nvy*rotx + (i*rotx*nvx - i*roty*nvy)
	
	f32 nvxt = -nvx*sc->camRotY + -nvy*sc->camRotX;
	f32 nvyt = nvx*sc->camRotX + -nvy*sc->camRotY;
	
	vx2 += vecProjectionLen(nvxt, nvyt, gvy, -gvx);
	

	if(onGround() || inWater || inLava) canBoost = true;
	if((inputKeysDown & KEY_B) && activePlayer && fabs(vx2) > 0.5 && canBoost)
	{
		boostTime = 20;
		vx2 *= 8; 
		if(vx2 > 3.3) vx2 = 3.3;
		if(vx2 < -3.3) vx2 = -3.3;
		canBoost = false;
	}


	if((inputKeysHeld & KEY_A) && activePlayer && inLava)
	{
		vy2 -= 0.6;
	}
	
	vx = vy2*gvx + vx2*gvy;
	vy = vy2*gvy - vx2*gvx;

	
	if((inputKeysDown & KEY_A) && activePlayer && onGround())
	{
		activated();
		vx += gx*5.2;
		vy += gy*5.2;
		groundTime = 100;
	}
	
	f32 max = 8;
	if(vx > max) vx = max;
	if(vx < -max) vx = -max;
	if(vy > max) vy = max;
	if(vy < -max) vy = -max;

	move(vx, vy);
	if(inWater) life--;
	else life++;
	
	if(life > MAX_LIFE) life = MAX_LIFE;

	activeFire = true;
	
	if(life != MAX_LIFE) activeFire = sc->time%8 == 0;
	
	
	if(life <= 0)
		die();
		/*
	if((inputKeysDown & KEY_X) && activePlayer)
	{
		die();
		inputKeysDown &= ~KEY_X;
	}*/
		
//	x += vx;
//	y += vy;
}

void PlayerActor::die()
{
	if(timeToDie != -1) return;

	timeToDie = 60;
	dieWin = false;
	
	for(int i = 0; i < 70; i++)		
	{
		Particle* p2 = sc->addParticle60();
		p2->r = r;
		p2->g = g;
		p2->b = b;
		p2->a = 18;
		p2->x = (x+frand(size*2)).tof5();
		p2->y = (y+frand(size*2)).tof5();

		p2->vx = (frand(5)).tof5();
		p2->vy = (frand(5)).tof5();

		if(t > 15)
		{
			p2->vx += (p2->x-x.tof5()) /50;
			p2->vy += (p2->y-y.tof5()) /50;
		}
		p2->nTexture = 1;
		p2->sizePerLife = 9;
		p2->effect = FX_RED;
	}
	
	for(int i = 0; i < 40; i++)
	{
		Particle *p2 = sc->addParticle180();
		p2->x = x.tof5();
		p2->y = y.tof5();
		p2->vx = frand(6).tof5();
		p2->vy = frand(6).tof5();
		p2->nTexture = 1;
		p2->sizePerLife = -20;
		p2->r = 255;
		p2->g = 255;
		p2->b = 255;
		p2->effect = FX_ALPHA;

	}	
}

void PlayerActor::render()
{
	if(timeToDie == -1)
		FireActor::render();
}

bool PlayerActor::win()
{
	if(timeToDie != -1) return false;
	if(!world->energyCompleted()) return false;
	timeToDie = 70;
	dieWin = true;
	collisionType = COLLISION_NONE;
	return true;
}


void PlayerActor::actorCollisionSub(Actor* act, int lev)
{
	if(act->isPlayer)
	{
		int sz = 40;
		
		f32 dx = act->x - x;
		f32 dy = act->y - y;
		if(fabs(dx) > sz) return;
		if(fabs(dy) > sz) return;
		
		f32 dist_sq = dx*dx+dy*dy;
		if(dist_sq <= sz*sz)
		{
			vecNormalize(dx, dy);
			
			f32 dy2 = vecProjectionLen(dx, dy, gvx, gvy);
			f32 dx2 = vecProjectionLen(dx, dy, gvy, -gvx);
			
			if(fabs(dx2) < f32(0.4))
				dx2 = 0;
				
			dx = dy2*gvx + dx2*gvy;
			dy = dy2*gvy - dx2*gvx;
			vecNormalize(dx, dy);

			act->x = x+dx*sz;
			act->y = y+dy*sz;
			act->gx = dx;
			act->gy = dy;
			vecProject(act->vx, act->vy, act->gy, -act->gx);
	//		vx = act->vx;
	//		vy = act->vy;
		}
	}
	else Actor::actorCollisionSub(act, lev);
}
