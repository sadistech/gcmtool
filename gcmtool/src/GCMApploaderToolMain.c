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

//commandline options:

#define ARG_HEX				"-h"
#define ARG_HEX_SYN			"--hex"
#define ARG_HEX_OPT			""
#define ARG_HEX_HELP		"Display 32bit int values as hex"

#define ARG_DATE			"-d"
#define ARG_DATE_SYN		"--date"
#define ARG_DATE_OPT		"<date>"
#define ARG_DATE_HELP		"Set the date to " ARG_DATE_OPT ".  ASCII date in format MM/DD/YYYY"

#define ARG_ENTRYPOINT		"-e"
#define ARG_ENTRYPOINT_SYN	"--entrypoint"
#define ARG_ENTRYPOINT_OPT	"<address>"
#define ARG_ENTRYPOINT_HELP	"Set the entrypoint. to " ARG_ENTRYPOINT_OPT " (unsigned 32-bit int)"

#define ARG_UNKNOWN			"-u"


void openFile(char *mode);
void closeFile();
char *filename; //the filename/path we are working with...
FILE *appldrFile; //the file that we're working with

int hexFlag;

void printUsage();
void printExtendedUsage();

void printApploader(GCMApploaderStruct *a);

int main(int argc, char **argv) {
	hexFlag = 0;
	
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
		} else if (CHECK_ARG(ARG_HEX)) {
			//hex output!

			hexFlag = 1;
		} else {
			//this is the file...
			filename = currentArg;

			break;
		}
	} while(*argv);

	openFile("r");

	u32 len = GetFilesizeFromStream(appldrFile);

	char *data = (char*)malloc(len);

	if (fread(data, 1, len, appldrFile) != len) {
		perror(filename);
		exit(1);
	}

	GCMApploaderStruct *a = GCMRawApploaderToStruct(data);
	
	printApploader(a);
	
	closeFile();

	return 0;
}

void printApploader(GCMApploaderStruct *a) {
	//display that info...
	printf("Date:       %s\n", a->date);

	char format[255] = "";

	sprintf(format, "Entrypoint: %s\n", hexFlag ? "0x%08X" : "%lu");
	printf(format, a->entrypoint);
	sprintf(format,"Size:       %s\n", hexFlag ? "0x%08X" : "%lu");
	printf(format, a->size);
	sprintf(format, "Unknown:    %s\n", hexFlag ? "0x%08X" : "%lu");
	printf(format, a->unknown);
}

void openFile(char *mode) {
	if (!(appldrFile = fopen(filename, mode))) {
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

