#ifndef _GCM_BNR_H_
#define _GCM_BNR_H_

#include <stdio.h>
#include "types.h"
#include "GCMBnrInfo.h"

#ifdef __cplusplus
extern "C" {
#endif

//file offsets and lengths...
#define GCM_BNR_MAGIC_WORD_PREFIX			"BNR"
#define GCM_BNR_MAGIC_WORD_SUFFIX_V1		"1"
#define GCM_BNR_MAGIC_WORD_SUFFIX_MULTI		"2"
#define GCM_BNR_MAGIC_WORD_PREFIX_LENGTH	0x0003
#define GCM_BNR_MAGIC_WORD_SUFFIX_LENGTH	0x0001
#define GCM_BNR_MAGIC_WORD_OFFSET			0x0000
#define GCM_BNR_MAGIC_WORD_SUFFIX_OFFSET	0x0003
#define GCM_BNR_MAGIC_WORD_LENGTH			0x0004
#define GCM_BNR_HEADER_PADDING				0x001c
#define GCM_BNR_GRAPHIC_DATA_OFFSET			0x0020
#define GCM_BNR_GRAPHIC_DATA_LENGTH			0x1800

#define GCM_BNR_LENGTH_V1					0x1960		/* file size of a version_1 BNR */
#define GCM_BNR_LENGTH_V2					asdfasdf	/* still working on this... this will error out */

//graphic constants...
#define GCM_BNR_GRAPHIC_WIDTH				96
#define GCM_BNR_GRAPHIC_HEIGHT				32
#define GCM_BNR_GRAPHIC_TILE_WIDTH			4
#define GCM_BNR_GRAPHIC_TILE_HEIGHT			4
#define GCM_BNR_GRAPHIC_RAW_FILE_LENGTH		3 * GCM_BNR_GRAPHIC_WIDTH * GCM_BNR_GRAPHIC_HEIGHT
#define GCM_BNR_PPM_HEADER_LENGTH			13
#define GCM_BNR_GRAPHIC_PPM_FILE_LENGTH		GCM_BNR_GRAPHIC_RAW_FILE_LENGTH + GCM_BNR_PPM_HEADER_LENGTH

#define GCM_BNR_ICON_COLOR_STEP				(256.0 / 32.0)  /* 8 */

typedef struct gcm_bnr_struct {
	char		version;										// the BNR version (1 or 2)
	char		graphic[GCM_BNR_GRAPHIC_DATA_LENGTH];			// graphical data
//	char		name[GCM_BNR_GAME_NAME_LENGTH];					// game's name
//	char		developer[GCM_BNR_DEVELOPER_LENGTH];			// company/developer
//	char		fullName[GCM_BNR_FULL_TITLE_LENGTH];			// full game title
//	char		fullDeveloper[GCM_BNR_FULL_DEVELOPER_LENGTH];   // full company/developer name
//	char		description[GCM_BNR_DESCRIPTION_LENGTH];		// game description
	GCMBnrInfoStruct *info;
} GCMBnrStruct;

//struct that stores an RGB value (0-255 for RGB and 0 or 1 for alpha)
typedef struct gcm_rgb_color {
	uchar		alpha; //on or off.......
	uchar		red;
	uchar		green;
	uchar		blue;
} GCMRgbColor;

//converting between raw Bnr and the struct...
GCMBnrStruct *GCMRawBnrToStruct(char *raw, int dataLen);
void GCMBnrStructToRaw(GCMBnrStruct *b, char *buf);

u32 GCMBnrRawSize(GCMBnrStruct *b); //returns the size you need to malloc to write this...

//GCMBnrAppendInfoRecord(GCMBnrStruct *b, GCMBnrInfoRecordStruct *r);

//color conversion
GCMRgbColor *GCMRGB5A1toColor(u16 s);
u16 GCMColorToRGB5A1(GCMRgbColor *c);

//working with the icon
void GCMBnrGetImageRaw(GCMBnrStruct *b, char *buf);
void GCMBnrGetImagePPM(GCMBnrStruct *b, char *buf);
void GCMBnrRawImageToGraphic(char *raw, char *buf);

#ifdef __cplusplus
};
#endif

#endif /* _GCM_BNR_H_ */

