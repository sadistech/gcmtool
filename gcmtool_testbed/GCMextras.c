/*
 *  GCMextras.c
 *  gcmtool
 *
 *  Created by spike on Sun Mar 28 2004.
 *  Copyright (c) 2004 Sadistech. All rights reserved.
 *
 */

#include "GCMextras.h"
#include "GCMutils.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

char *GCMSystemIDToStr(char sysID) {
	/*
	**  takes the system ID and makes it something human-readable
	**  strictly for convenience
	*/

	switch (sysID) {
	case GCM_GAMECUBE_ID:
		return "Gamecube";
	case GCM_GBA_PLAYER_ID:
		return "GBA Player";
	default:
		return "Other";
	}
}

char *GCMMakerCodeToStr(char *code) {
	/*
	**  takes the maker code and makes it something human-readable
	**  strictly for convenience...
	*/

	if (strcmp(code, GCM_MAKER_NINTENDO) == 0) {
		return "Nintendo";
	} else if (strcmp(code, GCM_MAKER_CAPCOM) == 0) {
		return "Capcom";
	} else if (strcmp(code, GCM_MAKER_EIDOS) == 0) {
		return "Eidos";
	} else if (strcmp(code, GCM_MAKER_ACCLAIM) == 0) {
		return "Acclaim";
	} else if (strcmp(code, GCM_MAKER_MIDWAY) == 0) {
		return "Midway";
	} else if (strcmp(code, GCM_MAKER_HUDSON) == 0) {
		return "Hudson";
	} else if (strcmp(code, GCM_MAKER_LUCAS_ARTS) == 0) {
		return "Lucas Arts";
	} else if (strcmp(code, GCM_MAKER_ELECTRONIC_ARTS) == 0) {
		return "Electronic Arts";
	} else if (strcmp(code, GCM_MAKER_TDK) == 0) {
		return "TDK Mediactive";
	} else if (strcmp(code, GCM_MAKER_SEGA) == 0) {
		return "Sega";
	} else if (strcmp(code, GCM_MAKER_NAMCO) == 0) {
		return "Namco";
	} else if (strcmp(code, GCM_MAKER_TOMY) == 0) {
		return "Tomy";
	} else if (strcmp(code, GCM_MAKER_KONAMI) == 0) {
		return "Konami";
	} else {
		return "Other";
	}
}

char *GCMRegionCodeToStr(char code) {
	/*
	**  converts the region code into something human-readable
	**  strictly for convenience...
	*/

	switch (code) {
	case GCM_USA_NTSC:
		return "USA/NTSC";
	case GCM_EUR_PAL:
		return "Europe/PAL";
	case GCM_JAP_NTSC:
		return "Japan/NTSC";
	default:
		return "Other";
	}
}

int GCMGetFileEntryCount(FILE *ifile) {
	/*
	**  returns the number of file entries
	**  returns -1 on error
	*/

	if (!ifile) {
		return -1;
	}
	
	//just grab the root entry, and return the how many files follow it...
	//very simple, eh?
	GCMFileEntryStruct *r = GCMGetRootFileEntry(ifile);
	u32 entryCount = r->length;
	free(r);
	
	return entryCount;
}

GCMFileEntryStruct *GCMGetRootFileEntry(FILE *ifile) {
	/*
	**  returns the root file entry (entry 0)
	*/

	GCMFileEntryStruct *entry = GCMGetNthFileEntry(ifile, 0);
	if (!entry) {
		return NULL;
	}
	entry->filename = "root";
	return entry;
}

GCMFileEntryStruct *GCMGetNthFileEntry(FILE *ifile, int n) {
	/*
	**  returns the nth file entry as a struct
	*/

	char *rawEntry = (char*)malloc(GCM_FST_ENTRY_LENGTH);
	GCMGetNthRawFileEntry(ifile, n, rawEntry);
	GCMFileEntryStruct *entry = GCMRawFileEntryToStruct(rawEntry);
	free(rawEntry);
	
	return entry;
}

GCMFileEntryStruct *GCMGetFileEntryByName(FILE *ifile, char *name) {
	/*
	**  returns the first file entry with the filename 'name'
	**  temporary kludge... will be replaced by GCMGetFileEntryAtPath(), but later...
	*/

	if (!ifile || !name) {
		return NULL;
	}
	
	GCMFileEntryStruct *entry;
	int i = 0;
	int entryCount = GCMGetFileEntryCount(ifile);
	for (i = 1; i <= entryCount; i++) {
		entry = GCMGetNthFileEntry(ifile, i);
		GCMFetchFilenameForFileEntry(ifile, entry);
		if (strcmp(entry->filename, name) == 0) {
			return entry;
		}
		free(entry->filename);
		free(entry);
	}
	
	return NULL;
}

