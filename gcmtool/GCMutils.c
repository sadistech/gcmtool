/*
 *  GCMutils.c
 *  gcmtool
 *
 *  Created by spike on Sun Mar 28 2004.
 *  Copyright (c) 2004 Sadistech. All rights reserved.
 *
 */

#include "GCMutils.h"
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

void GCMGetDiskHeader(FILE *ifile, char *buf) {
	/* 
	**	sets buf to the diskheader (boot.bin)
	**  use functions in GCMExtras.h to manipulate data in a useful manner
	*/
	
	if (!ifile || !buf) return;
	
	fseek(ifile, GCM_DISK_HEADER_OFFSET, SEEK_SET);
	if (fread(buf, 1, GCM_DISK_HEADER_LENGTH, ifile) != GCM_DISK_HEADER_LENGTH) {
		free(buf);
		return;
	}
}

void GCMGetDiskHeaderInfo(FILE *ifile, char *buf) {
	/*
	**  sets buf to the diskheaderinfo from ifile (bi2.bin)
	**  use functions from GCMExtras.h to manipulate it in a useful manner
	*/
	
	if (!ifile || !buf) return;
	
	fseek(ifile, GCM_DISK_HEADER_INFO_OFFSET, SEEK_SET);
	if (fread(buf, 1, GCM_DISK_HEADER_INFO_LENGTH, ifile) != GCM_DISK_HEADER_INFO_LENGTH) {
		free(buf);
		return;
	}
}

void GCMGetApploader(FILE *ifile, char *buf) {
	/*
	**  sets buf to the apploader (appldr.bin)
	**  use functions from GCMExtras.h to manipulate it in a useful manner
	*/
	
	if (!ifile || !buf) return;
	
	//this could be wrong...
	//currently it's grabbing apploaderSize + 0x0020 starting at the apploader's start
	
	//first we have to get the apploader size.....
	u32 appSize = GCMGetApploaderSize(ifile) + GCM_APPLOADER_CODE_OFFSET;
	
	fseek(ifile, GCM_APPLOADER_OFFSET, SEEK_SET);
	if (fread(buf, 1, appSize, ifile) != appSize) {
		free(buf);
		return;
	}
}

void GCMGetFST(FILE *ifile, char *buf) {
	/*
	**  sets buf to the entire filesystem table (fst.bin)
	**  be sure to use GCMGetFSTSize() to get the size of the FST when you allocate buf.
	**  the FST contains every file offset and filename offset. no actual file data, though.
	**  this function isn't really useful. GCMGetNthFileEntry() is more useful.
	*/
	
	if (!ifile || !buf) return;
	
	//first we need to get the size of the FST...
	u32 fstSize = GCMGetFSTSize(ifile);
	fseek(ifile, GCMGetFSTOffset(ifile), SEEK_SET);
	if (fread(buf, 1, fstSize, ifile) != fstSize) {
		free(buf);
		return;
	}
}

char GCMGetSystemID(FILE *ifile) {
	/*
	**  returns the system ID as stored in the diskheader
	**  see the system ID Codes section of GCMExtras.h for possible values
	**  returns 0 on error.
	*/
	
	if (!ifile) {
		return 0;
	}
	
	fseek(ifile, GCM_DISK_HEADER_OFFSET + GCM_SYSTEM_ID_OFFSET, SEEK_SET);
	
	return (char)fgetc(ifile);
}

void GCMGetGameID(FILE *ifile, char *buf) {
	/*
	**  sets buf to the 2 character gameID as stored in the diskheader
	**  returns 0 on error
	*/
	
	if (!ifile || !buf) {
		return;
	}
	
	fseek(ifile, GCM_DISK_HEADER_OFFSET + GCM_GAME_ID_OFFSET, SEEK_SET);
	
	if (fread(buf, 1, GCM_GAME_ID_LENGTH, ifile) != GCM_GAME_ID_LENGTH) {
		free(buf);
		return;
	}
}

char GCMGetRegionCode(FILE *ifile) {
	/*
	**  returns the region code as stored in the disk header.
	**  see GCMExtras.h for possible values
	**  Determines if the game is PAL or NTSC, Japanese or not.
	**  returns 0 on error
	*/
	
	if (!ifile) {
		return 0;
	}
	
	fseek(ifile, GCM_DISK_HEADER_OFFSET + GCM_REGION_CODE_OFFSET, SEEK_SET);
	
	return fgetc(ifile);
}

