#include<string>
#include <stdio.h>
#include "midi.h"
#include "as_lib9.h"
#include "music.h"
#include "input.h"

using namespace std;

int musicTrackPlayed[4];
int musicTrackPlaying[4];

int playingMusic = -1;
const char* filenamesMP3[] = {
   "dotmw.mp3", //0
   "dotmw2.mp3", //1
   "dotmw3.mp3", //2
//   "winter",
//   "unreal",
//   "stepfw",
//   "safari",
   "orbit.mp3", //3
//   "logctrl",
   "latenght.mp3", //4
   "hardtech.mp3", //5
   "happy.mp3", //6
   "dotmw4.mp3", //7
   };

const char* filenamesMID[] = {
   "dotmw.mid",
   "dotmw2.mid",
   "dotmw3.mid",
//   "winter",
//   "unreal",
//   "stepfw",
//   "safari",
   "orbit.mid",
//   "logctrl",
   "latenght.mid",
   "hardtech.mid",
   "happy.mid",
   "dotmw4.mid",
   };

const int syncChannels[][5] = {
	//Melody1, Melody2, percusmain, percussub
	{14, 13, 6, 4}, //dotmw
	{14, 13, 6, 4}, //dotmw2
	{14, 13, 6, 4}, //dotmw3
//	{0, -1, 11, 7}, //winternight
//	{17, 27, 25, 1}, //unreal
//	{-1, 12, 22, 25}, //stepfw
//	{5, -1, 2, 1}, //safari
	{-1, 8, 15, 18}, //orbit
//	{1, -1, 5, 10}, //logctrl
	{15, 0, 5, 7}, //latenght
	{0, -1, 6, 5}, //hardtech
	{17, 11, 3, 6}, //happy


};

char* midifile = NULL;
bool playing = false;

char* loadFile(const char *filename)
{
	FILE *f = fopen(filename, "rb");

	fseek(f, 0, SEEK_END);
	int size = ftell(f);
	fseek(f, 0, SEEK_SET);

	char* result = new char[size+1];
	fread(result, sizeof(char), size, f);

	fclose(f);
	result[size] = 0;
	return result;
}

int playTime[64];


void playMusic(int m)
{
	if(m == playingMusic) return;

//	return;
	for(int i = 0; i < 256; i++)
	{
		midiTrackPlayed[i] = false;
		midiTrackPlaying[i] = false;
	}

	for(int i = 0; i < 4; i++)
		musicTrackPlaying[i] = false;


	for(int i = 0; i < 32; i++)
		playTime[i] = 0;

	if(playing)
	{
		AS_MP3Stop();
		midiStop();
	}

	for(int i = 1; i < 4; i++) swiWaitForVBlank();

	playingMusic = m;
	playing = true;

	AS_MP3StreamPlay(filenamesMP3[m]);
	AS_SetMP3Loop(false);
	AS_MP3Unpause();

	if(midifile != NULL)
		delete[] midifile;

	midifile = loadFile(filenamesMID[m]);
	loadMidi((u8*)midifile);
}

void stopMusic()
{
}

void musicTick()
{
	if(!playing) return;

	if(AS_GetMP3Status() == MP3ST_STOPPED || AS_GetMP3Status() == MP3ST_OUT_OF_DATA)
	{
		midiStop();
		if(playingMusic == 1) {playing = false; return;}

		AS_MP3StreamPlay(filenamesMP3[playingMusic]);
		AS_SetMP3Loop(false);
		AS_MP3Unpause();
		loadMidi((u8*)midifile);
	}

	for(int i = 0; i < 256; i++)
		midiTrackPlayed[i] = false;
	midiVBL();

	for(int i = 0; i < 4; i++)
	{
		int ch = syncChannels[playingMusic][i];
		if(ch != -1)
		{
			musicTrackPlayed[i] = midiTrackPlayed[ch];
			musicTrackPlaying[i] = midiTrackPlaying[ch];
		}
	}

	printf("\x1b[20;1H");

	for(int i = 0; i < 64; i++)
	{
		if(playTime[i] != 0) playTime[i]--;
		if(midiTrackPlayed[i]) playTime[i] = 10;
		if(midiTrackPlaying[i])
		{
			if(playTime[i] == 10 || playTime[i] == 9) printf("#");
			else if(playTime[i] == 8 || playTime[i] == 7) printf("=");
			else if(playTime[i] == 6 || playTime[i] == 5) printf("-");
			else if(playTime[i] == 4 || playTime[i] == 3) printf("_");
			else printf(".");
		}
		else printf(" ");
	}

}
