#ifndef _LEVEL_H
#define _LEVEL_H

struct LevelObj;
class Level;

#include<string>
#include<nds.h>
#include "Actor.h"
#include "f32.h"

using namespace std;
class Actor;

struct BlockEntry
{
	u32 offs;
	u32 count;
};

struct LevelObj
{
	int x[4];
	int y[4];
	u16 type;
	u8 state;
	u8 reserved;
	int neighbors[4];
};

struct LevelAct
{
	int x;
	int y;
	int type;
	int data;
};

struct ObjectsBlock
{
	u32 objCount; //The java editor uses it.
	BlockEntry blocks[32][32];
	LevelObj objs[0];
};

struct ActorsBlock
{
	u32 actCount; //The java editor uses it.
	LevelAct acts[0];
};

struct LevelStruct
{
	u32 magic;
	u32 objectsOffs;
	u32 objectsLen;
	u32 actorsOffs;
	u32 actorsLen;
	u32 textOffs;
	u32 textLen;
}; 

class Scene;

class Level
{
	public:
	
	u8* levelFile;
	
	LevelStruct* main;
	ObjectsBlock* objs;
	ActorsBlock* acts;
	
	Scene* sc;
	const char* levelText;
	
	int sparkleTime;
	
	Level(const char* file, Scene* sc);
	~Level();
	
	void render(f32 x, f32 y); //2 f32's
	void renderBlock(int bx, int by, f32 x, f32 y, bool wall);

	void tick(f32 x, f32 y); //2 f32's
	void tickBlock(int bx, int by, f32 x, f32 y);

	LevelObj* collisionBlock(int x, int y, int ox,int oy, int& vxr, int& vyr);
//	bool collision(f32 x, f32 y);

	void fixCoordsWithObj(LevelObj& obj, Actor* act);
	void fixCoords(Actor* act);
	void addSparkles(LevelObj& obj);

	void liquidCollisions(Actor* act);
	
	bool isBehaviorSolid(int b);
};

void renderObject(LevelObj& obj, f32 xx, f32 yy, Scene* sc);
bool isObjOnScreen(LevelObj& obj, int xx, int  yy);

int whatSide(LevelObj& obj, int a, int b, int x, int y);
bool outside(LevelObj& obj, int a, int b, int x, int y);
bool collisionWith(LevelObj& obj, f32 x, f32 y);

void projectVecToLine(f32& cx, f32& cy, f32 ax, f32 ay, f32 bx, f32 by);
void projectToLine(f32& cx, f32& cy, f32 ax, f32 ay, f32 bx, f32 by);
f32 distToLine(f32  x, f32 y, f32 x1, f32 y1, f32 x2, f32 y2);


#endif