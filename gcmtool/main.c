/*
**  main.c
**  written by spike <spike666@mac.com>
**  www.sadistech.com
**  gcmtool.sourceforge.net
**  
**  part of the gcmtool project
**  commandline wrappers for some of the standard functions of GCMutils.h and GCMExtras.h
**
*/

//for the autoheader stuff... (grrr, I hate this thing...)
#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GCMutils.h"
#include "GCMextras.h"

#include "pathfunc.h"

//commandline arguments
#define GCM_TOOL_ARG_EXTRACT	"-e"
#define GCM_TOOL_ARG_LIST		"-l"


//macros... although they may be simple...
#define GET_NEXT_ARG *argv++
#define CHECK_ARG(ARG) strcmp(ARG, currentArg) == 0

//some utility functions...
void printEntry(GCMFileEntryStruct *e);
void printDirectory(GCMFileEntryStruct *e);

//regular functions...
void openFile(void);
void closeFile(void);

void printGCMInfo(void);
void printUsage(void);
void extractFiles(char *source, char *dest);

char *filepath;
char *filename;
FILE *ifile;

//for working with printing directories
int dirDepth;
int recursiveIndex; //for the recursive printing...

int main (int argc, char * const argv[]) {
	// start flags declarations...
	//for extracting:
	char *extractFrom;
	char *extractTo;

	int listFiles = 0;
	// end flag declarations
	
	//start argument parsing...
	char *currentArg = GET_NEXT_ARG; //loads the first argument (the executable... don't do anything with this).

	do {
		currentArg = GET_NEXT_ARG;
		if (!currentArg) {
			//there's no args! uh oh!
			
			printUsage();
			exit(0);
		} else if (CHECK_ARG(GCM_TOOL_ARG_EXTRACT)) {
			// extract files...
			// usage: -e <path> <destPath>
			
			extractFrom		= GET_NEXT_ARG;
			extractTo		= GET_NEXT_ARG;
			
			if (!extractFrom || !extractTo) {
				//argument error... something was omitted...
				printf("Argument error.\n");
				printUsage();
				exit(1);
			}
		} else if (CHECK_ARG(GCM_TOOL_ARG_LIST)) {
			// list filesystem
			
			listFiles++; //turn the listFiles flag on.
		} else {
			// do it to this file... this is the last argument... just ignore the rest...
			
			filepath = currentArg;
			filename = lastPathComponent(filepath);
			
			break;
		}
	} while(*argv);
	//end parsing arguments...
	
	//open the file... and start processing!
	openFile();

	// print the info...
	printGCMInfo();
	
	// extract files...
	if (extractFrom && extractTo) {
		extractFiles(extractFrom, extractTo);
	}

	// list the files, if necesary...
	if (listFiles) {
		dirDepth = 0;
		recursiveIndex = 0;
		GCMFileEntryStruct *r = GCMGetRootFileEntry(ifile);
		printDirectory(r);
		GCMFreeFileEntryStruct(r);
	}
	
	closeFile();
	
	return 0;
}

void openFile(void) {
	if (!filepath) {
		printUsage();
		exit(1);
	}
	
	if (!(ifile = fopen(filepath, "r"))) {
		printf("error opening file... (%s)\n", filepath);
		exit(1);
	}
}

void closeFile(void) {
	fclose(ifile);
}

void printGCMInfo(void) {
	char systemID = GCMGetSystemID(ifile);
	
	char *gameID = (char*)malloc(GCM_GAME_ID_LENGTH + 1);
	GCMGetGameID(ifile, gameID);
	
	char regionCode = GCMGetRegionCode(ifile);
	
	char *makerCode = (char*)malloc(GCM_MAKER_CODE_LENGTH + 1);
	GCMGetMakerCode(ifile, makerCode);
	
	char *gameName = (char*)malloc(256);
	GCMGetGameName(ifile, gameName);
	
	printf("Filename:\t%s\n", filename);
	printf("System ID:\t%c (%s)\n", systemID, GCMSystemIDToStr(systemID));
	printf("Game ID:\t%s\n", gameID);
	printf("Region:  \t%c (%s)\n", regionCode, GCMRegionCodeToStr(regionCode));
	printf("Maker Code:\t%s (%s)\n", makerCode, GCMMakerCodeToStr(makerCode));
	printf("Game Name:\t%s\n", gameName);
	
	printf("DOL offset:\t%ld\n", GCMGetDolOffset(ifile));
	
	GCMFileEntryStruct *r = GCMGetRootFileEntry(ifile);
	u32 entryCount = r->length;
	printf("File count:\t%ld\n", entryCount);
	GCMFreeFileEntryStruct(r);
}

void extractFiles(char *source, char *dest) {
	/*
	**  extract files from source (in GCM) to dest (in the local filesystem)
	*/
	
	GCMFileEntryStruct *e = GCMGetFileEntryAtPath(ifile, source);
	
	if (!e) {
		printf("File not found (%s)\n", source);
		return;
	}
	
	FILE *ofile = NULL;
	
	if (!(ofile = fopen(dest, "w"))) {
		printf("An error occurred trying to open %s\n", dest);
		return;
	}
	
	GCMFetchDataForFileEntry(ifile, e);
	
	if (fwrite(e->data, 1, e->length, ofile) != e->length) {
		printf("An error occurred trying to write %s\n", dest);
	}
	
	fclose(ofile);
}

void printEntry(GCMFileEntryStruct *e) {
	printf("\t%s\n", e->filename);
	printf("size:\t%ld\n\n", e->length);
}

void printDirectory(GCMFileEntryStruct *e) {
	if (!e) {
		return;
	}
	int j = 0;
	
	GCMFetchFilenameForFileEntry(ifile, e);
	
	for (j = 0; j < dirDepth; j++) {
		printf(" ");
	}
	
	if (dirDepth == 0) {
		printf("/\n");
	} else {
		if (!e->isDir)
			printf("%s\n", e->filename);
		else
			printf("%s\n", e->filename);
	}
	if (e->isDir) {
		dirDepth++;
		GCMFileEntryStruct *next;
		
		//printf("for(%d++; %d < %ld; i++)\n", i, i, e->length);
		
		for(recursiveIndex++; (unsigned long)recursiveIndex < e->length; recursiveIndex++) {
			next = GCMGetNthFileEntry(ifile, recursiveIndex);
			if (next) {
				printDirectory(next);
				free(next);
			}
		}
		recursiveIndex--;
		dirDepth--;
	}
}

void printUsage() {
	printf("GCMTool - A utility for workign with Nintendo GameCube DVD images.\n\tgcmtool.sourceforge.net\n\n");
	printf("Usage:");
	printf("  gcmtool [ options ] <filename>\n\n");
	printf("    Options:\n");
	printf("    -l\tList files\n");
}