void GCMGetMakerCode(FILE *ifile, char *buf) {
	/*
	**  sets buf to the 2-character maker code of the game as stored in the diskheader
	**  see GCMExtras.h for possible values
	**  returns 0 on error
	**  returns GCM_MAKER_CODE_LENGTH on success.
	*/
	
	if (!ifile || !buf) {
		return;
	}
	
	fseek(ifile, GCM_DISK_HEADER_OFFSET + GCM_MAKER_CODE_OFFSET, SEEK_SET);
	
	//let's zero out buf just to be sure it's still null terminated. It should be GCM_MAKER_CODE_LENGTH + 1 bytes.
	bzero(buf, GCM_MAKER_CODE_LENGTH + 1);
	
	if (fread(buf, 1, GCM_MAKER_CODE_LENGTH, ifile) != GCM_MAKER_CODE_LENGTH) {
		free(buf);
		return;
	}
}

void GCMGetGameName(FILE *ifile, char *buf) {
	/*
	**  sets buf to the game name as stored in the diskheader
	**  reads all of the extra zeros, you may want to trim the excess off at the end, but that's up to you.
	*/
	
	if (!ifile || !buf) {
		return;
	}
	
	fseek(ifile, GCM_DISK_HEADER_OFFSET + GCM_GAME_NAME_OFFSET, SEEK_SET);
	
	if (fread(buf, 1, GCM_GAME_NAME_LENGTH, ifile) != GCM_GAME_NAME_LENGTH) {
		free(buf);
		return;
	}
}

u32 GCMGetDolOffset(FILE *ifile) {
	/*
	**  returns the offset of the main executable DOL (bootfile) file found in the diskheader
	**  returns 0 on error.
	*/
	
	if (!ifile) {
		return 0;
	}
	
	fseek(ifile, GCM_DOL_OFFSET_OFFSET, SEEK_SET);
	u32 *buf = (u32*)malloc(sizeof(u32));
	if (fread(buf, 1, GCM_DOL_OFFSET_LENGTH, ifile) != GCM_DOL_OFFSET_LENGTH) {
		free(buf);
		return 0;
	}
	u32 offset = ntohl(*buf);
	free(buf);
	
	return offset;
}

u32 GCMGetApploaderSize(FILE *ifile) {
	/*
	**  returns the apploader size (varies game to game) (found in the apploader)
	**  returns 0 on error
	*/
	
	if (!ifile) return 0;
	
	//returns the size of the code for the apploader...
	
	fseek(ifile, GCM_APPLOADER_OFFSET + GCM_APPLOADER_SIZE_OFFSET, SEEK_SET);
	u32 *buf = (u32*)malloc(sizeof(u32));
	if (fread(buf, 1, GCM_APPLOADER_SIZE_LENGTH, ifile) != GCM_APPLOADER_SIZE_LENGTH) {
		free(buf);
		return 0;
	}
	u32 appSize = ntohl(*buf);
	free(buf);
	
	return appSize;
}

u32 GCMGetFSTOffset(FILE *ifile) {
	/*
	**  returns the FST offset (found in the diskheader)
	**  returns 0 on error
	*/
	
	if (!ifile) {
		return 0;
	}
	
	fseek(ifile, GCM_DISK_HEADER_OFFSET + GCM_FST_OFFSET_OFFSET, SEEK_SET);
	u32 *buf = (u32*)malloc(sizeof(u32));
	if (fread(buf, 1, GCM_FST_OFFSET_LENGTH, ifile) != GCM_FST_OFFSET_LENGTH) {
		free(buf);
		return 0;
	}
	u32 offset = ntohl(*buf);
	free(buf);
	
	return offset;
}

u32 GCMGetFSTSize(FILE *ifile) {
	/*
	**  returns the size of the FST (found in the diskheader)
	**  returns 0 on error
	*/
	
	if (!ifile) {
		return 0;
	}
	
	fseek(ifile, GCM_DISK_HEADER_OFFSET + GCM_FST_SIZE_OFFSET, SEEK_SET);
	u32 *buf = (u32*)malloc(sizeof(u32));
	if (fread(buf, 1, GCM_FST_SIZE_LENGTH, ifile) != GCM_FST_SIZE_LENGTH) {
		free(buf);
		return 0;
	}
	u32 size = ntohl(*buf);
	free(buf);
	
	return size;
}

void GCMGetNthRawFileEntry(FILE *ifile, int n, char *buf) {
	/*
	**  sets buf to the nth raw file entry.
	**  use functions in GCMExtras.h to do something useful with it
	*/
	
	if (!ifile || n < 0 || !buf) {
		return;
	}
	
	fseek(ifile, GCMGetFSTOffset(ifile) + (GCM_FST_ENTRY_LENGTH * n), SEEK_SET);
	if (fread(buf, 1, GCM_FST_ENTRY_LENGTH, ifile) != GCM_FST_ENTRY_LENGTH) {
		free(buf);
		return;
	}
}
