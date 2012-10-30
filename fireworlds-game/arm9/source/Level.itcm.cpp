#include "Level.h"
#include <stdio.h>
#include <cstdlib>
#include "textures.h"
#include "behaviors.h"
#include "LevelScene.h"
#include "f32.h"
#include "polyids.h"
#include "input.h"

Level::Level(const char* file, Scene* sc)
{
	this->sc = sc;
	
	FILE *f = fopen(file, "rb");
	fseek(f, 0, SEEK_END);

	int size = ftell(f);

	fseek(f, 0, SEEK_SET);
	levelFile = new u8[size+1];
	fread(levelFile, 1, size, f);

	fclose(f);
	
	main = (LevelStruct*) levelFile;
	objs = (ObjectsBlock*) (levelFile + main->objectsOffs);
	acts = (ActorsBlock*) (levelFile + main->actorsOffs);
	levelText = (const char*) (levelFile + main->textOffs);
	
	sparkleTime = 0;
//	st = (LevelStruct*)testlevel_bin;
}

Level::~Level()
{
	if(levelFile != NULL)
		delete levelFile;
}

void Level::render(f32 x, f32 y)
{
	int bx = (x.toint()-128) / 256;
	int by = (y.toint()-96) / 256;
	
	for(int xx = bx-2; xx <= bx+2; xx++)
		for(int yy = by -2; yy  <= by+2; yy++)
		{
			if(xx < 0) continue;
			if(yy < 0) continue;
			if(xx >= 32) continue;
			if(yy >= 32) continue;
			
			renderBlock(xx, yy, x, y, true);
		}

	for(int xx = bx-2; xx <= bx+2; xx++)
		for(int yy = by -2; yy  <= by+2; yy++)
		{
			if(xx < 0) continue;
			if(yy < 0) continue;
			if(xx >= 32) continue;
			if(yy >= 32) continue;
			
			renderBlock(xx, yy, x, y, false);
		}
}

void Level::tick(f32 x, f32 y)
{
	if(sparkleTime > 0) sparkleTime--;
	
	int bx = (x.toint()-128) / 256;
	int by = (y.toint()-96) / 256;
	
	for(int xx = bx-1; xx <= bx+1; xx++)
		for(int yy = by -1; yy  <= by+1; yy++)
		{
			if(xx < 0) continue;
			if(yy < 0) continue;
			if(xx >= 32) continue;
			if(yy >= 32) continue;
			
			tickBlock(xx, yy, x, y);
		}
}

