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
	**  returns a copy of the nth path component
	**  nthPathComponent("this/path/is/cool", 2) returns "is"
	*/
	
	if (!source || !buf) return NULL;
	
	int i = 0;
	char *pathItem = (char*)malloc(strlen(source));
	char *os1 = pathItem;
	char *s = source;
	
	for (i = 0; (i < n) && (s = index(s, kPathSeparator)); i++, *s++);

	//printf("- %s\n", s);

	while (*s != kPathSeparator && (*pathItem++ = *s++));

	*pathItem++ = 0;
	
	strcpy(buf, os1);
	
	//printf("%d of %s = %s (%s)\n", n, source, buf, os1);
	free(os1);

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
