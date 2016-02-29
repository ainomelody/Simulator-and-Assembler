BYTE str1[26] = "I design a program. It is "
BYTE str2[23] = "Simulator and Assembler"
BYTE newstr[49]

LOADI G 0
LOADI A 26
copy1:
LOADB B str1
STOREB B newstr
ADDI G 1
LT G A
CJMP copy1

LOADI G 0
LOADI A 23
LOADI B 26

copy2:
LOADB C str2
PUSH G
ADD G B Z
STOREB C newstr
POP G
ADDI G 1
ADDI B 1
LT G A
CJMP copy2

LOADI G 0
LOADI A 49

out:
LOADB B newstr
OUT B 15
ADDI G 1
LT G A
CJMP out
HLT