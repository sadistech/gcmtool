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
#define ARG_EXTRACT					"-e"
#define ARG_LIST						"-l"
#define ARG_INFO						"-i"
//extracting sections...
#define ARG_EXTRACT_DISK_HEADER		"-edh"
#define ARG_EXTRACT_DISK_HEADER_INFO   "-edhi"
#define ARG_EXTRACT_APPLOADER			"-eal"
#define ARG_EXTRACT_BOOT_DOL			"-ed"
//injecting sections
#define ARG_INJECT_DISK_HEADER			"-idh"
#define ARG_INJECT_DISK_HEADER_INFO	"-idhi"
#define ARG_INJECT_APPLOADER			"-ial"
#define ARG_INJECT_BOOT_DOL			"-ed"

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

//regular function prototypes...
void openFile(void);
void closeFile(void);

void printGCMInfo(void);
void printUsage(void);

void extractFiles(char *source, char *dest);
void extractDiskHeader(char *path);
void extractDiskHeaderInfo(char *path);
void extractApploader(char *path);
void extractBootDol(char *path);

void injectDiskHeader(char *sourcePath);
void injectDiskHeaderInfo(char *sourcePath);
void injectApploader(char *sourcePath);

// some file utility functions...
void writeToFile(char *data, u32 length, char *path);
u32 readFromFile(char *buf, char *path);
u32 getFilesize(char *path);

//globals...

char *filepath;		//path to the file we're working with...
char *filename;		//the name of the file we're working with
FILE *gcmFile;		//the file we're working with

//for working with printing directories
int dirDepth;
int recursiveIndex; //for the recursive printing...

