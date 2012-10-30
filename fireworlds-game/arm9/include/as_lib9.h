/*

  Advanced Sound Library (ASlib)
  ------------------------------

  file        : sound9.h 
  author      : Lasorsa Yohan (Noda)
  description : ARM9 sound functions

  history : 

    29/11/2007 - v1.0
      = Original release
      
    21/12/2007 - v1.1
      = corrected arm7/arm9 initialization (fix M3S/R4 problems)
      = fixed stereo detection problem (thanks to ThomasS)
      
    27/03/2008 - v1.2
      = corrected bug with AS_SetMP3Loop, wasn't working correctly
      = corrected bug with stereo panning for sounds when surround is off
      TODO: allocate memory on main RAM
      
*/

#ifndef __SOUND9_H__
#define __SOUND9_H__

//---- ASlib options ------------------------------------------
// uncomment the defines below to activate the desired option

// use the EFSlib to stream mp3 files instead of libfat
 //#define AS_USE_EFS

// use expanded stereo panning separation in surround mode, but decrease volume
//#define AS_USE_EXPANDED_SURROUND_STEREO

//-----------------------------------------------------------

// <PALIB-CHANGE> Replaced // with ///
#include <nds.h>
#include <stdio.h>
#include "PA_Transfer.h"

#ifndef AS_USE_EFS
#include <stdio.h>
#else
#include "efs_lib.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif

// mp3 static defines
#define MAX_NGRAN           2           // max granules
#define MAX_NCHAN           2           // max channels
#define MAX_NSAMP           576         // max samples per channel, per granule

// buffer sizes, should fit any usage (feel free to adjust if needed)
#define AS_AUDIOBUFFER_SIZE 8 * MAX_NCHAN * MAX_NGRAN * MAX_NSAMP // audio buffer size <PALIB-CHANGE> <WAS: 4>
#define AS_FILEBUFFER_SIZE  8 * 1024   // file buffer size

// locate the IPC structure after the libnds one
#define IPC_Sound   ((IPC_SoundSystem*)((u32)(PA_Transfer) + sizeof(PA_TransferRegion))) // <PALIB-CHANGE> the second

// file access functions
#define MP3FILE                         FILE
#define FILE_CLOSE(f)                   fclose(f)
#define FILE_OPEN(f)                    fopen(f, "rb")
#define FILE_READ(buf, size, num, f)    fread(buf, size, num, f)
#define FILE_SEEK(f, pos, off)          fseek(f, pos, off)
#define FILE_TELL(f)                    ftell(f)


// set up the surround stereo panning mode 
#ifdef AS_USE_EXPANDED_SURROUND_STEREO

#define AS_BASE_VOLUME      64
#define AS_VOL_NORMALIZE    65
#define AS_PANNING_SHIFT    0

#else

#define AS_BASE_VOLUME      96
#define AS_VOL_NORMALIZE    97
#define AS_PANNING_SHIFT    1

#endif

// <PALIB-CHANGE>
/** @addtogroup ASlib ASlib functions
 *  Functions to play RAW sounds and *shrug* MP3s.
 *  @{
 */

/// MP3 commands
typedef enum 
{
	MP3CMD_ARM9ALLOCDONE = 256,
	MP3CMD_NONE = 0,
	MP3CMD_MIX = 1,
	MP3CMD_MIXING = 2,
	MP3CMD_WAITING = 4,

	/// Initialize
	MP3CMD_INIT = 8,
	/// Stop
	MP3CMD_STOP = 16,
	/// Play
	MP3CMD_PLAY = 32,
	/// Pause
	MP3CMD_PAUSE = 64,
	/// Set rate
	MP3CMD_SETRATE = 128

} MP3Command;

/// Sound commands
typedef enum
{
    SNDCMD_ARM7READY = 128,
    SNDCMD_NONE = 0,
    SNDCMD_DELAY = 1,

    /// Stop
    SNDCMD_STOP = 2,
	/// Play
    SNDCMD_PLAY = 4,
	/// Set volume
    SNDCMD_SETVOLUME = 8,
	/// Set pan
    SNDCMD_SETPAN = 16,
	/// Set rate
    SNDCMD_SETRATE = 32,
	/// Set master volume
    SNDCMD_SETMASTERVOLUME = 64
    
} SoundCommand;

/// MP3 states
typedef enum
{
	/// Stopped
    MP3ST_STOPPED = 0,
	/// Playing
    MP3ST_PLAYING = 1,
	/// Paused
    MP3ST_PAUSED = 2,
	/// Out of data
    MP3ST_OUT_OF_DATA = 4,
	/// Decoding error
    MP3ST_DECODE_ERROR = 8,
	/// Initialization failed
	MP3ST_INITFAILED = 16,	// <PALIB-CHANGE>
    
} MP3Status;

