#include "simBas.h"
#include <limits.h>
#include "share.h"
#include <stdio.h>

void set_OF(int flag)
{
	unsigned short mask = 1 << 15;
	
	reg_PSW &= (~mask);
	flag &= 1;
	reg_PSW |= (flag << 15);
}

int get_OF()
{
	return (reg_PSW >> 15);
}

void set_CF(int flag)
{
	unsigned short mask = 1 << 14;
	
	reg_PSW &= (~mask);
	flag &= 1;
	reg_PSW |= (flag << 14);
}

int get_CF()
{
	return ((reg_PSW >> 14) & 1);
}

void push_ES(short data)
{
	*(reg_ES + --reg_EDI) = data;
}

short pop_ES()
{
	return *(reg_ES + reg_EDI++);
}

void jump(long address)
{
	reg_PC = address;
}

void writeREG(int index, short data)
{
	checkExit(index == 0, "Register Z is read-only", NULL);
	
	normalREG[index - 1] = data;
}

short readREG(int index)
{
	if (index == 0)
		return reg_Z;
	else
		return normalREG[index - 1];
}

void checkOF(int num)
{
	if (num > SHRT_MAX || num < SHRT_MIN)
		set_OF(1);
	else
		set_OF(0);
}