/*
**  GCMApploader.h
**  written by spike <spike666@mac.com>
**  www.sadistech.com
**  gcmtool.sourceforge.net
**  
**  part of the gcmtool project
**  Functions for handling Apploaders from Gamecube DVD Images
*/

#ifndef _GCM_APPLOADER_H_
#define _GCM_APPLOADER_H_

#include "GCMutils.h"
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

//structs
typedef struct gcm_apploader_struct {
	char			date[GCM_APPLOADER_DATE_LENGTH];
	u32				entrypoint;
	u32				size; //size of code
	u32				unknown;
	char			*code;
}GCMApploaderStruct;

GCMApploaderStruct *GCMRawApploaderToStruct(char *rawApploader);
void GCMApploaderStructToRaw(GCMApploaderStruct *a, char *buf);
void GCMFreeApploaderStruct(GCMApploaderStruct *a);

#ifdef __cplusplus
};
#endif

#endif


