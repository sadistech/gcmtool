/*
 *  GCMextras.h
 *  gcmtool
 *
 *  Created by spike on Sun Mar 28 2004.
 *  Copyright (c) 2004 Sadistech. All rights reserved.
 *
 *  a couple extra functions for working with GCMs
 */

#ifndef _GCM_EXTRAS_H_
#define _GCM_EXTRAS_H_

#include <stdio.h>
#include "GCMUtils.h"
#include "GCMDol.h"
#include "types.h"

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
//the following maker codes I figured out myself... (not from yagcd)
#define GCM_MAKER_ACTIVISION			"52"
#define GCM_MAKER_BANDAI				"B2"
#define GCM_MAKER_UBISOFT				"41"
#define GCM_MAKER_MIRAGE				"A4"


//structs...

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
GCMFileEntryStruct *GCMGetFileEntryAtPath(FILE *ifile, char *path);
GCMFileEntryStruct *GCMRawFileEntryToStruct(char *rawEntry);

//working with sections... as structs (EXPERIMENTAL)


u32 GCMGetStringTableOffset(FILE *ifile);
void GCMFetchFilenameForFileEntry(FILE *ifile, GCMFileEntryStruct *entry);
void GCMFetchDataForFileEntry(FILE *ifile, GCMFileEntryStruct *entry);

//working with bootfile.dol from a .gcm file
u32 GCMGetBootDolLength(FILE *ifile);
u32 GCMGetBootDol(FILE *ifile, char *buf);

//freeing up memory
void GCMFreeFileEntryStruct(GCMFileEntryStruct *fe);

#ifdef __cplusplus
};
#endif

#endif /*_GCM_EXTRAS_H_ */