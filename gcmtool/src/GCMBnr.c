#include "GCMBnr.h"
#include "GCMBnrInfo.h"
#include <string.h>
#include <stdlib.h>

static uchar GCMBnrReverseBits(uchar v, int bitCount);


GCMBnrStruct *GCMRawBnrToStruct(char *raw, int dataLen) {
	/*
	**  take a raw bnr (ie- from an opening.bnr file) and return a struct object...
	**  currently, this code only supports version 1 (US/Japan) BNRs, although it will
	**  correctly load version 2 BNRs, the information will be incomplete.
	*/
	
//	if (!raw) printf("wtf?! NULL BNR!\n");
	if (!raw) return NULL;

	char *start = raw;
	
	//check the magic word... (BNR)
	if (strncmp(raw, GCM_BNR_MAGIC_WORD_PREFIX, strlen(GCM_BNR_MAGIC_WORD_PREFIX)) != 0) return NULL;
	raw += 3;//strlen(GCM_BNR_MAGIC_WORD_PREFIX); //skip over the magic word...
	
	GCMBnrStruct *b = (GCMBnrStruct*)malloc(sizeof(GCMBnrStruct));
	
	b->version = raw[0];
	*raw++;

	raw += GCM_BNR_HEADER_PADDING; //skip the padding...
//	raw += GCM_BNR_GRAPHIC_DATA_OFFSET - GCM_BNR_MAGIC_WORD_LENGTH; //skip the padding
	
	//grab the graphic....
	bzero(b->graphic, GCM_BNR_GRAPHIC_DATA_LENGTH);
	memcpy(b->graphic, raw, GCM_BNR_GRAPHIC_DATA_LENGTH);
	raw += GCM_BNR_GRAPHIC_DATA_LENGTH;
	
	// grab the only info record (version 1) or...
	// loop until we get all of the info (for version 2)
	
	GCMBnrInfoStruct *recordHead = (GCMBnrInfoStruct*)malloc(sizeof(GCMBnrInfoStruct));
	GCMBnrInfoStruct *r = recordHead;
	
	int dataLeft = dataLen - (raw - start);
	
	while (dataLeft >= GCM_BNR_INFO_RECORD_LENGTH) {
		r->next = (GCMBnrInfoStruct*)malloc(sizeof(GCMBnrInfoStruct));
		r = r->next;
		
		bzero(r->name, GCM_BNR_GAME_NAME_LENGTH);
		memcpy(r->name, raw, GCM_BNR_GAME_NAME_LENGTH);
		raw += GCM_BNR_GAME_NAME_LENGTH;
	
		bzero(r->developer, GCM_BNR_DEVELOPER_LENGTH);
		memcpy(r->developer, raw, GCM_BNR_DEVELOPER_LENGTH);
		raw += GCM_BNR_DEVELOPER_LENGTH;
	
		bzero(r->fullName, GCM_BNR_FULL_TITLE_LENGTH);
		memcpy(r->fullName, raw, GCM_BNR_FULL_TITLE_LENGTH);
		raw += GCM_BNR_FULL_TITLE_LENGTH;
	
		bzero(r->fullDeveloper, GCM_BNR_FULL_DEVELOPER_LENGTH);
		memcpy(r->fullDeveloper, raw, GCM_BNR_FULL_DEVELOPER_LENGTH);
		raw += GCM_BNR_FULL_DEVELOPER_LENGTH;
	
		bzero(r->description, GCM_BNR_DESCRIPTION_LENGTH);
		memcpy(r->description, raw, GCM_BNR_DESCRIPTION_LENGTH);
		raw += GCM_BNR_DESCRIPTION_LENGTH;

		dataLeft = dataLen - (raw - start);
	}
	
	b->info = recordHead->next;
	free(recordHead);
	
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
	
	if (!b || !buf) return;
	
	char *start = buf;
	
	bzero(buf, GCM_BNR_LENGTH_V1); //zero that out...
	
	memcpy(buf, GCM_BNR_MAGIC_WORD_PREFIX, GCM_BNR_MAGIC_WORD_PREFIX_LENGTH);
	buf += GCM_BNR_MAGIC_WORD_PREFIX_LENGTH;
	
	char versionStr[2] = "";
	sprintf(versionStr, "%c", b->version);
	
	memcpy(buf, versionStr, GCM_BNR_MAGIC_WORD_SUFFIX_LENGTH);
	buf += GCM_BNR_MAGIC_WORD_SUFFIX_LENGTH;
	
	buf += GCM_BNR_HEADER_PADDING;
	
	memcpy(buf, b->graphic, GCM_BNR_GRAPHIC_DATA_LENGTH);
	buf += GCM_BNR_GRAPHIC_DATA_LENGTH;

	// basically what we need to do is loop and append data for all the fields
	// it's up to you, the programmer (or the user...) to make sure the version is 2 if there's more than one record...

	GCMBnrInfoStruct *head = b->info;
	GCMBnrInfoStruct *n = head;

	do {
		printf("writing: %s\n", n->name);
	
		memcpy(buf, n->name, GCM_BNR_GAME_NAME_LENGTH);
		buf += GCM_BNR_GAME_NAME_LENGTH;
		
		memcpy(buf, n->developer, GCM_BNR_DEVELOPER_LENGTH);
		buf += GCM_BNR_DEVELOPER_LENGTH;
		
		memcpy(buf, n->fullName, GCM_BNR_FULL_TITLE_LENGTH);
		buf += GCM_BNR_FULL_TITLE_LENGTH;
		
		memcpy(buf, n->fullDeveloper, GCM_BNR_FULL_DEVELOPER_LENGTH);
		buf += GCM_BNR_FULL_DEVELOPER_LENGTH;
		
		memcpy(buf, n->description, GCM_BNR_DESCRIPTION_LENGTH);
		buf += GCM_BNR_DESCRIPTION_LENGTH;
	} while (n = n->next);	

	buf = start;
}

