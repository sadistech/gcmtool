/*
**  GCMDiskHeader.c
**  written by spike <spike666@mac.com>
**  www.sadistech.com
**  gcmtool.sourceforge.net
**  
**  part of the gcmtool project
**  Functions for handling diskheaders from Gamecube DVD Images
*/

#include <stdlib.h>
#include "GCMDiskHeader.h"
#include <strings.h>

GCMDiskHeaderStruct *GCMNewDiskHeaderStruct() {
	/*
	**  creates a new diskheader struct and initializes it...
	*/
	
	GCMDiskHeaderStruct *d = (GCMDiskHeaderStruct*)malloc(sizeof(GCMDiskHeaderStruct));
	
	d->systemID = 0;
	bzero(d->gameID, GCM_GAME_ID_LENGTH + 1);
	d->regionCode = 0;
	bzero(d->makerCode, GCM_MAKER_CODE_LENGTH + 1);
	d->diskID = 0;
	d->version = 0;
	d->streaming = 0;
	d->streamBufSize = 0;
	d->unknown1 = 0;
	bzero(d->gameName, GCM_GAME_NAME_LENGTH + 1);
	d->debugMonitorOffset = 0;
	d->debugMonitorAddress = 0;
	d->dolOffset = 0;
	d->fstOffset = 0;
	d->fstSize = 0;
	d->fstSizeMax = 0;
	d->userPosition = 0;
	d->userLength = 0;
	d->unknown2 = 0;
	
	return d;
}

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

	GCMDiskHeaderStruct *h = GCMNewDiskHeaderStruct();
	
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

	h->unknown1 = *((u32*)rawHeader);
	h->unknown1 = ntohl(h->unknown1);
	*rawHeader++;
	//((u32*)rawHeader)++;
	
	bzero(h->gameName, GCM_GAME_NAME_LENGTH);
	memcpy(h->gameName, rawHeader, GCM_GAME_NAME_LENGTH);
	rawHeader += GCM_GAME_NAME_LENGTH;
	
	h->debugMonitorOffset = *((u32*)rawHeader);
	h->debugMonitorOffset = ntohl(h->debugMonitorOffset);
	*rawHeader++;
	
	h->debugMonitorAddress = *((u32*)rawHeader);
	h->debugMonitorAddress = ntohl(h->debugMonitorAddress);
	*rawHeader++;
	
	rawHeader += 24; //skip some padding?
	
	h->dolOffset	= *((u32*)rawHeader);
	h->dolOffset	= ntohl(h->dolOffset);
	*rawHeader++;
	
	h->fstOffset	= *((u32*)rawHeader);
	h->fstOffset	= ntohl(h->fstOffset);
	*rawHeader++;
	
	h->fstSize		= *((u32*)rawHeader);
	h->fstSize		= ntohl(h->fstSize);
	*rawHeader++;
	
	h->fstSizeMax   = *((u32*)rawHeader);
	h->fstSizeMax	= ntohl(h->fstSizeMax);
	*rawHeader++;
	
	h->userPosition = *((u32*)rawHeader);
	h->userPosition = ntohl(h->userPosition);
	*rawHeader++;
	
	h->userLength   = *((u32*)rawHeader);
	h->userLength	= ntohl(h->userLength);
	*rawHeader++;
	
	h->unknown2		= *((u32*)rawHeader);
	h->unknown2		= ntohl(h->unknown2);
	*rawHeader++;
	
	return h;
}

void GCMFreeDiskHeaderStruct(GCMDiskHeaderStruct *dh) {
	/*
	**  convenience method for freeing a diskheaderstruct
	*/

	if (!dh) return;
		
	free(dh);
}
