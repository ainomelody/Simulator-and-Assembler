#include "simBas.h"
#include <stdlib.h>
#include "share.h"
#include <stdio.h>

void func_HLT(long operands)
{
	exit(0);
}

void func_JMP(long operands)
{
	jump(operands);
}

void func_CJMP(long operands)
{
	if (get_CF())
		jump(operands);
}

void func_OJMP(long operands)
{
	if (get_OF())
		jump(operands);
}

void func_CALL(long operands)
{
	int i;
	
	for (i = 0; i < 7; i++)
		push_ES(normalREG[i]);
	push_ES(reg_PSW);
	push_ES(reg_PC);
	jump(operands);
}

void func_RET(long operands)
{
	int i;
	
	reg_PC = pop_ES();
	reg_PSW = pop_ES();
	for (i = 6; i >= 0; i--)
		normalREG[i] = pop_ES();
}

void func_PUSH(long operands)
{
	int reg0 = operands >> 24;
	
	*(reg_SS + --reg_SP) = readREG(reg0);
}

void func_POP(long operands)
{
	int reg0 = operands >> 24;
	
	writeREG(reg0, *(reg_SS + reg_SP++));
}

void func_LOADB(long operands)
{
	int reg0 = operands >> 24;
	char *start;
	
	start = reg_DS + (operands & 0xFFFFFF);
	writeREG(reg0, *(start + normalREG[6]));
}

void func_LOADW(long operands)
{
	int reg0 = operands >> 24;
	
	short *start;
	
	start = (short *)(reg_DS + (operands & 0xFFFFFF));
	writeREG(reg0, *(start + normalREG[6]));
}

void func_STOREB(long operands)
{
	int reg0 = operands >> 24;
	
	char *start = reg_DS + (operands & 0xFFFFFF);
	
	*(start + normalREG[6]) = readREG(reg0);
}

void func_STOREW(long operands)
{
	int reg0 = operands >> 24;
	short *start = (short *)(reg_DS + (operands & 0xFFFFFF));
	
	*(start + normalREG[6]) = readREG(reg0);
}

void func_LOADI(long operands)
{
	int reg0 = operands >> 24;
	
	
	writeREG(reg0, operands & 0xFFFF);
}

void func_NOP(long operands)
{
	
}

void func_IN(long operands)
{
	int reg0 = operands >> 24;
	
	checkExit((operands & 0xFF) != 0, "Invalid port", NULL);
	rewind(stdin);
	writeREG(reg0, getchar());
}

void func_OUT(long operands)
{
	int reg0 = operands >> 24;
	
	checkExit((operands & 0xFF) != 15, "Invalid port", NULL);
	putchar(readREG(reg0));
}

void func_ADD(long operands)
{
	int reg0, reg1, reg2;
	int result;
	
	reg0 = operands >> 24;
	reg1 = (operands >> 20) & 0xF;
	reg2 = (operands >> 16) & 0xF;
	result = readREG(reg1) + readREG(reg2);
	checkOF(result);
	writeREG(reg0, (short)result);
}

void func_ADDI(long operands)
{
	int reg0, result;
	
	reg0 = operands >> 24;
	result = operands & 0xFFFF;
	result += readREG(reg0);
	checkOF(result);
	writeREG(reg0, (short)result);
}

void func_SUB(long operands)
{
	int reg0, reg1, reg2, result;
	
	reg0 = operands >> 24;
	reg1 = (operands >> 20) & 0xF;
	reg2 = (operands >> 16) & 0xF;
	result = readREG(reg1) - readREG(reg2);
	checkOF(result);
	writeREG(reg0, (short)result);
}

void func_SUBI(long operands)
{
	int reg0, result;
	
	reg0 = operands >> 24;
	result = readREG(reg0) - (operands & 0xFFFF);
	checkOF(result);
	writeREG(reg0, (short)result);
}

