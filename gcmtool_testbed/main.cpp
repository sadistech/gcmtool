#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "GCMutils.h"
#include "GCMextras.h"

void printEntry(GCMFileEntryStruct *e);
void printDirectory(GCMFileEntryStruct *e);

char *filename;
FILE *ifile;
int dirDepth;

int i;

int main (int argc, char * const argv[]) {
	//filename = "/Users/spike/Desktop/Game-Naruto_Gekitou_Ninja_Taisen_2_JAP_NGC-WAR3X/naruto2_JAP.gcm";
	filename = argv[1];
	dirDepth = 0;
	
	if (!(ifile = fopen(filename, "r"))) {
		printf("error opening file...\n");
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
	
	u32 fst_offset	= GCMGetFSTOffset(ifile);
	u32 fst_size	= GCMGetFSTSize(ifile);
	
	printf("System ID:\t%c (%s)\n", systemID, GCMSystemIDToStr(systemID));
	printf("Game ID:\t%s\n", gameID);
	printf("Region:  \t%c (%s)\n", regionCode, GCMRegionCodeToStr(regionCode));
	printf("Maker Code:\t%s (%s)\n", makerCode, GCMMakerCodeToStr(makerCode));
	printf("Game Name:\t%s\n", gameName);
	printf("\n");
	printf("offset:\t%ld\n", fst_offset);
	printf("size:\t%ld\n", fst_size);
	
	printf("\n");
	GCMFileEntryStruct *r = GCMGetRootFileEntry(ifile);
	u32 entryCount = r->length;
	printf("number of entries: %ld\n", entryCount);
	
	printf("DOL offset: %ld\n", GCMGetDOLOffset(ifile));
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
	printDirectory(r);
	
	fclose(ifile);
	return 0;
	/*
	char *buf = (char*)malloc(12);
	fseek(ifile, fst_offset, SEEK_SET);
	if (fread(buf, 1, GCM_FST_ENTRY_LENGTH, ifile) != GCM_FST_ENTRY_LENGTH) {
		printf("error occured while reading FST\n");
		exit(1);
	}
	
	GCMFileEntry *rootEntry = rawFileEntryToStruct(buf);
	if (rootEntry->isDir == 1) {
		printf("it's a directory!\n");
	}
	
	printf("fn_offset: %ld\n", rootEntry->filenameOffset);
	printf("offset: %ld\n", rootEntry->offset);
	printf("entries: %ld\n", rootEntry->length);
	
	unsigned long filenameTableOffset = (fst_offset + (rootEntry->length * GCM_FST_ENTRY_LENGTH));
	unsigned long oldOffset;
	char filenamebuf[256];

	
	int i = 0;
	
	GCMFileEntry *fe = NULL;
	for (i = 1; i < (int)rootEntry->length; i++) {
		if (fread(buf, 1, GCM_FST_ENTRY_LENGTH, ifile) != GCM_FST_ENTRY_LENGTH) {
			printf("error while reading FST entry\n");
			exit(1);
		}
		fe = rawFileEntryToStruct(buf);
		
		oldOffset = ftell(ifile);
		fseek(ifile, filenameTableOffset + fe->filenameOffset, SEEK_SET);
		if (fread(filenamebuf, 1, 256, ifile) != 256) {
			printf("filename read error\n");
			exit(1);
		}
		fe->filename = (char *)malloc(strlen(filenamebuf) + 1);
		strcpy(fe->filename, filenamebuf);
		
		if (!fe->isDir) {
			fe->data = (char *)malloc(fe->length);
			fseek(ifile, fe->offset, SEEK_SET);
			if (fread(fe->data, 1, fe->length, ifile) != fe->length) {
				printf("data read error\n");
				exit(1);
			}
		}
		
		fseek(ifile, oldOffset, SEEK_SET);
		
		printf("%d:\t%s\n", i, fe->filename);
		printf("\tisDir:\t%s\n", (fe->isDir == 0) ? "NO" : "YES");
		printf("\tfn_offset:\t%ld\n", fe->filenameOffset);
		printf("\toffset: \t%ld\n", fe->offset);
		printf("\tentries:\t%ld\n", fe->length);
		free(fe);
	}
	
	printf("current location: %ld\n", ftell(ifile));
	
	fclose(ifile);
	
	exit(0);*/
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
			printf("%s\t(%d)\n", e->filename, i);
		else
			printf("%s\t(::%ld::)\n", e->filename, e->length);
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