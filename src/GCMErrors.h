/*
 *  GCMErrors.h
 *  gcmtool
 *
 *  Created by spike on 9.08.2004.
 *  Copyright 2004 Sadistech. All rights reserved.
 *
 */

#ifndef _GCM_ERRORS_
#define _GCM_ERRORS_

//for functions returning a GCMError...
#define GCMError int

//macros for returning error codes
#define GCM_RETURN_SUCCESS()	GCMErrno = GCM_ERR_NO_ERROR; return GCM_SUCCESS;
#define GCM_RETURN_ERROR(code)	GCMErrno = code; return GCM_ERROR;

//error return codes
enum gcm_error {
	GCM_SUCCESS = 0,
	GCM_ERROR = 1
};

//global variable that gets set when a GCM_ERROR is returned...
//a full list of GCMErrno will be provided as implemented
int GCMErrno;

//GCMErrno codes:
#define GCM_ERR_NO_ERROR		0
#define GCM_ERR_NOT_IMPLEMENTED	-1
#define GCM_ERR_NULL_ARG		100
#define GCM_ERR_FILE			500

#endif /* _GCM_ERRORS_ */

