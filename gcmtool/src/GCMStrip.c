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

#define ARG_OUTPUT_FILE						"-o"
#define ARG_OUTPUT_FILE_SYN					"--output-file"
#define ARG_OUTPUT_FILE_OPT					"<path>"
#define ARG_OUTPUT_FILE_HELP				"Write stripped GCM to " ARG_OUTPUT_FILE_OPT " instead of overwriting the input file."

//prototypes
void printUsage();
void stripGCM(FILE *ifile, FILE *ofile);

//global vars
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
	u32 filesize = GetFilesizeFromStream(gcmFile);

	printf("Filesize: %ld\n", filesize);
	printf("FST Offset: %ld\n", fstOffset);
	printf("FST Size: %ld\n", fstSize);
	printf("String Table: %ld\n", stringTableOffset);
	printf("entry count: %ld\n", root->length);
	
	int i = 0;
	u32 highestStringTableOffset = 0;
	u32 firstFileOffset = GetFilesizeFromStream(gcmFile);
	u32 endOfFileList = 0;
	
	for (i = 0; i < root->length; i++) {
		GCMFileEntryStruct *e = GCMGetNthFileEntry(gcmFile, i);

		GCMFetchFilenameForFileEntry(gcmFile, e);
		printf("%s\n", e->filename);
		
		if (e->filenameOffset >= highestStringTableOffset) {
			highestStringTableOffset = e->filenameOffset + strlen(e->filename) + 1; // +1 for \0
		}

		if (!(e->isDir) && e->offset < firstFileOffset) {
			firstFileOffset = e->offset;
		}

		if (!(e->isDir) && e->offset + e->length > endOfFileList) {
			endOfFileList = (e->offset + e->length);
		}
	}

	printf("\n");
	printf("HighestStringTableOffset: %ld\n", highestStringTableOffset);
	printf("firstFileOffset: %ld\n", firstFileOffset);
	printf("endOfFileList: %ld\n", endOfFileList);

	printf("\n");
	printf("Savings: %ld\n", firstFileOffset - (stringTableOffset + highestStringTableOffset));

	printf("Verifying...\n");

	fseek(gcmFile, stringTableOffset + highestStringTableOffset, SEEK_SET);

	while (!fgetc(gcmFile));

	printf("currentOffset: %ld\n", ftell(gcmFile));

	if (ftell(gcmFile) >= firstFileOffset) {
		printf("Everything seems ok... YAY!\n");
	} else {
		printf("EEK! no good! something's wrong.\n");
	}

	fclose(gcmFile);
}


void stripGCM(FILE *ifile, FILE *ofile);
void printUsage() {
	printf("GCMStrip %s- Strips un-needed padding from Gamecube DVD images and reduces filesize.\n\tgcmtool.sourceforge.net\n\n", VERSION);
	printf("Usage:");
	printf("  gcmstrip [ options ] <filename>\n\n");
	printf("Use -? for extended usage.\n\n");
}
