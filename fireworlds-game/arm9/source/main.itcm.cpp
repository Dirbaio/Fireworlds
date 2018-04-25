#include <nds.h>
#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "as_lib9.h"
#include <filesystem.h>

#include <subsprites.h>
#include <topbg.h>
#include "music.h"
#include "textures.h"
#include "LevelScene.h"
#include "TestScene.h"
#include "LevelSelectScene.h"
#include "CreditsScene.h"
#include "Level.h"
#include <fat.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "f32.h"
#include <malloc.h>
#include "motionblur.h"
#include "hdma.h"
#include "IntroScene.h"
#include "polyids.h"
#include "sub.h"
#include "saver.h"

#include "video.h"

using namespace std;

#define GFX_RDLINES              (*(vu32*) 0x04000320)

void myCallList(const u32* list) {
	u32 count = *list++;
	DMA_SRC(1) = (u32)list;
	DMA_DEST(1) = 0x4000400;
	DMA_CR(1) = DMA_FIFO | count;
}

int transitionTime;
bool transitioning;
bool transitioningDone;
int inputKeysDown;
int inputKeysHeld;
int inputKeysRepeat;
touchPosition inputTouch;

bool runningInEmu;
bool debugShown = true;
bool slowMotion = false;
bool doMotionBlur = true;

Scene* sc;

void VblankHandler(void)
{
	if(transitioning)
		hdma_start();
	else
	{
		DMA_CR(0) = 0;
		REG_DISPCNT &= ~DISPLAY_WIN0_ON;
	}
}

extern "C" {
	bool isEmulator() __attribute__ ((noinline)) ;
}

/*

		glPushMatrix();
		glRotateYi(rot*100);

		glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK | POLY_FORMAT_LIGHT0 | POLY_FORMAT_LIGHT1 |
			  1<<11) ;

		rot += 1;
		glMaterialf(GL_AMBIENT, RGB15(8,8,8));
		glMaterialf(GL_DIFFUSE, RGB15(8,16,16));
		glMaterialf(GL_SPECULAR, RGB15(8,8,8));
		glMaterialf(GL_EMISSION, RGB15(5,5,5));
		setNoTexture();
//		myCallList((u32*)sphereModel);

		glPopMatrix(1);
*/


void changeSceneIfNeeded()
{
	if(transitioning)
	{
		transitionTime -= 9;
		if(transitioningDone)
		{
			if(transitionTime <= 0)
				transitioning = false;
			else
			{
				hdma_transition(transitionTime);
			}
		}
		else
		{
			if(transitionTime <= 0)
			{
				Scene* nw = sc->newScene;
				delete sc;
				sc = nw;

				subClear();
				sc->onSceneBegin();

				transitioningDone = true;
				transitionTime = 255;
				AS_SetMP3Volume(127);
			}
			else
			{
				if(sc->musicNumber != sc->newScene->musicNumber)
					AS_SetMP3Volume(transitionTime / 2);

				hdma_transition(255- transitionTime);
			}
		}
	}
	else if(sc->newScene != NULL) //Begin transition
	{
		transitionTime = 255;
		transitioning = true;
		transitioningDone = false;
	}
}

void renderScenePerspective()
{
	//Perspective projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70, 256.0 / 192.0, 0.1, 80);
	glMatrixMode(GL_MODELVIEW);

	glPushMatrix();
	glTranslatef32(0, -200, -10000);
	glRotateXi(700);
	sc->renderPerspective();
	glPopMatrix(1);
}

void renderSceneOrtho()
{
	//Ortho projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	int xs = 256;
	int ys = 192;
	int zoom = 16; //higher means zoom out
	glOrthof32(-xs*zoom, xs*zoom, -ys*zoom, ys*zoom, -100000, 100000);
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	glPushMatrix();
	int cosine = -sc->camRotY.raw();
	int sine = -sc->camRotX.raw();

	MATRIX_MULT3x3 = cosine;
	MATRIX_MULT3x3 = sine;
	MATRIX_MULT3x3 = 0;

	MATRIX_MULT3x3 = - sine;
	MATRIX_MULT3x3 = cosine;
	MATRIX_MULT3x3 = 0;

	MATRIX_MULT3x3 = 0;
	MATRIX_MULT3x3 = 0;
	MATRIX_MULT3x3 = inttof32(1);

	glScalef32(sc->zoom.raw(), sc->zoom.raw(), sc->zoom.raw());
	sc->render();
	glPopMatrix(1);
	sc->renderUnrotated();
}


