LOADI A 5
LOADI B 0	#和
LOADI C 1

sumloop:
PUSH C
CALL fact
POP F
ADD B B F
ADDI C 1
LTE C A
CJMP sumloop

PUSH B
CALL print
HLT

fact:
POP A
LOADI B 1
LOADI C 1
mulloop:
MUL C C B
ADDI B 1
LTE B A
CJMP mulloop
PUSH C
RET

print:			#输出10进制数
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