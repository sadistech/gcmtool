/*
**  GCMetras.c
**  written by spike <spike666@mac.com>
**  www.sadistech.com
**  gcmtool.sourceforge.net
**  
**  part of the gcmtool project
**  Extra functions for working with Gamecube DVD images.
*/

#include "GCMextras.h"
#include "GCMutils.h"
#include "pathfunc.h"
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
	//anything that follows are companies that I added myself (didn't come from yagcd)
	} else if (strcmp(code, GCM_MAKER_ACTIVISION) == 0) {
		return "Activision";
	} else if (strcmp(code, GCM_MAKER_BANDAI) == 0) {
		return "Bandai";
	} else if (strcmp(code, GCM_MAKER_UBISOFT) == 0) {
		return "Ubisoft";
	} else if (strcmp(code, GCM_MAKER_MIRAGE) == 0) {
		return "Mirage Studios";
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
	//entry->filename = "";
	return entry;
}

GCMFileEntryStruct *GCMGetNthFileEntry(FILE *ifile, int n) {
	/*
	**  returns the nth file entry as a struct
	*/

	char *rawEntry = (char*)malloc(GCM_FST_ENTRY_LENGTH);
	GCMGetNthRawFileEntry(ifile, n, rawEntry);
	GCMFileEntryStruct *entry = GCMRawFileEntryToStruct(rawEntry, n);
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

GCMFileEntryStruct *GCMGetFileEntryAtPath(FILE *ifile, char *path) {
	/*
	**  returns the file entry at path
	**  entry WILL have it's filename attribute set.
	**
	**  path must be an absolute path (ie: /this/is/the/file.raw)
	**  it must have a leading /
	*/
	
	int curPathComponent = 1;
	int i = 0;
	int entryCount = GCMGetFileEntryCount(ifile);
	
	// loop through all of the file entries...
	for (i = 1; i < entryCount; i++) {
		// grab the current entry...
		GCMFileEntryStruct *e = GCMGetNthFileEntry(ifile, i);
		GCMFetchFilenameForFileEntry(ifile, e);
		
		if (!e) {
			printf("an error occurred! e is NULL!\n");
			exit(1);
		}
		
		// if this entry matches the next pathComponent, then....
		char *nthPC = (char*)malloc(512);
		if (strcmp(e->filename, nthPathComponent(path, curPathComponent, nthPC)) == 0) {
			
			if (curPathComponent == pathComponentCount(path)) {
				// it's the file we're looking for! return it!
				return e;
			} else {
				// otherwise... let's 'cd' into that directory, and continue...
				curPathComponent++;
			}
			
		} else if (e->isDir) {
			// it's not what we're looking for, and it's a directory, 
			// so just skip over everything and move onto the next file...
			
			i = (int)e->length - 1;
		}
		free(nthPC);

		// free our working entry so we can move on to the next one...
		GCMFreeFileEntryStruct(e);
		//free(e);
	}
	
	//file not found, so just return NULL...
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


u32 GCMGetBootDolLength(FILE *ifile) {
	/*
	**  returns the size of the entire bootfile DOL from the GCM ifile
	**  size = (HEADERSIZE + CODESIZE)
	**
	**  returns 0 on error
	*/
	
	char *headerBuf = (char*)malloc(GCM_DOL_HEADER_LENGTH);
	
	u32 dolOffset = GCMGetDolOffset(ifile);
	
	fseek(ifile, dolOffset, SEEK_SET);
	
	if (fread(headerBuf, 1, GCM_DOL_HEADER_LENGTH, ifile) != GCM_DOL_HEADER_LENGTH) {
		free(headerBuf);
		return 0;
	}
	
	GCMDolHeaderStruct *h = GCMRawToDolHeaderStruct(headerBuf);
	free(headerBuf);
	
	return GCMDolGetTotalCodeLength(h) + GCM_DOL_HEADER_LENGTH;
}

u32 GCMGetBootDol(FILE *ifile, char *buf) {
	/*
	**  sets buf to the main executable bootfile DOL from the GCM file ifile.
	**  uses GCMDol.h to calculate the size it needs to grab...
	**
	**  you've got to allocate the memory for buf, first...
	**  use GCMGetBootDolLength() to do that...
	**
	**  returns the size of the DOL
	**  returns 0 on error
	*/
	
	u32 lengthToRead = GCMGetBootDolLength(ifile);
	u32 dolOffset = GCMGetDolOffset(ifile);
	
	fseek(ifile, dolOffset, SEEK_SET);
	
	if (fread(buf, 1, lengthToRead, ifile) != lengthToRead) {
		free(buf);
		return 0;
	}
	
	return lengthToRead;
}
