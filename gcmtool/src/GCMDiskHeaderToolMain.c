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
#include <string.h>

#include "FileFunctions.h"
#include "types.h"
#include "GCMCommandline.h"
#include "GCMDiskHeader.h"
#include "GCMutils.h"
#include "GCMextras.h"


// Commandline arguments:
#define ARG_SYSTEMID		"-sid"
#define ARG_SYSTEMID_SYN	"--system-id"
#define ARG_SYSTEMID_OPT	"<id>"
#define ARG_SYSTEMID_HELP	"Set the systemID to " ARG_SYSTEMID_OPT " (G, U)"

#define ARG_GAMEID		"-gid"
#define ARG_GAMEID_SYN		"--game-id"
#define ARG_GAMEID_OPT		"<id>"
#define ARG_GAMEID_HELP		"Set the gameID to " ARG_GAMEID_OPT " (2 characters)"

#define ARG_REGION		"-r"
#define ARG_REGION_SYN		"--region"
#define ARG_REGION_OPT		"<region>"
#define ARG_REGION_HELP		"Set the region to " ARG_REGION_OPT " (E, P, J)"

#define ARG_MAKER		"-m"
#define ARG_MAKER_SYN		"--maker-code"
#define ARG_MAKER_OPT		"<code>"
#define ARG_MAKER_HELP		"Set the maker-code to " ARG_MAKER_OPT " (2 characters, A-Z, a-z, 0-9)"

#define ARG_DISKID		"-d"
#define ARG_DISKID_SYN		"--disk-id"
#define ARG_DISKID_OPT		"<id>"
#define ARG_DISKID_HELP		"Set the diskID to " ARG_DISKID_OPT " (0-255)"

#define ARG_VERSION		"-V"
#define ARG_VERSION_SYN		"--version"
#define ARG_VERSION_OPT		"<version>"
#define ARG_VERSION_HELP	"Set the version to " ARG_VERSION_OPT " (0-255)"

#define ARG_STREAMING		"-s"
#define ARG_STREAMING_SYN	"--streaming"
#define ARG_STREAMING_OPT	"<streaming>"
#define ARG_STREAMING_HELP	"Set streaming to " ARG_STREAMING_OPT " (0-255)"

#define ARG_STREAMING_BUF	"-sb"
#define ARG_STREAMING_BUF_SYN	"--streaming-buf-size"
#define ARG_STREAMING_BUF_OPT	"<size>"
#define ARG_STREAMING_BUF_HELP	"Set streaming buffer size to " ARG_STREAMING_BUF_OPT " (0-255)"

void printUsage();
void printExtendedUsage();

void printDiskHeader(GCMDiskHeaderStruct *d);

void openFile();
void closeFile();

FILE *dhFile;
char *filename;

int main(int argc, char **argv) {
	//commandline argument flags:
	int fileChanged = 0;
	
	char *newSystemID = NULL;
	char *newGameID = NULL;
	char *newRegionCode = NULL;
	char *newMakerCode = NULL;
	int newDiskID = 0;
	int newVersion = 0;
	int newStreaming = 0;
	int newStreamBufSize = 0;
	

	//start processing the arguments...
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
		} else if (CHECK_ARG(ARG_SYSTEMID)) {
			//they want to change the systemID

			newSystemID = GET_NEXT_ARG;

			if (strlen(newSystemID) != 1) {
				printf("SystemID MUST be one character. (%s is invalid)\n", newSystemID);
				exit(1);
			}
		} else if (CHECK_ARG(ARG_GAMEID)) {
			// they want to change the gameID

			newGameID = GET_NEXT_ARG;

			if (strlen(newGameID) != GCM_GAME_ID_LENGTH) {
				printf("GameID MUST be 2 characters. (%s is invalid)\n", newGameID);
				exit(1);
			}
		} else if (CHECK_ARG(ARG_REGION)) {
			// they want to change the region

			newRegionCode = GET_NEXT_ARG;

			if (strlen(newRegionCode) != 1) {
				printf("Region MUST be 1 character. (%s is invalid)\n", newRegionCode);
				exit(1);
			}
		} else if (CHECK_ARG(ARG_DISKID)) {
			// they want to change the diskid

			newDiskID = atoi(GET_NEXT_ARG);

			if (newDiskID > 255 || newDiskID < 0) {
				printf("DiskID is out of range. Must be 0-255. (%d is invalid)\n", newDiskID);
				exit(1);
			}
		} else if (CHECK_ARG(ARG_VERSION)) {
			// they want to change the version

			newVersion = atoi(GET_NEXT_ARG);

			if (newVersion > 255 || newVersion < 0) {
				printf("Version is out of range. Must be 0-255. (%d is invalid)\n", newVersion);
				exit(1);
			}
		} else if (CHECK_ARG(ARG_STREAMING)) {
			// they want to change the streaming (?)

			newStreaming = atoi(GET_NEXT_ARG);

			if (newStreaming > 255 || newStreaming < 0) {
				printf("Streaming is out of range. Must be 0-255. (%d is invalid)\n", newStreaming);
				exit(1);
			}
		} else if (CHECK_ARG(ARG_STREAMING_BUF)) {
			// they want to change the streaming buffer size... (?)

			newStreamBufSize = atoi(GET_NEXT_ARG);

			if (newStreamBufSize > 255 || newStreamBufSize < 0) {
				printf("Streaming buffer size is out of range. Must be 0-255. (%d is invalid)\n", newStreamBufSize);
				exit(1);
			}
		} else {
			//this is the file...
			filename = currentArg;

			break;
		}
	} while(*argv);

	openFile();

	u32 len = GetFilesizeFromStream(dhFile);

	// Disk headers are supposed to be a specific size...
	// If it's not the right size, then consider it to be invalid.
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
	
	printDiskHeader(d); //print the diskheader...

	//perform any operations on it...


	// If any changes were made, print diskheader again...
	if (fileChanged) {
		printDiskHeader(d);
	}

	closeFile();

	return 0;
}

void printDiskHeader(GCMDiskHeaderStruct *d) {
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
	PRINT_HELP(ARG_SYSTEMID);
	PRINT_HELP(ARG_GAMEID);
	PRINT_HELP(ARG_REGION);
	PRINT_HELP(ARG_MAKER);
	PRINT_HELP(ARG_DISKID);
	PRINT_HELP(ARG_VERSION);
	PRINT_HELP(ARG_STREAMING);
	PRINT_HELP(ARG_STREAMING_BUF);
}

