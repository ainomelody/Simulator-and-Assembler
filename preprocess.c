#include "share.h"
#include "preprocess.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

typedef struct symbol
{
	char name[40];									//符号名
	int size;										//包含元素个数
	int type;										//类型：1表示BYTE，2表示WORD
	unsigned long location;							//在数据段中的起始地址
	union{											//存储符号包含的元素
		char *byte_ptr;
		short *word_ptr;
	}data;
	struct symbol *next;
}Symbol;

typedef struct label
{
	char name[40];
	unsigned long location;
	struct label *next;
}Label;

static Symbol *shead = NULL, *send = NULL;
static Label *lhead = NULL;

static unsigned long dataSize = 0;

static void process1(FILE *fin, FILE *fout);		//去除前置空格和注释
static void process2(FILE *fin, FILE *fout);		//去除伪指令
static void process3(FILE *fin, FILE *fout);		//去除标号

static void removeSpace(FILE *fin);					//去除前置空格
static int readLine(FILE *fin, char *storage);		//读入一行，存储在storage中，返回此行后是否有注释
static void eatComment(FILE *fin);					//去掉注释
static int checkIdentifier(char *name, int mode);	//检查符号、标号是否可用，mode：1表示符号，2表示标号

FILE *preprocess(FILE *fin)
{
	FILE *temp1, *temp2, *temp3;
	
	temp1 = tmpfile();
	temp2 = tmpfile();
	temp3 = tmpfile();
	
	process1(fin, temp1);

	fseek(temp1, 0, SEEK_SET);
	process2(temp1, temp2);
	
	fseek(temp2, 0, SEEK_SET);
	process3(temp2, temp3);
	
	fclose(temp1);
	fclose(temp2);
	fseek(temp3, 0, SEEK_SET);
	
	return temp3;
}

static void removeSpace(FILE *fin)
{
	int c;
	
	while (isspace(c = fgetc(fin)) && !feof(fin));
	ungetc(c, fin);
}

static int readLine(FILE *fin, char *storage)
{
	char c;
	
	while ((c = fgetc(fin)) != '\n' && c != '#' && !feof(fin))
		*(storage++) = c;
	*storage = '\0';
	
	return (c == '#');
}

static void eatComment(FILE *fin)
{
	char c;
	
	while (!feof(fin) && (c = fgetc(fin)) != '\n');
}

static void process1(FILE *fin, FILE *fout)
{
	char line[INSTLENGTH];
	
	while (1)
	{
		removeSpace(fin);
		if (readLine(fin, line))
			eatComment(fin);
		if (!strcmp(line, ""))
			continue;
		fputs(line, fout);
		
		if (feof(fin))
			break;
		fputc('\n', fout);				//除最后一行，行末需加换行符
	}
}

static void process2(FILE *fin, FILE *fout)
{
	char line[INSTLENGTH];
	char name[INSTLENGTH];
	char *type, *declare, *assign;		//伪指令构成：类型 声明 [初始化]
	int assignNum, size;				//被初始化的元素个数，symbol包含的元素数目
	Symbol *newsym;
	char *lquote, *rquote, *lbracket;	//指向左引号、右引号、左中括号
	char *num;
	int i;
	
	while (!feof(fin))
	{
		size = 1;
		fgets(line, INSTLENGTH, fin);
		if (!strstr(line, "BYTE") && !strstr(line, "WORD"))		//不包含伪指令
		{
			fputs(line, fout);
			continue;
		}
		
		type = strtok(line, " \t");		//BYTE或WORD
		declare = strtok(NULL, "=");	//声明部分xx或xx[N]
		assign = strtok(NULL, "=");		//初始化部分
		
		/*BYTExxxx xxxx或标号+伪指令之类*/
		checkExit(strcmp(type, "BYTE") && strcmp(type, "WORD"), "Wrong dummy instruction", NULL);
		checkExit(declare == NULL, "Wrong dummy instruction", NULL);
		
		lbracket = strchr(declare, '[');
		
		if (lbracket)					
		{
			*lbracket = '\0';
			checkExit(sscanf(lbracket + 1, "%d", &size) == 0, "Illegal symbol declaration", NULL);	//[]中不写数字
		}
		
		checkExit(splitStr(declare, " \t") != 1, "Illegal symbol", declare);					//标号名包含空白				
		
		sscanf(declare, " %s", name);
		checkExit(checkIdentifier(name, 1) == 0, "Illegal symbol", name);
		
		newsym = (Symbol *)malloc(sizeof(Symbol));
		strcpy(newsym->name, name);
		newsym->size = size;
		newsym->type = !strcmp(type, "BYTE") ? 1 : 2;
		newsym->location = dataSize;
		newsym->data.byte_ptr = (char *)malloc(newsym->size * newsym->type);
		newsym->next = NULL;
		dataSize += newsym->size * newsym->type;
		if (send == NULL)
			shead = send = newsym;
		else
		{
			send->next = newsym;
			send = newsym;
		}
		
		if (assign)												//有初始化部分
		{
			if ((lquote = strchr(assign,'"')))					//字符串方式初始化
			{
				rquote = strrchr(assign, '"');
				checkExit(lquote == rquote, "Wrong assignment in dummy instruction", NULL);
				*rquote = '\0';
				assignNum = strlen(lquote + 1);					//需要初始化的元素个数
				checkExit(assignNum > size, "Too many elements in assignment", NULL);
				for (i = 0; i < assignNum; i++)
					if (newsym->type == 1)
						newsym->data.byte_ptr[i] = lquote[i + 1];
					else
						newsym->data.word_ptr[i] = lquote[i + 1];
			}
			else												//数字初始化方式
			{
				assignNum = splitStr(assign, ",");
				checkExit(assignNum > size, "Too many elements in assignment", NULL);
				lbracket = strchr(assign, '{');
				if (lbracket)
					*lbracket = ' ';
				
				for (i = 0; i < assignNum; i++)
				{
					if (i == 0)
						num = strtok(assign, ",");
					else
						num = strtok(NULL, ",");
					if (newsym->type == 1)
						newsym->data.byte_ptr[i] = atoi(num);
					else
						newsym->data.word_ptr[i] = atoi(num);
				}
			}
		}
	}
}

