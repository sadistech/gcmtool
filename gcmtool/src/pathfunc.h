/*
 *  pathfunc.h
 *  various functions for working with paths (splitting and appending and whatnot)
 *
 *  Created by spike on Fri May 21 2004.
 *  Copyright (c) 2004 Sadistech. All rights reserved.
 *
 */

#ifndef _PATHFUNC_H_
#define _PATHFUNC_H_

#define kPathSeparator '/'

#ifdef __cplusplus
extern "C" {
#endif

char *appendPathComponent(char *source, char *append);
char *lastPathComponent(char *source);
char *nthPathComponent(char *source, int n, char *buf);
int pathComponentCount(char *source);

#ifdef __cplusplus
};
#endif

#endif  /* _PATHFUNC_H_ */

