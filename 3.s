BYTE str[23] = "Simulator and Assembler"
BYTE newstr[23]

LOADI G 0
LOADI A 23

copy:
LOADB B str
STOREB B newstr
ADDI G 1
LT G A
CJMP copy

LOADI G 0

out:
LOADB B newstr
OUT B 15
ADDI G 1
LT G A
CJMP out

HLT