/*
 *  FileFunctions.c
 *  gcmtool
 *
 *  Created by spike on Fri Jun 25 2004.
 *  Copyright (c) 2004 Sadistech. All rights reserved.
 *
 */

#include "FileFunctions.h"
#include <stdlib.h>

u32 CopyData(FILE *source, FILE *dest, u32 length) {
	/*
	**  copies data from source to dest
	**  does it in FF_BUFFER_SIZE chunks to keep it fast and from using up too much memory...
	**  returns the amount of data copied...
	*/
	
	if (!source || !dest || !length) return 0;
	
	u32 copiedSize = 0; //the amount we've copied so far...
	char *buf = (char*)malloc(FF_BUFFER_SIZE);
	
	while (length > 0) {
		u32 sizeToRead = FF_BUFFER_SIZE;
		
		if (length < sizeToRead) {
			sizeToRead = length;
		}
				
		if (fread(buf, 1, sizeToRead, source) != sizeToRead) {			
			break;
		}
		
		u32 chunkSize = fwrite(buf, 1, sizeToRead, dest);
		copiedSize += chunkSize;
		
		if (chunkSize != sizeToRead) {
			break;
		}
		
		length -= chunkSize;
//		printf("len: %ld\n", length);
	}
	
	free(buf);
	return copiedSize;
}

void WriteDataToFile(char *data, u32 length, char *path) {
	/*
	**  Takes data of length and writes it to a file path. Displays errors when they happen...
	*/
	
/*	char msg[1024] = "Writing to file ";
	strcat(msg, path);
	verbosePrint(msg);
*/	
	if (!data || !length || !path) return;
	
	FILE *ofile = NULL;
	
	if (!(ofile = fopen(path, "w"))) {
		perror(path);
//		printf("An error occurred trying to open %s (%d)\n", path, errno);
		return;
	}
	
	if (fwrite(data, 1, length, ofile) != length) {
		printf("An error occurred trying to write to %s\n", path);
	}
	
	fclose(ofile);
}

u32 ReadDataFromFile(char *buf, char *path) {
	/*
	**  reads from the file at path, 
	**  sets buf to the contents of said file...
	**  and returns the length of data.
	**
	**  use getFilesize() when you allocate buf
	**
	**  returns 0 and displays messages on error;
	*/
	
	/*
	char msg[1024] = "Reading file ";
	strcat(msg, path);
	verbosePrint(msg);
	*/
	if (!path || !buf) return 0;
	
	FILE *ifile = NULL;
	
	u32 length = GetFilesizeFromPath(path); //get how much to read...
	
	if (!length || !(ifile = fopen(path, "r"))) {
		printf("An error occurred trying to open %s\n", path);
		return 0;
	}
	
	if (fread(buf, 1, length, ifile) != length) {
		printf("An error occurred when trying to read %s\n", path);
		fclose(ifile);
		return 0;
	}
	
	fclose(ifile);
	return length;
}

u32 GetFilesizeFromPath(char *path) {
	/*
	**  returns the filesize of the file at *path
	**  useful for when you read a file... gotta make sure you allocate enough memory for the file...
	*/
	
	FILE *ifile = NULL;
	
	if (!(ifile = fopen(path, "r"))) {
		return 0;
	}
	
	fseek(ifile, 0, SEEK_END);
	u32 len = ftell(ifile);
	fclose(ifile);
	
	return len;
}

u32 GetFilesizeFromStream(FILE *stream) {
	/*
	**  returns the filesize of a stream
	**  resets the streams position before returning...
	*/
	
	if (!stream) return 0;
	
	u32 oldPos = ftell(stream);
	fseek(stream, 0, SEEK_END);
	u32 len = ftell(stream);
	fseek(stream, oldPos, SEEK_SET);
	
	return len;
}