void renderObject(LevelObj& obj, f32 xx, f32 yy, Scene* sc)
{
	int x = xx.toint();
	int y = yy.toint();
	const int z = 1000;

		if(obj.type == BEH_WATER || obj.type == BEH_LAVA || obj.type == BEH_WATERDOWN || obj.type == BEH_LAVADOWN)
		{
			bool top = obj.type == BEH_WATER || obj.type == BEH_LAVA;
			bool lava = obj.type == BEH_LAVADOWN || obj.type == BEH_LAVA;

			if(top)
			{
				setTexture(TEX_VERTGRAD);
				glPolyFmt(POLY_ALPHA(28) | POLY_ID(WATER_POLYID) | POLY_CULL_NONE);
				glBegin(GL_QUAD_STRIP);
				int divnum = 32;
				f32 dx = obj.x[1]-obj.x[0];
				f32 dy = obj.y[1]-obj.y[0];
				f32 len = fsqrt(dx*dx+dy*dy);
				if(len < 90) divnum = 16;
				if(len < 45) divnum = 8;
				if(!top) divnum = 1;
				for(int j = 0; j <= divnum; j++)
				{
					int x1 = (obj.x[0]*j+obj.x[1]*(divnum-j))/divnum;
					int y1 = (obj.y[0]*j+obj.y[1]*(divnum-j))/divnum;
					int x2 = (obj.x[3]*j+obj.x[2]*(divnum-j))/divnum;
					int y2 = (obj.y[3]*j+obj.y[2]*(divnum-j))/divnum;
					
					f32 dx = x2-x1;
					f32 dy = y2-y1;
					f32 len = fsqrt(dx*dx+dy*dy);
					dx /= len;
					dy /= len;
					
					int in = x1*19+y1*14+sc->time*20;
					int in2 = x1*24-y1*18-sc->time*12;
	//				int in2 = x1*18-y1*21-sc->time*15;
					if(top)
					{
						int xw = 0;
						if(!lava)
						{
							xw += sinLerp(in*40)/600;
							xw += sinLerp(in2*23)/600;
						}
						else
						{
							xw += sinLerp(in*27)/800;
							xw += sinLerp(in2*13)/300;
						}
						dx *= xw;
						dy *= xw;
						x1 += dx.toint();
						y1 += dy.toint();
						
						if(!lava)
							glColor3b(130, 180, 255);
						else
							glColor3b(255, 180, 30);
					}
					
					GFX_TEX_COORD = TEXTURE_PACK(inttot16(64), inttot16(64));
					glVertex3v16((x1-x)*32, (y1-y)*32, z);

					if(!lava)
						glColor3b(20, 50, 215);
					else
						glColor3b(255, 60, 0);
					GFX_TEX_COORD = TEXTURE_PACK(inttot16(64), inttot16(0));
					glVertex3v16((x2-x)*32, (y2-y)*32, z);
				}
				glEnd();
			}
			else
			{
				if(!lava)
					glColor3b(20, 50, 215);
				else
					glColor3b(255, 60, 0);

				setNoTexture();
				glPolyFmt(POLY_ALPHA(7) | POLY_ID(WATER_POLYID) | POLY_CULL_NONE);
				
				glBegin(GL_QUAD);

				glVertex3v16((obj.x[0]-x)*32, (obj.y[0]-y)*32, z);
				glVertex3v16((obj.x[1]-x)*32, (obj.y[1]-y)*32, z);
				glVertex3v16((obj.x[2]-x)*32, (obj.y[2]-y)*32, z);
				glVertex3v16((obj.x[3]-x)*32, (obj.y[3]-y)*32, z);

				glEnd();
				
			}
		}
		else
		{

			int a = 28;
			if(obj.type == BEH_WALL)
				glColor3b(200, 200, 255);
			else if(obj.type == BEH_BOUNCY)
				glColor3b(255, 100, 210);
			else if(obj.type == BEH_PORTAL1)
				glColor3b(100, 255, 20);
			else if(obj.type == BEH_PORTAL2)
				glColor3b(30, 140, 210);

			if(obj.type == BEH_PORTAL1 && sc->switchesActive[0]) a = 6;
			if(obj.type == BEH_PORTAL2 && sc->switchesActive[1]) a = 6;
			
			if(debugShown)
			{
				if(obj.state == 1)
					glColor3b(0, 200, 0);
				else if(obj.state == 2)
					glColor3b(0, 000, 200);
			}
			
			setTexture(TEX_INVBALL);
			glPolyFmt(POLY_ALPHA(a) | POLY_ID(LEVEL_POLYID) | POLY_CULL_NONE);

			glBegin(GL_QUAD);

			GFX_TEX_COORD = TEXTURE_PACK(inttot16(64), inttot16(0));
			glVertex3v16((obj.x[0]-x)*32, (obj.y[0]-y)*32, z);
			GFX_TEX_COORD = TEXTURE_PACK(inttot16(64), inttot16(64));
			glVertex3v16((obj.x[1]-x)*32, (obj.y[1]-y)*32, z);
			GFX_TEX_COORD = TEXTURE_PACK(inttot16(0), inttot16(64));
			glVertex3v16((obj.x[2]-x)*32, (obj.y[2]-y)*32, z);
			GFX_TEX_COORD = TEXTURE_PACK(inttot16(0), inttot16(0));
			glVertex3v16((obj.x[3]-x)*32, (obj.y[3]-y)*32, z);

			glEnd();
		}
}
void Level::renderBlock(int bx, int by, f32 xx, f32 yy, bool walls)
{
	
	int offs = objs->blocks[bx][by].offs;
	int count = objs->blocks[bx][by].count;
	
	for(int i = 0; i < count; i++)
	{
		LevelObj& obj = objs->objs[offs+i];
		
		if(walls)
		{
			if(obj.type != BEH_WALL) continue;
		}
		else
		{
			if(obj.type == BEH_WALL) continue;
		}

		renderObject(obj, xx, yy, sc);
	}
}