/// ASlib modes
typedef enum
{
	/// use mp3
    AS_MODE_MP3 = 1,
	/// use surround
    AS_MODE_SURROUND = 2,
	/// use all DS channels
    AS_MODE_16CH = 4,
	/// use DS channels 1-8 only
    AS_MODE_8CH = 8
    
} AS_MODE;

/// Delay values
typedef enum
{
	/// 0 ms delay
    AS_NO_DELAY = 0,
	/// 16 ms delay
    AS_SURROUND = 1,
	/// 66 ms delay
    AS_REVERB = 4,
    
} AS_DELAY;

/// Sound formats
typedef enum
{
	/// 8-bit PCM
    AS_PCM_8BIT = 0,
	/// 16-bit PCM
    AS_PCM_16BIT = 1,
	/// 4-bit ADPCM
    AS_ADPCM = 2
    
} AS_SOUNDFORMAT;

/// Sound info
typedef struct
{
	/// Pointer to data
    u8  *data;
	/// Size in bytes
    u32 size;
	/// Format (see AS_SOUNDFORMAT)
    u8  format;
	/// Rate in Hz
    s32 rate;
	/// Volume (0-127)
    u8  volume;
	/// Pan (0-64-127)
    s8  pan;
	/// Loop (0 or 1)
    u8  loop;
	/// Priority
    u8  priority;
	/// Delay
    u8  delay;
    
} SoundInfo;

// sound info !![PAlib change] Word alignment!!
typedef struct
{
    u8  *data;
    u32 size;
    s32  rate;
    u8  format;
    u8  volume;
    s8  pan;
    u8  loop;
    u8  priority;
    u8  delay;
    
} SoundInfoAlign;

// sound channel info
typedef struct
{
    SoundInfoAlign snd; // !![PAlib change] Word alignment!!
    u8  busy;
    u8  reserved;
    s8  volume;
    s8  pan;
    u8  cmd;

} SoundChannel;

// MP3 player info
typedef struct
{
	u8  *alloc_ram;
    s8  *mixbuffer;
    u32 buffersize;
    s32 rate;
    u32 state;
    u32 soundcursor;
    u32 numsamples;
    s32 prevtimer;
    u8  *mp3buffer;
    u32 mp3buffersize;    
    u32 mp3filesize;    
    u32 cmd;    
    u8  channelL, channelR;
    u8  loop;
    u8  stream;
    u8  needdata;
    u8  delay;

} MP3Player;

// IPC structure for the sound system
typedef struct
{
    MP3Player mp3;
    u8 surround; // !![PAlib change] Word alignment!!
    u8 num_chan; //  !![PAlib change] Word alignment!!
    SoundChannel chan[16];
    u8 volume;
} IPC_SoundSystem;

/// Easiest way to play a sound, using default settings
#define AS_SoundQuickPlay(name) AS_SoundDefaultPlay((u8*)name, (u32)name##_size, 127, 64, false, 0)

/// Initialize ASlib
void AS_Init(u8 mode);

/// Reserve a particular DS channel (so it won't be used for the sound pool)
static inline void AS_ReserveChannel(u8 channel);

/// Set the master volume (0..127)
static inline void AS_SetMasterVolume(u8 volume);

/// Set the default sound settings
static inline void AS_SetDefaultSettings(u8 format, s32 rate, u8 delay);

/// Play a sound using the priority system.
/// Returns the sound channel allocated or -1 if the sound was skipped.
int AS_SoundPlay(SoundInfo sound);

/// Play a sound using the priority system with the default settings.
/// Returns the sound channel allocated or -1 if the sound was skipped.
static inline int AS_SoundDefaultPlay(u8 *data, u32 size, u8 volume, u8 pan, u8 loop, u8 prio);

/// Set the panning of a sound (0=left, 64=center, 127=right)
void AS_SetSoundPan(u8 chan, u8 pan);

/// Set the volume of a sound (0..127)
void AS_SetSoundVolume(u8 chan, u8 volume);

/// Set the sound sample rate
void AS_SetSoundRate(u8 chan, u32 rate);

/// Stop playing a sound
static inline void AS_SoundStop(u8 chan);

/// Play a sound directly using the given channel
void AS_SoundDirectPlay(u8 chan, SoundInfo sound);

/// Play a MP3 directly from memory
void AS_MP3DirectPlay(u8 *buffer, u32 size);

/// Play a MP3 stream
void AS_MP3StreamPlay(const char *path);

