
#include <nds.h>
#include <dswifi7.h>
#include <maxmod7.h>
#include "as_lib7.h"
#include "PA_Transfer.h"

//---------------------------------------------------------------------------------
void VblankHandler(void) {
//---------------------------------------------------------------------------------
	Wifi_Update();
	AS_SoundVBL();
}


//---------------------------------------------------------------------------------
void VcountHandler() {
//---------------------------------------------------------------------------------
	inputGetAndSend();
}

volatile bool exitflag = false;

//---------------------------------------------------------------------------------
void powerButtonHandler() {
//---------------------------------------------------------------------------------
	exitflag = true;
}

#ifdef __cplusplus
extern "C" {
#endif

void enableSound(); // a libnds function that isn't in the headers.
void swiSoftReset_0(void);
#ifdef __cplusplus
}
#endif



//---------------------------------------------------------------------------------
int main() {
//---------------------------------------------------------------------------------
//	systemMsgHandler(0, 0);
	readUserSettings();
	
	irqInit();
	fifoInit();
	enableSound();
	AS_Init();

//	mmInstall(FIFO_MAXMOD);
	// Start the RTC tracking IRQ
	initClockIRQ();

	SetYtrigger(80);

	installWifiFIFO();
	installSoundFIFO();

	installSystemFIFO();
	
	irqSet(IRQ_VCOUNT, VcountHandler);
	irqSet(IRQ_VBLANK, VblankHandler);

	irqEnable( IRQ_VBLANK | IRQ_VCOUNT | IRQ_NETWORK);   

	// Keep the ARM7 mostly idle
	while (!exitflag) {
		
		if ( 0 == (REG_KEYINPUT & (KEY_SELECT | KEY_START | KEY_L | KEY_R))) {
			exitflag = true;
		}
		timerStart(2, ClockDivider_1024, 0, 0);
		AS_MP3Engine(); 
		int shit = BUS_CLOCK / 100 / 60 / 1024;
		int dd = timerElapsed(2)/shit;
		PA_Transfer->cpuUsage = dd;
		timerStop(2);
		swiWaitForVBlank();
	}
	return 0;
}
