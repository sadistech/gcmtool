//for the autoheader stuff... (grrr, I hate this thing...)
#if HAVE_CONFIG_H
#include <config.h>
#else
//if you're not configuring from the commandline...
#define VERSION ""
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GCMutils.h"
#include "GCMextras.h"
#include "GCMFileEntry.h"

#include "GCMCommandline.h"
#include "pathfunc.h"
#include "types.h"

#define ARG_VERBOSE							"-v"
#define ARG_VERBOSE_SYN						"--verbose"
#define ARG_VERBOSE_OPT						""
#define ARG_VERBOSE_HELP					"Verbose output"

void printUsage();

int verboseFlag;

char *filename;
char *filepath;

int main(int argc, char **argv) {
	verboseFlag = 0;

	char *currentArg = NULL;

	do {
		currentArg = GET_NEXT_ARG;

		if (!currentArg) {
			//there's no args! uh oh!

			printUsage();
			exit(EXIT_SUCCESS); //exit success because we're just printing usage...
		} else if (CHECK_ARG(ARG_VERBOSE)) {
			//turn on verbosity!

			verboseFlag++;
	//		verbosePrint("Verbose output ON.");

		} else {
			//this is the file we want to open...

			filepath = currentArg;
			filename = lastPathComponent(filepath);

			break;
		}
	} while(*argv);
	
	printf("opening %s\n", filename);

	FILE *gcmFile;

	if (!(gcmFile = fopen(filepath, "r"))) {
		perror("error opening!\n");
		exit(EXIT_FAILURE);
	}

	u32 fstOffset = GCMGetFSTOffset(gcmFile);
	u32 fstSize = GCMGetFSTSize(gcmFile);
	u32 stringTableOffset = GCMGetStringTableOffset(gcmFile);
	GCMFileEntryStruct *root = GCMGetRootFileEntry(gcmFile);

	printf("FST Offset: %ld\n", fstOffset);
	printf("FST Size: %ld\n", fstSize);
	printf("String Table: %ld\n", stringTableOffset);
	printf("entry count: %ld\n", root->length);
	
	int i = 0;
	u32 highestStringTableOffset = 0;
	u32 firstFileOffset = GetFilesizeFromStream(gcmFile);
	
	for (i = 0; i < root->length; i++) {
		GCMFileEntryStruct *e = GCMGetNthFileEntry(gcmFile, i);

		if (e->filenameOffset > highestStringTableOffset) {
			GCMFetchFilenameForFileEntry(gcmFile, e);
			highestStringTableOffset = e->filenameOffset + strlen(e->filename) + 1; // +1 for \0
		}

		if (!(e->isDir) && e->offset < firstFileOffset) {
			firstFileOffset = e->offset;
		}
	}

	printf("\n");
	printf("HighestStringTableOffset: %ld\n", highestStringTableOffset);
	printf("firstFileOffset: %ld\n", firstFileOffset);

	printf("\n");
	printf("Savings: %ld\n", firstFileOffset - (stringTableOffset + highestStringTableOffset));

	fclose(gcmFile);
}

void printUsage() {
	printf("GCMStrip %s- Strips un-needed padding from Gamecube DVD images and reduces filesize.\n\tgcmtool.sourceforge.net\n\n", VERSION);
	printf("Usage:");
	printf("  gcmstrip [ options ] <filename>\n\n");
	printf("Use -? for extended usage.\n\n");
}