void copy(const void* src, void* dest, int len)
{
	const u16* srcu = (u16*) src;
	u16* dstu = (u16*) dest;

	for(int i = 0; i < len / 2; i++)
	{
		dstu[i] = srcu[i];
	}
}

void printDebugStuff()
{

	//Debug
	int shit = BUS_CLOCK / 100 / 60 / 1024;
	int dd = timerElapsed(0)/shit;

	iprintf("\x1b[1;1HFireworlds beta - By Dirbaio!");
	iprintf("\x1b[3;1HParticle buffers: %d %d %d/%d \n %d %d    ",
		sc->buffer10->ptsCount,
		sc->buffer20->ptsCount,
		sc->buffer60->ptsCount, sc->buffer60b->ptsCount,
		sc->buffer180->ptsCount,
		sc->buffer1000->ptsCount);

	printf("\x1b[10;1HActors:  %d         ", sc->actors.size());
	printf("\x1b[11;1HCPU:  %d %%        ", dd);
	printf("\x1b[12;1HCPU7: %d %%        ", PA_Transfer->cpuUsage);
	printf("\x1b[13;1HGFX_RDLINES: %d        ", GFX_RDLINES );
	printf("\x1b[14;1HVertex RAM: %d        ", GFX_VERTEX_RAM_USAGE);
	printf("\x1b[15;1HPolygon RAM: %d        ", GFX_POLYGON_RAM_USAGE);
	printf("\x1b[16;1HEmulator?: %d        ", runningInEmu);

	struct mallinfo inf = mallinfo();
	printf("\x1b[8;1HRAM  %d       ", inf.uordblks);

}

void error(const char* msg)
{
	iprintf(msg);
	while(1)
		swiWaitForVBlank();
}

f32 test_rand1()
{
	return frand(37);
}
f32 test_rand2()
{
	return frand(16);
}

