/*
**  GCMDiskHeader.h
**  written by spike <spike666@mac.com>
**  www.sadistech.com
**  gcmtool.sourceforge.net
**  
**  part of the gcmtool project
**  Functions for handling diskheaders from Gamecube DVD Images
*/

#ifndef _GCM_DISK_HEADER_H_
#define _GCM_DISK_HEADER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "GCMutils.h"
#include "types.h"

typedef struct gcm_disk_header_struct {
	char	systemID;
	char	gameID[GCM_GAME_ID_LENGTH + 1];
	char	regionCode;
	char	makerCode[GCM_MAKER_CODE_LENGTH + 1];
	char	diskID;
	char	version;
	char	streaming;
	char	streamBufSize;
	u32		unknown1;				// ? seems to be (c2 33 9f 3d) in all files
	char	gameName[GCM_GAME_NAME_LENGTH];
	u32		debugMonitorOffset;		// ?
	u32		debugMonitorAddress;	// ?
	u32		dolOffset;
	u32		fstOffset;
	u32		fstSize;
	u32		fstSizeMax;
	u32		userPosition;			// ?
	u32		userLength;				// ?
	u32		unknown2;				// ?
}GCMDiskHeaderStruct;

//functions...
void GCMDiskHeaderStructToRaw(GCMDiskHeaderStruct *dh, char *buf);
GCMDiskHeaderStruct *GCMRawDiskHeaderToStruct(char *rawHeader);
void GCMFreeDiskHeaderStruct(GCMDiskHeaderStruct *dh);

#ifdef __cplusplus
};
#endif

#endif /* _GCM_DISK_HEADER_H_ */