static void process3(FILE *fin, FILE *fout)
{
	char line[INSTLENGTH];
	unsigned long raw = 0;														//已读取的指令行数，用于标记标号位置
	char *colon;
	char name[INSTLENGTH];
	Label *newLabel;
	
	while (!feof(fin))
	{
		fgets(line, INSTLENGTH, fin);
		checkExit(strchr(line, ':') != strrchr(line, ':'), "Define more than one label in a line", NULL);
		colon = strchr(line, ':');
		if (colon)																//如果此行有标号
		{
			*colon = 0;
			checkExit(splitStr(line, " \t") != 1, "Illegal label", line);		//标号名中包含空白
			sscanf(line, " %s", name);
			checkExit(checkIdentifier(name, 2) == 0, "Illegal label", name);
			newLabel = (Label *)malloc(sizeof(Label));
			strcpy(newLabel->name, name);
			newLabel->location = raw;
			newLabel->next = lhead;
			lhead = newLabel;
			if (splitStr(colon + 1, " \t\n") == 0)								//检查标号后是否有指令
				continue;
			fputs(colon + 1, fout);
		}
		else																	//无标号原样输出
			fputs(line, fout);
		raw++;
	}
}

void disposeList()
{
	Symbol *stemp;
	Label *ltemp;
	
	while (shead != NULL)
	{
		stemp = shead->next;
		free(shead->data.byte_ptr);
		free(shead);
		shead = stemp;
	}
	
	while (lhead != NULL)
	{
		ltemp = lhead->next;
		free(lhead);
		lhead = ltemp;
	}
	
}

static int checkIdentifier(char *name, int mode)
{
	Symbol *stemp = shead;
	Label *ltemp = lhead;
	
	int i;
	
	for (i = 0; i < 32; i++)
		if (!strcmp(name, instruction[i]))
			return 0;
	
	if (!strcmp(name, "BYTE") || !strcmp(name, "WORD"))
		return 0;
	
	if (mode == 1)
	{
		while (stemp != NULL)
		{
			if (!strcmp(name, stemp->name))
				return 0;
			stemp = stemp->next;
		}
	}
	else
	{
		while (ltemp != NULL)
		{
			if (!strcmp(name, ltemp->name))
				return 0;
			ltemp = ltemp->next;
		}
	}
	
	return 1;
}

void outputData(FILE *fout)
{
	Symbol *temp = shead;
	
	fwrite(&dataSize, 4, 1, fout);
	while (temp != NULL)
	{
		fwrite(temp->data.byte_ptr, 1, temp->size * temp->type, fout);
		temp = temp->next;
	}
}

unsigned long transSymbol(char *symName)
{
	Symbol *temp = shead;
	
	while (temp != NULL)
	{
		if (!strcmp(symName, temp->name))
			return temp->location;
		temp = temp->next;
	}
	
	checkExit(1, "Undefined symbol", symName);
	return 0;
}

unsigned long transLabel(char *labName)
{
	Label *temp = lhead;
	
	while (temp != NULL)
	{
		if (!strcmp(labName, temp->name))
			return temp->location;
		temp = temp->next;
	}
	
	checkExit(1, "Undefined label", labName);
	return 0;
}