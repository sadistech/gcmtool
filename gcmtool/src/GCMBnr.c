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

	raw += GCM_BNR_HEADER_PADDING; //skip the padding...
//	raw += GCM_BNR_GRAPHIC_DATA_OFFSET - GCM_BNR_MAGIC_WORD_LENGTH; //skip the padding
	
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

void GCMBnrStructToRaw(GCMBnrStruct *b, char *buf) {
	/*
	**  Copies *b into *buf as raw data
	**  meant for copying changes made to a GCMBnrStruct back into a .bnr file
	**
	**  the allocated size of *buf should be malloc'd to GCM_BNR_LENGTH_V1
	**  since version_2 BNRs are still a slight mystery (I've only got one of them), don't even try to play with those...
	*/
	
	if (!b) return;
	
	char *start = buf;
	
	bzero(buf, GCM_BNR_LENGTH_V1); //zero that out...
	
	memcpy(buf, GCM_BNR_MAGIC_WORD_PREFIX, GCM_BNR_MAGIC_WORD_PREFIX_LENGTH);
	buf += GCM_BNR_MAGIC_WORD_PREFIX_LENGTH;
	
	char versionStr[2];
	sprintf(versionStr, "%d", b->version);
	memcpy(buf, versionStr, GCM_BNR_MAGIC_WORD_SUFFIX_LENGTH);
	buf += GCM_BNR_MAGIC_WORD_SUFFIX_LENGTH;
	
	buf += GCM_BNR_HEADER_PADDING;
	
	memcpy(buf, b->graphic, GCM_BNR_GRAPHIC_DATA_LENGTH);
	buf += GCM_BNR_GRAPHIC_DATA_LENGTH;
	
	memcpy(buf, b->name, GCM_BNR_GAME_NAME_LENGTH);
	buf += GCM_BNR_GAME_NAME_LENGTH;
	
	memcpy(buf, b->developer, GCM_BNR_DEVELOPER_LENGTH);
	buf += GCM_BNR_DEVELOPER_LENGTH;
	
	memcpy(buf, b->fullName, GCM_BNR_FULL_TITLE_LENGTH);
	buf += GCM_BNR_FULL_TITLE_LENGTH;
	
	memcpy(buf, b->fullDeveloper, GCM_BNR_FULL_DEVELOPER_LENGTH);
	buf += GCM_BNR_FULL_DEVELOPER_LENGTH;
	
	memcpy(buf, b->description, GCM_BNR_DESCRIPTION_LENGTH);
	
	buf = start;
}


GCMRgbColor *GCMRGB5A1toColor(u16 s) {
	/*
	**  convert from RGB5A1 to GCMRgbColor
	**  it's a bit of a hack, right now... I've gotta clean it up at some point
	**  basically, RGB5A1 is 5-bit color with 1-bit alpha... (16 bits or 2 bytes per pixel)
	**  arranged like: ARRRRRGG GGGBBBBB
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
	
	r = ((255.0 / 31.0) * r2);
	g = ((255.0 / 31.0) * g2);
	b = ((255.0 / 31.0) * b2);
	
	GCMRgbColor *c = (GCMRgbColor*)malloc(sizeof(GCMRgbColor));
	
	c->red = r;
	c->green = g;
	c->blue = b;
	c->alpha = a;
	
	return c;
}

void GCMBnrGetImageRaw(GCMBnrStruct *b, char *buf) {
	/*
	**  sets buf to raw RGB image data. 96 pixels wide by 32 pixels tall
	**  since the BNR stores the graphic data in 4x4 pixel tiles, we've gotta re-order them...
	**  buf should be allocated with (3 * WIDTH * HEIGHT)
	*/
	
	if (!b || !buf) return;
	
	u16 *curPixel = (u16*)(b->graphic);
	
	int i = 0;
	for (i = 0; i < (GCM_BNR_GRAPHIC_WIDTH * GCM_BNR_GRAPHIC_HEIGHT); i++) {
		// we've gotta rearrange the order of the pixels for the raw image
		int j = ((i / 4) * (4 * 4) + (i % 4)) % (96 * 4);
		j = j + (i / 96 % 4 * 4) + (i / 384 % 8 * 384);
		
		//convert the pixel into an RgbColor
		GCMRgbColor *c = GCMRGB5A1toColor(curPixel[j]);
		
		//put the data into buf
		buf[i * 3] = c->red;
		buf[i * 3 + 1] = c->green;
		buf[i * 3 + 2] = c->blue;

		//free the color...
		free(c);
	}

}

void GCMBnrGetImagePPM(GCMBnrStruct *b, char *buf) {
	/*
	**  for working with various programs like imagemagic and stuff...
	**  you should allocate enough memory for the Raw image plus an additional 256 bytes (for the header), minimum.
	**
	**  At some point, I'd like to #define some of these magic values to allow for more formats...
	*/

	char header[256];
	
	sprintf(header, "%s\n%d %d\n255\n", "P6", GCM_BNR_GRAPHIC_WIDTH, GCM_BNR_GRAPHIC_HEIGHT);
	
	strcpy(buf, header);
	buf += strlen(header);
	GCMBnrGetImageRaw(b, buf);
}
