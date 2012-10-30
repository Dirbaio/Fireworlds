#include<nds.h>
using namespace std;

#ifndef _MIDI_H
#define _MIDI_H

void loadMidi(u8* ptr);
void playTrack(int track, int deltatime);
void midiStop();
void midiVBL();

extern int midiTrackPlayed[256];
extern int midiTrackPlaying[256];

#endif