/// Pause a MP3
static inline void AS_MP3Pause();

/// Unpause a MP3
static inline void AS_MP3Unpause();

/// Stop a MP3
static inline void AS_MP3Stop();

/// Get the current MP3 status
static inline int AS_GetMP3Status();

/// Set the MP3 volume (0..127)
static inline void AS_SetMP3Volume(u8 volume);

/// Set the MP3 panning (0=left, 64=center, 127=right)
void AS_SetMP3Pan(u8 pan);

/// Set the default MP3 delay mode (warning: high values can cause glitches)
static inline void AS_SetMP3Delay(u8 delay);

/// Set the MP3 loop mode (false = one shot, true = loop indefinitely)
static inline void AS_SetMP3Loop(u8 loop);

/// Set the MP3 sample rate
static inline void AS_SetMP3Rate(s32 rate);

/// Regenerate buffers for MP3 stream.
/// Must be called each VBlank (only needed if mp3 is used)
void AS_SoundVBL();


//-------------------------------------------------------------------------------
// definition of inlined functions
//-------------------------------------------------------------------------------

// variables defined in as_lib9.cpp
extern MP3FILE *mp3file;
extern u8 as_default_format;
extern s32 as_default_rate; 
extern u8 as_default_delay;

// private functions, defined in as_lib9.cpp
extern void AS_MP3FillBuffer(u8 *buffer, u32 bytes);


// reserve a particular DS channel (so it won't be used for the sound pool)
static inline void AS_ReserveChannel(u8 channel) 
{ 
    IPC_Sound->chan[channel].reserved = true; 
}

// set the master volume (0..127)
static inline void AS_SetMasterVolume(u8 volume) 
{
    IPC_Sound->volume = volume;
    IPC_Sound->chan[0].cmd |= SNDCMD_SETMASTERVOLUME;
}

// set the default sound settings
static inline void AS_SetDefaultSettings(u8 format, s32 rate, u8 delay)
{
    as_default_format = format;
    as_default_rate = rate; 
    as_default_delay = delay;
}

// play a sound using the priority system with the default settings
// return the sound channel allocated or -1 if the sound was skipped
static inline int AS_SoundDefaultPlay(u8 *data, u32 size, u8 volume, u8 pan, u8 loop, u8 prio)
{
    SoundInfo snd = { 
        data, 
        size, 
        as_default_format, 
        as_default_rate, 
        volume, 
        pan, 
        loop, 
        prio, 
        as_default_delay 
    };
    return AS_SoundPlay(snd);
}

// stop playing a sound
static inline void AS_SoundStop(u8 chan)
{
    IPC_Sound->chan[chan].cmd = SNDCMD_STOP;
    
    if(IPC_Sound->surround)
        IPC_Sound->chan[chan + IPC_Sound->num_chan].cmd = SNDCMD_STOP;
}

// pause an mp3
static inline void AS_MP3Pause()
{
    if(IPC_Sound->mp3.state & MP3ST_PLAYING)
        IPC_Sound->mp3.cmd = MP3CMD_PAUSE;
}

// unpause an mp3
static inline void AS_MP3Unpause()
{
    if(IPC_Sound->mp3.state & MP3ST_PAUSED)
        IPC_Sound->mp3.cmd = MP3CMD_PLAY;
}

// stop an mp3
static inline void AS_MP3Stop()
{
    IPC_Sound->mp3.cmd = MP3CMD_STOP;
    FILE_CLOSE(mp3file);
}

// get the current mp3 status
static inline int AS_GetMP3Status()
{
    return IPC_Sound->mp3.state;
}

// set the mp3 volume (0..127)
static inline void AS_SetMP3Volume(u8 volume)
{
    IPC_Sound->chan[IPC_Sound->mp3.channelL].snd.volume = volume * AS_BASE_VOLUME / 127;
    AS_SetMP3Pan(IPC_Sound->chan[IPC_Sound->mp3.channelL].snd.pan);
}

// set the default mp3 delay mode (warning: high values can cause glitches)
static inline void AS_SetMP3Delay(u8 delay)
{
    IPC_Sound->mp3.delay = delay;
}

// set the mp3 loop mode (false = one shot, true = loop indefinitely)
static inline void AS_SetMP3Loop(u8 loop)
{
    IPC_Sound->mp3.loop = loop;
}

// set the mp3 sample rate
static inline void AS_SetMP3Rate(s32 rate)
{
    IPC_Sound->mp3.rate = rate;
    IPC_Sound->mp3.cmd |= MP3CMD_SETRATE;
}

/** @} */ // end of ASlib <PALIB-CHANGE>


#ifdef __cplusplus
}
#endif

#endif

