/*
**	GCMBnrInfoRecord.h
**	written by spike <spike666@mac.com>
**	www.sadistech.com
**	gcmtool.sourceforge.net
**
**	gcmtool
**	Functions for dealing with banners (version 1 and 2)
*/

#ifndef _GCM_BNR_INFO_RECORD_H_
#define _GCM_BNR_INFO_RECORD_H_

#include "GCMBnr.h"

#define GCM_BNR_INFO_GAME_NAME_OFFSET		0x0000
#define GCM_BNR_INFO_DEVELOPER_OFFSET		0x0020
#define GCM_BNR_INFO_FULL_TITLE_OFFSET		0x0040
#define GCM_BNR_INFO_FULL_DEVELOPER_OFFSET  	0x0080
#define GCM_BNR_INFO_DESCRIPTION_OFFSET		0x00c0

#define GCM_BNR_INFO_RECORD_SIZE		0x0140	/* the size of each record field in the BNR file */

typedef struct gcm_bnr_info_record_struct {
	char			name[GCM_BNR_GAME_NAME_LENGTH];
	char			developer[GCM_BNR_DEVELOPER_LENGTH];
	char			fullName[GCM_BNR_FULL_TITLE_LENGTH];
	char			fullDeveloper[GCM_BNR_FULL_DEVELOPER_LENGTH];
	char			description[GCM_BNR_DESCRIPTION_LENGTH];
	struct gcm_bnr_info_record_struct	*next;
} GCMBnrInfoRecordStruct;

void GCMFreeBnrInfoRecordStruct(GCMBnrInfoRecordStruct *r);

int GCMBnrInfoRecordCount(GCMBnrInfoRecordStruct *r);

GCMBnrInfoRecordStruct *GCMBnrGetNthInfoRecord(GCMBnrInfoRecordStruct *r, int n);

#endif	/* GCM_BNR_INFO_RECORD_H_ */

