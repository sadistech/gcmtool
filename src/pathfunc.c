/*
 *  pathfunc.c
 *  gcmtool
 *
 *  Created by spike on Fri May 21 2004.
 *  Copyright (c) 2004 Sadistech. All rights reserved.
 *
 */

#include "pathfunc.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *appendPathComponent(char *source, char *append) {
	register char *os1;

	os1 = source;
	while(*source++); //get to the end of source
	 
	source -= 2; //back up 2 spots

	//check if source already has a path separator on the end
	//if not, then put one there...
	if (*source != kPathSeparator) {
		source++;
		*source = kPathSeparator;
	}
	source++; //move on to \0 so we overwrite it (and not the path separator)

	while(*source++ = *append++); //do the appending

	return os1;
}

char *lastPathComponent(char *source) {
	/*
	**  returns a pointer to the start of the last path component...
	**  lastPathComponent("this/path/is/cool") returns a pointer to "cool"
	*/
	
	char *last = rindex(source, kPathSeparator);
	
	if (last && last != '\0') {
		*last++;
		return last;
	} else { 
		return source;
	}
}

char *nthPathComponent(char *source, int n, char *buf) {
	/*
	**	returns a pointer to buf
	**	sets buf to the nth path component of source.
	**	nthPathComponent("this/path/is/cool", 2) returns "is"
	*/
	
	if (!source || !buf) return NULL;
	
	int i = 0;
	char *pathItem = (char*)malloc(strlen(source) + 1);
	char *os1 = pathItem;
	char *s = source;
	
	for (i = 0; (i < n) && (s = index(s, kPathSeparator)); i++, *s++);

	//printf("- %s\n", s);

	//there was an error! (not enough path components!)
	if (!s) {
		printf("not enough path components!!!!!\n");
		free(pathItem);
		return NULL;
	}

	while (*s != kPathSeparator && (*os1++ = *s++));	
	
	*os1++ = 0;
	
	strcpy(buf, pathItem);
	
	//printf("%d of %s = %s (%s)\n", n, source, buf, os1);
	free(pathItem);

	return buf;
}

int pathComponentCount(char *source) {
	int count = 0;
	
	while(*source) {
		if (*source++ == kPathSeparator) {
			count++;
		}
	}
	
	return count;
}
