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
#define ARG_HEX				"-h"
#define ARG_HEX_SYN			"--hex"
#define ARG_HEX_OPT			""
#define ARG_HEX_HELP		"Print all 32-bit ints in hexidecimal notation"

#define ARG_SYSTEMID		"-sid"
#define ARG_SYSTEMID_SYN	"--system-id"
#define ARG_SYSTEMID_OPT	"<id>"
#define ARG_SYSTEMID_HELP	"Set the systemID to " ARG_SYSTEMID_OPT " (G, U)"

#define ARG_GAMEID			"-gid"
#define ARG_GAMEID_SYN		"--game-id"
#define ARG_GAMEID_OPT		"<id>"
#define ARG_GAMEID_HELP		"Set the gameID to " ARG_GAMEID_OPT " (2 characters)"

#define ARG_REGION			"-r"
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

#define ARG_UNKNOWN1		"-u1"
#define ARG_UNKNOWN1_SYN	"--unknown-1"
#define ARG_UNKNOWN1_OPT	"<value>"
#define ARG_UNKNOWN1_HELP	"Set unknown1 to " ARG_UNKNOWN1_OPT " (unsigned 32-bit int)"

#define ARG_GAME_NAME		"-n"
#define ARG_GAME_NAME_SYN	"--game-name"
#define ARG_GAME_NAME_OPT	"<name>"
#define ARG_GAME_NAME_HELP	"Set game's name to " ARG_GAME_NAME_OPT " (string, max-chars: \" GCM_GAME_NAME_LENGTH \")"

#define ARG_DEBUG_OFFSET	"-dmo"
#define ARG_DEBUG_OFFSET_SYN	"--debug-monitor-offset"
#define ARG_DEBUG_OFFSET_OPT	"<offset>"
#define ARG_DEBUG_OFFSET_HELP	"Set the debug monitor offset to " ARG_DEBUG_OFFSET_OPT " (unsigned 32-bit int)"

#define ARG_DEBUG_ADDRESS	"-dma"
#define ARG_DEBUG_ADDRESS_SYN	"--debug-monitor-address"
#define ARG_DEBUG_ADDRESS_OPT	"<address>"
#define ARG_DEBUG_ADDRESS_HELP	"Set the debug monitor address to " ARG_DEBUG_ADDRESS_OPT " (unsigned 32-bit int)"

#define ARG_DOL_OFFSET		"-do"
#define ARG_DOL_OFFSET_SYN	"--dol-offset"
#define ARG_DOL_OFFSET_OPT	"<offset>"
#define ARG_DOL_OFFSET_HELP	"Set the offset of the main executable DOL to " ARG_DOL_OFFSET_OPT " (unsigned 32-bit int)"

#define ARG_FST_OFFSET		"-fo"
#define ARG_FST_OFFSET_SYN	"--fst-offset"
#define ARG_FST_OFFSET_OPT	"<offset>"
#define ARG_FST_OFFSET_HELP	"Set the offset of the FST to " ARG_FST_OFFSET_OPT " (unsigned 32-bit int)"

#define ARG_FST_SIZE		"-fs"
#define ARG_FST_SIZE_SYN	"--fst-size"
#define ARG_FST_SIZE_OPT	"<size>"
#define ARG_FST_SIZE_HELP	"Set the size of the FST to " ARG_FST_SIZE_OPT " (unsigned 32-bit int)"

#define ARG_FST_SIZE_MAX	"-fsm"
#define ARG_FST_SIZE_MAX_SYN	"--fst-size-max"
#define ARG_FST_SIZE_MAX_OPT	"<size>"
#define ARG_FST_SIZE_MAX_HELP	"Set the size of FST Size-Max (for multi-disc games) to " ARG_FST_SIZE_MAX_OPT " (unsigned 32-bit int)"

#define ARG_USER_POS		"-up"
#define ARG_USER_POS_SYN	"--user-position"
#define ARG_USER_POS_OPT	"<position>"
#define ARG_USER_POS_HELP	"Set the user position to " ARG_USER_POS_OPT " (unsigned 32-bit int)"

#define ARG_USER_LEN		"-ul"
#define ARG_USER_LEN_SYN	"--user-length"
#define ARG_USER_LEN_OPT	"<length>"
#define ARG_USER_LEN_HELP	"Set the user length to " ARG_USER_LEN_OPT " (unsigned 32-bit int)"

#define ARG_UNKNOWN2		"-u2"
#define ARG_UNKNOWN2_SYN	"--unknown-2"
#define ARG_UNKNOWN2_OPT	"<value>"
#define ARG_UNKNOWN2_HELP	"Set unknown2 to " ARG_UNKNOWN2_OPT " (unsigned 32-bit int)"

void printUsage();
void printExtendedUsage();

void printDiskHeader(GCMDiskHeaderStruct *d);

void openFile(char *mode);
void closeFile();

FILE *dhFile;
char *filename;

int hexFlag = 0;


