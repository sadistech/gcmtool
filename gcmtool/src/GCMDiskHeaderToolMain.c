/*
**	GCMDiskHeaderToolMain.c
**	Spike Grobstein <spike666@mac.com>
**	
**	Part of the GCMTool Project
**	http://gcmtool.sourceforge.net
**	
**	Utility for working with the diskheader of GameCube DVD images.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#define VERSION ""
#endif

#include <stdio.h>
#include <stdlib.h>

#include "FileFunctions.h"
#include "types.h"
#include "GCMCommandline.h"
#include "GCMDiskHeader.h"
#include "GCMutils.h"
#include "GCMextras.h"

void printUsage();
void printExtendedUsage();

void openFile();
void closeFile();

FILE *dhFile;
char *filename;

int main(int argc, char **argv) {
	char *currentArg = NULL;

	do {
		currentArg = GET_NEXT_ARG;

		if (!currentArg) {
			printUsage();
			exit(1);
		} else if (CHECK_ARG(ARG_HELP)) {
			//they want extend usage...

			printExtendedUsage();
			exit(0);
		} else {
			//this is the file...
			filename = currentArg;

			break;
		}
	} while(*argv);

	openFile();

	u32 len = GetFilesizeFromStream(dhFile);

	if (len != GCM_DISK_HEADER_LENGTH) {
		printf("%s does not appear to be a GCM diskheader.\n");
		closeFile();
		exit(1);
	}

	char *data = (char*)malloc(len);

	if (fread(data, 1, len, dhFile) != len) {
		perror(filename);
		exit(1);
	}

	GCMDiskHeaderStruct *d = GCMRawDiskHeaderToStruct(data);

	//display info...
	printf("System ID:             %c (%s)\n", d->systemID, GCMSystemIDToStr(d->systemID));
	printf("Game ID:               %s\n", d->gameID);
	printf("Region:                %c (%s)\n", d->regionCode, GCMRegionCodeToStr(d->regionCode));
	printf("Maker:                 %s (%s)\n", d->makerCode, GCMMakerCodeToStr(d->makerCode));
	printf("Disk ID:               %d\n", d->diskID);
	printf("Version:               %d\n", d->version);
	printf("Streaming:             %d\n", d->streaming);
	printf("Stream Buffer Size:    %d\n", d->streamBufSize);
	printf("Unknown1:              %08X\n", d->unknown1);
	printf("Name:                  %s\n", d->gameName);
	printf("Debug Monitor Offset:  %08X\n", d->debugMonitorOffset);
	printf("Debug Monitor Address: %08X\n", d->debugMonitorAddress);
	printf("DOL Offset:            %08X\n", d->dolOffset);
	printf("FST Offset:            %08X\n", d->fstOffset);
	printf("FST Size:              %08X\n", d->fstSize);
	printf("FST Size-Max:          %08X\n", d->fstSizeMax);
	printf("User Position:         %08X\n", d->userPosition);
	printf("User Length:           %08X\n", d->userLength);
	printf("Unknown2:              %08X\n", d->unknown2);

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
	fclose(dhFile);
}

void printUsage() {
	printf("gcmdiskheadertool %s- Utility for working with GameCube DVD Image diskheaders.\n", VERSION);
	printf("http://gcmtool.sourceforge.net\n\n");
	printf("Usage:\t");
	printf("gcmdiskheadertool <diskheader_file>\n\n");
	printf("Use %s to view extended usage.\n\n", ARG_HELP);
}

void printExtendedUsage() {
	printUsage();

	PRINT_HELP(ARG_HELP);
}

