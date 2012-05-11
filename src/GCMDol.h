/*
**  GCMDol.h
**  written by spike <spike666@mac.com>
**  www.sadistech.com
**  gcmtool.sourceforge.net
**  
**  part of the gcmtool project
**  Functions for handling DOLs from Gamecube DVD Images
**  These functions are explicitly for working with .dol files
**  Some functions may be used to aide in working with .gcm files.
*/

#ifndef _GCM_DOL_H_
#define _GCM_DOL_H_

#include <stdio.h>
#include <stdlib.h>
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GCM_DOL_TEXT_MAX				7
#define GCM_DOL_DATA_MAX				11

#define GCM_DOL_HEADER_LENGTH			0x0100

#define GCM_DOL_TEXT_OFFSET_START		0x0000
#define GCM_DOL_DATA_OFFSET_START		0x001c
#define GCM_DOL_TEXT_MEM_ADDRESS_START  0x0048
#define GCM_DOL_DATA_MEM_ADDRESS_START  0x0064
#define GCM_DOL_TEXT_LENGTH_START		0x0090
#define GCM_DOL_DATA_LENGTH_START		0x00ac
#define GCM_DOL_BSS_MEM_ADDRESS_OFFSET  0x00d8
#define GCM_DOL_BSS_SIZE_OFFSET			0x00dc
#define GCM_DOL_ENTRY_POINT_OFFSET		0x00e0
#define GCM_DOL_DATA_START_OFFSET		0x0100

typedef struct gcm_dol_section_struct {
	u32		offset;
	u32		memAddress;
	u32		length;
	
	char	*data;
}GCMDolSectionStruct;

typedef struct gcm_dol_header_struct {
	GCMDolSectionStruct   text[GCM_DOL_TEXT_MAX];
	GCMDolSectionStruct   data[GCM_DOL_DATA_MAX];
	
	u32				bssMemAddress;
	u32				bssSize;
	
	u32				entryPoint;
}GCMDolHeaderStruct;

GCMDolHeaderStruct *GCMGetDolHeader(FILE *ifile);
GCMDolHeaderStruct *GCMRawToDolHeaderStruct(char *rawHeader);

void GCMDolFetchSectionData(FILE *ifile, GCMDolSectionStruct *section);
u32 GCMDolGetTotalCodeLength(GCMDolHeaderStruct *header);

// some utility functions...
int GCMDolTextCount(GCMDolHeaderStruct *header);
int GCMDolDataCount(GCMDolHeaderStruct *header);

//for freeing memory...
void GCMFreeDolHeaderStruct(GCMDolHeaderStruct *d);
void GCMFreeDolSectionStruct(GCMDolSectionStruct *d);

#ifdef __cplusplus
};
#endif

#endif /* _GCM_DOL_H_ */
