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
#include <stdlib.h>
#include <string.h>
#include "GCMBnr.h"
#include "GCMBnrInfo.h"
#include "GCMCommandline.h" /* for commandline macros */
#include "FileFunctions.h"  /* for reading, writing, and getting the filesize of files */

//commandline params...
#define ARG_SET_ICON				"-i"
#define ARG_SET_ICON_SYN			"--icon"
#define ARG_SET_ICON_OPT			"[ " OPT_FORMAT_RAW " | " OPT_FORMAT_PPM " ] <pathname>"
#define ARG_SET_ICON_HELP			"Sets the icon from <pathname> and injects it into the bnr. " OPT_FORMAT_RAW " is the default"

#define ARG_GET_ICON				"-e"
#define ARG_GET_ICON_SYN			"--extract-icon"
#define ARG_GET_ICON_OPT			"[ " OPT_FORMAT_RAW " | " OPT_FORMAT_PPM " ] <pathname>"
#define ARG_GET_ICON_HELP			"Extracts the icon from the bnr in the desired format. " OPT_FORMAT_RAW " is the default"

#define ARG_SET_NAME				"-n"
#define ARG_SET_NAME_SYN			"--name"
#define ARG_SET_NAME_OPT			"<name>"
#define ARG_SET_NAME_HELP			"Sets the name field to <name>"

#define ARG_SET_DEVELOPER			"-d"
#define ARG_SET_DEVELOPER_SYN		"--developer"
#define ARG_SET_DEVELOPER_OPT		"<developer>"
#define ARG_SET_DEVELOPER_HELP		"Sets the developer field to <developer>"

#define ARG_SET_FULL_NAME			"-N"
#define ARG_SET_FULL_NAME_SYN		"--full-name"
#define ARG_SET_FULL_NAME_OPT		"<full_name>"
#define ARG_SET_FULL_NAME_HELP		"Sets the full name field to <full_name>"

#define ARG_SET_FULL_DEVELOPER		"-D"
#define ARG_SET_FULL_DEVELOPER_SYN  "--full-developer"
#define ARG_SET_FULL_DEVELOPER_OPT  "<full_developer>"
#define ARG_SET_FULL_DEVELOPER_HELP "Sets the full developer field to <full_developer>"

#define ARG_SET_DESCRIPTION			"-s"
#define ARG_SET_DESCRIPTION_SYN		"--description"
#define ARG_SET_DESCRIPTION_OPT		"<full_description>"
#define ARG_SET_DESCRIPTION_HELP	"Sets the description field to <full_description>"

#define ARG_OUTPUT_FILE				"-f"
#define ARG_OUTPUT_FILE_SYN			"--file"
#define ARG_OUTPUT_FILE_OPT			"<path>"
#define ARG_OUTPUT_FILE_HELP		"Write out to a file at <path> instead of the file we read from."

#define ARG_NEW						"-c"
#define ARG_NEW_SYN					"--create"
#define ARG_NEW_OPT					"<name> <developer> <full_name> <full_developer> <full_description>"
#define ARG_NEW_HELP				"Create a new BNR file with those fields"

#define ARG_MOD_NTH					"-m"
#define ARG_MOD_NTH_SYN				"--modify-nth"
#define ARG_MOD_NTH_OPT				"<index>"
#define ARG_MOD_NTH_HELP			"Modify the nth record (only for multi-lingual BNRs)"

//additional options...
#define OPT_FORMAT_RAW				"-raw"
#define OPT_FORMAT_PPM				"-ppm"

#define RAW_FORMAT					0
#define PPM_FORMAT					1


void printUsage();
void printExtendedUsage();

void openBnr();
void closeBnr();

void printBnr(GCMBnrStruct *b);
void printBnrInfo(GCMBnrInfoStruct *r);

FILE *bnrFile;
char *filename;

