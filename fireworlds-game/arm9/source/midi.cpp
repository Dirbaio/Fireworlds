#include "midi.h"
#include <stdio.h>

#define MICROSECONDS_PER_MINUTE 60000000

//At 60 FPS
#define MICROSECONDS_PER_FRAME (1000000 / 60) 


//Fucking Big-Endian I/O functions below
inline u32 read32(u8*& ptr)
{
	u32 res = 0;
	res = (res<<8) | *ptr; ptr++;
	res = (res<<8) | *ptr; ptr++;
	res = (res<<8) | *ptr; ptr++;
	res = (res<<8) | *ptr; ptr++;
	return res;
}

inline u32 read24Const(u8* ptr)
{
	u32 res = 0;
	res = (res<<8) | *ptr; ptr++;
	res = (res<<8) | *ptr; ptr++;
	res = (res<<8) | *ptr; ptr++;
	return res;
}

inline u16 read16(u8*& ptr)
{
	u16 res = 0;
	res = (res<<8) | *ptr; ptr++;
	res = (res<<8) | *ptr; ptr++;
	return res;
}

inline u8 read8(u8*& ptr)
{
	u8 res = *ptr;
	ptr++;
	return res;
}

inline u32 readVarLen(u8*& ptr)
{
	int res = 0;
	while(1)
	{
		u8 dat = *ptr; ptr++;
		res = (res<<7) | (dat & 0x7F);
		if((dat & 0x80) == 0) break;
	}
	
	return res;
}


inline u32 readVarLenConst(u8* ptr)
{
	int res = 0;
	while(1)
	{
		u8 dat = *ptr; ptr++;
		res = (res<<7) | (dat & 0x7F);
		if((dat & 0x80) == 0) break;
	}
	
	return res;
}

u8* tracks[256];
u32 trackTimeFromLast[256];
float tempo;

bool trackEnded[256];

u32 nTracks;
u32 timeDiv; //In delta positions per quarter note

int midiTrackPlayed[256];
int midiTrackPlaying[256];

u32 ticksToPlay;
void midiTick()
{
	ticksToPlay++;
//	playTrack(13, 1);
}

void loadMidi(u8* ptr)
{ 
	u32 id = read32(ptr);
	u32 headsz = read32(ptr);
	u16 fmt = read16(ptr);
	nTracks = read16(ptr);
	timeDiv = read16(ptr);
//	iprintf("Midi loaded. fmt %d, ntrks %d, timediv %d\n", fmt, nTracks, timeDiv);
	for(u32 i = 0; i < nTracks; i++)
	{
		u32 trkID = read32(ptr);
		u32 trkSize = read32(ptr);
//		iprintf("Track %d: %x %d\n", i, trkID, trkSize);
		tracks[i] = ptr;
		trackTimeFromLast[i] = 0;
		trackEnded[i] = false;
		ptr += trkSize;
	}
	
	for(int i = 0; i < 256; i++)
	{
		midiTrackPlayed[i] = false;
		midiTrackPlaying[i] = false;
	}
	
	tempo = MICROSECONDS_PER_MINUTE/120.0f; //Default tempo 120BPM

	float bpm = MICROSECONDS_PER_MINUTE/tempo;
	float hz = (bpm/60)*timeDiv + 1;
//	printf("TIMER HZ: %10.5f", hz);
	timerStart(2, ClockDivider_256, TIMER_FREQ_256((int)hz), midiTick);
	ticksToPlay = 0;
}

void midiVBL()
{
	int ticks = ticksToPlay;
	ticksToPlay = 0;
	for(u32 i = 0; i < nTracks; i++)
		playTrack(i, ticks);
}

void midiStop()
{
	timerStop(2);
}

void playTrack(int track, int deltatime)
{
	if(trackEnded[track]) return;
//	iprintf("%d\n", deltatime);
	
	u32& timefromlast = trackTimeFromLast[track];
	u8*& ptr = tracks[track];
	
	timefromlast += deltatime;
	
	while(true)
	{
		deltatime = readVarLenConst(ptr);
		if(deltatime <= timefromlast)
		{
//			for(int i = 0; i < 60; i++) swiWaitForVBlank();
			timefromlast -= readVarLen(ptr);
			u8 event = read8(ptr);
			u8 chan = event & 0xF;
			event = event>>4;
//			iprintf("%d %d %d ", deltatime, event, chan);
			
			switch(event)
			{
				case 0:
				case 1:
				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
				case 7: //Bad events!?
					iprintf("BAD event.\n");
					break;
				case 8: //Note Off
				case 9: //Note On
				case 10: //Note Aftertouch
				case 11: //Controller
				case 12: //Program Change
				case 13: //Channel Aftertouch
				case 14: //Pitch Bend
				{
					u8 param1 = read8(ptr);
					u8 param2 = 123;
					if(event != 12 && event != 13) //Events 12 and 13 only takes 1 param
						param2 = read8(ptr);
//					iprintf("%d %d\n", param1, param2);
					
					if(event == 9)
					{
						if(param2 == 0)
							midiTrackPlaying[track] = 0;
						else
						{
							midiTrackPlayed[track] = param1;
							midiTrackPlaying[track] = param1;
						}
					}
					break;
				}
				case 15: //Meta/SysEx event
				{
					if(chan == 0xF) //meta event
					{
						u8 type = read8(ptr);
						u32 len = readVarLen(ptr);
						if(type == 0x51)
							tempo = read24Const(ptr);
						if(type == 0x2F)
						{
							trackEnded[track] = true;
							return;
						}
						ptr += len;
//						iprintf("Meta event %d %d\n", type, len);
					}
					else if(chan == 0x0 || chan == 0x7) //SysEx event
					{
						iprintf("SysEx events unsupported\n");
					}
					else
						iprintf("Bad 0xF event\n");
					break;
				}
			}
		}
		else break;
	}
}