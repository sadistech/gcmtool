/*
**	GCMBnrToolMain.c
**	Written by spike <spike@sadistech.com>
**
**	Comments to be added soon enough... blah. ;)
*/

//autoheader stuff
#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#define VERSION ""
#endif 

#include <stdio.h>
#include "GCMBnr.h"

//commandline params...
#define ARG_SET_ICON				"-i"
#define ARG_SET_ICON_SYN			"--icon"
#define ARG_SET_ICON_OPT			"[ " OPT_FORMAT_RAW " | " OPT_FORMAT_PPM " ] <pathname>"
#define ARG_SET_ICON_HELP			"Grabs the icon from <pathname>. " OPT_FORMAT_RAW " is the default."

#define ARG_SET_NAME				"-n"
#define ARG_SET_NAME_SYN			"--name"
#define ARG_SET_NAME_OPT			"<name>"
#define ARG_SET_NAME_HELP			"Sets the name field to <name>"

#define ARG_SET_DEVELOPER			"-d"
#define ARG_SET_DEVELOPER_SYN		"--developer"
#define ARG_SET_DEVELOPER_OPT		"<developer>"
#define ARG_SET_DEVELOPER_HELP		"Sets the developer field to <developer>"

#define ARG_SET_FULL_NAME			"-fn"
#define ARG_SET_FULL_NAME_SYN		"--full-name"
#define ARG_SET_FULL_NAME_OPT		"<full_name>"
#define ARG_SET_FULL_NAME_HELP		"Sets the full name field to <full_name>"

#define ARG_SET_FULL_DEVELOPER		"-fd"
#define ARG_SET_FULL_DEVELOPER_SYN  "--full-deveoper"
#define ARG_SET_FULL_DEVELOPER_OPT  "<full_developer>"
#define ARG_SET_FULL_DEVELOPER_HELP "Sets the full developer field to <full_developer>"

#define ARG_SET_DESCRIPTION			"-d"
#define ARG_SET_DESCRIPTION_SYN		"--description"
#define ARG_SET_DESCRIPTION_OPT		"<description>"
#define ARG_SET_DESCRIPTION_HELP	"Sets the description field to <full_description>"

//additional options...
#define OPT_FORMAT_RAW				"-raw"
#define OPT_FORMAT_PPM				"-ppm"

//macros... although they may be simple...
//these are for getting help and synonyms and stuff
#define ARG_SYN(ARG)		ARG ## _SYN
#define PRINT_HELP(ARG)		printf("\t" ARG "%s" ARG ## _SYN " " ARG ## _OPT "\n\t\t" ARG ## _HELP "\n\n", strcmp("", ARG ## _SYN) == 0 ? "" : ", ");

// these are for the argument parsing engine...
#define GET_NEXT_ARG		*(++argv)
#define SKIP_NARG(n)		*(argv += n)	
#define CHECK_ARG(ARG)		strcmp(ARG, currentArg) == 0 || strcmp(ARG ## _SYN, currentArg) == 0
#define PEEK_ARG			*(argv + 1)
#define PEEK_NARG(n)		*(argv + n)


void printUsage();

int main(int argc, char **argv) {
	if (argc == 1) {
		printUsage();
		exit(1);
	}

	FILE *ifile = NULL;

	char *filename = argv[1];

	if (!(ifile = fopen(filename, "r"))) {
		printf("oops!\n");
		exit(1);
	}

	fseek(ifile, 0, SEEK_END);
	unsigned long len = ftell(ifile);
	rewind(ifile);

	char *data = (char*)malloc(len);

	if (fread(data, 1, len, ifile) != len) {
		printf("error!\n");
		exit(1);
	}

	GCMBnrStruct *b = GCMRawBnrToStruct(data);
	fclose(ifile);

	printf("Version:	   \t%c\n", b->version);
	printf("Name:          \t%s\n", b->name);
	printf("Developer:     \t%s\n", b->developer);
	printf("Full Name:     \t%s\n", b->fullName);
	printf("Full Developer:\t%s\n", b->fullDeveloper);
	printf("Description:   \t%s\n", b->description);


	FILE *ofile = NULL;

	char outfilename[255];
	strcpy(outfilename, filename);
	strcat(outfilename, ".ppm");

	if (!(ofile = fopen(outfilename, "w"))) {
		printf("ERROR!\n");
		exit(1);
	}

	len = GCM_BNR_GRAPHIC_WIDTH * GCM_BNR_GRAPHIC_HEIGHT * 3 + 256;
	char *pic = (char*)malloc(len);
	
	GCMBnrGetImagePPM(b, pic);

	if (fwrite(pic, 1, len, ofile) != len) {
		printf("woops!\n");
		exit(1);
	}

	fclose(ofile);
}

void printUsage() {
	printf("gcmbnrtool %s- Utility for working with .bnr files from GameCube DVD Images (GCMs)\n", VERSION);
	printf("http://gcmtool.sourceforge.net\n\n");
	printf("Usage:\t");
	printf("gcmbnrtool <bnr_file>\n\n");
}