#include "GCMBnrInfoRecord.h"
#include <stdlib.h>

void GCMFreeBnrInfoRecordStruct(GCMBnrInfoRecordStruct *r) {
	/*
	**	recursively frees the BNR infoRecords...
	*/
	
	if (r->next) {
		GCMFreeBnrInfoRecordStruct(r->next);
	}

	free(r);
}

int GCMBnrInfoRecordCount(GCMBnrInfoRecordStruct *r) {
	/*
	**	returns the number of info records in the record chain including r
	*/

	if (r->next) {
		return 1 + GCMBnrInfoRecordCount(r->next);
	} else {
		return 1;
	}
}

GCMBnrInfoRecordStruct *GCMBnrGetNthInfoRecord(GCMBnrInfoRecordStruct *r, int n) {
	/*
	**	returns the nth info record starting at r
	**	if n == 0, assume you want THIS record.
	**	if there's no next, there must be an error... return NULL
	*/
	
	if (n == 0) {
		return r;
	} else if (r->next) {
		return GCMBnrGetNthInfoRecord(r->next, n - 1);
	} else {
		return NULL;
	}
}

