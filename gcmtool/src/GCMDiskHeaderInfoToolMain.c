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