int main()
{
    vramSetBankA(VRAM_A_LCD);
    vramSetBankB(VRAM_B_LCD);
	vramSetBankC(VRAM_C_LCD);
    vramSetBankD(VRAM_D_LCD);
    vramSetBankE(VRAM_E_LCD);
    vramSetBankF(VRAM_F_LCD);
    vramSetBankG(VRAM_G_LCD);
    vramSetBankH(VRAM_H_LCD);
    vramSetBankI(VRAM_I_LCD);

	//Reset heap tracker.
//	*((vu32*)0x04FFF020) = 1234;


	//defaultExceptionHandler();

	lcdMainOnBottom();
	videoSetModeSub(MODE_0_2D);
	vramSetBankH(VRAM_H_SUB_BG);
	consoleInit(NULL, 0, BgType_Text4bpp, BgSize_T_256x256, 0, 1, false, true );

	if(isEmulator())
		runningInEmu = true;
	else
		runningInEmu = false;

	if(runningInEmu)
	{
		iprintf("WARNING: Running in an\nemulator. This is not\nrecommended.\n\nLoading...\n\n");
//		for(int i = 0; i < 200; i++) swiWaitForVBlank();
/*		iprintf("Press A to continue\n\n");
		while(!keysDown() & KEY_A)
		{
			scanKeys();
			swiWaitForVBlank();
		}*/
	}

	if(!runningInEmu)
	{
		AS_Init(AS_MODE_MP3 | AS_MODE_SURROUND | AS_MODE_16CH);
		AS_SetDefaultSettings(AS_PCM_8BIT, 11025, AS_SURROUND);
	}

	iprintf("Trying to init NitroFS...\n");
	if(!nitroFSInit(NULL))
		error("FATAL ERROR: Couldnt init FAT!\nMake sure you've correctly\nDLDI patched the ROM.\nHalting.");

	iprintf("Trying to open test file...\n");
	FILE* testLevel = fopen("/01.dgl", "rb");
	if(testLevel == NULL) error("FATAL ERROR: Couldnt open test\nfile.Make sure you copied the\ndata folder in the root\nof the microSD.");
	fclose(testLevel);
	iprintf("All Ok! Starting!\n");

    vramSetBankC(VRAM_C_LCD);
	for(int i = 0; i < 128*1024; i++)
		VRAM_C[i] = 0;

	lcdMainOnBottom();
	videoSetModeSub(MODE_0_2D);
	vramSetBankH(VRAM_H_SUB_BG);
	consoleInit(NULL, 0, BgType_Text4bpp, BgSize_T_256x256, 0, 1, false, true );


//	consoleDebugInit(DebugDevice_NOCASH);

	vramSetBankD(VRAM_D_SUB_SPRITE);
	copy(subspritesTiles, SPRITE_GFX_SUB, subspritesTilesLen);
	copy(topbgTiles, SPRITE_GFX_SUB+128*64, topbgTilesLen);

	vramSetBankI(VRAM_I_LCD);
	copy(subspritesPal, VRAM_I_EXT_SPR_PALETTE[0], subspritesPalLen);
	copy(topbgPal, VRAM_I_EXT_SPR_PALETTE[1], topbgPalLen);
	VRAM_I_EXT_SPR_PALETTE[2][1] = RGB15(31, 31, 31);
	vramSetBankI(VRAM_I_SUB_SPRITE_EXT_PALETTE);

	oamInit(&oamSub, SpriteMapping_1D_256 , true);
	oamEnable(&oamSub);
	initSub();

    videoSetMode(MODE_0_3D);

    glInit();
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_ANTIALIAS);
	glEnable(GL_BLEND);

	loadTextures();
	loadSaver();

	// setup the rear plane
	glClearColor(0, 0, 0, 31);
	glClearPolyID(BG_POLYID);
	glClearDepth(0x7FFF);

	glViewport(0,0,255,191);

	swiWaitForVBlank();
	srand(0);

	sc = new IntroScene();
	sc->onSceneBegin();

	//glLight(0, RGB15(20,20,20) , floattov10(1), floattov10(-1.0), -1);
	//glLight(1, RGB15(10,10,16) , floattov10(-1), floattov10(1.0), 0);
	irqSet(IRQ_VBLANK, VblankHandler);

    if(debugShown) bgShow(4);
    else bgHide(4);

	keysSetRepeat(20, 6);
	while(1)
	{

		if(sc->videoPath != NULL)
		{
			oamClear(&oamSub, 0, 0);
			oamUpdate(&oamSub);

			if(videoStart(sc->videoPath))
			{
				while(1)
				{
					if(!videoVBL()) break;
					//READ INPUT
					scanKeys();
					inputKeysDown = keysDown();
					inputKeysHeld = keysHeld();
					inputKeysRepeat = keysDownRepeat();
					touchRead(&inputTouch);
					if(inputKeysDown & KEY_START) break;

					//Music updates
					AS_SoundVBL();
					musicTick();
				}
				videoStop();
				videoSetMode(MODE_0_3D);
				sc->videoPath = NULL;
			}
			transitioning = false;
		}

		timerStart(0, ClockDivider_1024, 0, NULL);

		changeSceneIfNeeded();

		//READ INPUT
		scanKeys();
		inputKeysDown = keysDown();
		inputKeysHeld = keysHeld();
		inputKeysRepeat = keysDownRepeat();
		touchRead(&inputTouch);

		subFrameStart();

		renderBg();

		//Update scene
		sc->tick();

        if(doMotionBlur) {
    		//Do motion blur
    		motionBlurRender();
        }
		renderScenePerspective();
		renderSceneOrtho();

		//Debug!
		if(debugShown)
		{
			printDebugStuff();
			oamClear(&oamSub, 0, 0);
		}

		oamUpdate(&oamSub);
		glFlush(GL_TRANS_MANUALSORT);

		swiWaitForVBlank();
		if(slowMotion)
		{
			swiWaitForVBlank();
			swiWaitForVBlank();
			swiWaitForVBlank();
		}

        if(doMotionBlur) {
    		//Do motion blur for next frame.
    		motionBlurFrame();
    		REG_DISPCAPCNT |= DCAP_ENABLE;
        }

		//Music updates
		AS_SoundVBL();
		musicTick();

		oamClear(&oamSub, 0, 0);

		if(inputKeysDown & KEY_R)
		{
			debugShown = !debugShown;
			if(debugShown) bgShow(4);
			else bgHide(4);
		}

		if(inputKeysDown & KEY_L)
			doMotionBlur = !doMotionBlur;

		int shit = BUS_CLOCK / 100 / 60 / 1024;
		int dd = timerElapsed(0)/shit;
		if(dd > 90)	printf("\x1b[16;1HLAG");
		else printf("\x1b[16;1H...");

		timerStop(0);

	}
}