void func_MUL(long operands)
{
	int reg0, reg1, reg2, result;
	
	reg0 = operands >> 24;
	reg1 = (operands >> 20) & 0xF;
	reg2 = (operands >> 16) & 0xF;
	result = readREG(reg1) * readREG(reg2);
	checkOF(result);
	writeREG(reg0, (short)result);
}

void func_DIV(long operands)
{
	int reg0, reg1, reg2;
	
	reg0 = operands >> 24;
	reg1 = (operands >> 20) & 0xF;
	reg2 = (operands >> 16) & 0xF;
	checkExit(readREG(reg2) == 0, "Divided by zero", NULL);
	writeREG(reg0, readREG(reg1) / readREG(reg2));
}

void func_AND(long operands)
{
	int reg0, reg1, reg2;
	
	reg0 = operands >> 24;
	reg1 = (operands >> 20) & 0xF;
	reg2 = (operands >> 16) & 0xF;
	writeREG(reg0, readREG(reg1) & readREG(reg2));
}

void func_OR(long operands)
{
	int reg0, reg1, reg2;
	
	reg0 = operands >> 24;
	reg1 = (operands >> 20) & 0xF;
	reg2 = (operands >> 16) & 0xF;
	writeREG(reg0, readREG(reg1) | readREG(reg2));
}

void func_NOR(long operands)
{
	int reg0, reg1, reg2;
	
	reg0 = operands >> 24;
	reg1 = (operands >> 20) & 0xF;
	reg2 = (operands >> 16) & 0xF;
	writeREG(reg0, readREG(reg1) ^ readREG(reg2));
}

void func_NOTB(long operands)
{
	int reg0, reg1;
	
	reg0 = operands >> 24;
	reg1 = (operands >> 20) & 0xF;
	writeREG(reg0, ~readREG(reg1));
}

void func_SAL(long operands)
{
	int reg0, reg1, reg2;
	
	reg0 = operands >> 24;
	reg1 = (operands >> 20) & 0xF;
	reg2 = (operands >> 16) & 0xF;
	writeREG(reg0, readREG(reg1) << readREG(reg2));
}

void func_SAR(long operands)
{
	int reg0, reg1, reg2;
	short mask = 0, result = 0;
	int num, move, i;
	
	reg0 = operands >> 24;
	reg1 = (operands >> 20) & 0xF;
	reg2 = (operands >> 16) & 0xF;
	num = readREG(reg1);
	move = readREG(reg2);
	for (i = 0; i < move; i++)
	{
		mask <<= 1;
		mask += 1;
	}
	result = num >> move;
	result &= (~mask);
	if (num & 0x7FFF)
		result |= mask;
	writeREG(reg0, result);
}

void func_EQU(long operands)
{
	int reg0, reg1;
	
	reg0 = operands >> 24;
	reg1 = (operands >> 20) & 0xF;
	set_CF(readREG(reg0) == readREG(reg1));
}

void func_LT(long operands)
{
	int reg0, reg1;
	
	reg0 = operands >> 24;
	reg1 = (operands >> 20) & 0xF;
	set_CF(readREG(reg0) < readREG(reg1));
}

void func_LTE(long operands)
{
	int reg0, reg1;
	
	reg0 = operands >> 24;
	reg1 = (operands >> 20) & 0xF;
	set_CF(readREG(reg0) <= readREG(reg1));
}

void func_NOTC(long operands)
{
	set_CF(!get_CF());
}

void (*exeInst[32])(long operands) = {
	func_HLT, func_JMP, func_CJMP, func_OJMP, func_CALL, func_RET, func_PUSH,
	func_POP, func_LOADB, func_LOADW, func_STOREB, func_STOREW, func_LOADI,
	func_NOP, func_IN, func_OUT, func_ADD, func_ADDI, func_SUB, func_SUBI,
	func_MUL, func_DIV, func_AND, func_OR, func_NOR, func_NOTB, func_SAL,
	func_SAR, func_EQU, func_LT, func_LTE, func_NOTC
};