#include "GCMFileEntry.h"
#include "GCMutils.h"
#include "GCMextras.h"
#include <stdlib.h>

//prototype for static function
static int readCString(char *buf, int maxLen, FILE *ifile);

GCMFileEntryStruct *GCMRawFileEntryToStruct(char *rawEntry, int index) {
	/*
	**  converts the rawEntry into a struct.
	**  doesn't fetch the filename or data (to save memory/speed things up)
	**  use GCMFetchFilenameForFileEntry() to get the filename...
	**
	**  Format of a raw file entry:
	**  start__|__size__|__Description
	**   0x00     1		   flags; 0: file, 1: directory
	**   0x01     3		   filename_offset (relative to string table)
	**   0x04     4        file: file_offset, dir: parent_offset
	**   0x08     4        file: file_length, dir: next_offset
	*/

	if (!rawEntry) {
		return NULL;
	}
	
	GCMFileEntryStruct *fe = (GCMFileEntryStruct*)malloc(sizeof(GCMFileEntryStruct));
	
	//it's a file if the first byte is 1, directory if 0
	fe->isDir = *rawEntry;
	
	//then get the filename_offset
	u32 *l;
	l = (u32*)rawEntry;
	if (rawEntry[0] != 0)
		fe->filenameOffset = ntohl(*l) - 0x01000000;
	else 
		fe->filenameOffset = ntohl(*l);
	
	//get the file_offset/parent_offset
	l = (u32*)rawEntry + 1;
	fe->offset = ntohl(*l);
	
	//get the file_length/next_offset
	l = (u32*)rawEntry + 2;
	fe->length = ntohl(*l);
	
	fe->index = index;
	
	return fe;
}

void GCMFileEntryStructToRaw(GCMFileEntryStruct *e, char *buf) {
	/*
	**  ADD DESCRIPTION...
	*/
	
	if (!e || !buf) return;
	
	bzero(buf, GCM_FST_ENTRY_LENGTH);
	
	char *start = buf;
	
	if (e->isDir) {
		buf[0] = 1;
	}
	*buf++;
	
	//this gets tricky, because we only have a 24bit unsigned int, here...
	//there's a better way to do this... it's on my to-do list.
	u32 *tempFilenameOffset = (u32*)malloc(sizeof(u32));
	*tempFilenameOffset = (e->filenameOffset << 8); //shift it one byte...
	memcpy(buf, tempFilenameOffset, 3); //since it's only 3 bytes that we want...
	buf += 3;
	
	u32 *offset = (u32*)malloc(sizeof(u32));
	*offset = htonl(e->offset);
	memcpy(buf, offset, sizeof(u32));
	buf += sizeof(u32);
	
	u32 *length = (u32*)malloc(sizeof(u32));
	*length = htonl(e->length);
	memcpy(buf, length, sizeof(u32));
	buf += sizeof(u32);
	
	buf = start;
}

static int readCString(char *buf, int maxLen, FILE *ifile) {
	/*
	**  reads a string only up to a \0... to prevent it from getting data we don't need.
	*/
	
	if (!buf || !maxLen || !ifile) return 0;
	
	int i = 0;
	
	for (i = 0;(*buf++ = fgetc(ifile)) && i < maxLen; i++);
	
	return i + 1;
}

void GCMFetchFilenameForFileEntry(FILE *ifile, GCMFileEntryStruct *entry) {
	/*
	**  inspects entry, looks up the filename and
	**  sets entry->filename to the filename for that entry as found in ifile...
	*/

	if (!ifile || !entry) {
		return;
	}
	
	if (entry->index == 0) { //it's the root entry...
		entry->filename = (char*)malloc(2);
		strcpy(entry->filename, "/");
		return;
	}
	
	fseek(ifile, GCMGetStringTableOffset(ifile) + entry->filenameOffset, SEEK_SET);
	char *buf = (char*)malloc(MAXFILENAMESIZE);
	/*if (fread(buf, 1, MAXFILENAMESIZE, ifile) != MAXFILENAMESIZE) {
		free(buf);
		return;
	}*/
	
	int len = readCString(buf, MAXFILENAMESIZE, ifile);
	
	//to save memory, just allocate enough memory for the filename + the \0
	entry->filename = (char*)malloc(len);
	strcpy(entry->filename, buf);
	free(buf);
}

void GCMFetchDataForFileEntry(FILE *ifile, GCMFileEntryStruct *entry) {
	/*
	**  looks up the offset for the file data by inspecting entry
	**  then sets entry->data to the file's data.
	**  look at entry->length to see what the filesize is (in bytes)
	*/

	if (!ifile || !entry || entry->isDir) {
		return;
	}
	
	fseek(ifile, entry->offset, SEEK_SET);
	char *buf = (char*)malloc(entry->length);
	if (fread(buf, 1, entry->length, ifile) != entry->length) {
		free(buf);
		return;
	}
	
	entry->data = buf;
}

void GCMGetFullPathForFileEntry(FILE *ifile, GCMFileEntryStruct *entry, char *buf) {
	/*
	**  set buf to the full path of file entry
	**  since each directory entry has the fileEntryOffset of its parent (stored in offset)
	**  you can just recursively jump up the tree until you get to an entry whose parent_offset is 0.
	**
	**  The only gotcha is that if you want to get the full path of an actual file, you have to 
	**  remember what its parent is... ug...
	**
	**  buf should be allocaed with enough space to store the full path. 512 should be more than enough...
	*/
	
	if (!ifile || !entry || !entry->isDir || !buf) return;
	
	//if it's the root entry (entry[0]), just set buf to ""
	//this prevents breaking if you want the full path of a file in the root directory...
	//if (entry->index == 0){
	//	strcpy(buf, "");
	//	return;
	//}
	
	GCMFetchFilenameForFileEntry(ifile, entry);

	//printf("%d %s\n", entry->index, entry->filename);
	
	if (entry->index == 0) {
		strcpy(buf, "/");
		return;
	}
	
	char fullPath[1024] = ""; //allocate 1024, just to be safe...
		
	strcpy(fullPath, entry->filename);
	
	GCMFileEntryStruct *e = entry; // = GCMGetNthFileEntry(ifile, entry->offset);
	
	while (e->offset) {
		e = GCMGetNthFileEntry(ifile, e->offset);
		GCMFetchFilenameForFileEntry(ifile, e);
		
		char s[1024] = "";
		strcpy(s, fullPath);
		sprintf(fullPath, "%s/%s", e->filename, s);
	}
	
	strcpy(buf, "/"); //leading / on absolute path...
	strcat(buf, fullPath); //append the fullPath...
}

void GCMFreeFileEntryStruct(GCMFileEntryStruct *fe) {
	/*
	**  convenience method for freeing a fileEntryStruct
	**  not really implemented too well...
	*/

	if (!fe) return;
	
	if (fe->data != NULL)
		free(fe->data);
	if (fe->filename != NULL)
		free(fe->filename);
		
	free(fe);
}