void Level::tickBlock(int bx, int by, f32 x, f32 y)
{
	int offs = objs->blocks[bx][by].offs;
	int count = objs->blocks[bx][by].count;
	
	for(int i = 0; i < count; i++)
	{
		LevelObj& obj = objs->objs[offs+i];
		obj.state = 0;
		if(obj.type == BEH_WATER || obj.type == BEH_LAVA || obj.type == BEH_WATERDOWN || obj.type == BEH_LAVADOWN)
		{
			bool top = obj.type == BEH_WATER || obj.type == BEH_LAVA;
			bool lava = obj.type == BEH_LAVADOWN || obj.type == BEH_LAVA;

			if(top)
			{
				int j = rand() % 128;
				int x1 = (obj.x[0]*j+obj.x[1]*(128-j))/128;
				int y1 = (obj.y[0]*j+obj.y[1]*(128-j))/128;
				int x2 = (obj.x[3]*j+obj.x[2]*(128-j))/128;
				int y2 = (obj.y[3]*j+obj.y[2]*(128-j))/128;
				
				f32 dx = x1-x2;
				f32 dy = y1-y2;
				f32 len = fsqrt(dx*dx+dy*dy);
				dx /= len;
				dy /= len;
				
				int in = x1*19+y1*14+sc->time*20;
				int in2 = x1*24-y1*18-sc->time*12;

				int xw = 0;
				xw += sinLerp(in*40)/600;
				xw += sinLerp(in2*23)/600;

				x1 += (dx*xw).toint();
				y1 += (dy*xw).toint();
				
				if(sc->inCam(x1, y1))
				{
					Particle *star = sc->addParticle20();
					star->x = x1*32;
					star->y = y1*32;
					if(lava)
					{
						star->vx = (dx*100).toint() + rand()%100-50;
						star->vy = (dy*100).toint() + rand()%100-50;
						star->r = 200+rand() % 50;
						star->g = 120+rand() % 50;
						star->b = 50;
						star->effect = FX_BLUR;
					}
					else
					{
						int d = (rand()%2)*2-1;
						star->vx = -(dy*20).toint()*d + rand()%30-15;
						star->vy = (dx*20).toint()*d + rand()%30-15;
						star->r = 205+rand() % 50;
						star->g = 205+rand() % 50;
						star->b = 255;
						star->effect = FX_NONE;
						
					}
					star->vx2 = 0;
					star->vy2 = 0;
					star->nTexture = 3;
					star->a = 10+rand() % 21;
					star->sizePerLife = 12+rand() % 10;
				}
			}
			
			if(sc->time % 3 != 0) continue;

			int xMin = obj.x[0];
			int xMax = obj.x[0];
			int yMin = obj.y[0];
			int yMax = obj.y[0];
			
			for(int i = 1; i < 4; i++)
			{
				if(obj.x[i] < xMin) xMin = obj.x[i];
				if(obj.x[i] > xMax) xMax = obj.x[i];
				if(obj.y[i] < yMin) yMin = obj.y[i];
				if(obj.y[i] > yMax) yMax = obj.y[i];
			}
			
			int xs = xMax-xMin;
			int ys = yMax-yMin;
	
			int x = xMin + rand() % xs;
			int y = yMin + rand() % ys;
			if(!collisionWith(obj, x, y)) continue;
			
			Particle *star = sc->addParticle20();
			star->x = x*32;
			star->y = y*32;
			star->vx = rand() % 100 -50;
			star->vy = rand() % 100 -50;
			if(lava)
			{
				star->r = 200+rand() % 50;
				star->g = 120+rand() % 50;
				star->b = 50;				
			}
			else
			{
				star->r = 205+rand() % 50;
				star->g = 205+rand() % 50;
				star->b = 255;
			}

			star->nTexture = 3;

			star->a = 10+rand() % 21;
			star->sizePerLife = 4+rand() % 6;
			
			star->effect = FX_NONE;
		}
		else
		{
		}
	}
}


inline int whatSide(LevelObj& obj, int a, int b, int x, int y)
{
    return (obj.x[b]-obj.x[a])*(y-obj.y[a]) - (obj.y[b]-obj.y[a])*(x-obj.x[a]);
}

inline bool outside(LevelObj& obj, int a, int b, int x, int y)
{
	int r = whatSide(obj, a, b, x, y);
	
	return r > 0;
}

inline bool collisionWith(LevelObj& obj, f32 px, f32 py)
{
	int x = px.toint();
	int y = py.toint();
	if(outside(obj, 0, 1, x, y)) return false;
	if(outside(obj, 1, 2, x, y)) return false;
	if(outside(obj, 2, 3, x, y)) return false;
	if(outside(obj, 3, 0, x, y)) return false;
	
	return true;
}