GCMFileEntryStruct *GCMRawFileEntryToStruct(char *rawEntry) {
	/*
	**  converts the rawEntry into a struct.
	**  doesn't fetch the filename or data (to save memory/speed things up)
	**  use GCMFetchFilenameForFileEntry() to get the filename...
	**
	**  Format of a raw file entry:
	**  start__|__size__|__Description
	**   0x00     1		   flags; 0: file, 1: directory
	**   0x01     3		   filename_offset (relative to string table)
	**   0x04     4        file: file_offset, dir: parent_offset
	**   0x08     4        file: file_length, dir: next_offset
	*/

	if (!rawEntry) {
		return NULL;
	}
	
	GCMFileEntryStruct *fe = (GCMFileEntryStruct*)malloc(sizeof(GCMFileEntryStruct));
	
	//it's a file if the first byte is 1, directory if 0
	fe->isDir = *rawEntry;
	
	//then get the filename_offset
	unsigned long *l;
	l = (unsigned long*)rawEntry;
	if (rawEntry[0] != 0)
		*l -= 0x01000000;
	fe->filenameOffset = *l;
	
	//get the file_offset/parent_offset
	l = (unsigned long*)rawEntry + 1;
	fe->offset = *l;
	
	//get the file_length/next_offset
	l = (unsigned long*)rawEntry + 2;
	fe->length = *l;
	
	return fe;
}

GCMDiskHeaderStruct *GCMRawDiskHeaderToStruct(char *rawHeader) {
	/*
	**  experimental method for loading the diskheader into a struct
	**  currently not working!
	*/

	GCMDiskHeaderStruct *h = (GCMDiskHeaderStruct*)malloc(sizeof(GCMDiskHeaderStruct));
	
	h->systemID = rawHeader[0];
	//h->gameID = (char*)malloc(GCM_GAME_ID_LENGTH + 1);
	bzero(h->gameID, GCM_GAME_ID_LENGTH + 1);
	
	int i = 0;
	for (i = 1; i < GCM_GAME_ID_LENGTH; i++) {
		h->gameID[i] = *rawHeader + i;
	}
	
	return NULL;
}

GCMDiskHeaderInfoStruct *GCMRawDiskHeaderInfoToStruct(char *rawInfo) {
	/*
	**  experimental function for creating a struct out of the diskheaderinfo
	**  NOT WORKING (obviously)
	*/

	return NULL;
}

GCMApploaderStruct *GCMRawApploaderToStruct(char *rawApploader) {
	/*
	**  experimental function for creating a struct out of the apploader
	**  NOT WORKING (obviously)
	*/

	return NULL;
}

u32 GCMGetStringTableOffset(FILE *ifile) {
	/*
	**  returns the offset of the stringtable
	*/

	if (!ifile) {
		return 0;
	}
	
	return GCMGetFSTOffset(ifile) + GCMGetFileEntryCount(ifile) * GCM_FST_ENTRY_LENGTH;
}

void GCMFetchFilenameForFileEntry(FILE *ifile, GCMFileEntryStruct *entry) {
	/*
	**  inspects entry, looks up the filename and
	**  sets entry->filename to the filename for that entry
	*/

	if (!ifile || !entry) {
		return;
	}
	
	fseek(ifile, GCMGetStringTableOffset(ifile) + entry->filenameOffset, SEEK_SET);
	char *buf = (char*)malloc(MAXFILENAMESIZE);
	if (fread(buf, 1, MAXFILENAMESIZE, ifile) != MAXFILENAMESIZE) {
		free(buf);
		return;
	}
	
	//to save memory, just allocate enough memory for the filename + the \0
	entry->filename = (char*)malloc(strlen(buf) + 1);
	strcpy(entry->filename, buf);
	free(buf);
}

void GCMFetchDataForFileEntry(FILE *ifile, GCMFileEntryStruct *entry) {
	/*
	**  looks up the offset for the file data by inspecting entry
	**  then sets entry->data to the file's data.
	**  look at entry->length to see what the filesize is (in bytes)
	*/

	if (!ifile || !entry || entry->isDir) {
		return;
	}
	
	fseek(ifile, entry->offset, SEEK_SET);
	char *buf = (char*)malloc(entry->length);
	if (fread(buf, 1, entry->length, ifile) != entry->length) {
		free(buf);
		return;
	}
	
	entry->data = buf;
}

void GCMFreeDiskHeaderStruct(GCMDiskHeaderStruct *dh) {
	/*
	**  convenience method for freeing a diskheaderstruct
	*/

	if (!dh) return;
	
	if (dh->gameID != NULL)
		free(dh->gameID);
	if (dh->makerCode != NULL)
		free(dh->makerCode);
	if (dh->gameName != NULL)
		free(dh->gameName);
		
	free(dh);
}

void GCMFreeDiskHeaderInfoStruct(GCMDiskHeaderInfoStruct *dhi) {
	/*
	**  convenience method for freeing a DiskHeaderInfoStruct
	*/

	if (!dhi) return;
	
	free(dhi);
}

void GCMFreeApploaderStruct(GCMApploaderStruct *a) {
	/*
	**  convenience method for freeing an ApploaderStruct
	*/

	if (!a) return;
	
	if (a->date != NULL)
		free(a->date);
	if (a->code != NULL)
		free(a->code);
		
	free(a);
}

void GCMFreeFileEntryStruct(GCMFileEntryStruct *fe) {
	/*
	**  convenience method for freeing a fileEntryStruct
	*/

	if (!fe) return;
	
	if (fe->data != NULL)
		free(fe->data);
	if (fe->filename != NULL)
		free(fe->filename);
		
	free(fe);
}