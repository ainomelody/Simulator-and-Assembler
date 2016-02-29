#ifndef PREPROCESS_H__

#define PREPROCESS_H__

#include <stdio.h>

FILE *preprocess(FILE *fin);				//预处理函数，返回用于汇编的文件指针
void disposeList();							//释放链表
void outputData(FILE *fout);				//输出数据段
unsigned long transLabel(char *labName);	//翻译标号
unsigned long transSymbol(char *symName);	//翻译符号

#endif