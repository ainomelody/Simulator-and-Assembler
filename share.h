#ifndef SHARE_H__

#define SHARE_H__

#define INSTLENGTH 80

extern const char * const instruction[32];						//指令名
const char *encodeFuncIndex;									//编码指令所用的函数
const char *operandsNum;											//指令所需的操作数个数

int splitStr(const char *str, const char *delim);				//str能用delim分割成多少部分
void checkExit(int condition, char *tips, char *addition);		//如果condition为真则输出tips和addition，addition可为NULL
int indexInst(char *inst);										//指令的编号

#endif