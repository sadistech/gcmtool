/*
**  GCMetras.h
**  written by spike <spike666@mac.com>
**  www.sadistech.com
**  gcmtool.sourceforge.net
**  
**  part of the gcmtool project
**  Extra functions for working with Gamecube DVD images.
*/

#ifndef _GCM_EXTRAS_H_
#define _GCM_EXTRAS_H_

#include <stdio.h>
#include "GCMutils.h"
#include "GCMFileEntry.h"
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
//the following maker codes I figured out myself... (not from yagcd, HA!)
#define GCM_MAKER_ACTIVISION			"52"
#define GCM_MAKER_BANDAI				"B2"
#define GCM_MAKER_UBISOFT				"41"
#define GCM_MAKER_MIRAGE				"A4"

//filenames for various sections...
#define GCM_DISK_HEADER_FILENAME		"boot.bin"
#define GCM_DISK_HEADER_INFO_FILENAME   "bi2.bin"
#define GCM_APPLOADER_FILENAME			"appldr.bin"
#define GCM_FST_FILENAME				"fst.bin"
#define GCM_BOOT_DOL_FILENAME			"boot.dol"


char *GCMSystemIDToStr(char sysID);
char *GCMMakerCodeToStr(char *code);
char *GCMRegionCodeToStr(char code);

GCMFileEntryStruct *GCMGetRootFileEntry(FILE *ifile);
GCMFileEntryStruct *GCMGetNthFileEntry(FILE *ifile, int n);
GCMFileEntryStruct *GCMGetFileEntryByName(FILE *ifile, char *name);
GCMFileEntryStruct *GCMGetFileEntryAtPath(FILE *ifile, char *path);

u32 GCMGetStringTableOffset(FILE *ifile);

//working with bootfile.dol from a .gcm file
u32 GCMGetBootDolLength(FILE *ifile);
u32 GCMGetBootDol(FILE *ifile, char *buf);


#ifdef __cplusplus
};
#endif

#endif /*_GCM_EXTRAS_H_ */
