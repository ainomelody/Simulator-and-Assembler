#include "share.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void checkExit(int condition, char *tips, char *addition)
{
	if (condition)
	{
		printf("Error: %s", tips);
		if (addition)
			printf(" %s\n", addition);
		else
			putchar('\n');
		exit(1);
	}
}

const char * const instruction[32] = {
	"HLT", "JMP", "CJMP", "OJMP", "CALL", "RET",
	"PUSH", "POP",
	"LOADB", "LOADW", "STOREB", "STOREW", "LOADI", "NOP",
	"IN", "OUT",
	"ADD", "ADDI", "SUB", "SUBI", "MUL", "DIV",
	"AND", "OR", "NOR", "NOTB", "SAL", "SAR",
	"EQU", "LT", "LTE", "NOTC"
};

const char *encodeFuncIndex = "01111022333340556464666667667770";

const char *operandsNum = "01111011222220223232333332332220";

int splitStr(const char *str, const char *delim)
{
	char copy[INSTLENGTH];
	char *part;
	int count  = 0;
	
	if (str == NULL || !strcmp(str, ""))
		return 0;
	
	strcpy(copy, str);
	part = strtok(copy, delim);
	
	while (part != NULL)
	{
		count++;
		part = strtok(NULL, delim);
	}
	
	return count;
}

int indexInst(char *inst)
{
	int i;
	
	for (i = 0; i < 32; i++)
		if (!strcmp(inst, instruction[i]))
			return i;
		
	return -1;
}