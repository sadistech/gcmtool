/*
 *  GCMextras.h
 *  gcmtool
 *
 *  Created by spike on Sun Mar 28 2004.
 *  Copyright (c) 2004 Sadistech. All rights reserved.
 *
 */

#ifndef _GCM_EXTRAS_H_
#define _GCM_EXTRAS_H_

#include <stdio.h>
#include "GCMUtils.h"

#ifdef __cplusplus
extern "C" {
#endif

//system ID codes
#define GCM_GAMECUBE_ID					'G'
#define GCM_GBA_PLAYER_ID				'U'

//region codes
#define GCM_USA_NTSC					'E'
#define GCM_EUR_PAL						'P'
#define GCM_JAP_NTSC					'J'

//maker codes:
#define GCM_MAKER_NINTENDO				"01"
#define GCM_MAKER_CAPCOM				"08"
#define GCM_MAKER_EIDOS					"4F"
#define GCM_MAKER_ACCLAIM				"51"
#define GCM_MAKER_MIDWAY				"5D"
#define GCM_MAKER_HUDSON				"5G"
#define GCM_MAKER_LUCAS_ARTS			"64"
#define GCM_MAKER_ELECTRONIC_ARTS		"69"
#define GCM_MAKER_TDK					"6S"
#define GCM_MAKER_SEGA					"8P"
#define GCM_MAKER_NAMCO					"AF"
#define GCM_MAKER_TOMY					"DA"
#define GCM_MAKER_KONAMI				"EM"

//structs...

typedef struct gcm_disk_header_struct {
	char			systemID;
	char			gameID[GCM_GAME_ID_LENGTH];
	char			regionCode;
	char			makerCode[GCM_MAKER_CODE_LENGTH];
	char			diskID;
	char			version;
	char			streaming;
	unsigned long   streamBufSize;
	char			gameName[GCM_GAME_NAME_LENGTH];
	unsigned long   dolOffset;
	unsigned long   fstOffset;
	unsigned long   fstSize;
	unsigned long	fstSizeMax;
}GCMDiskHeaderStruct;

typedef struct gcm_disk_header_info_struct {
	unsigned long   debugMonitorSize;
	unsigned long   simulatedMemorySize;
	unsigned long   argumentOffset;
	unsigned long   debugFlag;
	unsigned long   trackLocation;
	unsigned long   trackSize;
	unsigned long   countryCode;
}GCMDiskHeaderInfoStruct;

typedef struct gcm_apploader_struct {
	char			*date;
	long			entrypoint;
	unsigned long   size;
	char			*code;
}GCMApploaderStruct;

typedef struct gcm_file_entry_struct {
	char			isDir;
	long			filenameOffset;
	unsigned long   offset;   //file_offset or parent_offset (dir)
	unsigned long   length;   //file_length or num_entries (root) or next_offset (dir)

	char			*data;
	char			*filename;
}GCMFileEntryStruct;


char *GCMSystemIDToStr(char sysID);
char *GCMMakerCodeToStr(char *code);
char *GCMRegionCodeToStr(char code);

//working with file entries...
GCMFileEntryStruct *GCMGetRootFileEntry(FILE *ifile);
GCMFileEntryStruct *GCMGetNthFileEntry(FILE *ifile, int n);
GCMFileEntryStruct *GCMGetFileEntryByName(FILE *ifile, char *name);
GCMFileEntryStruct *GCMRawFileEntryToStruct(char *rawEntry);

//working with sections... as structs (EXPERIMENTAL)
GCMDiskHeaderStruct *GCMRawDiskHeaderToStruct(char *rawHeader);
GCMDiskHeaderInfoStruct *GCMRawDiskHeaderInfoToStruct(char *rawInfo);
GCMApploaderStruct *GCMRawApploaderToStruct(char *rawApploader);

u32 GCMGetStringTableOffset(FILE *ifile);
void GCMFetchFilenameForFileEntry(FILE *ifile, GCMFileEntryStruct *entry);
void GCMFetchDataForFileEntry(FILE *ifile, GCMFileEntryStruct *entry);

//freeing up memory
void GCMFreeDiskHeaderStruct(GCMDiskHeaderStruct *dh);
void GCMFreeDiskHeaderInfoStruct(GCMDiskHeaderInfoStruct *dhi);
void GCMFreeApploaderStruct(GCMApploaderStruct *a);
void GCMFreeFileEntryStruct(GCMFileEntryStruct *fe);

#ifdef __cplusplus
};
#endif

#endif /*_GCM_EXTRAS_H_ */