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

#define ARG_DEBUG_MON_SIZE		"-dms"
#define ARG_DEBUG_MON_SIZE_SYN		"--debug-monitor-size"
#define ARG_DEBUG_MON_SIZE_OPT		"<size>"
#define ARG_DEBUG_MON_SIZE_HELP		"Set the debug monitor size to " ARG_DEBUG_MON_SIZE_OPT " (unsigned 32-bit int)"

#define ARG_SIM_MEM_SIZE		"-sms"
#define ARG_SIM_MEM_SIZE_SYN		"--simulated-memory-size"
#define ARG_SIM_MEM_SIZE_OPT		"<size>"
#define ARG_SIM_MEM_SIZE_HELP		"Set the simulated memory size to " ARG_SIM_MEM_SIZE_OPT " (unsigned 32-bit int)"

#define ARG_ARGUMENT_OFFSET		"-a"
#define ARG_ARGUMENT_OFFSET_SYN		"--argument-offset"
#define ARG_ARGUMENT_OFFSET_OPT		"<offset>"
#define ARG_ARGUMENT_OFFSET_HELP	"Set the argument offset to " ARG_ARGUMENT_OFFSET_OPT " (unsigned 32-bit int)"

#define ARG_DEBUG_FLAG			"-d"
#define ARG_DEBUG_FLAG_SYN		"--debug-flag"
#define ARG_DEBUG_FLAG_OPT		"<flag>"
#define ARG_DEBUG_FLAG_HELP		"Set the debug flag to " ARG_DEBUG_FLAG_OPT " (unsigned 32-bit int)"

#define ARG_TRACK_LOCATION		"-tl"
#define ARG_TRACK_LOCATION_SYN		"--track-location"
#define ARG_TRACK_LOCATION_OPT		"<location>"
#define ARG_TRACK_LOCATION_HELP		"Set the track location to " ARG_TRACK_LOCATION_OPT " (unsigned 32-bit int)"

#define ARG_TRACK_SIZE			"-ts"
#define ARG_TRACK_SIZE_SYN		"--track-size"
#define ARG_TRACK_SIZE_OPT		"<size>"
#define ARG_TRACK_SIZE_HELP		"Set the track size to " ARG_TRACK_SIZE_OPT " (unsigned 32-bit int)"

#define ARG_COUNTRY_CODE		"-c"
#define ARG_COUNTRY_CODE_SYN		"--country-code"
#define ARG_COUNTRY_CODE_OPT		"<code>"
#define ARG_COUNTRY_CODE_HELP		"Set the country code to " ARG_COUNTRY_CODE_OPT " (unsigned 32-bit int)"

#define ARG_UNKNOWN1			"-u1"
#define ARG_UNKNOWN1_SYN		"--unknown-1"
#define ARG_UNKNOWN1_OPT		"<value>"
#define ARG_UNKNOWN1_HELP		"Set unknown1 to " ARG_UNKNOWN1_OPT " (unsigned 32-bit int)"

#define ARG_UNKNOWN2			"-u2"
#define ARG_UNKNOWN2_SYN		"--unknown-2"
#define ARG_UNKNOWN2_OPT		"<value>"
#define ARG_UNKNOWN2_HELP		"Set unknown2 to " ARG_UNKNOWN2_OPT " (unsigned 32-bit int)"

void openFile();
void closeFile();
char *filename; //the filename/path we are working with...
FILE *dhiFile; //the file that we're working with

void printUsage();
void printExtendedUsage();

void printDiskHeaderInfoStruct(GCMDiskHeaderInfoStruct *d);

