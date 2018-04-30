#include <nds.h>
#include <cstdio>

bool videoPlaying;
int videoFrames;
FILE* videoFile;


u8 videoFrame[256*192*4];
u32 videoFrameLen;
bool vramBank;


struct rawBlk
{
    s16 offs;
    u16 data[8*8];
};



bool videoReadFrame()
{
    //Read frame data.
    if(fread(&videoFrameLen, 1, 4, videoFile) != 4) return false;
    if(fread(videoFrame, 1, videoFrameLen, videoFile) != videoFrameLen) return false;
    return true;
}


void videoTick()
{
    videoFrames++;
}

void videoDecodeFrame()
{
    
    //Swap banks.
    u16* targetImg = vramBank?VRAM_C:VRAM_B;
    u16* refImg = vramBank?VRAM_B:VRAM_C;
    
    
    u8* data = videoFrame;
    
    for(int by = 0; by < 24; by++)
        for(int bx = 0; bx < 32; bx++)
        {
            int p = bx*8 + by*8*256;
            rawBlk* blk = (rawBlk*) data;
            
            if(blk->offs == (s16)0x8080) //Key block
            {
                for(int y = 0; y < 8; y++)
                    for(int x = 0; x < 8; x++)
                        targetImg[p+x+y*256] = blk->data[x+y*8];
                
                data += sizeof(rawBlk);
            }
            else //Moved block.
            {
                s16 d = blk->offs;

                for(int y = 0; y < 8; y++)
                    for(int x = 0; x < 8; x++)
                        targetImg[p+x+y*256] = refImg[p+x+y*256+d];
                data += 2;
            }
        }
    
    videoSetMode(vramBank?MODE_FB2:MODE_FB1);
    vramBank = !vramBank;
}


bool videoStart(const char* file)
{
    vramSetBankB(VRAM_B_LCD);
    vramSetBankC(VRAM_C_LCD);
//  videoSetMode(MODE_FB1);

    if(videoPlaying) return false;
    
    videoFile = fopen(file, "rb");
    if (videoFile == NULL) return false;

    timerStart(3, ClockDivider_256, TIMER_FREQ_256(20), videoTick);
    videoFrames = 0;
    videoPlaying = true;
    vramBank = false;
    return true;
}

void videoStop()
{
    fclose(videoFile);
    videoFile = NULL; //just in case :P
    timerStop(3);
    videoPlaying = false;
}

bool videoVBL()
{
    if(!videoPlaying) return false;
    
    while(videoFrames == 0) swiWaitForVBlank();
    videoFrames--;
    
    printf("\x1b[13;1HOVERHEAD:  %d         ", videoFrames);
    
    timerStart(0, ClockDivider_1024, 0, NULL);
    bool ok = videoReadFrame();
    int dd = timerElapsed(0)/ (BUS_CLOCK / 100 / 60 / 1024);
    printf("\x1b[11;1HREAD TIME:  %d %%        ", dd);
    timerStop(0);
    
    if(!ok) return false;
    timerStart(0, ClockDivider_1024, 0, NULL);
    videoDecodeFrame();
    dd = timerElapsed(0)/ (BUS_CLOCK / 100 / 60 / 1024);
    printf("\x1b[12;1HDISP TIME:  %d %%        ", dd);
    timerStop(0);
    printf("\x1b[14;1HFRAME SIZE: %d        ", videoFrameLen);
    
    return true;
}
