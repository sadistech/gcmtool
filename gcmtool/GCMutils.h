/*
 *  GCMutils.h
 *  gcmtool
 *
 *  Created by spike on Sun Mar 28 2004.
 *  Copyright (c) 2004 Sadistech. All rights reserved.
 *
 *  Information gathered from gc-linux.org's yagcd
 *  special thanks to gc-linux.org and the gamecube hacking community.
 *
 */
#ifndef _GCM_UTIL_H_
#define _GCM_UTIL_H_

#include <stdio.h>
#include <string.h>
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// BEGIN CODE...

//constants...
#define MAXFILENAMESIZE					256

//offsets...
#define GCM_DISK_HEADER_OFFSET			ntohl(0x0000)
#define GCM_DISK_HEADER_LENGTH			ntohl(0x0440)
#define GCM_DISK_HEADER_INFO_OFFSET		ntohl(0x0440)
#define GCM_DISK_HEADER_INFO_LENGTH		ntohl(0x2000)
#define GCM_APPLOADER_OFFSET			ntohl(0x2440)

//NOTE: following offsets are relative to the offset of the section they're in (see above)

//disk header stuff
#define GCM_SYSTEM_ID_OFFSET			ntohl(0x0000)
#define GCM_GAME_ID_OFFSET				ntohl(0x0001)
#define GCM_GAME_ID_LENGTH				ntohl(0x0002)
#define GCM_REGION_CODE_OFFSET			ntohl(0x0003)
#define GCM_MAKER_CODE_OFFSET			ntohl(0x0004)
#define GCM_MAKER_CODE_LENGTH			ntohl(0x0002)
#define GCM_GAME_NAME_OFFSET			ntohl(0x0020)
#define GCM_GAME_NAME_LENGTH			ntohl(0x03e0)
#define GCM_DOL_OFFSET_OFFSET			ntohl(0x0420)
#define GCM_DOL_OFFSET_LENGTH			ntohl(0x0004)
#define GCM_FST_OFFSET_OFFSET			ntohl(0x0424)
#define GCM_FST_OFFSET_LENGTH			ntohl(0x0004)
#define GCM_FST_SIZE_OFFSET				ntohl(0x0428)
#define GCM_FST_SIZE_LENGTH				ntohl(0x0004)
#define GCM_USER_POS_OFFSET				ntohl(0x0430)
#define GCM_USER_POS_LENGTH				ntohl(0x0004)
#define GCM_USER_LENGTH_OFFSET			ntohl(0x0434)
#define GCM_USER_LENGTH_LENGTH			ntohl(0x0004)

//apploader stuff
#define GCM_APPLOADER_DATE_OFFSET		ntohl(0x0000)
#define GCM_APPLOADER_DATE_LENGTH		ntohl(0x0010)
#define GCM_APPLOADER_ENTRYPOINT_OFFSET ntohl(0x0010)
#define GCM_APPLOADER_ENTRYPOINT_LENGTH ntohl(0x0004)
#define GCM_APPLOADER_SIZE_OFFSET		ntohl(0x0014)
#define GCM_APPLOADER_SIZE_LENGTH		ntohl(0x0004)
#define GCM_APPLOADER_UNKNOWN_OFFSET	ntohl(0x0018)
#define GCM_APPLOADER_UNKNOWN_LENGTH	ntohl(0x0004)
#define GCM_APPLOADER_CODE_OFFSET		ntohl(0x0020)


//file entry constants
#define GCM_FST_ENTRY_LENGTH			ntohl(0x000c)

// disk sections
void GCMGetDiskHeader(FILE *ifile, char *buf);
void GCMGetDiskHeaderInfo(FILE *ifile, char *buf);
void GCMGetApploader(FILE *ifile, char *buf);
void GCMGetFST(FILE *ifile, char *buf);

//read from header...
char GCMGetSystemID(FILE *ifile);
void GCMGetGameID(FILE *ifile, char *buf);
char GCMGetRegionCode(FILE *ifile);
void GCMGetMakerCode(FILE *ifile, char *buf);
void GCMGetGameName(FILE *ifile, char *buf);

u32 GCMGetDolOffset(FILE *ifile);
u32 GCMGetApploaderSize(FILE *ifile);
u32 GCMGetFSTOffset(FILE *ifile);
u32 GCMGetFSTSize(FILE *ifile);
u32 GCMGetFSTMaxSize(FILE *ifile);

//file entry stuff
void GCMGetNthRawFileEntry(FILE *ifile, int n, char *buf);

#ifdef __cplusplus
};
#endif

#endif /* _GCM_UTIL_H_ */