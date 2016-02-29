#include "share.h"
#include "preprocess.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

typedef struct symbol
{
	char name[40];									//������
	int size;										//����Ԫ�ظ���
	int type;										//���ͣ�1��ʾBYTE��2��ʾWORD
	unsigned long location;							//�����ݶ��е���ʼ��ַ
	union{											//�洢���Ű�����Ԫ��
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

static void process1(FILE *fin, FILE *fout);		//ȥ��ǰ�ÿո��ע��
static void process2(FILE *fin, FILE *fout);		//ȥ��αָ��
static void process3(FILE *fin, FILE *fout);		//ȥ�����

static void removeSpace(FILE *fin);					//ȥ��ǰ�ÿո�
static int readLine(FILE *fin, char *storage);		//����һ�У��洢��storage�У����ش��к��Ƿ���ע��
static void eatComment(FILE *fin);					//ȥ��ע��
static int checkIdentifier(char *name, int mode);	//�����š�����Ƿ���ã�mode��1��ʾ���ţ�2��ʾ���

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
		fputc('\n', fout);				//�����һ�У���ĩ��ӻ��з�
	}
}

static void process2(FILE *fin, FILE *fout)
{
	char line[INSTLENGTH];
	char name[INSTLENGTH];
	char *type, *declare, *assign;		//αָ��ɣ����� ���� [��ʼ��]
	int assignNum, size;				//����ʼ����Ԫ�ظ�����symbol������Ԫ����Ŀ
	Symbol *newsym;
	char *lquote, *rquote, *lbracket;	//ָ�������š������š���������
	char *num;
	int i;
	
	while (!feof(fin))
	{
		size = 1;
		fgets(line, INSTLENGTH, fin);
		if (!strstr(line, "BYTE") && !strstr(line, "WORD"))		//������αָ��
		{
			fputs(line, fout);
			continue;
		}
		
		type = strtok(line, " \t");		//BYTE��WORD
		declare = strtok(NULL, "=");	//��������xx��xx[N]
		assign = strtok(NULL, "=");		//��ʼ������
		
		/*BYTExxxx xxxx����+αָ��֮��*/
		checkExit(strcmp(type, "BYTE") && strcmp(type, "WORD"), "Wrong dummy instruction", NULL);
		checkExit(declare == NULL, "Wrong dummy instruction", NULL);
		
		lbracket = strchr(declare, '[');
		
		if (lbracket)					
		{
			*lbracket = '\0';
			checkExit(sscanf(lbracket + 1, "%d", &size) == 0, "Illegal symbol declaration", NULL);	//[]�в�д����
		}
		
		checkExit(splitStr(declare, " \t") != 1, "Illegal symbol", declare);					//����������հ�				
		
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
		
		if (assign)												//�г�ʼ������
		{
			if ((lquote = strchr(assign,'"')))					//�ַ�����ʽ��ʼ��
			{
				rquote = strrchr(assign, '"');
				checkExit(lquote == rquote, "Wrong assignment in dummy instruction", NULL);
				*rquote = '\0';
				assignNum = strlen(lquote + 1);					//��Ҫ��ʼ����Ԫ�ظ���
				checkExit(assignNum > size, "Too many elements in assignment", NULL);
				for (i = 0; i < assignNum; i++)
					if (newsym->type == 1)
						newsym->data.byte_ptr[i] = lquote[i + 1];
					else
						newsym->data.word_ptr[i] = lquote[i + 1];
			}
			else												//���ֳ�ʼ����ʽ
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
	unsigned long raw = 0;														//�Ѷ�ȡ��ָ�����������ڱ�Ǳ��λ��
	char *colon;
	char name[INSTLENGTH];
	Label *newLabel;
	
	while (!feof(fin))
	{
		fgets(line, INSTLENGTH, fin);
		checkExit(strchr(line, ':') != strrchr(line, ':'), "Define more than one label in a line", NULL);
		colon = strchr(line, ':');
		if (colon)																//��������б��
		{
			*colon = 0;
			checkExit(splitStr(line, " \t") != 1, "Illegal label", line);		//������а����հ�
			sscanf(line, " %s", name);
			checkExit(checkIdentifier(name, 2) == 0, "Illegal label", name);
			newLabel = (Label *)malloc(sizeof(Label));
			strcpy(newLabel->name, name);
			newLabel->location = raw;
			newLabel->next = lhead;
			lhead = newLabel;
			if (splitStr(colon + 1, " \t\n") == 0)								//����ź��Ƿ���ָ��
				continue;
			fputs(colon + 1, fout);
		}
		else																	//�ޱ��ԭ�����
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