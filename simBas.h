#ifndef SIMBAS_H__

#define SIMBAS_H__

extern short normalREG[7], reg_PSW, *reg_ES, *reg_SS;
extern unsigned long reg_SP, reg_PC, reg_EDI;
extern const short reg_Z;
char *reg_DS;

void set_OF(int flag);
int get_OF();
void set_CF(int flag);
int get_CF();
void push_ES(short data);					//将data压入附加段
short pop_ES();								//从附加段中弹出2B数据
void jump(long address);					//跳转到address处
void writeREG(int index, short data);		//将data写入index对应的寄存器
short readREG(int index);					//读index对应的寄存器
void checkOF(int num);						//检查num是否超出short的范围并设置OF

#endif