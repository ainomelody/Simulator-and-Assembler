#include <stdio.h>
#include "share.h"
#include <stdlib.h>
#include "preprocess.h"
#include "as.h"

int main(int argc, char *argv[])
{
	FILE *fin, *fout, *temp;
	
	checkExit(argc != 3, "Wrong parameters.\nUsage: sas input-file output-file", NULL);
	fin = fopen(argv[1], "r");
	checkExit(fin == NULL, "Can't open file", argv[1]);
	fout = fopen(argv[2], "wb");
	checkExit(fout == NULL, "Can't open file", argv[2]);
	atexit(disposeList);
	
	temp = preprocess(fin);
	assembly(temp, fout);
	fclose(temp);
	
	return 0;
}