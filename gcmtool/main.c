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
#else
//if you're not configuring from the commandline...
#define VERSION ""
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GCMutils.h"
#include "GCMextras.h"

#include "pathfunc.h"

//commandline arguments
#define GCM_TOOL_ARG_EXTRACT					"-e"
#define GCM_TOOL_ARG_LIST						"-l"
#define GCM_TOOL_ARG_EXTRACT_DISK_HEADER		"-edh"
#define GCM_TOOL_ARG_EXTRACT_DISK_HEADER_INFO   "-edhi"
#define GCM_TOOL_ARG_EXTRACT_APPLOADER			"-eal"

//commandline options (modifiers to the arguments... hehe)
#define GCM_TOOL_OPT_FILE						"-f"

//macros... although they may be simple...
// these are for the argument parsing engine...
#define GET_NEXT_ARG		*(++argv)
#define SKIP_NARG(n)		*(argv += n)	
#define CHECK_ARG(ARG)		strcmp(ARG, currentArg) == 0
#define PEEK_ARG			*(argv + 1)
#define PEEK_NARG(n)		*(argv + n)

//some utility functions...
void printEntry(GCMFileEntryStruct *e);
void printDirectory(GCMFileEntryStruct *e);

//regular functions...
void openFile(void);
void closeFile(void);

void printGCMInfo(void);
void printUsage(void);
void extractFiles(char *source, char *dest);
void extractDiskHeader(char *path);
void extractDiskHeaderInfo(char *path);
void extractApploader(char *path);

void writeToFile(char *data, u32 length, char *path);

char *filepath;
char *filename;
FILE *ifile;

//for working with printing directories
int dirDepth;
int recursiveIndex; //for the recursive printing...

