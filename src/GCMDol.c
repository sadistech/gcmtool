/*
**  GCMDol.c
**  written by spike <spike666@mac.com>
**  www.sadistech.com
**  gcmtool.sourceforge.net
**  
**  part of the gcmtool project
**  Functions for handling DOLs from Gamecube DVD Images
**  These functions are explicitly for working with .dol files
**  Some functions may be used to aide in working with .gcm files.
*/

#include "GCMDol.h"

GCMDolHeaderStruct *GCMGetDolHeader(FILE *ifile) {
	/*
	**  grabs the header from a .dol file and returns a struct
	**  GCMRawToDolHeaderStruct() does all the work, though...
	*/

	if (!ifile) return NULL;
	
	char *buf = (char*)malloc(GCM_DOL_HEADER_LENGTH);
	fseek(ifile, 0, SEEK_SET);
	
	if (fread(buf, 1, GCM_DOL_HEADER_LENGTH, ifile) != GCM_DOL_HEADER_LENGTH) {
		free(buf);
		return NULL;
	}
	
	GCMDolHeaderStruct *d = GCMRawToDolHeaderStruct(buf);
	free(buf);
	return d;
}

GCMDolHeaderStruct *GCMRawToDolHeaderStruct(char *rawHeader) {
	/*
	**  used for taking data either read from a .dol file or extracted from a .gcm
	**  and doing something useful with it.
	**
	**  rawHeader can be just the header, or the entire .dol... it just ignores everything
	**  after the header anyway...
	*/
	
	if (!rawHeader) return NULL;
	
	GCMDolHeaderStruct *d = (GCMDolHeaderStruct*)malloc(sizeof(GCMDolHeaderStruct));
	u32 *l = (u32*)rawHeader;
	
	int i = 0;
	
	for (i = 0; i < GCM_DOL_TEXT_MAX; i++) {
		d->text[i].offset = ntohl(*l++);
	}
	
	for (i = 0; i < GCM_DOL_DATA_MAX; i++) {
		d->data[i].offset = ntohl(*l++);
	}
	
	for (i = 0; i < GCM_DOL_TEXT_MAX; i++) {
		d->text[i].memAddress = ntohl(*l++);
	}
	
	for (i = 0; i < GCM_DOL_DATA_MAX; i++) {
		d->data[i].memAddress = ntohl(*l++);
	}
	
	for (i = 0; i < GCM_DOL_TEXT_MAX; i++) {
		d->text[i].length = ntohl(*l++);
	}
	
	for (i = 0; i < GCM_DOL_DATA_MAX; i++) {
		d->data[i].length = ntohl(*l++);
	}
	
	d->bssMemAddress	=   ntohl(*l++);
	d->bssSize			=   ntohl(*l++);
	d->entryPoint		=   ntohl(*l++);
	
	return d;
}

void GCMDolFetchSectionData(FILE *ifile, GCMDolSectionStruct *section) {
	/*
	**  sets section->data to the data portion of the .dol file
	**  this function is only worthwhile for when you're dealing with .dol files
	*/
	
	if (!ifile || !section) return;
	
	// if data has already been set to something, this will reset it.
	// that way, if we err-out, you can test for that by checking if (section->data == NULL)
	if (section->data) {
		free(section->data);
	}
	
	char *buf = (char*)malloc(section->length);
	fseek(ifile, section->offset, SEEK_SET);
	if (fread(buf, 1, section->length, ifile) != section->length) {
		free(buf);
		return;
	}
	
	section->data = buf;
}

u32 GCMDolGetTotalCodeLength(GCMDolHeaderStruct *header) {
	/*
	**  totals up the lengths of all of the sections
	**  to get the total DOL length take the result of this function and add GCM_DOL_HEADER_LENGTH
	*/
	
	u32 total = 0;
	
	int i = 0;
	for (i = 0; i < GCM_DOL_TEXT_MAX; i++) {
		total += header->text[i].length;
	}
	
	for (i = 0; i < GCM_DOL_DATA_MAX; i++) {
		total += header->data[i].length;
	}
	
	return total;
}

int GCMDolTextCount(GCMDolHeaderStruct *header) {
	/*
	**  Returns the number of TEXT sections of the dol
	**  if a TEXT section's offset == zero, then that's the end of the list...
	*/
	
	int i = 0;
	
	for (i = 0; (i < GCM_DOL_TEXT_MAX) && (header->text->offset); i++);
	
	return i;
}

int GCMDolDataCount(GCMDolHeaderStruct *header) {
	/*
	**  Returns the number of DATA sections of the dol
	**  if a DATA section's offset == zero, then that's the end of the list...
	*/
	
	int i = 0;
	
	for (i = 0; (i < GCM_DOL_DATA_MAX) && (header->data->offset); i++);
	
	return i;
}

//for freeing memory...
void GCMFreeDolHeaderStruct(GCMDolHeaderStruct *d) {
	if (!d) return;
	
	int i = 0;
	
	
	for (i = 0; i < GCM_DOL_TEXT_MAX; i++) {
		if (d->text[i].offset != 0) {
			GCMFreeDolSectionStruct(&d->text[i]);
		}
	}
	
	for (i = 0; i < GCM_DOL_DATA_MAX; i++) {
		if (d->data[i].offset != 0) {
			GCMFreeDolSectionStruct(&d->data[i]);
		}
	}
	
	free(d);
}

void GCMFreeDolSectionStruct(GCMDolSectionStruct *d) {
	if (!d) return;
	
	if (d->data) {
		free(d->data);
	}
//	free(d);
}