int main(int argc, char **argv) {
	//commandline argument flags:
	
	int fileChanged = 0;
	
	char *newSystemID = NULL;
	char *newGameID = NULL;
	char *newRegionCode = NULL;
	char *newMakerCode = NULL;
	int newDiskID = -1;
	int newVersion = -1;
	int newStreaming = -1;
	int newStreamBufSize = -1;
	
	int modUnknown1 = 0; //since unknown1 can be 0, and is an UNSIGNED int, we gotta label it for change somehow...
	u32 newUnknown1 = 0;
	
	char *newGameName = NULL;
	
	int modDebugMonitorOffset = 0;
	u32 newDebugMonitorOffset = 0;

	int modDebugMonitorAddress = 0;
	u32 newDebugMonitorAddress = 0;

	int modDolOffset = 0;
	u32 newDolOffset = 0;

	int modFstOffset = 0;
	u32 newFstOffset = 0;

	int modFstSize = 0;
	u32 newFstSize = 0;

	int modFstSizeMax = 0;
	u32 newFstSizeMax = 0;

	int modUserPosition = 0;
	u32 newUserPosition = 0;

	int modUserLength = 0;
	u32 newUserLength = 0;

	int modUnknown2 = 0;
	u32 newUnknown2 = 0;

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
		} else if (CHECK_ARG(ARG_HEX)) {
			//the want hex notation...

			hexFlag = 1;
		
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
		} else if (CHECK_ARG(ARG_MAKER)) {
			// change the maker code...

			newMakerCode = GET_NEXT_ARG;

			if (strlen(newMakerCode) != GCM_MAKER_CODE_LENGTH) {
				printf("Maker code MUST be 2 characters, a-z, A-Z, 0-9. (%s is invalid)\n", newMakerCode);
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
		} else if (CHECK_ARG(ARG_UNKNOWN1)) {
			// they want to change the unknown1 (?)

			newUnknown1 = strtoul(GET_NEXT_ARG, NULL, 0);
			modUnknown1++;
			
		} else if (CHECK_ARG(ARG_GAME_NAME)) {
			// they want to change the gamename

			newGameName = GET_NEXT_ARG;

			if (strlen(newGameName) > GCM_GAME_NAME_LENGTH) {
				printf("Game name is too long. Must be %d characters or less.\n", GCM_GAME_NAME_LENGTH);
				exit(1);
			}
		} else if (CHECK_ARG(ARG_DEBUG_OFFSET)) {
			// they want to change the debug monitor offset...

			newDebugMonitorOffset = strtoul(GET_NEXT_ARG, NULL, 0);
			modDebugMonitorOffset++;
			
		} else if (CHECK_ARG(ARG_DEBUG_ADDRESS)) {
			// change the debug monitor length...

			newDebugMonitorAddress = strtoul(GET_NEXT_ARG, NULL, 0);
			modDebugMonitorAddress++;
			
		} else if (CHECK_ARG(ARG_DOL_OFFSET)) {
			//change DOL offset

			newDolOffset = strtoul(GET_NEXT_ARG, NULL, 0);
			modDolOffset++;
			
		} else if (CHECK_ARG(ARG_FST_OFFSET)) {
			// change FST offset
			
			newFstOffset = strtoul(GET_NEXT_ARG, NULL, 0);
			modFstOffset++;
			
		} else if (CHECK_ARG(ARG_FST_SIZE)) {
			// change FST size
			
			newFstSize = strtoul(GET_NEXT_ARG, NULL, 0);
			modFstSize++;
			
		} else if (CHECK_ARG(ARG_FST_SIZE_MAX)) {
			// change FST max size (for multi-disk)
			
			newFstSizeMax = strtoul(GET_NEXT_ARG, NULL, 0);
			modFstSizeMax++;
			
		} else if (CHECK_ARG(ARG_USER_POS)) {
			// change user position
			
			newUserPosition = strtoul(GET_NEXT_ARG, NULL, 0);
			modUserPosition++;

		} else if (CHECK_ARG(ARG_USER_LEN)) {
			// change user address
			
			newUserLength = strtoul(GET_NEXT_ARG, NULL, 0);
			modUserLength++;
			
		} else if (CHECK_ARG(ARG_UNKNOWN2)) {
			// change unknown
			
			newUnknown2 = strtoul(GET_NEXT_ARG, NULL, 0);
			modUnknown2++;
			
		} else {
			//this is the file...
			filename = currentArg;

			break;
		}
	} while(*argv);

	openFile("r");

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
	
	closeFile();
	
	//perform any operations on it...
	if (newSystemID != NULL) {
		d->systemID = newSystemID[0];
		fileChanged++;
	}

	if (newGameID != NULL) {
		strcpy(d->gameID, newGameID);
		fileChanged++;
	}

	if (newRegionCode != NULL) {
		d->regionCode = newRegionCode[0];
		fileChanged++;
	}

	if (newMakerCode != NULL) {
		strcpy(d->makerCode, newMakerCode);
		fileChanged++;
	}

	if (newDiskID >= 0) {
		d->diskID = (char)newDiskID;
		fileChanged++;
	}

	if (newVersion >= 0) {
		d->version = (char)newVersion;
		fileChanged++;
	}

	if (newStreaming >= 0) {
		d->streaming = (char)newStreaming;
		fileChanged++;
	}

	if (newStreamBufSize >= 0) {
		d->streamBufSize = (char)newStreamBufSize;
		fileChanged++;
	}

	if (modUnknown1) {
		d->unknown1 = newUnknown1;
		fileChanged++;
	}

	if (newGameName != NULL) {
		strcpy(d->gameName, newGameName);
		fileChanged++;
	}

	if (modDebugMonitorOffset) {
		d->debugMonitorOffset = newDebugMonitorOffset;
		fileChanged++;
	}

	if (modDebugMonitorAddress) {
		d->debugMonitorAddress = newDebugMonitorAddress;
		fileChanged++;
	}

	if (modDolOffset) {
		d->dolOffset = newDolOffset;
		fileChanged++;
	}

	if (modFstOffset) {
		d->fstOffset = newFstOffset;
		fileChanged++;
	}

	if (modFstSize) {
		d->fstSize = newFstSize;
		fileChanged++;
	}

	if (modFstSizeMax) {
		d->fstSizeMax = newFstSizeMax;
		fileChanged++;
	}

	if (modUserPosition) {
		d->userPosition = newUserPosition;
		fileChanged++;
	}

	if (modUserLength) {
		d->userLength = newUserLength;
		fileChanged++;
	}

	if (modUnknown2) {
		d->unknown2 = newUnknown2;
		fileChanged++;
	}

	// If any changes were made, print diskheader again...
	if (fileChanged) {
		//printf("\nWRITING TO FILE:\n");
		openFile("r+");
		
		printDiskHeader(d);
		char *data = (char*)malloc(GCM_DISK_HEADER_LENGTH);

		GCMDiskHeaderStructToRaw(d, data);

		rewind(dhFile);
		if (fwrite(data, 1, GCM_DISK_HEADER_LENGTH, dhFile) != GCM_DISK_HEADER_LENGTH) {
			perror("Something went wrong writing to the file:");
			exit(1);
		}

		free(data);
		closeFile();
	}

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
	
	char format[255] = "";
	sprintf(format, "Unknown1:              %s\n", hexFlag ? "0x%08lX" : "%lu");
	printf(format, d->unknown1);

	printf("Name:                  %s\n", d->gameName);

	sprintf(format, "Debug Monitor Offset:  %s\n", hexFlag ? "0x%08lX" : "%lu");
	printf(format, d->debugMonitorOffset);
	
	sprintf(format, "Debug Monitor Address: %s\n", hexFlag ? "0x%08lX" : "%lu");
	printf(format, d->debugMonitorAddress);
	
	sprintf(format, "DOL Offset:            %s\n", hexFlag ? "0x%08lX" : "%lu");
	printf(format, d->dolOffset);
	
	sprintf(format, "FST Offset:            %s\n", hexFlag ? "0x%08lX" : "%lu");
	printf(format, d->fstOffset);
	
	sprintf(format, "FST Size:              %s\n", hexFlag ? "0x%08lX" : "%lu");
	printf(format, d->fstSize);
	
	sprintf(format, "FST Size-Max:          %s\n", hexFlag ? "0x%08lX" : "%lu");
	printf(format, d->fstSizeMax);
	
	sprintf(format, "User Position:         %s\n", hexFlag ? "0x%08lX" : "%lu");
	printf(format, d->userPosition);
	
	sprintf(format, "User Length:           %s\n", hexFlag ? "0x%08lX" : "%lu");
	printf(format, d->userLength);
	
	sprintf(format, "Unknown2:              %s\n", hexFlag ? "0x%08lX" : "%lu");
	printf(format, d->unknown2);
}

void openFile(char *mode) {
	if (!(dhFile = fopen(filename, mode))) {
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
	PRINT_HELP(ARG_HEX);
	PRINT_HELP(ARG_SYSTEMID);
	PRINT_HELP(ARG_GAMEID);
	PRINT_HELP(ARG_REGION);
	PRINT_HELP(ARG_MAKER);
	PRINT_HELP(ARG_DISKID);
	PRINT_HELP(ARG_VERSION);
	PRINT_HELP(ARG_STREAMING);
	PRINT_HELP(ARG_STREAMING_BUF);
	PRINT_HELP(ARG_UNKNOWN1);
	PRINT_HELP(ARG_GAME_NAME);
	PRINT_HELP(ARG_DEBUG_OFFSET);
	PRINT_HELP(ARG_DEBUG_ADDRESS);
	PRINT_HELP(ARG_DOL_OFFSET);
	PRINT_HELP(ARG_FST_OFFSET);
	PRINT_HELP(ARG_FST_SIZE);
	PRINT_HELP(ARG_FST_SIZE_MAX);
	PRINT_HELP(ARG_USER_POS);
	PRINT_HELP(ARG_USER_LEN);
	PRINT_HELP(ARG_UNKNOWN2);
}

