#include<nds.h>

void initSub();

void renderBg();
void subFrameStart();

void subRenderObjChar(int x, int y, char c);
void subRenderObjString(int x, int y, const char* str);
void subRenderObjNum(int x, int y, int n);

void subSetPixel(int x, int y, int shit);
void subRenderPixelChar(int x, int y, char c);
void subRenderPixelString(int x, int y, const char* str);

void subClear();
int charWidth(char c);