int main (int argc, char * const argv[]) {
	// start flags declarations...
	//for extracting:
	char *extractFileFrom = NULL;
	char *extractFileTo = NULL;

	int extractDiskHeaderFlag = 0;
	char *extractDiskHeaderFile = GCM_DISK_HEADER_FILENAME;
	
	int extractDiskHeaderInfoFlag = 0;
	char *extractDiskHeaderInfoFile = GCM_DISK_HEADER_INFO_FILENAME;

	int extractApploaderFlag = 0;
	char *extractApploaderFile = GCM_APPLOADER_FILENAME;

	int listFilesFlag = 0;
	// end flag declarations
	
	//start argument parsing...
	char *currentArg = NULL;

	do {
		currentArg = GET_NEXT_ARG;
		if (!currentArg) {
			//there's no args! uh oh!
			
			printUsage();
			exit(0);
		} else if (CHECK_ARG(GCM_TOOL_ARG_EXTRACT)) {
			// extract files...
			// usage: -e <path> <destPath>
			
			extractFileFrom		= GET_NEXT_ARG;
			extractFileTo		= GET_NEXT_ARG;
			
			if (!extractFileFrom || !extractFileTo) {
				//argument error... something was omitted...
				printf("Argument error.\n");
				printUsage();
				exit(1);
			}
		} else if (CHECK_ARG(GCM_TOOL_ARG_EXTRACT_DISK_HEADER)) {
			// extract disk header... (to a file called "boot.bin")
			
			extractDiskHeaderFlag++;
			
			if (PEEK_ARG && strcmp(PEEK_ARG, GCM_TOOL_OPT_FILE) == 0) {
				// if they're specifying a file...
				
				SKIP_NARG(1); //skip that -f we just looked at...
				extractDiskHeaderFile = GET_NEXT_ARG;
			}
		} else if (CHECK_ARG(GCM_TOOL_ARG_EXTRACT_DISK_HEADER_INFO)) {
			// extract disk header info... (to a file called "bi2.bin")
			
			extractDiskHeaderInfoFlag++;
			
			if (PEEK_ARG && strcmp(PEEK_ARG, GCM_TOOL_OPT_FILE) == 0) {
				// if they're specifying a file...
				
				SKIP_NARG(1); //skip that -f we just looked at...
				extractDiskHeaderInfoFile = GET_NEXT_ARG;
			}
		} else if (CHECK_ARG(GCM_TOOL_ARG_EXTRACT_APPLOADER)) {
			//extract apploader... (to a file called "appldr.bin")
			
			extractApploaderFlag++;
			
			if (PEEK_ARG && strcmp(PEEK_ARG, GCM_TOOL_OPT_FILE) == 0) {
				// if they're specifying a file...
				
				SKIP_NARG(1); //skip that -f we just looked at...
				extractApploaderFile = GET_NEXT_ARG;
			}
		} else if (CHECK_ARG(GCM_TOOL_ARG_LIST)) {
			// list filesystem
			
			listFilesFlag++; //turn the listFiles flag on.
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
	if (extractFileFrom && extractFileTo) {
		extractFiles(extractFileFrom, extractFileTo);
	}
	
	//extract diskheader
	if (extractDiskHeaderFlag) {
		extractDiskHeader(extractDiskHeaderFile);
	}
	
	//extract diskheaderinfo
	if (extractDiskHeaderInfoFlag) {
		extractDiskHeaderInfo(extractDiskHeaderInfoFile);
	}
	
	//extract apploader
	if (extractApploaderFlag) {
		extractApploader(extractApploaderFile);
	}

	// list the files, if necesary...
	if (listFilesFlag) {
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
	/*
	**  opens the GCM file for reading...
	*/
	
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
	/*
	**  This just prints all of the relevant info for the ROM
	**  fun fun fun
	*/

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
	
	//check to see if the file was actually found...
	if (!e) {
		printf("File not found (%s)\n", source);
		return;
	}
	
	//fetch the data
	GCMFetchDataForFileEntry(ifile, e);
	
	writeToFile(e->data, e->length, dest);
	
	free(e->data);
	free(e);
}

void extractDiskHeader(char *path) {
	/*
	**  extracts the disk header to boot.bin
	*/
		
	//get the data...
	char *buf = (char*)malloc(GCM_DISK_HEADER_LENGTH);
	GCMGetDiskHeader(ifile, buf);
		
	writeToFile(buf, GCM_DISK_HEADER_LENGTH, path);
		
	free(buf);
}

void extractDiskHeaderInfo(char *path) {
	/*
	**  extracts the diskheaderinfo to bi2.bin
	*/
	
	//get the data...
	char *buf = (char*)malloc(GCM_DISK_HEADER_INFO_LENGTH);
	GCMGetDiskHeaderInfo(ifile, buf);
	
	writeToFile(buf, GCM_DISK_HEADER_INFO_LENGTH, path);
	
	free(buf);
}

void extractApploader(char *path) {
	/*
	**  extracts the apploader to appldr.bin
	*/
	
	//get the data...
	u32 apploaderLength = GCMGetApploaderSize(ifile) + GCM_APPLOADER_CODE_OFFSET;
	char *buf = (char*)malloc(apploaderLength);
	GCMGetApploader(ifile, buf);
	
	writeToFile(buf, apploaderLength, path);
	
	free(buf);
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

void writeToFile(char *data, u32 length, char *path) {
	/*
	**  Takes data of length and writes it to a file path. Displays errors when they happen...
	*/
	
	if (!data || !length || !path) return;
	
	FILE *ofile = NULL;
	
	if (!(ofile = fopen(path, "w"))) {
		printf("An error occurred trying to open %s\n", path);
		return;
	}
	
	if (fwrite(data, 1, length, ofile) != length) {
		printf("An error occurred trying to write to %s\n", path);
	}
}

void printUsage() {
	printf("GCMTool %s- A utility for working with Nintendo GameCube DVD images.\n\tgcmtool.sourceforge.net\n\n", VERSION);
	printf("Usage:");
	printf("  gcmtool [ options ] <filename>\n\n");
	printf("    Options:\n");
	printf("    -l\tList files\n");
	printf("    -e <gcm_source> <dest>\tExtract a file from a GCM\n");
	printf("    -edh\tExtract the Disk Header (boot.bin)\n");
	printf("    -edhi\tExtract the Disk Header Info (bi2.bin)\n");
	printf("    -eal\tExtract the Apploader (appldr.bin)\n");
}
