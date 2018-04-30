/*

  Advanced Sound Library (ASlib)
  ------------------------------

  file        : sound9.c 
  author      : Lasorsa Yohan (Noda)
  description : ARM7 sound functions

  history : 

    29/11/2007 - v1.0
      = Original release

    07/02/2008 - v1.1

      = corrected arm7/arm9 initialization (fix M3S/R4 problems)
      = fixed stereo detection problem (thanks to ThomasS)      
      = corrected panning when surround is not activated

*/

#include <nds.h>
#include <malloc.h>
#include <string.h>

#include "as_lib9.h"


#ifdef __cplusplus
extern "C" {
#endif

// variable for the mp3 file stream
MP3FILE *mp3file = NULL;
u8* mp3filebuffer = NULL;

// default settings for sounds
u8 as_default_format;
s32 as_default_rate; 
u8 as_default_delay;


// initialize ASlib
void AS_Init(u8 mode)
{
    int i, nb_chan = 16;
    
    // initialize default settings
    as_default_format = AS_PCM_8BIT;
    as_default_rate = 16384; // <PALIB-CHANGE> this is a good rate
    as_default_delay = AS_SURROUND;
    
    // wait for the ARM7 to be ready
    while( !(IPC_Sound->chan[0].cmd & SNDCMD_ARM7READY) )
        swiWaitForVBlank();    

    // initialize channels
    for(i = 0; i < 16; i++) {
        IPC_Sound->chan[i].busy = false;
        IPC_Sound->chan[i].reserved = false;
        IPC_Sound->chan[i].volume = 0;
        IPC_Sound->chan[i].pan = 64;
        IPC_Sound->chan[i].cmd = SNDCMD_NONE;
    }

    // use only 8 channels
    if(mode & AS_MODE_8CH) {

        nb_chan = 8;
        for(i = 8; i < 16; i++)
            IPC_Sound->chan[i].reserved = true;
    }
    
    // use surround
    if(mode & AS_MODE_SURROUND) {

        IPC_Sound->surround = true;
        for(i = nb_chan / 2; i < nb_chan; i++)
            IPC_Sound->chan[i].reserved = true;
    
    } else {
        IPC_Sound->surround = false;
    }

    IPC_Sound->num_chan = nb_chan / 2;

    // use mp3
    if(mode & AS_MODE_MP3) {
    
        // allocate ram for the ARM7 mp3 decoder
        IPC_Sound->mp3.alloc_ram = (u8*)memalign(4, (int)IPC_Sound->mp3.alloc_ram);
        IPC_Sound->mp3.cmd = MP3CMD_ARM9ALLOCDONE;
        
        // initialize mp3 structure
        IPC_Sound->mp3.mixbuffer = (s8*)memalign(4, AS_AUDIOBUFFER_SIZE * 2);
        IPC_Sound->mp3.buffersize = AS_AUDIOBUFFER_SIZE / 2;
        IPC_Sound->mp3.channelL = 0;
        IPC_Sound->mp3.prevtimer = 0;
        IPC_Sound->mp3.soundcursor = 0;
        IPC_Sound->mp3.numsamples = 0;
        IPC_Sound->mp3.delay = AS_SURROUND;
        IPC_Sound->mp3.cmd |= MP3CMD_INIT;
        IPC_Sound->mp3.state = MP3ST_STOPPED;
        
        IPC_Sound->chan[0].reserved = true;
        
        if(IPC_Sound->surround) {
            IPC_Sound->mp3.channelR = nb_chan / 2;
            IPC_Sound->chan[nb_chan / 2].reserved = true;
        } else {
            IPC_Sound->mp3.channelR = 1;
            IPC_Sound->chan[1].reserved = true;
        }

        IPC_Sound->chan[IPC_Sound->mp3.channelL].snd.pan = 64;
        AS_SetMP3Volume(127);

        // wait for the mp3 engine to be initialized
        while(IPC_Sound->mp3.cmd & MP3CMD_INIT)
            swiWaitForVBlank();
    }
    
    AS_SetMasterVolume(127);
}

// play a sound using the priority system
// return the sound channel allocated or -1 if the sound was skipped
int AS_SoundPlay(SoundInfo sound) 
{
    int i, free_ch = -1, minp_ch = -1;

    // search a free channel
    for(i = 0; i < 16; i++) {
        if(!(IPC_Sound->chan[i].reserved || IPC_Sound->chan[i].busy))
            free_ch = i;
    }

    // if a free channel was found
    if(free_ch != -1) {

        // play the sound
        AS_SoundDirectPlay(free_ch, sound);
        return free_ch;
    
    } else {
    
        // find the channel with the least priority
        for(i = 0; i < 16; i++) {
            if(!IPC_Sound->chan[i].reserved) {
                if(minp_ch == -1)
                    minp_ch = i;
                else if(IPC_Sound->chan[i].snd.priority < IPC_Sound->chan[minp_ch].snd.priority)
                    minp_ch = i;
            }
        }
        
        // if the priority of the found channel is <= the one of the sound
        if( IPC_Sound->chan[minp_ch].snd.priority <= sound.priority) {
        
            // play the sound
            AS_SoundDirectPlay(minp_ch, sound);
            return minp_ch;

        } else {
        
            // skip the sound
            return -1;
        }
    }
}

// set the panning of a sound (0=left, 64=center, 127=right)
void AS_SetSoundPan(u8 chan, u8 pan)
{
    IPC_Sound->chan[chan].snd.pan = pan;

    if(IPC_Sound->surround) {
        
        int difference = ((pan - 64) >> AS_PANNING_SHIFT) * IPC_Sound->chan[chan].snd.volume / AS_VOL_NORMALIZE;
    
        IPC_Sound->chan[chan].pan = 0;
        IPC_Sound->chan[chan].volume = IPC_Sound->chan[chan].snd.volume + difference;

        if(IPC_Sound->chan[chan].volume < 0)
            IPC_Sound->chan[chan].volume = 0;
    
        IPC_Sound->chan[chan].cmd |= SNDCMD_SETVOLUME;
        IPC_Sound->chan[chan].cmd |= SNDCMD_SETPAN;        

        IPC_Sound->chan[chan + IPC_Sound->num_chan].pan = 127;
        IPC_Sound->chan[chan + IPC_Sound->num_chan].volume = IPC_Sound->chan[chan].snd.volume - difference;

        if(IPC_Sound->chan[chan + IPC_Sound->num_chan].volume < 0)
            IPC_Sound->chan[chan + IPC_Sound->num_chan].volume = 0;
            
        IPC_Sound->chan[chan + IPC_Sound->num_chan].cmd |= SNDCMD_SETVOLUME;
        IPC_Sound->chan[chan + IPC_Sound->num_chan].cmd |= SNDCMD_SETPAN;
    
    } else {
    
        IPC_Sound->chan[chan].cmd |= SNDCMD_SETPAN;
        IPC_Sound->chan[chan].pan = pan;
        
    }
}

// set the volume of a sound (0..127)
void AS_SetSoundVolume(u8 chan, u8 volume)
{   
    if(IPC_Sound->surround) {
        IPC_Sound->chan[chan].snd.volume = volume * AS_BASE_VOLUME / 127;
        AS_SetSoundPan(chan, IPC_Sound->chan[chan].snd.pan);
    } else {
        IPC_Sound->chan[chan].volume = volume;
        IPC_Sound->chan[chan].cmd |= SNDCMD_SETVOLUME;
    }
}

// set the sound sample rate
void AS_SetSoundRate(u8 chan, u32 rate)
{
    IPC_Sound->chan[chan].snd.rate = rate;
    IPC_Sound->chan[chan].cmd |= SNDCMD_SETRATE;
    
    if(IPC_Sound->surround) {
        IPC_Sound->chan[chan + IPC_Sound->num_chan].snd.rate = rate;
        IPC_Sound->chan[chan + IPC_Sound->num_chan].cmd |= SNDCMD_SETRATE;
    }
}

static inline void _AS_CopyToSoundIPC(SoundInfoAlign* out, SoundInfo* in){
    out->data = in->data;
    out->size = in->size;
    out->rate = in->rate;
    out->format = in->format;
    out->volume = in->volume;
    out->pan = in->pan;
    out->loop = in->loop;
    out->priority = in->priority;
    out->delay = in->delay;
}

// play a sound directly using the given channel
void AS_SoundDirectPlay(u8 chan, SoundInfo sound)
{
    _AS_CopyToSoundIPC(&IPC_Sound->chan[chan].snd, &sound);
    //IPC_Sound->chan[chan].snd = sound;
    IPC_Sound->chan[chan].busy = true;
    IPC_Sound->chan[chan].cmd = SNDCMD_PLAY;
    IPC_Sound->chan[chan].volume = sound.volume;
    
    if(IPC_Sound->surround) {
        _AS_CopyToSoundIPC(&IPC_Sound->chan[chan + IPC_Sound->num_chan].snd, &sound);
        //IPC_Sound->chan[chan + IPC_Sound->num_chan].snd = sound;
        IPC_Sound->chan[chan + IPC_Sound->num_chan].busy = true;
        IPC_Sound->chan[chan + IPC_Sound->num_chan].cmd = SNDCMD_DELAY;

        // set the correct surround volume & pan
        AS_SetSoundVolume(chan, sound.volume);
    } else {
        IPC_Sound->chan[chan].pan = sound.pan;
    }
}

// fill the given buffer with the required amount of mp3 data
void AS_MP3FillBuffer(u8 *buffer, u32 bytes)
{
    u32 read = FILE_READ(buffer, 1, bytes, mp3file);
    
    if((read < bytes) && IPC_Sound->mp3.loop) {
        FILE_SEEK(mp3file, 0, SEEK_SET);
        FILE_READ(buffer + read, 1, bytes - read, mp3file);
    }
}

// play an mp3 directly from memory
void AS_MP3DirectPlay(u8 *mp3_data, u32 size) 
{
    if(IPC_Sound->mp3.state & (MP3ST_PLAYING | MP3ST_PAUSED))
        return;

    IPC_Sound->mp3.mp3buffer = mp3_data;
    IPC_Sound->mp3.mp3filesize = size;
    IPC_Sound->mp3.stream = false;
    IPC_Sound->mp3.cmd = MP3CMD_PLAY;
}

// play an mp3 stream
void AS_MP3StreamPlay(const char *path) 
{
    if(IPC_Sound->mp3.state & (MP3ST_PLAYING | MP3ST_PAUSED))
        return;

    if(mp3file)
        FILE_CLOSE(mp3file);

    mp3file = FILE_OPEN(path);

    if(mp3file) {
    
        // allocate the file buffer the first time
        if(!mp3filebuffer) {
            mp3filebuffer = (u8*)memalign(4, AS_FILEBUFFER_SIZE * 2);   // 2 buffers, to swap
            IPC_Sound->mp3.mp3buffer = mp3filebuffer;
            IPC_Sound->mp3.mp3buffersize = AS_FILEBUFFER_SIZE;
        }
        
        // get the file size
        FILE_SEEK(mp3file, 0, SEEK_END);
        IPC_Sound->mp3.mp3filesize = FILE_TELL(mp3file);
        
        // fill the file buffer
        FILE_SEEK(mp3file, 0, SEEK_SET);
        AS_MP3FillBuffer(mp3filebuffer, AS_FILEBUFFER_SIZE * 2);
        
        // start playing
        IPC_Sound->mp3.stream = true;
        IPC_Sound->mp3.cmd = MP3CMD_PLAY;
    }
    
}

// set the mp3 panning (0=left, 64=center, 127=right)
void AS_SetMP3Pan(u8 pan)
{
    int difference = ((pan - 64) >> AS_PANNING_SHIFT) * IPC_Sound->chan[IPC_Sound->mp3.channelL].snd.volume / AS_VOL_NORMALIZE;

    IPC_Sound->chan[IPC_Sound->mp3.channelL].snd.pan = pan;
    IPC_Sound->chan[IPC_Sound->mp3.channelL].pan = 0;
    IPC_Sound->chan[IPC_Sound->mp3.channelL].volume = IPC_Sound->chan[IPC_Sound->mp3.channelL].snd.volume - difference;

    if(IPC_Sound->chan[IPC_Sound->mp3.channelL].volume < 0)
        IPC_Sound->chan[IPC_Sound->mp3.channelL].volume = 0;

    IPC_Sound->chan[IPC_Sound->mp3.channelL].cmd |= SNDCMD_SETVOLUME;
    IPC_Sound->chan[IPC_Sound->mp3.channelL].cmd |= SNDCMD_SETPAN;
    
    IPC_Sound->chan[IPC_Sound->mp3.channelR].pan = 127;
    IPC_Sound->chan[IPC_Sound->mp3.channelR].volume = IPC_Sound->chan[IPC_Sound->mp3.channelL].snd.volume + difference;

    if(IPC_Sound->chan[IPC_Sound->mp3.channelR].volume < 0)
        IPC_Sound->chan[IPC_Sound->mp3.channelR].volume = 0;
        
    IPC_Sound->chan[IPC_Sound->mp3.channelR].cmd |= SNDCMD_SETVOLUME;
    IPC_Sound->chan[IPC_Sound->mp3.channelR].cmd |= SNDCMD_SETPAN;
        
}

// regenerate buffers for mp3 stream, must be called each VBlank (only needed if mp3 is used)
void AS_SoundVBL()
{
    // refill mp3 file  buffer if needed
    if(IPC_Sound->mp3.needdata) {
        AS_MP3FillBuffer(IPC_Sound->mp3.mp3buffer + AS_FILEBUFFER_SIZE, AS_FILEBUFFER_SIZE);
        IPC_Sound->mp3.needdata = false;
    }
}

#ifdef __cplusplus
}
#endif

