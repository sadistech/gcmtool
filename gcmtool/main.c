/*
**  main.cpp
**  written by spike <spike666@mac.com>
**  www.sadistech.com
**  gcmtool.sourceforge.net
**  
**  part of the gcmtool project
**  commandline wrappers for some of the standard functions of GCMutils.h and GCMExtras.h
**
*/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>
#include <string.h>

#include "GCMutils.h"
#include "GCMextras.h"

#include "pathfunc.h"

//commandline arguments
#define GCM_TOOL_ARG_EXTRACT	"-e"
#define GCM_TOOL_ARG_LIST		"-l"


//some utility functions...
void printEntry(GCMFileEntryStruct *e);
void printDirectory(GCMFileEntryStruct *e);

//regular functions...
void printUsage();

char *filepath;
char *filename;
FILE *ifile;

//for working with printing directories
int dirDepth;

int i;

int main (int argc, char * const argv[]) {
	//for extracting:
	char *extractFrom;
	char *extractTo;

	int listFiles = 0;

	for(i = 1; i < argc; i++) {
		//printf("parsing arg: %s\n", argv[i]);
		if (strcmp(argv[i], GCM_TOOL_ARG_EXTRACT) == 0) {
			// extract files...
			// usage: -e <path> <destPath>
			
			extractFrom = argv[++i];
			extractTo = argv[++i];
		} else if (strcmp(argv[i], GCM_TOOL_ARG_LIST) == 0) {
			// list filesystem
			
			listFiles++;
		} else {
			// do it to this file...
			
			filepath = argv[i];
			filename = lastPathComponent(filepath);
			
			break;
		}
	}
	
	dirDepth = 0;
	
	//filepath = "/Users/spike/Downloads/True.Crime.Streets.of.LA.USA.NGC-STARCUBE/s3-tcsla.iso";
	
	if (!filepath) {
		printUsage();
		exit(1);
	}
	
	if (!(ifile = fopen(filepath, "r"))) {
		printf("error opening file... (%s)\n", filepath);
		exit(1);
	}
	
	char systemID = GCMGetSystemID(ifile);
	
	char *gameID = (char*)malloc(GCM_GAME_ID_LENGTH + 1);
	GCMGetGameID(ifile, gameID);
	
	char regionCode = GCMGetRegionCode(ifile);
	
	char *makerCode = (char*)malloc(GCM_MAKER_CODE_LENGTH + 1);
	GCMGetMakerCode(ifile, makerCode);
	
	char *gameName = (char*)malloc(256);
	GCMGetGameName(ifile, gameName);
	
//	u32 fst_offset	= GCMGetFSTOffset(ifile);
//	u32 fst_size	= GCMGetFSTSize(ifile);
	
	printf("Filename:\t%s\n", filename);
	printf("System ID:\t%c (%s)\n", systemID, GCMSystemIDToStr(systemID));
	printf("Game ID:\t%s\n", gameID);
	printf("Region:  \t%c (%s)\n", regionCode, GCMRegionCodeToStr(regionCode));
	printf("Maker Code:\t%s (%s)\n", makerCode, GCMMakerCodeToStr(makerCode));
	printf("Game Name:\t%s\n", gameName);
	//printf("\n");
//	printf("Diskheader Offset: %ld\n", 0);
//	printf("DiskHeaderInfo Offset: %ld\n", GCM_DISK_HEADER_INFO_OFFSET);
//	printf("Apploader Offset: %ld\n", GCM_APPLOADER_OFFSET);
//	printf("FST Offset:\t%ld\n", fst_offset);
//	printf("FST Size:\t%ld\n", fst_size);
	
	printf("DOL offset: %ld\n", GCMGetDolOffset(ifile));
	
	//let's try to extract bi2.bin //testing purposes...
	/*
	FILE *biFile;
	
	if (!(biFile = fopen("bi2.bin", "w"))) {
		printf("ERROR OPENING bi2.bin\n");
		exit(1);
	}
	
	char *biData = (char*)malloc(GCM_DISK_HEADER_INFO_LENGTH);
	
	GCMGetDiskHeaderInfo(ifile, biData);
	
	if (fwrite(biData, 1, GCM_DISK_HEADER_INFO_LENGTH, biFile) != GCM_DISK_HEADER_INFO_LENGTH) {
		printf("ERROR WRITING DATA!\n");
		exit(1);
	}
	
	free(biData);
	fclose(biFile);*/
	
	GCMFileEntryStruct *r = GCMGetRootFileEntry(ifile);
	u32 entryCount = r->length;
	printf("File count:\t%ld\n", entryCount);
	
	/*
	GCMDiskHeaderStruct *dh = GCMGetDiskHeaderStruct(ifile);
	printf("SystemID:\t%c\n", dh->systemID);
	printf("GameID:  \t%s\n", dh->gameID);
	printf("Region:  \t%c\n", dh->regionCode);
	printf("Maker:   \t%s\n", dh->makercode);
	printf("DiskID:  \t%d\n", dh->diskID);
	printf("Version: \t%d\n", dh->version);
	printf("Streaming:\t%c\n", dh->streaming);
	printf("StreamBuf:\t%ld\n", dh->streamBufSize);
	printf("Game Name:\t%s\n", dh->gameName);
	printf("DOL Offset:\t%ld\n", dh->dolOffset);
	printf("FSTOffset:\t%ld\n", dh->fstOffset);
	printf("FSTSize: \t%ld\n", dh->fstSize);
	printf("FSTSizeMax:\t%ld\n", dh->fstMaxSize);
	*/
	i = 0;
	if (listFiles) {
		printDirectory(r);
	}
	
	fclose(ifile);
	return 0;
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
		
		for(i++; (unsigned long)i < e->length; i++) {
			next = GCMGetNthFileEntry(ifile, i);
			if (next) {
				printDirectory(next);
				free(next);
			}
		}
		i--;
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