int main(int argc, char **argv) {

	char *newName = NULL;
	char *newDeveloper = NULL;
	char *newFullName = NULL;
	char *newFullDeveloper = NULL;
	char *newDescription = NULL;
	
	char *extractIconPath = NULL;
	char *injectIconPath = NULL;

	int modIndex = 0;
	
	int extractFormat = RAW_FORMAT;
	int injectFormat = RAW_FORMAT;

	char *currentArg = NULL;
	do {
		currentArg = GET_NEXT_ARG;
		if (!currentArg) {
			//eek, there's no arg! must be an argument error... print usage...
			
			printUsage();
			exit(1);
		} else if (CHECK_ARG(ARG_HELP)) {
			//they want the extended usage, so print it and bail...
			
			printExtendedUsage();
			exit(1);
		} else if (CHECK_ARG(ARG_MOD_NTH)) {
			//they only want the nth record to be printed...
		
			modIndex = atoi(GET_NEXT_ARG);
			
			if (modIndex < 1) {
				printf(" index is out of bounds! (%d)\n", modIndex);
				exit(1);
			}
		} else if (CHECK_ARG(ARG_SET_NAME)) {
			//they want to set the name...
			if (PEEK_ARG) {
				//if there is a next argument...
				newName = GET_NEXT_ARG;
			}
		} else if (CHECK_ARG(ARG_SET_DEVELOPER)) {
			//they want to set the developer...
			if (PEEK_ARG) {
				//if there's a next argument
				newDeveloper = GET_NEXT_ARG;
			}
		} else if (CHECK_ARG(ARG_SET_FULL_NAME)) {
			//the want to set the full name
			if (PEEK_ARG) {
				//if there's a next argument
				newFullName = GET_NEXT_ARG;
			}
		} else if (CHECK_ARG(ARG_SET_FULL_DEVELOPER)) {
			//they want to set the full developer
			if (PEEK_ARG) {
				//if there's a next argument
				newFullDeveloper = GET_NEXT_ARG;
			}
		} else if (CHECK_ARG(ARG_SET_DESCRIPTION)) {
			//they want to set the description...
			if (PEEK_ARG) {
				//if there's a next argument
				newDescription = GET_NEXT_ARG;
			}
		} else if (CHECK_ARG(ARG_GET_ICON)) {
			//they want to extract the icon
			if (PEEK_ARG) {
				//make sure there's at least one more argument
				if (strcmp(PEEK_ARG, OPT_FORMAT_RAW) == 0) {
					extractFormat = RAW_FORMAT;
					SKIP_NARG(1);
				} else if (strcmp(PEEK_ARG, OPT_FORMAT_PPM) == 0) {
					extractFormat = PPM_FORMAT;
					SKIP_NARG(1);
				}
				
				if (PEEK_ARG) {
					extractIconPath = GET_NEXT_ARG;
				}
			}
		} else if (CHECK_ARG(ARG_SET_ICON)) {
			//they want to inject the icon...
			
			if (PEEK_ARG) {
				//make sure there's at least one more argument...
				if (strcmp(PEEK_ARG, OPT_FORMAT_RAW) == 0) {
					injectFormat = RAW_FORMAT;
					SKIP_NARG(1);
				} else if (strcmp(PEEK_ARG, OPT_FORMAT_PPM) == 0) {
					injectFormat = PPM_FORMAT;
					SKIP_NARG(1);
				}
				
				if (PEEK_ARG) {
					injectIconPath = GET_NEXT_ARG;
				}
			}
		} else {
			//if the argument doesn't fit anything else... it must be the filename.
			// set the filename and stop looping... start processing!
			filename = currentArg;
			
			break;
		}
	}while (*argv);
	
	openBnr();

	//read the file into a buffer...
	unsigned long len = GetFilesizeFromStream(bnrFile);

	char *data = (char*)malloc(len);
	
	if (ReadDataFromFile(data, filename) != len) {
		perror(filename);
		exit(1);
	}

	GCMBnrStruct *b = GCMRawBnrToStruct(data, len);
	
	if (!b) {
		printf("error opening banner!\n");
		exit(1);
	}
	
	printBnr(b);
	
	int fileChanged = 0;
	
	GCMBnrInfoStruct *mod = NULL;
	if (b->version != '1') {
		mod = GCMBnrGetNthInfo(b->info, modIndex - 1);
	} else {
		mod = b->info;
	}
	
	if (!mod) {
		printf("ERROR! mod == NULL (this shouldn't happen!)\n");
		exit(1);
	}
	
	if (newName != NULL) {
		//let's set the name...
		bzero(mod->name, GCM_BNR_GAME_NAME_LENGTH);
		strcpy(mod->name, newName);
		fileChanged = 1;
	}
	
	if (newDeveloper != NULL) {
		//let's set the developer...
		bzero(mod->developer, GCM_BNR_DEVELOPER_LENGTH);
		strcpy(mod->developer, newDeveloper);
		fileChanged = 1;
	}
	
	if (newFullName != NULL) {
		bzero(mod->fullName, GCM_BNR_FULL_TITLE_LENGTH);
		strcpy(mod->fullName, newFullName);
		fileChanged = 1;
	}
	
	if (newFullDeveloper != NULL) {
		bzero(mod->fullDeveloper, GCM_BNR_FULL_DEVELOPER_LENGTH);
		strcpy(mod->fullDeveloper, newFullDeveloper);
		fileChanged = 1;
	}
	
	if (newDescription != NULL) {
		bzero(mod->description, GCM_BNR_DESCRIPTION_LENGTH);
		strcpy(mod->description, newDescription);
		fileChanged = 1;
	}
	
	if (extractIconPath != NULL) {
		char *imageData = NULL;
		u32 len;
		
		if (extractFormat == RAW_FORMAT) {
			//if we're extracting to a .raw file... (default)
			len = GCM_BNR_GRAPHIC_RAW_FILE_LENGTH;
			imageData = (char*)malloc(len);
			GCMBnrGetImageRaw(b, imageData);
		} else {
			//if we're extracting to a .ppm file...
			len = GCM_BNR_GRAPHIC_PPM_FILE_LENGTH;
			imageData = (char*)malloc(len);
			GCMBnrGetImagePPM(b, imageData);
		}
		
		WriteDataToFile(imageData, len, extractIconPath);
	}
	
	if (injectIconPath != NULL) {
		//printf("going to inject...\n");
		u32 len = GetFilesizeFromPath(injectIconPath);
		char *imageData = (char*)malloc(len);
		
		if (injectFormat == RAW_FORMAT) {
			//if the file we're injecting is in raw format (default)
			if (ReadDataFromFile(imageData, injectIconPath) != len) {
				perror(injectIconPath);
				exit(1);
			}
		} else {
			//if the file we're injecting is in ppm format...
			//this isn't implemented yet... so tell the user and bail
			printf("PPM IMPORT IS NOT IMPLEMENTED! (file not affected)\n\n");
			exit(1);
		}
		
		char *iconData = (char*)malloc(GCM_BNR_GRAPHIC_DATA_LENGTH);
		GCMBnrRawImageToGraphic(imageData, iconData);
		
		memcpy(b->graphic, iconData, GCM_BNR_GRAPHIC_DATA_LENGTH);
		
		free(iconData);
		fileChanged = 1;
	}
	
	if (fileChanged) {
		//printf("Writing bnr file...\n");
		rewind(bnrFile);
		u32 bnrSize = GCMBnrRawSize(b);
		char *buf = (char*)malloc(bnrSize);
		GCMBnrStructToRaw(b, buf);
		if (fwrite(buf, 1, bnrSize, bnrFile) != bnrSize) {
			perror(filename);
			//printf("error writing to bnr! (%s)\n", filename);
			exit(1);
		}
	}

	GCMFreeBnrInfoStruct(b->info);
	closeBnr();
	
	return 0;
}

