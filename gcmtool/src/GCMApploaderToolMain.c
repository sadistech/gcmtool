/*
**	GCMApploaderToolMain.c
**	Spike Grobstein <spike666@mac.com>
**	
**	Part of the GCMTool Project
**	http://gcmtool.sourceforge.net
**	
**	Utility for working with the apploaders of GameCube DVD images.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#define VERSION ""
#endif

#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "GCMCommandline.h"
#include "FileFunctions.h"
#include "GCMutils.h"
#include "GCMextras.h"
#include "GCMApploader.h"

void openFile();
void closeFile();
char *filename; //the filename/path we are working with...
FILE *appldrFile; //the file that we're working with

void printUsage();
void printExtendedUsage();

int main(int argc, char **argv) {
	char *currentArg = NULL;

	do {
		currentArg = GET_NEXT_ARG;

		if (!currentArg) {
			printUsage();
			exit(1);
		} else if (CHECK_ARG(ARG_HELP)) {
			// the want extended usage...

			printExtendedUsage();
			exit(0);
		} else {
			//this is the file...
			filename = currentArg;

			break;
		}
	} while(*argv);

	openFile();

	u32 len = GetFilesizeFromStream(appldrFile);

	char *data = (char*)malloc(len);

	if (fread(data, 1, len, appldrFile) != len) {
		perror(filename);
		exit(1);
	}

	GCMApploaderStruct *a = GCMRawApploaderToStruct(data);

	//display that info...
	printf("Date:       %s\n", a->date);
	printf("Entrypoint: %08X\n", a->entrypoint);
	printf("Size:       %08X\n", a->size);
	printf("Unknown:    %08X\n", a->unknown);
	
	closeFile();

	return 0;
}

void openFile() {
	if (!(appldrFile = fopen(filename, "r+"))) {
		perror(filename);
		exit(1);
	}
}

void closeFile() {
	fclose(appldrFile);
}

void printUsage() {
	printf("gcmapploadertool %s- Utility for working with GameCube DVD Image apploaders.\n", VERSION);
	printf("http://gcmtool.sourceforge.net\n\n");
	printf("Usage:\t");
	printf("gcmapploadertool <apploader_file>\n\n");
	printf("Use %s to view extended usage.\n\n", ARG_HELP);
}

void printExtendedUsage() {
	printUsage();
	
	PRINT_HELP(ARG_HELP);
}