int main(int argc, char **argv) {
	int fileChanged = 0;

	int modDebugMonitorSize = 0;
	u32 newDebugMonitorSize = 0;

	int modSimulatedMemorySize = 0;
	u32 newSimulatedMemorySize = 0;
	
	int modArgumentOffset = 0;
	u32 newArgumentOffset = 0;
	
	int modDebugFlag = 0;
	u32 newDebugFlag = 0;
	
	int modTrackLocation = 0;
	u32 newTrackLocation = 0;
	
	int modTrackSize = 0;
	u32 newTrackSize = 0;
	
	int modCountryCode = 0;
	u32 newCountryCode = 0;
	
	int modUnknown1 = 0;
	u32 newUnknown1 = 0;
	
	int modUnknown2 = 0;
	u32 newUnknown2 = 0;

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
		} else if (CHECK_ARG(ARG_DEBUG_MON_SIZE)) {
			// set the debug monitor size

			newDebugMonitorSize = atol(GET_NEXT_ARG);
			modDebugMonitorSize++;
			
		} else if (CHECK_ARG(ARG_SIM_MEM_SIZE)) {
			// set the simulated memory size

			newSimulatedMemorySize = atol(GET_NEXT_ARG);
			modSimulatedMemorySize++;
			
		} else if (CHECK_ARG(ARG_ARGUMENT_OFFSET)) {
			//set the argument offset

			newArgumentOffset = atol(GET_NEXT_ARG);
			modArgumentOffset++;
			
		} else if (CHECK_ARG(ARG_DEBUG_FLAG)) {
			//set the debug flag

			newDebugFlag = atol(GET_NEXT_ARG);
			modDebugFlag++;
			
		} else if (CHECK_ARG(ARG_TRACK_LOCATION)) {
			// set the track location

			newTrackLocation = atol(GET_NEXT_ARG);
			modTrackLocation++;
			
		} else if (CHECK_ARG(ARG_TRACK_SIZE)) {
			//set the track size

			newTrackSize = atol(GET_NEXT_ARG);
			modTrackSize++;
			
		} else if (CHECK_ARG(ARG_COUNTRY_CODE)) {
			// set the country code

			newCountryCode = atol(GET_NEXT_ARG);
			modCountryCode++;
			
		} else if (CHECK_ARG(ARG_UNKNOWN1)) {
			// set unknown1

			newUnknown1 = atol(GET_NEXT_ARG);
			modUnknown1++;
			
		} else if (CHECK_ARG(ARG_UNKNOWN2)) {
			// set unknown2

			newUnknown2 = atol(GET_NEXT_ARG);
			modUnknown2++;
			
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

	printDiskHeaderInfoStruct(d);

	if (modDebugMonitorSize) {
		d->debugMonitorSize = newDebugMonitorSize;
		fileChanged++;
	}

	if (modSimulatedMemorySize) {
		d->simulatedMemorySize = newSimulatedMemorySize;
		fileChanged++;
	}

	if (modArgumentOffset) {
		d->argumentOffset = newArgumentOffset;
		fileChanged++;
	}

	if (modDebugFlag) {
		d->debugFlag = newDebugFlag;
		fileChanged++;
	}

	if (modTrackLocation) {
		d->trackLocation = newTrackLocation;
		fileChanged++;
	}

	if (modTrackSize) {
		d->trackSize = newTrackSize;
		fileChanged++;
	}

	if (modCountryCode) {
		d->countryCode = newCountryCode;
		fileChanged++;
	}

	if (modUnknown1) {
		d->unknown1 = newUnknown1;
		fileChanged++;
	}

	if (modUnknown2) {
		d->unknown2 = newUnknown2;
		fileChanged++;
	}

	if (fileChanged) {
		printDiskHeaderInfoStruct(d);

		char *data = (char*)malloc(GCM_DISK_HEADER_INFO_LENGTH);
		GCMDiskHeaderInfoStructToRaw(d, data);

		rewind(dhiFile);
		if (fwrite(data, 1, GCM_DISK_HEADER_INFO_LENGTH, dhiFile) != GCM_DISK_HEADER_INFO_LENGTH) {
			perror("Error writing diskheaderinfo!");
			exit(1);
		}
		
		free(data);
	}

	
	closeFile();

	return 0;
}

void printDiskHeaderInfoStruct(GCMDiskHeaderInfoStruct *d) {	
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
}

void openFile() {
	if (!(dhiFile = fopen(filename, "r+"))) {
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
	PRINT_HELP(ARG_DEBUG_MON_SIZE);
	PRINT_HELP(ARG_SIM_MEM_SIZE);
	PRINT_HELP(ARG_ARGUMENT_OFFSET);
	PRINT_HELP(ARG_DEBUG_FLAG);
	PRINT_HELP(ARG_TRACK_LOCATION);
	PRINT_HELP(ARG_TRACK_SIZE);
	PRINT_HELP(ARG_COUNTRY_CODE);
	PRINT_HELP(ARG_UNKNOWN1);
	PRINT_HELP(ARG_UNKNOWN2);
}