void printBnr(GCMBnrStruct *b) {
	//display bnr...
	printf("Version:        %c\n", b->version);
	
	if (GCMBnrInfoCount(b->info) > 1) {
		printf(" Count:   %d\n", GCMBnrInfoCount(b->info));
		printf("\n");
	}
	
	GCMBnrInfoStruct *info = b->info;
	do {
		printBnrInfo(info);
		printf("\n");
	} while (info = info->next);
}

void printBnrInfo(GCMBnrInfoStruct *r) {
	printf("Name:           %s\n", r->name);
	printf("Developer:      %s\n", r->developer);
	printf("Full Name:      %s\n", r->fullName);
	printf("Full Developer: %s\n", r->fullDeveloper);
	printf("Description:    %s\n", r->description);
}

void openBnr() {
	if (!(bnrFile = fopen(filename, "r+"))) {
		printf("Error opening %s\n", filename);
		exit(1);
	}
}

void closeBnr() {
	fclose(bnrFile);
}

void printUsage() {
	printf("gcmbnrtool %s- Utility for working with .bnr files from GameCube DVD Images (GCMs)\n", VERSION);
	printf("http://gcmtool.sourceforge.net\n\n");
	printf("Usage:\t");
	printf("gcmbnrtool [options] <bnr_file>\n\n");
	printf("Use -? to view extended usage.\n\n");
}

void printExtendedUsage() {
	printUsage();
	
	PRINT_HELP(ARG_HELP);
	PRINT_HELP(ARG_SET_ICON);
	PRINT_HELP(ARG_GET_ICON);
	PRINT_HELP(ARG_MOD_NTH);
	printf("When editing BNR2s, you must use %s to declare which field you'd like to edit.\n", ARG_MOD_NTH);
	PRINT_HELP(ARG_SET_NAME);
	PRINT_HELP(ARG_SET_DEVELOPER);
	PRINT_HELP(ARG_SET_FULL_NAME);
	PRINT_HELP(ARG_SET_FULL_DEVELOPER);
	PRINT_HELP(ARG_SET_DESCRIPTION);
}