inline void Level::addSparkles(LevelObj& obj)
{
	if(sparkleTime != 0) return;
	sparkleTime = 15;
	
	int xMin = obj.x[0];
	int xMax = obj.x[0];
	int yMin = obj.y[0];
	int yMax = obj.y[0];
	
	for(int i = 1; i < 4; i++)
	{
		if(obj.x[i] < xMin) xMin = obj.x[i];
		if(obj.x[i] > xMax) xMax = obj.x[i];
		if(obj.y[i] < yMin) yMin = obj.y[i];
		if(obj.y[i] > yMax) yMax = obj.y[i];
	}
	
	int xs = xMax-xMin;
	int ys = yMax-yMin;
	
	int ct = 0;
	while(ct < 70)
	{
		int x = xMin + rand() % xs;
		int y = yMin + rand() % ys;
		if(!collisionWith(obj, x, y)) continue;
		
		Particle *star = sc->addParticle60();
		star->x = x*32;
		star->y = y*32;
		star->vx = rand() % 100 -50;
		star->vy = rand() % 100 -50;
		if(rand() % 2 == 0)
		{
			star->vx *= 5;
			star->vy *= 5;
		}
//			star->x = o->x * 16*32 + rand() % (o->tx*16*32);
//			star->y = -o->y * 16*32 - rand() % (o->ty*16*32);
		
		star->nTexture = 3;
		star->r = 150+rand() % 100;
		star->g = 150+rand() % 100;
		star->b = 255;
		star->a = 10+rand() % 21;
		star->sizePerLife = 4+rand() % 6;
		ct++;
	}
}


inline void projectToLine(f32& cx, f32& cy, f32 ax, f32 ay, f32 bx, f32 by)
{ 
	f32 r_numerator = (cx-ax)*(bx-ax) + (cy-ay)*(by-ay);
	f32 r_denomenator = (bx-ax)*(bx-ax) + (by-ay)*(by-ay);
	f32 r = r_numerator / r_denomenator;

    cx = ax + r*(bx-ax);
    cy = ay + r*(by-ay);
}


inline void projectVecToLine(f32& cx, f32& cy, f32 ax, f32 ay, f32 bx, f32 by)
{ 
	f32 r_numerator = cx*(bx-ax) + cy*(by-ay);
	f32 r_denomenator = (bx-ax)*(bx-ax) + (by-ay)*(by-ay);
	f32 r = r_numerator / r_denomenator;

    cx = r*(bx-ax);
    cy = r*(by-ay);
}

inline f32 distToLine(f32  cx, f32 cy, f32 ax, f32 ay, f32 bx, f32 by)
{ 
	f32 r_numerator = (cx-ax)*(bx-ax) + (cy-ay)*(by-ay);
	f32 r_denomenator = (bx-ax)*(bx-ax) + (by-ay)*(by-ay);
	f32 r = r_numerator / r_denomenator;

    f32 px = ax + r*(bx-ax);
    f32 py = ay + r*(by-ay);

    f32 s =  ((ay-cy)*(bx-ax)-(ax-cx)*(by-ay) ) / r_denomenator;

	f32 distanceLine = fabs(s)*fsqrt(r_denomenator);
	return distanceLine;
}	

void Level::fixCoordsWithObj(LevelObj& obj, Actor* act)// f32& xx, f32& yy, f32& vx, f32& vy, f32& gx, f32& gy)
{	
	obj.state = 1;
	f32 px = act->x+act->gvx*2;
	f32 py = act->y+act->gvy*2;
	
	int dists[4];
	dists[0] = distToLine(px, py, obj.x[0], obj.y[0], obj.x[1], obj.y[1]).toint();
	dists[1] = distToLine(px, py, obj.x[1], obj.y[1], obj.x[2], obj.y[2]).toint();
	dists[2] = distToLine(px, py, obj.x[2], obj.y[2], obj.x[3], obj.y[3]).toint();
	dists[3] = distToLine(px, py, obj.x[3], obj.y[3], obj.x[0], obj.y[0]).toint();

	int best1 = -1;
	int bestd;
	for(int i = 0; i < 4; i++)
		if((dists[i] < bestd || best1 == -1)) bestd=dists[i], best1 = i;
		
	int best2 = -1;
	for(int i = 0; i < 4; i++)
		if((dists[i] < bestd || best2 == -1) && i != best1) bestd=dists[i], best2 = i;
	
	bool invalid1 = obj.neighbors[best1] != -1;
	bool invalid2 = obj.neighbors[best2] != -1;
	
	if(invalid1 && invalid2) //Corner: Just collide with the neighbor
		fixCoordsWithObj(objs->objs[obj.neighbors[best1]], act);
	else
	{
		if(invalid1)
		{
			best1 = best2;
			invalid1 = invalid2;
		}
		
		int n = best1;
		int n2 = (n+1)%4;

		f32 oldx = act->x;
		projectToLine(act->x, act->y, obj.x[n], obj.y[n], obj.x[n2], obj.y[n2]);

		f32 fx, fy;
		fx = act->vx;
		fy = act->vy;
		projectVecToLine(fx, fy, obj.y[n2], obj.x[n], obj.y[n], obj.x[n2]);

		bool bounce = obj.type == BEH_BOUNCY || (fabs(fx)+fabs(fy))>8;
		if(!bounce)
		{
			projectVecToLine(act->vx, act->vy, obj.x[n], obj.y[n], obj.x[n2], obj.y[n2]);
			
			fx = -(obj.y[n2]-obj.y[n]);
			fy = obj.x[n2]-obj.x[n];
			vecNormalize(fx, fy);
			
			act->gx = fx;
			act->gy = fy;
			
			if(fabs(act->gx) < 0.4) 
			{
				act->gx = 0;
//				act->vx = 0;
				act->x = oldx;
			}
			
			act->x += act->gx;
			act->y += act->gy;
		}
		else
		{
			addSparkles(obj);
			fx = act->vx;
			fy = act->vy;
			
			f32 nx = -(obj.y[n2]-obj.y[n]);
			f32 ny = obj.x[n2]-obj.x[n];
			vecNormalize(nx, ny);
			
			f32 dotp = -fx*nx-fy*ny;
			
			fx = fx+nx*dotp*2;
			fy = fy+ny*dotp*2;
			
			if(obj.type != BEH_BOUNCY)
			{
				fx /= 2;
				fy /= 2;
			}
			else
			{
				fx *= f32(1.7);
				fy *= f32(1.7);
			}
			
			act->vx = fx + nx/2;
			act->vy = fy + ny/2;
			
			act->x += nx*3;
			act->y += ny*3;
		}
	}
}

