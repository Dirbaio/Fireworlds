#include<stdio.h>

int levelsBeaten;

void loadSaver()
{
	FILE* f = fopen("fat:/data/fworlds/saver.dat", "rb");
	if(f == NULL) levelsBeaten = 0;
	else
	{
		levelsBeaten = fgetc(f);
		fclose(f);
	}
}

void writeSaver()
{
	FILE* f = fopen("fat:/data/fworlds/saver.dat", "wb");
	if(f == NULL) levelsBeaten = 0;
	else
	{
		fputc(levelsBeaten, f);
		fclose(f);
	}
}