/*
**	GCMDiskHeaderInfoToolMain.c
**	Spike Grobstein <spike666@mac.com>
**	
**	Part of the GCMTool Project
**	http://gcmtool.sourceforge.net
**	
**	Utility for working with the diskheaderinfo of GameCube DVD images.
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
#include "GCMDiskHeaderInfo.h"

void openFile();
void closeFile();
char *filename; //the filename/path we are working with...
FILE *dhiFile; //the file that we're working with

void printUsage();
void printExtendedUsage();

int main(int argc, char **argv) {
	char *currentArg = NULL;

	do {
		currentArg = GET_NEXT_ARG;

		if (!curentArg) {
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

	u32 len = GetFilesizeFromStream(dhiFile);

	if (len != GCM_DISK_HEADER_INFO_LENGTH) {
		printf("%s does not appear to be a GCM diskheaderinfo.\n");
		closeFile();
		exit(1);
	}

	char *data = (char*)malloc(len);

	if (fread(data, 1, len, dhiFile) != len) {
		perror(filename);
		exit(1);
	}

	GCMDiskHeaderInfoStruct *d = GCMRawDiskHeaderInfoToStruct(data);

	//display that info...
	printf("Debug Monitor Size:    %08X\n", d->debugMonitorSize);
	printf("Simulated Memory Size: %08X\n", d->simulatedMemorySize);
	printf("Argument Offset:       %08X\n", d->argumentOffset);
	printf("Debug Flag:            %08X\n", d->debugFlag);
	printf("Track Location:        %08X\n", d->trackLocation);
	printf("Track Size:            %08X\n", d->trackSize);
	printf("Country Code:          %08X\n", d->countryCode);
	printf("Unknown 1:             %08X\n", d->unknown1);
	printf("Unknown 2:             %08X\n", d->unknown2);
	
	closeFile();

	return 0;
}

void openFile() {
	if (!(dhFile = fopen(filename, "r+"))) {
		perror(filename);
		exit(1);
	}
}

void closeFile() {
	fclose(dhiFile);
}

void printUsage() {
	printf("gcmdiskheaderinfotool %s- Utility for working with GameCube DVD Image diskheaderinfo.\n", VERSION);
	printf("http://gcmtool.sourceforge.net\n\n");
	printf("Usage:\t");
	printf("gcmdiskheaderinfotool <diskheaderinfo_file>\n\n");
	printf("Use %s to view extended usage.\n\n", ARG_HELP);
}

void printExtendedUsage() {
	printUsage();
	
	PRINT_HELP(ARG_HELP);
}

