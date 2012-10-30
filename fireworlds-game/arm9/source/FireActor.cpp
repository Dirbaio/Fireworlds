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
		for(int i = 0; i < 2; i++)		
		{
			Particle* p2 = sc->addParticle60();
			p2->r = r;
			p2->g = g;
			p2->b = b;
			p2->a = 18;
			p2->x = (x+frand(firesize*2)).tof5();
			p2->y = (y+frand(firesize*2)).tof5();

			p2->vx = (vx+frand(1)).tof5();
			p2->vy = (vy+frand(1)).tof5();

			if(t > 15)
			{
				p2->vx += (p2->x-x.tof5()) /50;
				p2->vy += (p2->y-y.tof5()) /50;
			}
			p2->nTexture = 1;
			p2->sizePerLife = 6;
			p2->effect = FX_RED;
		}
	
	if(activePlayer)
	{
		if(musicTrackPlayed[1])
		{
			for(int ang = 0; ang < DEGREES_IN_CIRCLE; ang+=DEGREES_IN_CIRCLE / 30)
			{
				Particle *p2 = sc->addParticle60();
				p2->vx = vx.tof5()+sinLerp(ang)/40+irand(10);
				p2->vy = vy.tof5()+cosLerp(ang)/40+irand(10);
				p2->x = x.tof5()+p2->vx*3;
				p2->y = y.tof5()+p2->vy*3;
				if(rand() % 10 == 0)
				{
					p2->vx = vx.tof5()+(rand() % 256*4) - 128*4;
					p2->vy = vy.tof5()+(rand() % 256*4) - 128*4;
				}
				p2->nTexture = 1;
				p2->r = r;
				p2->g = g;
				p2->b = b;
				p2->a = 18;
				p2->sizePerLife = 7;
				p2->effect = FX_RED;
			}
		}

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
				p2->sizePerLife = -4;
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

//	move(vx, vy);
	
	
}



void FireActor::activated()
{
	for(int ang = 0; ang < DEGREES_IN_CIRCLE; ang+=DEGREES_IN_CIRCLE / 30)
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
		p2->r = r;
		p2->g = g;
		p2->b = b;
		p2->a = 18;
		p2->sizePerLife = 7;
		p2->effect = FX_RED;
	}
}

void FireActor::render()
{

	f32 size = 4*14 + t*8;
	f32 xcam = sc->xCam;
	f32 ycam = sc->yCam;
	
	if(!activeFire) return;
	if(!activePlayer) return;
	if(!onScreen()) return;

	setTexture(1);
	glPolyFmt(POLY_ALPHA(15+irand(3)) | POLY_ID(FIREGLOW_POLYID+colorShift) | POLY_CULL_NONE);
	//draw the obj
	glColor3b(r/2+128,g/2+128,b/2+128);
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