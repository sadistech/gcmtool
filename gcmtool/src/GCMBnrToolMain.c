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
#include "GCMCommandline.h" /* for commandline macros */

//commandline params...
#define ARG_HELP					"-?"
#define ARG_HELP_SYN				"--help"
#define ARG_HELP_OPT				""
#define ARG_HELP_HELP				"Displays this help."

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

#define ARG_SET_FULL_NAME			"-fn"
#define ARG_SET_FULL_NAME_SYN		"--full-name"
#define ARG_SET_FULL_NAME_OPT		"<full_name>"
#define ARG_SET_FULL_NAME_HELP		"Sets the full name field to <full_name>"

#define ARG_SET_FULL_DEVELOPER		"-fd"
#define ARG_SET_FULL_DEVELOPER_SYN  "--full-developer"
#define ARG_SET_FULL_DEVELOPER_OPT  "<full_developer>"
#define ARG_SET_FULL_DEVELOPER_HELP "Sets the full developer field to <full_developer>"

#define ARG_SET_DESCRIPTION			"-d"
#define ARG_SET_DESCRIPTION_SYN		"--description"
#define ARG_SET_DESCRIPTION_OPT		"<description>"
#define ARG_SET_DESCRIPTION_HELP	"Sets the description field to <full_description>"

//additional options...
#define OPT_FORMAT_RAW				"-raw"
#define OPT_FORMAT_PPM				"-ppm"

#define RAW_FORMAT					0
#define PPM_FORMAT					1


void printUsage();
void printExtendedUsage();

void openBnr();
void closeBnr();

void writeToFile(char *data, u32 length, char *path);
u32 readFromFile(char *buf, char *path);
u32 getFilesize(char *path);

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
	fseek(bnrFile, 0, SEEK_END);
	unsigned long len = ftell(bnrFile);
	rewind(bnrFile);

	char *data = (char*)malloc(len);
	
	if (fread(data, 1, len, bnrFile) != len) {
		printf("Error reading from file... (%s)\n", filename);
		exit(1);
	}

	GCMBnrStruct *b = GCMRawBnrToStruct(data);

	//display bnr...
	printf("Version:       \t%c\n", b->version);
	printf("Name:          \t%s\n", b->name);
	printf("Developer:     \t%s\n", b->developer);
	printf("Full Name:     \t%s\n", b->fullName);
	printf("Full Developer:\t%s\n", b->fullDeveloper);
	printf("Description:   \t%s\n", b->description);

	int fileChanged = 0;

	if (newName != NULL) {
		//let's set the name...
		bzero(b->name, GCM_BNR_GAME_NAME_LENGTH);
		strcpy(b->name, newName);
		fileChanged = 1;
	}
	
	if (newDeveloper != NULL) {
		//let's set the developer...
		bzero(b->developer, GCM_BNR_DEVELOPER_LENGTH);
		strcpy(b->developer, newDeveloper);
		fileChanged = 1;
	}
	
	if (newFullName != NULL) {
		bzero(b->fullName, GCM_BNR_FULL_TITLE_LENGTH);
		strcpy(b->fullName, newFullName);
		fileChanged = 1;
	}
	
	if (newFullDeveloper != NULL) {
		bzero(b->fullDeveloper, GCM_BNR_FULL_DEVELOPER_LENGTH);
		strcpy(b->fullDeveloper, newFullDeveloper);
		fileChanged = 1;
	}
	
	if (newDescription != NULL) {
		bzero(b->description, GCM_BNR_DESCRIPTION_LENGTH);
		strcpy(b->description, newDescription);
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
		
		writeToFile(imageData, len, extractIconPath);
	}
	
	if (injectIconPath != NULL) {
		char *imageData = NULL;
		u32 len = getFilesize(injectIconPath);
		
		if (injectFormat == RAW_FORMAT) {
			//if the file we're injecting is in raw format (default)
			readFromFile(injectIconPath, imageData);
		} else {
			//if the file we're injecting is in ppm format...
			
		}
		
		char *iconData = (char*)malloc(GCM_BNR_GRAPHIC_DATA_LENGTH);
		GCMBnrRawImageToGraphic(imageData, iconData);
		
		memcpy(b->graphic, iconData, GCM_BNR_GRAPHIC_DATA_LENGTH);
		
		free(iconData);
		
	}
	
	if (fileChanged) {
		printf("Writing bnr file...\n");
		rewind(bnrFile);
		char *buf = (char*)malloc(GCM_BNR_LENGTH_V1);
		GCMBnrStructToRaw(b, buf);
		if (fwrite(buf, 1, GCM_BNR_LENGTH_V1, bnrFile) != GCM_BNR_LENGTH_V1) {
			printf("error writing to bnr! (%s)\n", filename);
			exit(1);
		}
	}
	
	closeBnr();
/*
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

	fclose(ofile);*/
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

void writeToFile(char *data, u32 length, char *path) {
	/*
	**  Takes data of length and writes it to a file path. Displays errors when they happen...
	*/
	
	//char msg[1024] = "Writing to file ";
	//strcat(msg, path);
	//verbosePrint(msg);
	
	if (!data || !length || !path) return;
	
	FILE *ofile = NULL;
	
	if (!(ofile = fopen(path, "w"))) {
		printf("An error occurred trying to open %s\n", path);
		return;
	}
	
	if (fwrite(data, 1, length, ofile) != length) {
		printf("An error occurred trying to write to %s\n", path);
	}
}

u32 readFromFile(char *buf, char *path) {
	/*
	**  reads from the file at path, 
	**  sets buf to the contents of said file...
	**  and returns the length of data.
	**
	**  use getFilesize() when you allocate buf
	**
	**  returns 0 and displays messages on error;
	*/
	
	if (!path || !buf) return 0;
	
	FILE *ifile = NULL;
	
	u32 length = getFilesize(path); //get how much to read...
	
	if (!length || !(ifile = fopen(path, "r"))) {
		printf("An error occurred trying to open %s\n", path);
		return 0;
	}
	
	if (fread(buf, 1, length, ifile) != length) {
		printf("An error occurred when trying to read %s\n", path);
		fclose(ifile);
		return 0;
	}
	
	fclose(ifile);
	return length;
}

u32 getFilesize(char *path) {
	/*
	**  returns the filesize of the file at *path
	**  useful for when you read a file... gotta make sure you allocate enough memory for the file...
	*/
	
	FILE *ifile = NULL;
	
	if (!(ifile = fopen(path, "r"))) {
		return 0;
	}
	
	fseek(ifile, 0, SEEK_END);
	u32 len = ftell(ifile);
	fclose(ifile);
	
	return len;
}

void printUsage() {
	printf("gcmbnrtool %s- Utility for working with .bnr files from GameCube DVD Images (GCMs)\n", VERSION);
	printf("http://gcmtool.sourceforge.net\n\n");
	printf("Usage:\t");
	printf("gcmbnrtool <bnr_file>\n\n");
	printf("Use -? to view extended usage.\n\n");
}

void printExtendedUsage() {
	printUsage();
	
	PRINT_HELP(ARG_HELP);
	PRINT_HELP(ARG_SET_ICON);
	PRINT_HELP(ARG_GET_ICON);
	PRINT_HELP(ARG_SET_NAME);
	PRINT_HELP(ARG_SET_DEVELOPER);
	PRINT_HELP(ARG_SET_FULL_NAME);
	PRINT_HELP(ARG_SET_FULL_DEVELOPER);
	PRINT_HELP(ARG_SET_DESCRIPTION);
}