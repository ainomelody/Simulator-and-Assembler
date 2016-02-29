#include "as.h"
#include "share.h"
#include "preprocess.h"
#include <string.h>

static unsigned long (*encodeFunc[8])(char *operands);		//8种格式指令的汇编函数
static int transREG(char *reg);								//将寄存器翻译为编号

void assembly(FILE *fin, FILE *fout)
{
	char line[INSTLENGTH];
	char *inst, *operands;
	int index;
	unsigned long result;
	
	outputData(fout);
	
	while (!feof(fin))
	{
		fgets(line, INSTLENGTH, fin);
		
		inst = strtok(line, " \t\n");
		operands = strtok(NULL, "\n");
		index = indexInst(inst);
		
		checkExit(index < 0, "Illegal instruction", NULL);
		/*操作数个数错误*/
		checkExit(splitStr(operands, " \t") != (operandsNum[index] - '0'), "Wrong usage of instruction", inst);
		result = index << 27;
		result += encodeFunc[encodeFuncIndex[index] - '0'](operands);
		fwrite(&result, 4, 1, fout);
	}
}

static int transREG(char *reg)
{
	checkExit(strlen(reg) > 1, "Illegal register", reg);
	if (*reg == 'Z')
		return 0;
	
	checkExit(*reg < 'A' || *reg > 'G', "Illegal register", reg);
	return (*reg - 'A' + 1);
}
static unsigned long func0(char *operands)
{
	return 0;
}

static unsigned long func1(char *operands)
{
	char label[INSTLENGTH];
	
	sscanf(operands, " %s", label);
	return transLabel(label);
}

static unsigned long func2(char *operands)
{
	char reg[INSTLENGTH] = {0};
	
	sscanf(operands, " %s", reg);
	return (transREG(reg) << 24);
}

static unsigned long func3(char *operands)
{
	char reg[INSTLENGTH], symbol[INSTLENGTH];
	
	sscanf(operands, " %s %s", reg, symbol);
	return ((transREG(reg) << 24) + (transSymbol(symbol) & 0xFFFFFF));
}

static unsigned long func4(char *operands)
{
	char reg[INSTLENGTH];
	int num;
	
	checkExit(sscanf(operands, " %s %d", reg, &num) != 2, "Wrong operands", NULL);
	return ((transREG(reg) << 24) + (num & 0xFFFF));
}

static unsigned long func5(char *operands)
{
	char reg[INSTLENGTH];
	int num;
	
	checkExit(sscanf(operands, " %s %d", reg, &num) != 2, "Wrong operands", NULL);
	return ((transREG(reg) << 24) + (num & 0xFF));
}

static unsigned long func6(char *operands)
{
	char reg0[INSTLENGTH], reg1[INSTLENGTH], reg2[INSTLENGTH];
	
	sscanf(operands, " %s %s %s", reg0, reg1, reg2);
	return ((transREG(reg0) << 24) + (transREG(reg1) << 20) + (transREG(reg2) << 16));
}

static unsigned long func7(char *operands)
{
	char reg0[INSTLENGTH], reg1[INSTLENGTH];
	
	sscanf(operands, " %s %s", reg0, reg1);
	return ((transREG(reg0) << 24) + (transREG(reg1) << 20));
}

static unsigned long (*encodeFunc[8])(char *operands) = {
	func0, func1, func2, func3, func4, func5, func6, func7
};