u32 GCMBnrRawSize(GCMBnrStruct *b) {
	/*
	**  Returns the amount you need to malloc if you want to
	**  get the BnrStruct as raw data...
	*/
	
	return (GCM_BNR_MAGIC_WORD_LENGTH + GCM_BNR_HEADER_PADDING + GCM_BNR_GRAPHIC_DATA_LENGTH + (GCMBnrInfoCount(b->info) * GCM_BNR_INFO_RECORD_LENGTH));
}

static uchar GCMBnrReverseBits(uchar v, int bitCount) {
	/*
	**  functions SPECIFICALLY written for converting colors
	**  treats v as a bitCount-bit whole numbers... so we could [theoretically] use this for RGB565 conversion, too
	*/
	
	uchar t = 0;
	int i = 0;

//	printf("%d\n", v);

	for (i = bitCount; i; i--)
	{
		t <<= 1;
		if (v % 2) 
			t++;
		
		v >>= 1;
	}

//	t >>= 8 - bitCount - 1; //gotta shift it over, since this is for 5-bit integers
	return t;
}

GCMRgbColor *GCMRGB5A1toColor(u16 s) {
	/*
	**  convert from RGB5A1 to GCMRgbColor
	**  it's a bit of a hack, right now... I've gotta clean it up at some point
	**  basically, RGB5A1 is 5-bit color with 1-bit alpha... (16 bits or 2 bytes per pixel)
	**  arranged like: ARRRRRGG GGGBBBBB
	*/

	s = ntohs(s);
	
	int i = 0;
	
	uchar r = 0;
	uchar g = 0;
	uchar b = 0;
	uchar a = 0;
	
	for (i = 0; i < 5; i++) {
		b = b << 1;
		if (s % 2) {
			b++;
		}
		
		s = s >> 1;
	}
	//b <<= 1;
	b = GCMBnrReverseBits(b, 5);
	
	for (i = 0; i < 5; i++) {
		g = g << 1;
		if (s % 2) {
			g++;
		}
		
		s = s >> 1;
	}
	//g <<= 1;
	g = GCMBnrReverseBits(g, 5);
	
	for (i = 0; i < 5; i++) {
		r = r << 1;
		if (s % 2) {
			r++;
		}
		
		s = s >> 1;
	}
	//r <<= 1;
	r = GCMBnrReverseBits(r, 5);
	
	if (s % 2) {
		a++;
	}
	
	r = ((256.0 / 32.0) * r);
	g = ((256.0 / 32.0) * g);
	b = ((256.0 / 32.0) * b);
	
	GCMRgbColor *c = (GCMRgbColor*)malloc(sizeof(GCMRgbColor));
	
	c->red = r;
	c->green = g;
	c->blue = b;
	c->alpha = a;

	//printf("converted: %03d, %03d, %03d\n", c->red, c->green, c->blue);

	return c;
}

