//for the autoheader stuff... (grrr, I hate this thing...)
#if HAVE_CONFIG_H
#include <config.h>
#else
//if you're not configuring from the commandline...
#define VERSION ""
#endif

#include <stdio.h>
#include <stdlib.h>

#include "GCMutils.h"
#include "GCMCommandline.h"
#include "pathfunc.h"

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
}

void printUsage() {
	printf("GCMStrip %s- Strips un-needed padding from Gamecube DVD images and reduces filesize.\n\tgcmtool.sourceforge.net\n\n", VERSION);
	printf("Usage:");
	printf("  gcmstrip [ options ] <filename>\n\n");
	printf("Use -? for extended usage.\n\n");
}
