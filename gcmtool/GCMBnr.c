#include "GCMBnr.h"
#include <string.h>
#include <stdlib.h>

GCMBnrStruct *GCMRawBnrToStruct(char *raw) {
	/*
	**  take a raw bnr (ie- from an opening.bnr file) and return a struct object...
	**  currently, this code only supports version 1 (US/Japan) BNRs, although it will
	**  correctly load version 2 BNRs, the information will be incomplete.
	*/
	
	if (!raw) return NULL;
	
	//check the magic word... (BNR)
	if (strncmp(raw, GCM_BNR_MAGIC_WORD_PREFIX, strlen(GCM_BNR_MAGIC_WORD_PREFIX)) != 0) return NULL;
	raw += 3;//strlen(GCM_BNR_MAGIC_WORD_PREFIX); //skip over the magic word...
	
	GCMBnrStruct *b = (GCMBnrStruct*)malloc(sizeof(GCMBnrStruct));
	
	b->version = raw[0];
	*raw++;
	
	raw += GCM_BNR_GRAPHIC_DATA_OFFSET - GCM_BNR_MAGIC_WORD_LENGTH;
	
	bzero(b->graphic, GCM_BNR_GRAPHIC_DATA_LENGTH);
	memcpy(b->graphic, raw, GCM_BNR_GRAPHIC_DATA_LENGTH);
	raw += GCM_BNR_GRAPHIC_DATA_LENGTH;
	
	bzero(b->name, GCM_BNR_GAME_NAME_LENGTH);
	memcpy(b->name, raw, GCM_BNR_GAME_NAME_LENGTH);
	raw += GCM_BNR_GAME_NAME_LENGTH;
	
	bzero(b->developer, GCM_BNR_DEVELOPER_LENGTH);
	memcpy(b->developer, raw, GCM_BNR_DEVELOPER_LENGTH);
	raw += GCM_BNR_DEVELOPER_LENGTH;
	
	bzero(b->fullName, GCM_BNR_FULL_TITLE_LENGTH);
	memcpy(b->fullName, raw, GCM_BNR_FULL_TITLE_LENGTH);
	raw += GCM_BNR_FULL_TITLE_LENGTH;
	
	bzero(b->fullDeveloper, GCM_BNR_FULL_DEVELOPER_LENGTH);
	memcpy(b->fullDeveloper, raw, GCM_BNR_FULL_DEVELOPER_LENGTH);
	raw += GCM_BNR_FULL_DEVELOPER_LENGTH;
	
	bzero(b->description, GCM_BNR_DESCRIPTION_LENGTH);
	memcpy(b->description, raw, GCM_BNR_DESCRIPTION_LENGTH);
	raw += GCM_BNR_DESCRIPTION_LENGTH;
	
	return b;
}

void GCMBnrStructToRaw(GCMBnrStruct *b) {
	// to be written...
}


GCMRgbColor *GCMRGB5A1toColor(u16 s) {
	/*
	**  convert from RGB5A1 to GCMColor
	*/
	
	int i = 0;
	
	uchar r = 0;
	uchar g = 0;
	uchar b = 0;
	uchar a = 0;
	
	for (i = 0; i < 5; i++) {
		if (s % 2) {
			b++;
		}
		
		b = b << 1;
		s = s >> 1;
	}
	
	for (i = 0; i < 5; i++) {
		if (s % 2) {
			g++;
		}
		
		g = g << 1;
		s = s >> 1;
	}
	
	for (i = 0; i < 5; i++) {
		if (s % 2) {
			r++;
		}
		
		r = r << 1;
		s = s >> 1;
	}
	
	if (s % 2) {
		a++;
	}
	
	//I've gotta fix this function so you don't have to reverse the bits again...
	
	uchar r2 = 0;
	uchar g2 = 0;
	uchar b2 = 0;
	
	for (i = 0; i < 5; i++) {
		if (r % 2) {
			r2++;
		}
		r = r >> 1;
		r2 = r2 << 1;
	}
	for (i = 0; i < 5; i++) {
		if (g % 2) {
			g2++;
		}
		g = g >> 1;
		g2 = g2 << 1;
	}
	for (i = 0; i < 5; i++) {
		if (b % 2) {
			b2++;
		}
		b = b >> 1;
		b2 = b2 << 1;
	}
	
	r = ((255 / 31) * r2);
	g = ((255 / 31) * g2);
	b = ((255 / 31) * b2);
	
	GCMRgbColor *c = (GCMRgbColor*)malloc(sizeof(GCMRgbColor));
	
	c->red = r;
	c->green = g;
	c->blue = b;
	c->alpha = a;
	
	return c;
}

void GCMBnrGetImage(GCMBnrStruct *b, char *buf) {
	/*
	**  buf should be allocated with 3 * WIDTH * HEIGHT
	*/
	
	if (!b || !buf) return;
	
	u16 *curPixel = (u16*)(b->graphic);
	
	int i = 0;
	for (i = 0; i < (GCM_BNR_GRAPHIC_WIDTH * GCM_BNR_GRAPHIC_HEIGHT); i++) {
		int j = ((i / 4) * (4 * 4) + (i % 4)) % (96 * 4);
		j = j + (i / 96 % 4 * 4) + (i / 384 % 8 * 384);
		
		GCMRgbColor *c = GCMRGB5A1toColor(curPixel[j]);
		
		//printf("%d, %d, %d\n", c->red, c->green, c->blue);
		
		
		buf[i * 3] = c->red;
		buf[i * 3 + 1] = c->green;
		buf[i * 3 + 2] = c->blue;

		free(c);
	}

}