int main (int argc, char * const argv[]) {
	// start flags declarations...
	//for extracting:
	char *extractFileFrom = NULL;
	char *extractFileTo = NULL;
	
	int showInfoFlag = 1;

	int extractDiskHeaderFlag = 0;
	char *extractDiskHeaderFile = GCM_DISK_HEADER_FILENAME;
	
	int extractDiskHeaderInfoFlag = 0;
	char *extractDiskHeaderInfoFile = GCM_DISK_HEADER_INFO_FILENAME;

	int extractApploaderFlag = 0;
	char *extractApploaderFile = GCM_APPLOADER_FILENAME;
	
	int extractBootDolFlag = 0;
	char *extractBootDolFile = GCM_BOOT_DOL_FILENAME;
	
	int injectDiskHeaderFlag = 0;
	char *injectDiskHeaderFile = GCM_DISK_HEADER_FILENAME;
	
	int injectDiskHeaderInfoFlag = 0;
	char *injectDiskHeaderInfoFile = GCM_DISK_HEADER_INFO_FILENAME;
	
	int injectApploaderFlag = 0;
	char *injectApploaderFile = GCM_APPLOADER_FILENAME;

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
		} else if (CHECK_ARG(ARG_INFO)) {
			//they want to see info...
			
			showInfoFlag++;
			
		} else if (CHECK_ARG(ARG_EXTRACT)) {
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
			
		} else if (CHECK_ARG(ARG_EXTRACT_DISK_HEADER)) {
			// extract disk header... (to a file called "boot.bin")
			
			extractDiskHeaderFlag++;
			
			if (PEEK_ARG && strcmp(PEEK_ARG, GCM_TOOL_OPT_FILE) == 0) {
				// if they're specifying a file...
				
				SKIP_NARG(1); //skip that -f we just looked at...
				extractDiskHeaderFile = GET_NEXT_ARG;
			}
			
		} else if (CHECK_ARG(ARG_EXTRACT_DISK_HEADER_INFO)) {
			// extract disk header info... (to a file called "bi2.bin")
			
			extractDiskHeaderInfoFlag++;
			
			if (PEEK_ARG && strcmp(PEEK_ARG, GCM_TOOL_OPT_FILE) == 0) {
				// if they're specifying a file...
				
				SKIP_NARG(1); //skip that -f we just looked at...
				extractDiskHeaderInfoFile = GET_NEXT_ARG;
			}
			
		} else if (CHECK_ARG(ARG_EXTRACT_APPLOADER)) {
			//extract apploader... (to a file called "appldr.bin")
			
			extractApploaderFlag++;
			
			if (PEEK_ARG && strcmp(PEEK_ARG, GCM_TOOL_OPT_FILE) == 0) {
				// if they're specifying a file...
				
				SKIP_NARG(1); //skip that -f we just looked at...
				extractApploaderFile = GET_NEXT_ARG;
			}
		} else if (CHECK_ARG(ARG_EXTRACT_BOOT_DOL)) {
			//extract the boot dol...
			
			extractBootDolFlag++;
			
			if (PEEK_ARG && strcmp(PEEK_ARG, GCM_TOOL_OPT_FILE) == 0) {
				//if they specify a file...
				
				SKIP_NARG(1); //skip that -f
				extractBootDolFile = GET_NEXT_ARG;
			}
		} else if (CHECK_ARG(ARG_INJECT_DISK_HEADER)) {
			//inject the diskheader
			
			injectDiskHeaderFlag++;
			
			if (PEEK_ARG && strcmp(PEEK_ARG, GCM_TOOL_OPT_FILE) == 0) {
				//if they're specifying a file... (otherwise use the default);
				
				SKIP_NARG(1); //skip the -f we just looked at...
				injectDiskHeaderFile = GET_NEXT_ARG;
			}
			
		} else if (CHECK_ARG(ARG_INJECT_DISK_HEADER_INFO)) {
			//inject the diskheaderinfo...
			
			injectDiskHeaderInfoFlag++;
			
			if (PEEK_ARG && strcmp(PEEK_ARG, GCM_TOOL_OPT_FILE) == 0) {
				// if they're specifying a file...
				
				SKIP_NARG(1);
				injectDiskHeaderInfoFile = GET_NEXT_ARG;
			}
		} else if (CHECK_ARG(ARG_INJECT_APPLOADER)) {
			//inject the apploader...
			
			injectApploaderFlag++;
			
			if (PEEK_ARG && strcmp(PEEK_ARG, GCM_TOOL_OPT_FILE) == 0) {
				//if they're specifying a file...
				
				SKIP_NARG(1);
				injectApploaderFile = GET_NEXT_ARG;
			}
		} else if (CHECK_ARG(ARG_LIST)) {
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
	if (showInfoFlag) {
		printGCMInfo();
	}
	
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
	
	if (extractBootDolFlag) {
		extractBootDol(extractBootDolFile);
	}
	
	//inject the diskheader
	if (injectDiskHeaderFlag) {
		injectDiskHeader(injectDiskHeaderFile);
	}
	
	//inject the diskheaderinfo
	if (injectDiskHeaderInfoFlag) {
		injectDiskHeaderInfo(injectDiskHeaderInfoFile);
	}
	
	//inject the apploader
	if (injectApploaderFlag) {
		injectApploader(injectApploaderFile);
	}

	// list the files, if necesary...
	if (listFilesFlag) {
		dirDepth = 0;
		recursiveIndex = 0;
		GCMFileEntryStruct *r = GCMGetRootFileEntry(gcmFile);
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
	
	if (!(gcmFile = fopen(filepath, "r+"))) { //open as r+ so we can inject data, too...
		printf("error opening file... (%s)\n", filepath);
		exit(1);
	}
}

void closeFile(void) {
	fclose(gcmFile);
}

void printGCMInfo(void) {
	/*
	**  This just prints all of the relevant info for the ROM
	**  fun fun fun
	*/

	char systemID = GCMGetSystemID(gcmFile);
	
	char *gameID = (char*)malloc(GCM_GAME_ID_LENGTH + 1);
	GCMGetGameID(gcmFile, gameID);
	
	char regionCode = GCMGetRegionCode(gcmFile);
	
	char *makerCode = (char*)malloc(GCM_MAKER_CODE_LENGTH + 1);
	GCMGetMakerCode(gcmFile, makerCode);
	
	char *gameName = (char*)malloc(256);
	GCMGetGameName(gcmFile, gameName);
	
	printf("Filename:\t%s\n", filename);
	printf("System ID:\t%c (%s)\n", systemID, GCMSystemIDToStr(systemID));
	printf("Game ID:\t%s\n", gameID);
	printf("Region:  \t%c (%s)\n", regionCode, GCMRegionCodeToStr(regionCode));
	printf("Maker Code:\t%s (%s)\n", makerCode, GCMMakerCodeToStr(makerCode));
	printf("Game Name:\t%s\n", gameName);
	
	printf("DOL offset:\t%ld\n", GCMGetDolOffset(gcmFile));
	
	GCMFileEntryStruct *r = GCMGetRootFileEntry(gcmFile);
	u32 entryCount = r->length;
	printf("File count:\t%ld\n", entryCount);
	GCMFreeFileEntryStruct(r);
}

void extractFiles(char *source, char *dest) {
	/*
	**  extract files from source (in GCM) to dest (in the local filesystem)
	*/
	
	GCMFileEntryStruct *e = GCMGetFileEntryAtPath(gcmFile, source);
	
	//check to see if the file was actually found...
	if (!e) {
		printf("File not found (%s)\n", source);
		return;
	}
	
	//fetch the data
	GCMFetchDataForFileEntry(gcmFile, e);
	
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
	GCMGetDiskHeader(gcmFile, buf);
		
	writeToFile(buf, GCM_DISK_HEADER_LENGTH, path);
		
	free(buf);
}

void extractDiskHeaderInfo(char *path) {
	/*
	**  extracts the diskheaderinfo to bi2.bin
	*/
	
	//get the data...
	char *buf = (char*)malloc(GCM_DISK_HEADER_INFO_LENGTH);
	GCMGetDiskHeaderInfo(gcmFile, buf);
	
	writeToFile(buf, GCM_DISK_HEADER_INFO_LENGTH, path);
	
	free(buf);
}

void extractApploader(char *path) {
	/*
	**  extracts the apploader to appldr.bin
	*/
	
	//get the data...
	u32 apploaderLength = GCMGetApploaderSize(gcmFile) + GCM_APPLOADER_CODE_OFFSET;
	char *buf = (char*)malloc(apploaderLength);
	GCMGetApploader(gcmFile, buf);
	
	writeToFile(buf, apploaderLength, path);
	
	free(buf);
}

void extractBootDol(char *path) {
	/*
	**  extracts the boot DOL from the GCM and saves it to path...
	*/
	
	if (!path) return;
	
	u32 length = GCMGetBootDolLength(gcmFile);
	char *buf = (char*)malloc(length);
	
	if (GCMGetBootDol(gcmFile, buf) != length) {
		printf("An error occurred when getting the DOL.\n");
		free(buf);
		return;
	}
	
	writeToFile(buf, length, path);
}

void injectDiskHeader(char *sourcePath) {
	/*
	**  take a diskHeader (boot.bin) from sourcePath and inject it into gcmFile.
	*/
	
	char *buf = (char*)malloc(getFilesize(sourcePath));
	
	if (readFromFile(buf, sourcePath) != GCM_DISK_HEADER_LENGTH) {
		printf("This does not appear to be a diskheader (%s)\n", sourcePath);
		free(buf);
		return;
	}
	
	if (GCMPutDiskHeader(gcmFile, buf) != GCM_SUCCESS) {
		printf("An error occurred when writing the disk header! (%d)\n", ferror(gcmFile));
	}
	
	free(buf);
	return;
}

void injectDiskHeaderInfo(char *sourcePath) {
	/*
	**  take a diskHeaderInfo (bi2.bin) from sourcePath and inject it into gcmFile.
	*/
	
	char *buf = (char*)malloc(getFilesize(sourcePath));
	
	if (readFromFile(buf, sourcePath) != GCM_DISK_HEADER_INFO_LENGTH) {
		printf("This does not appear to be a diskheaderinfo (%s)\n", sourcePath);
		free(buf);
		return;
	}
	
	if (GCMPutDiskHeaderInfo(gcmFile, buf) != GCM_SUCCESS) {
		printf("An error occurred when writing the disk header info!\n");
	}
	
	free(buf);
	return;
}

void injectApploader(char *sourcePath) {
	/*
	**  this doesn't work, yet... requires some serious shifting of other data...
	*/
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
	
	GCMFetchFilenameForFileEntry(gcmFile, e);
	
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
			next = GCMGetNthFileEntry(gcmFile, recursiveIndex);
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

u32 readFromFile(char *buf, char *path) {
	/*
	**  reads from the file at path, 
	**  sets buf to the contents of said file...
	**  and returns the length of data.
	**
	**  use getFilesize() when you allocate buf
	**
	**  returns 0 and displays messages on error;
	*/
	
	if (!path || !buf) return 0;
	
	FILE *ifile = NULL;
	
	u32 length = getFilesize(path); //get how much to read...
	
	if (!length || !(ifile = fopen(path, "r"))) {
		printf("An error occurred trying to open %s\n", path);
		return 0;
	}
	
	if (fread(buf, 1, length, ifile) != length) {
		printf("An error occurred when trying to read %s\n", path);
		fclose(ifile);
		return 0;
	}
	
	fclose(ifile);
	return length;
}

u32 getFilesize(char *path) {
	/*
	**  returns the filesize of the file at *path
	*/
	
	FILE *ifile = NULL;
	
	if (!(ifile = fopen(path, "r"))) {
		return 0;
	}
	
	fseek(ifile, 0, SEEK_END);
	u32 len = ftell(ifile);
	fclose(ifile);
	
	return len;
}

void printUsage() {
	printf("GCMTool %s- A utility for working with Nintendo GameCube DVD images.\n\tgcmtool.sourceforge.net\n\n", VERSION);
	printf("Usage:");
	printf("  gcmtool [ options ] <filename>\n\n");
	printf("    Options:\n");
	printf("    -l\tList files\n");
	printf("    -e <gcm_source> <dest>\tExtract a file from a GCM\n");
	printf("\n");
	printf("  You can use -f <filename> to specify a filename for the following options...\n");
	printf("    -edh  [ -f <filename> ]\tExtract the Disk Header (boot.bin)\n");
	printf("    -edhi [ -f <filename> ]\tExtract the Disk Header Info (bi2.bin)\n");
	printf("    -eal  [ -f <filename> ]\tExtract the Apploader (appldr.bin)\n");
	printf("    -ed   [ -f <filename> ]\tExtract the main executable DOL (boot.dol)\n");
	printf("    -idh  [ -f <filename> ]\tInject the Disk Header\n");
	printf("    -idhi [ -f <filename> ]\tInject the Disk Header Info\n");
	printf("    -ial  [ -f <filename> ]\tInject the Apploader (NOT IMPLEMENTED)\n");
	printf("    -id   [ -f <filename> ]\tInject the main executable DOL (NOT IMPLEMENTED)\n");
}
