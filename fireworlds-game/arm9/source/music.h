#ifndef _MUSIC_H
#define _MUSIC_H

void playMusic(int m);
void stopMusic();
void musicTick();

extern int musicTrackPlayed[4];
extern int musicTrackPlaying[4];

#endif