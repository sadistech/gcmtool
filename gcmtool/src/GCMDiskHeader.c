/*
 *  GCMDiskHeader.c
 *  gcmtool
 *
 *  Created by spike on Mon May 24 2004.
 *  Copyright (c) 2004 Sadistech. All rights reserved.
 *
 */

#include <stdlib.h>
#include "GCMDiskHeader.h"

void GCMDiskHeaderStructToRaw(GCMDiskHeaderStruct *dh, char *buf) {
	/*
	**  takes a GCMDiskHeaderStruct and stores it as raw data into buf
	**  suitable for writing to boot.bin, or back into a GCM.
	**
	**  when allocating memory for buf, use GCM_DISK_HEADER_LENGTH
	*/
	
	if (!dh || !buf) return;
	
	//first just zero out the buffer, just to be sure......
	bzero(buf, GCM_DISK_HEADER_LENGTH);
	
	char *start = buf;
	
	buf[0] = dh->systemID;
	*buf++;
	
	memcpy(buf, dh->gameID, GCM_GAME_ID_LENGTH);
	buf += GCM_GAME_ID_LENGTH;
	
	buf[0] = dh->regionCode;
	*buf++;
	
	memcpy(buf, dh->makerCode, GCM_MAKER_CODE_LENGTH);
	buf += GCM_MAKER_CODE_LENGTH;
	
	buf[0] = dh->diskID;
	*buf++;
	
	buf[0] = dh->version;
	*buf++;
	
	buf[0] = dh->streaming;
	*buf++;
	
	buf[0] = dh->streamBufSize;
	*buf++;
	
	//skip over some padding...
	buf += 18;
	
	u32 *unknown1 = (u32*)malloc(sizeof(u32));
	*unknown1 = htonl(dh->unknown1);
	memcpy(buf, unknown1, sizeof(u32));
	free(unknown1);
	buf += sizeof(u32);
	
	memcpy(buf, dh->gameName, GCM_GAME_NAME_LENGTH);
	buf += GCM_GAME_NAME_LENGTH;
	
	u32 *debugMonitorOffset = (u32*)malloc(sizeof(u32));
	*debugMonitorOffset = htonl(dh->debugMonitorOffset);
	memcpy(buf, debugMonitorOffset, sizeof(u32));
	free(debugMonitorOffset);
	buf += sizeof(u32);
	
	u32 *debugMonitorAddress = (u32*)malloc(sizeof(u32));
	*debugMonitorAddress = htonl(dh->debugMonitorAddress);
	memcpy(buf, debugMonitorAddress, sizeof(u32));
	free(debugMonitorAddress);
	buf += sizeof(u32);
	
	//skip over some more padding...
	buf += 24;
	
	u32 *dolOffset = (u32*)malloc(sizeof(u32));
	*dolOffset = htonl(dh->dolOffset);
	memcpy(buf, dolOffset, sizeof(u32));
	free(dolOffset);
	buf += sizeof(u32);
	
	u32 *fstOffset = (u32*)malloc(sizeof(u32));
	*fstOffset = htonl(dh->fstOffset);
	memcpy(buf, fstOffset, sizeof(u32));
	free(fstOffset);
	buf += sizeof(u32);
	
	u32 *fstSize = (u32*)malloc(sizeof(u32));
	*fstSize = htonl(dh->fstSize);
	memcpy(buf, fstSize, sizeof(u32));
	free(fstSize);
	buf += sizeof(u32);
	
	u32 *fstSizeMax = (u32*)malloc(sizeof(u32));
	*fstSizeMax = htonl(dh->fstSizeMax);
	memcpy(buf, fstSizeMax, sizeof(u32));
	free(fstSizeMax);
	buf += sizeof(u32);
	
	u32 *userPosition = (u32*)malloc(sizeof(u32));
	*userPosition = htonl(dh->userPosition);
	memcpy(buf, userPosition, sizeof(u32));
	free(userPosition);
	buf += sizeof(u32);
	
	u32 *userLength = (u32*)malloc(sizeof(u32));
	*userLength = htonl(dh->userLength);
	memcpy(buf, userLength, sizeof(u32));
	free(userLength);
	buf += sizeof(u32);
	
	u32 *unknown2 = (u32*)malloc(sizeof(u32));
	*unknown2 = htonl(dh->unknown2);
	memcpy(buf, unknown2, sizeof(u32));
	free(unknown2);
	buf += sizeof(u32);
	
	buf = start;
}

GCMDiskHeaderStruct *GCMRawDiskHeaderToStruct(char *rawHeader) {
	/*
	**  experimental method for loading the diskheader into a struct
	*/

	GCMDiskHeaderStruct *h = (GCMDiskHeaderStruct*)malloc(sizeof(GCMDiskHeaderStruct));
	
	h->systemID = rawHeader[0];
	*rawHeader++;
	
	bzero(h->gameID, GCM_GAME_ID_LENGTH + 1);
	memcpy(h->gameID, rawHeader, GCM_GAME_ID_LENGTH);
	rawHeader += GCM_GAME_ID_LENGTH;
	
	h->regionCode = rawHeader[0];
	*rawHeader++;
	
	bzero(h->makerCode, GCM_MAKER_CODE_LENGTH + 1);
	memcpy(h->makerCode, rawHeader, GCM_MAKER_CODE_LENGTH);
	rawHeader += GCM_MAKER_CODE_LENGTH;
	
	h->diskID = rawHeader[0];
	*rawHeader++;
	
	h->version = rawHeader[0];
	*rawHeader++;
	
	h->streaming = rawHeader[0];
	*rawHeader++;
	
	h->streamBufSize = rawHeader[0];
	*rawHeader++;
	
	rawHeader += 18; //skip padding ?
	
	h->unknown1 = ntohl(*((u32*)rawHeader)++);
	
	bzero(h->gameName, GCM_GAME_NAME_LENGTH);
	memcpy(h->gameName, rawHeader, GCM_GAME_NAME_LENGTH);
	rawHeader += GCM_GAME_NAME_LENGTH;
	
	h->debugMonitorOffset = ntohl(*((u32*)rawHeader)++);
	h->debugMonitorAddress = ntohl(*((u32*)rawHeader));
	
	rawHeader += 28; //skip some padding?
	
	h->dolOffset	= ntohl(*((u32*)rawHeader)++);
	h->fstOffset	= ntohl(*((u32*)rawHeader)++);
	h->fstSize		= ntohl(*((u32*)rawHeader)++);
	h->fstSizeMax   = ntohl(*((u32*)rawHeader)++);
	h->userPosition = ntohl(*((u32*)rawHeader)++);
	h->userLength   = ntohl(*((u32*)rawHeader)++);
	
	h->unknown2		= ntohl(*((u32*)rawHeader)++);
	
	return h;
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