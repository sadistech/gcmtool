/*
**	GCMBnrToolMain.c
**	Written by spike <spike@sadistech.com>
**
**	Comments to be added soon enough... blah. ;)
*/

#include <stdio.h>
#include "GCMBnr.h"

int main(int argc, char **argv) {
	FILE *ifile = NULL;

	char *filename = argv[1];

	if (!(ifile = fopen(filename, "r"))) {
		printf("oops!\n");
		exit(1);
	}

	fseek(ifile, 0, SEEK_END);
	unsigned long len = ftell(ifile);
	rewind(ifile);

	char *data = (char*)malloc(len);

	if (fread(data, 1, len, ifile) != len) {
		printf("error!\n");
		exit(1);
	}

	GCMBnrStruct *b = GCMRawBnrToStruct(data);
	fclose(ifile);

	printf("version:\t%c\n", b->version);
	printf("name:   \t%s\n", b->name);
	printf("developer:\t%s\n", b->developer);
	printf("fullname:\t%s\n", b->fullName);
	printf("fullDev:\t%s\n", b->fullDeveloper);
	printf("desc:\t%s\n", b->description);


	FILE *ofile = NULL;

	char outfilename[255];
	strcpy(outfilename, filename);
	strcat(outfilename, ".ppm");

	if (!(ofile = fopen(outfilename, "w"))) {
		printf("ERROR!\n");
		exit(1);
	}

	len = GCM_BNR_GRAPHIC_WIDTH * GCM_BNR_GRAPHIC_HEIGHT * 3 + 256;
	char *pic = (char*)malloc(len);
	
	GCMBnrGetImagePPM(b, pic);

	if (fwrite(pic, 1, len, ofile) != len) {
		printf("woops!\n");
		exit(1);
	}

	fclose(ofile);
}
