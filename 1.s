LOADI A 1
LOADI B 100		#控制循环
LOADI C 0		#和

sumloop:
ADD C C A
ADDI A 1
LTE A B
CJMP sumloop

PUSH C
CALL print

HLT

print:			#输出十进制数
POP A
LOADI B 10
LOADI C 0
divloop:
DIV D A B		#D=A/10
MUL E D B
SUB F A E		#F为最后一位
PUSH F
ADDI C 1		#C为位数
ADD A D Z
EQU A Z
NOTC			#A!=0
CJMP divloop

outloop:
POP F
ADDI F 48		#F += '0'
OUT F 15
SUBI C 1
EQU C Z
NOTC			#C != 0
CJMP outloop

RET