void Level::fixCoords(Actor* act)
{
	int bx = act->x.toint() / 256;
	int by = act->y.toint() / 256;
		
	for(int xx = bx-1; xx <= bx; xx++)
		for(int yy = by -1; yy  <= by; yy++)
		{
			if(xx < 0) continue;
			if(yy < 0) continue;
			if(xx >= 32) continue;
			if(yy >= 32) continue;

			int offs = objs->blocks[xx][yy].offs;
			int count = objs->blocks[xx][yy].count;
			
			for(int i = 0; i < count; i++)
			{
				LevelObj& obj = objs->objs[offs+i];
				
				if(isBehaviorSolid(obj.type) && collisionWith(obj, act->x, act->y))
				{
					fixCoordsWithObj(obj, act);
					obj.state = 2;
				}
			}
		}
} 	

void Level::liquidCollisions(Actor* act)
{

	f32 xxx = (act->x+act->gx*5);
	f32 yyy = (act->y+act->gy*5);
	int bx = xxx.toint() / 256;
	int by = yyy.toint() / 256;
		
	for(int xx = bx-1; xx <= bx; xx++)
		for(int yy = by -1; yy  <= by; yy++)
		{
			if(xx < 0) continue;
			if(yy < 0) continue;
			if(xx >= 32) continue;
			if(yy >= 32) continue;

			int offs = objs->blocks[xx][yy].offs;
			int count = objs->blocks[xx][yy].count;
			
			for(int i = 0; i < count; i++)
			{
				LevelObj& obj = objs->objs[offs+i];
				if(!collisionWith(obj, xxx, yyy)) continue;
				
				if(obj.type == BEH_WATER ||obj.type == BEH_WATERDOWN)
					act->inWater = true;
				else if(obj.type == BEH_LAVA ||obj.type == BEH_LAVADOWN)
					act->inLava = true;
			}
		}	
}

bool Level::isBehaviorSolid(int b)
{
	if(b == BEH_WALL) return true;
	if(b == BEH_BOUNCY) return true;
	if(b == BEH_PORTAL1 && !sc->switchesActive[0]) return true;
	if(b == BEH_PORTAL2 && !sc->switchesActive[1]) return true;
	return false;
}

bool isObjOnScreen(LevelObj& obj, int xx, int yy)
{
	int xMin = obj.x[0];
	int xMax = obj.x[0];
	int yMin = obj.y[0];
	int yMax = obj.y[0];
	
	for(int i = 1; i < 4; i++)
	{
		if(obj.x[i] < xMin) xMin = obj.x[i];
		if(obj.x[i] > xMax) xMin = obj.x[i];
		if(obj.y[i] < yMin) yMin = obj.y[i];
		if(obj.y[i] > yMax) yMin = obj.y[i];
	}
	
	if(xMin < xx+128 && xMax > xx-128)
		if(yMin < yy+128 && yMax > yy-128)
			return true;
	
	
	return false;
}



