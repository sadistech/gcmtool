/*
**	GCMBnrToolMain.c
**	Written by spike <spike@sadistech.com>
**
**	Comments to be added soon enough... blah. ;)
*/

//autoheader stuff
#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#define VERSION ""
#endif 

#include <stdio.h>
#include "GCMBnr.h"

void printUsage();

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

	printf("Version:	   \t%c\n", b->version);
	printf("Name:          \t%s\n", b->name);
	printf("Developer:     \t%s\n", b->developer);
	printf("Full Name:     \t%s\n", b->fullName);
	printf("Full Developer:\t%s\n", b->fullDeveloper);
	printf("Description:   \t%s\n", b->description);


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

void printUsage() {
	printf("gcmbnrtool %s- Utility for working with .bnr files from GameCube DVD Images (GCMs).\n", VERSION);
	printf("http://gcmtool.sourceforge.net\n\n");
	printf("Usage:\t");
	printf("gcmbnrtool <bnr_file>\n\n");
}