u16 GCMColorToRGB5A1(GCMRgbColor *c) {
	/*
	**  used for converting a GCMRgbColor back to a RGB5A1 short...
	*/
	
	//first we reverse the bits...
	uchar r = GCMBnrReverseBits((uchar)(c->red / (256.0 / 32.0)), 5);
	uchar g = GCMBnrReverseBits((uchar)(c->green / (256.0 / 32.0)), 5);
	uchar b = GCMBnrReverseBits((uchar)(c->blue / (256.0 / 32.0)), 5);
//	printf("rgb:\t%d %d %d\n", r, g, b);
	
	u16 s = 0;
	
	if (c->alpha) {
		s++;
		s <<= 1;
	}
	
	int i = 0;
	
	for (i = 0; i < 5; i++) {
		if (r % 2) {
			s++;
		}
		s <<= 1;
		r >>= 1;
	}
	
	for (i = 0; i < 5; i++) {
		if (g % 2) {
			s++;
		}
		s <<= 1;
		g >>= 1;
	}
	
	for (i = 0; i < 5; i++) {
		if (b % 2) {
			s++;
		}
		if (i != 4)
			s <<= 1;
		b >>= 1;
	}
	
	//printf("%d\n", b);
	
	return s;
}

void GCMBnrGetImageRaw(GCMBnrStruct *b, char *buf) {
	/*
	**  sets buf to raw RGB image data. 96 pixels wide by 32 pixels tall
	**  since the BNR stores the graphic data in 4x4 pixel tiles, we've gotta re-order them...
	**  buf should be allocated with (3 * WIDTH * HEIGHT) or GCM_BNR_RAW_FILE_LENGTH
	*/
	
	if (!b || !buf) return;
	
	u16 *curPixel = (u16*)(b->graphic);
	
	int i = 0;
	for (i = 0; i < (GCM_BNR_GRAPHIC_WIDTH * GCM_BNR_GRAPHIC_HEIGHT); i++) {
		// we've gotta rearrange the order of the pixels for the raw image
		int j = ((i / 4) * (4 * 4) + (i % 4)) % (96 * 4);
		j = j + (i / 96 % 4 * 4) + (i / 384 % 8 * 384);
		
		//convert the pixel into an RgbColor
		//printf("%d ", i);
		
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

void GCMBnrRawImageToGraphic(char *raw, char *buf) {
	/*
	**  takes *raw (which contains the pixels of an icon) and re-orders the pixels
	**  for the bnr icon format as well as converting the 24bit color back into 15bit, 1alpha...
	**
	**  NOTE: this completely ignores any alpha... beware.
	*/
	
	if (!raw || !buf) {
		return;
	}
	
	int i = 0;
	for (i = 0; i < (GCM_BNR_GRAPHIC_WIDTH * GCM_BNR_GRAPHIC_HEIGHT); i++) {
		// we've gotta rearrange the order of the pixels for the raw image
		int j = (i % 4) + (i / 16 % 24 * 4);
		j += (i / 4 % 4 * 96) + (i / 384 % 8 * 384);
		
		//convert the pixel into an RgbColor
		GCMRgbColor *c = (GCMRgbColor*)malloc(sizeof(GCMRgbColor));
		c->red = raw[j * 3];
		c->green = raw[j * 3 + 1];
		c->blue = raw[j * 3 + 2];
		
		u16 *p = (u16*)malloc(sizeof(u16));
		*p = GCMColorToRGB5A1(c);
		*p = htons(*p);
		
		//put the data into buf
		memcpy(buf, (char*)p, sizeof(u16));
		buf += sizeof(u16);
		
		//free the color...
		free(c);
		free(p);
	}
}

