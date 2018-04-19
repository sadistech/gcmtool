/*
**  GCMDiskHeaderInfo.c
**  written by spike <spike666@mac.com>
**  www.sadistech.com
**  gcmtool.sourceforge.net
**  
**  part of the gcmtool project
**  Functions for handling diskheaderinfo from Gamecube DVD Images
*/

#include "GCMDiskHeaderInfo.h"
#include "GCMutils.h"
#include <string.h>
#include <strings.h>
#include <stdlib.h>

GCMDiskHeaderInfoStruct *GCMNewDiskHeaderInfoStruct() {
	/*
	**  allocates, initializes and returns a new diskheaderinfostruct
	*/
	
	GCMDiskHeaderInfoStruct *d = (GCMDiskHeaderInfoStruct*)malloc(sizeof(GCMDiskHeaderInfoStruct));
	
	d->debugMonitorSize = 0;
	d->simulatedMemorySize = 0;
	d->argumentOffset = 0;
	d->debugFlag = 0;
	d->trackLocation = 0;
	d->trackSize = 0;
	d->countryCode = 0;
	d->unknown1 = 0;
	d->unknown2 = 0;
	
	return d;
}

void GCMDiskHeaderInfoStructToRaw(GCMDiskHeaderInfoStruct *d, char *buf) {
	/*
	**  sets buf to a DiskHeaderInfoStruct as raw data
	**  suitable for writing to a file or dumping back to a GCM
	**  
	**  when allocating memory for buf, you should use GCM_DISK_HEADER_INFO_LENGTH
	*/
	
	if (!d || !buf) return;
	
	char *start = buf;
	
	//first, let's zero out the buffer
	bzero(buf, GCM_DISK_HEADER_INFO_LENGTH);
	
	u32 *debugMonitorSize = (u32*)malloc(sizeof(u32));
	*debugMonitorSize = htonl(d->debugMonitorSize);
	memcpy(buf, debugMonitorSize, sizeof(u32));
	free(debugMonitorSize);
	buf += sizeof(u32);
	
	u32 *simulatedMemorySize = (u32*)malloc(sizeof(u32));
	*simulatedMemorySize = htonl(d->simulatedMemorySize);
	memcpy(buf, simulatedMemorySize, sizeof(u32));
	free(simulatedMemorySize);
	buf += sizeof(u32);
	
	u32 *argumentOffset = (u32*)malloc(sizeof(u32));
	*argumentOffset = htonl(d->argumentOffset);
	memcpy(buf, argumentOffset, sizeof(u32));
	free(argumentOffset);
	buf += sizeof(u32);
	
	u32 *debugFlag = (u32*)malloc(sizeof(u32));
	*debugFlag = htonl(d->debugFlag);
	memcpy(buf, debugFlag, sizeof(u32));
	free(debugFlag);
	buf += sizeof(u32);
	
	u32 *trackLocation = (u32*)malloc(sizeof(u32));
	*trackLocation = htonl(d->trackLocation);
	memcpy(buf, trackLocation, sizeof(u32));
	free(trackLocation);
	buf += sizeof(u32);
	
	u32 *trackSize = (u32*)malloc(sizeof(u32));
	*trackSize = htonl(d->trackSize);
	memcpy(buf, trackSize, sizeof(u32));
	free(trackSize);
	buf += sizeof(u32);
	
	u32 *countryCode = (u32*)malloc(sizeof(u32));
	*countryCode = htonl(d->countryCode);
	memcpy(buf, countryCode, sizeof(u32));
	free(countryCode);
	buf += sizeof(u32);
	
	u32 *unknown1 = (u32*)malloc(sizeof(u32));
	*unknown1 = htonl(d->unknown1);
	memcpy(buf, unknown1, sizeof(u32));
	free(unknown1);
	buf += sizeof(u32);
	
	u32 *unknown2 = (u32*)malloc(sizeof(u32));
	*unknown2 = htonl(d->unknown2);
	memcpy(buf, unknown2, sizeof(u32));
	free(unknown2);
	buf += sizeof(u32);
	
	buf = start;
}

GCMDiskHeaderInfoStruct *GCMRawDiskHeaderInfoToStruct(char *rawInfo) {
	/*
	**  experimental function for creating a struct out of the diskheaderinfo
	**  
	*/

	GCMDiskHeaderInfoStruct *d = GCMNewDiskHeaderInfoStruct();

	d->debugMonitorSize	= *((u32*)rawInfo);
	d->debugMonitorSize	= ntohl(d->debugMonitorSize);
	*rawInfo++;
	
	d->simulatedMemorySize  = *((u32*)rawInfo);
	d->simulatedMemorySize	= ntohl(d->simulatedMemorySize); 
	*rawInfo++;
	
	d->argumentOffset		= *((u32*)rawInfo);
	d->argumentOffset		= ntohl(d->argumentOffset);
	*rawInfo++;
	
	d->debugFlag			= *((u32*)rawInfo);
	d->debugFlag			= ntohl(d->argumentOffset);
	*rawInfo++;
	
	d->trackLocation		= *((u32*)rawInfo);
	d->trackLocation		= ntohl(d->trackLocation);
	*rawInfo++;
	
	d->trackSize			= *((u32*)rawInfo);
	d->trackSize			= ntohl(d->trackSize);
	*rawInfo++;
	
	d->countryCode			= *((u32*)rawInfo);
	d->countryCode			= ntohl(d->countryCode);
	*rawInfo++;
	
	d->unknown1				= *((u32*)rawInfo);
	d->unknown1				= ntohl(d->unknown1);
	*rawInfo++;
	
	d->unknown2				= *((u32*)rawInfo);
	d->unknown2				= ntohl(d->unknown2);
	*rawInfo++;
	
	return d;
}

void GCMFreeDiskHeaderInfoStruct(GCMDiskHeaderInfoStruct *dhi) {
	/*
	**  convenience method for freeing a DiskHeaderInfoStruct
	*/

	if (!dhi) return;
	
	free(dhi);
}

