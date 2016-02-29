all: sas ssim

sas: sas.o as.o preprocess.o share.o
	gcc sas.o as.o preprocess.o share.o -o sas.exe

sas.o as.o preprocess.o share.o inst.o simBas.o ssim.o:%.o:%.c
	gcc $< -c -o $@ -std=c99 -Wall -pedantic

ssim: inst.o share.o simBas.o ssim.o
	gcc inst.o share.o simBas.o ssim.o -o ssim.exe
clean:
	del *.o
	del *.exe