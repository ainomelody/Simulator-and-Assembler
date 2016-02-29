#include <stdlib.h>
#include <math.h>
#include "share.h"
#include <stdio.h>

extern void (*exeInst[32])(long oprands);

const short reg_Z = 0;
short normalREG[7];

char *memory;

static unsigned long *reg_CS, reg_IR;
unsigned long reg_SP, reg_EDI, reg_PC;
char *reg_DS;
short *reg_ES, *reg_SS;
unsigned short reg_PSW;

static void init();
static void load(FILE *fin);
static void clean();
static void run();

int main(int argc, char *argv[])
{
	FILE *fin;
	
	checkExit(argc != 2, "Wrong parameters.\nUsage: ssim file", NULL);
	fin = fopen(argv[1], "rb");
	checkExit(fin == NULL, "Failed to load file", argv[1]);
	init();
	load(fin);
	atexit(clean);
	run();
	
	return 0;
}

static void init()
{
	size_t size;
	
	size = pow(2, 24);
	memory = (char *)malloc(size);
	reg_CS = (unsigned long *)memory;
	reg_DS = memory + size / 4;
	reg_SS = (short *)(memory + size / 2);
	reg_ES = (short *)(memory + size * 3 / 4);
	reg_SP = reg_EDI = size / 8;
}

static void load(FILE *fin)
{
	unsigned long size;
	unsigned long *cs = reg_CS;
	
	fread(&size, 4, 1, fin);
	fread(reg_DS, 1, size, fin);
	while (!feof(fin))
		fread(cs++, 4, 1, fin);
}

static void clean()
{
	free(memory);
}

static void run()
{
	int inst;
	long oprands;
	
	while (1)
	{
		reg_IR = *(reg_CS + reg_PC++);
		inst = reg_IR >> 27;
		oprands = reg_IR & 0x7FFFFFF;
		exeInst[inst](oprands);
	}
	
}