#include "GCMBnrInfo.h"
#include <stdlib.h>

void GCMFreeBnrInfoStruct(GCMBnrInfoStruct *r) {
	/*
	**	recursively frees the BNR infos...
	*/
	
	if (r->next) {
		GCMFreeBnrInfoStruct(r->next);
	}

	free(r);
}

int GCMBnrInfoCount(GCMBnrInfoStruct *r) {
	/*
	**	returns the number of info records in the record chain including r
	*/

	if (r->next) {
		return 1 + GCMBnrInfoCount(r->next);
	} else {
		return 1;
	}
}

GCMBnrInfoStruct *GCMBnrGetNthInfo(GCMBnrInfoStruct *r, int n) {
	/*
	**	returns the nth info record starting at r
	**	if n == 0, assume you want THIS record.
	**	if there's no next, there must be an error... return NULL
	*/
	
	if (n == 0) {
		return r;
	} else if (r->next) {
		return GCMBnrGetNthInfo(r->next, n - 1);
	} else {
		return NULL;
	}
}

