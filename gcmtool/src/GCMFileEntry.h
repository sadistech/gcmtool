/*
**  Part of GCMTools Package
**  spike@sadistech.com
**  http://gcmtool.sourceforge.net
**
**  Functions and types for working with file entries
*/

#ifndef _GCM_FILE_ENTRY_H_
#define _GCM_FILE_ENTRY_H_

#include <stdio.h>
#include "types.h"

typedef struct gcm_file_entry_struct {
	int				index;		//for recursion... this is the file's index in the GCM... the file_entry_offset, if you will...

	char			isDir;
	u32				filenameOffset;
	unsigned long   offset;		//file_offset or parent_offset (dir)
	unsigned long   length;		//file_length or num_entries (root) or next_offset (dir)

	char			*data;
	char			*filename;
}GCMFileEntryStruct;

GCMFileEntryStruct *GCMRawFileEntryToStruct(char *rawEntry, int index);
void GCMFileEntryStructToRaw(GCMFileEntryStruct *e, char *buf);

void GCMFetchFilenameForFileEntry(FILE *ifile, GCMFileEntryStruct *entry);
void GCMFetchDataForFileEntry(FILE *ifile, GCMFileEntryStruct *entry);

void GCMGetFullPathForFileEntry(FILE *ifile, GCMFileEntryStruct *entry, char *buf);

//freeing up memory
void GCMFreeFileEntryStruct(GCMFileEntryStruct *fe);

#endif /* _GCM_FILE_ENTRY_H_ */
