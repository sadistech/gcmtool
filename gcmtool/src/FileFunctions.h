/*
 *  FileFunctions.h
 *  gcmtool
 *
 *  Created by spike on Fri Jun 25 2004.
 *  Copyright (c) 2004 Sadistech. All rights reserved.
 *
 */

#ifndef _FILE_FUNCTIONS_H_
#define _FILE_FUNCTIONS_H_

#include <stdio.h>
#include "types.h"

#define FF_BUFFER_SIZE  (8 * 1024 * 1024)   /* 8MB */

u32 CopyData(FILE *source, FILE *dest, u32 length);

void WriteDataToFile(char *data, u32 length, char *path);
u32 ReadDataFromFile(char *buf, char *path);

u32 GetFilesizeFromPath(char *path);
u32 GetFilesizeFromStream(FILE *stream);

#endif /* _FILE_FUNCTIONS_H_ */
