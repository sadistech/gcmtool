/*
**  main.c
**  written by spike <spike666@mac.com>
**  www.sadistech.com
**  gcmtool.sourceforge.net
**  
**  part of the gcmtool project
**  commandline wrappers for some of the standard functions of GCMutils.h and GCMExtras.h
**
*/

//for the autoheader stuff... (grrr, I hate this thing...)
#if HAVE_CONFIG_H
#include <config.h>
#else
//if you're not configuring from the commandline...
#define VERSION ""
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>

#include <unistd.h> /* for mktemp() */

//for mkdir:
#include <sys/types.h>
#include <sys/stat.h>

#include "GCMutils.h"
#include "GCMextras.h"
#include "GCMCommandline.h"
#include "pathfunc.h"
#include "FileFunctions.h"

//commandline arguments
#define ARG_EXTRACT							"-e"
#define ARG_EXTRACT_SYN						""
#define ARG_EXTRACT_OPT						"<source> <dest_dir>"
#define ARG_EXTRACT_HELP					"Extracts the file from <source> (in the GCM) to <dest_dir> (local filesystem)"

#define ARG_VERBOSE							"-v"
#define ARG_VERBOSE_SYN						"--verbose"
#define ARG_VERBOSE_OPT						""
#define ARG_VERBOSE_HELP					"Verbose output"

#define ARG_INFO							"-i"
#define ARG_INFO_SYN						"--info"
#define ARG_INFO_OPT						""
#define ARG_INFO_HELP						"Display additional info about the GCM (useful for auditing)"

#define ARG_LIST							"-l"
#define ARG_LIST_SYN						"--list"
#define ARG_LIST_OPT						"[ " OPT_FILE_INFO " " OPT_FULL_PATH " ]"
#define ARG_LIST_HELP						"Lists the filesystem of the GCM. If " OPT_FILE_INFO " is included, it will show the filesizes or filecount (for directories). If " OPT_FULL_PATH " is included, it will print the full paths of each file (for easy grepping)."

#define ARG_HEX								"-h"
#define ARG_HEX_SYN							"--hex"
#define ARG_HEX_OPT							""
#define ARG_HEX_HELP						"Display file offsets in hexadecimal notation"

// commands:
// command_appreviation, command_synonym, command_helptext
// extracting sections...
#define ARG_EXTRACT_DISK_HEADER				"-edh"
#define ARG_EXTRACT_DISK_HEADER_SYN			"--extract-disk-header"
#define ARG_EXTRACT_DISK_HEADER_OPT			"[ " OPT_FILE " " OPT_FILE_OPT " ]"
#define ARG_EXTRACT_DISK_HEADER_HELP		"Extract the disk header (" GCM_DISK_HEADER_FILENAME ")"

#define ARG_EXTRACT_DISK_HEADER_INFO		"-edhi"
#define ARG_EXTRACT_DISK_HEADER_INFO_SYN	"--extract-disk-header-info"
#define ARG_EXTRACT_DISK_HEADER_INFO_OPT	"[ " OPT_FILE " " OPT_FILE_OPT " ]"
#define ARG_EXTRACT_DISK_HEADER_INFO_HELP   "Extract the disk header info (" GCM_DISK_HEADER_INFO_FILENAME ")"

#define ARG_EXTRACT_APPLOADER				"-eal"
#define ARG_EXTRACT_APPLOADER_SYN			"--extract-apploader"
#define ARG_EXTRACT_APPLOADER_OPT			"[ " OPT_FILE " " OPT_FILE_OPT " ]"
#define ARG_EXTRACT_APPLOADER_HELP			"Extract the apploader (" GCM_APPLOADER_FILENAME ")"

#define ARG_EXTRACT_BOOT_DOL				"-ed"
#define ARG_EXTRACT_BOOT_DOL_SYN			"--extract-boot-dol"
#define ARG_EXTRACT_BOOT_DOL_OPT			"[ " OPT_FILE " " OPT_FILE_OPT " ]"
#define ARG_EXTRACT_BOOT_DOL_HELP			"Extract the main executable DOL (" GCM_BOOT_DOL_FILENAME ")"

#define ARG_INJECT_DISK_HEADER				"-idh"
#define ARG_INJECT_DISK_HEADER_SYN			"--inject-disk-header"
#define ARG_INJECT_DISK_HEADER_OPT			"[ " OPT_FILE " " OPT_FILE_OPT " ]"
#define ARG_INJECT_DISK_HEADER_HELP			"Inject the disk header"

#define ARG_INJECT_DISK_HEADER_INFO			"-idhi"
#define ARG_INJECT_DISK_HEADER_INFO_SYN		"--inject-disk-header-info"
#define ARG_INJECT_DISK_HEADER_INFO_OPT		"[ " OPT_FILE " " OPT_FILE_OPT " ]"
#define ARG_INJECT_DISK_HEADER_INFO_HELP	"Inject the disk header info"

#define ARG_INJECT_APPLOADER				"-ial"
#define ARG_INJECT_APPLOADER_SYN			"--inject-apploader"
#define ARG_INJECT_APPLOADER_OPT			"[ " OPT_FILE " " OPT_FILE_OPT " ]"
#define ARG_INJECT_APPLOADER_HELP			"Inject the apploader"

#define ARG_INJECT_BOOT_DOL					"-id"
#define ARG_INJECT_BOOT_DOL_SYN				"--inject-boot-dol"
#define ARG_INJECT_BOOT_DOL_OPT				"[ " OPT_FILE " " OPT_FILE_OPT " ]"
#define ARG_INJECT_BOOT_DOL_HELP			"Inject the main executable DOL"

#define ARG_REPLACE_FILE					"-rf"
#define ARG_REPLACE_FILE_SYN				"--replace-file"
#define ARG_REPLACE_FILE_OPT				"<gcm_path> <local_path>"
#define ARG_REPLACE_FILE_HELP				"Replace a file at <gcm_path> with <local_path> (Files MUST be same size)"

#define ARG_REPLACE_FILESYSTEM				"-rfs"
#define ARG_REPLACE_FILESYSTEM_SYN			"--replace-file-system"
#define ARG_REPLACE_FILESYSTEM_OPT			"<fs_root>"
#define ARG_REPLACE_FILESYSTEM_HELP			"Replaces the filesystem from fs_root"

//commandline options (modifiers to the arguments... hehe)
#define OPT_FILE							"+f"
#define OPT_FILE_SYN						"--file"
#define OPT_FILE_OPT						"<filename>"

#define OPT_FULL_PATH						"+p"
#define OPT_FULL_PATH_SYN					"--full-path"

#define OPT_FILE_INFO						"+i"
#define OPT_FILE_INFO_SYN					"--full-info"

#pragma mark -

//some utility functions...
void printEntry(GCMFileEntryStruct *e);
//void printDirectory(GCMFileEntryStruct *e);

void recurseFileEntry(GCMFileEntryStruct *e, void (*func)(GCMFileEntryStruct *));

//regular function prototypes...
void openFile(char *mode);
void closeFile();

void verbosePrint(char *s);

void printGCMInfo(int hexFlag);
void printUsage();
void printExtendedUsage();

void listFiles();

void extractFileEntry(GCMFileEntryStruct *e);
void extractFile(GCMFileEntryStruct *e, char *dest);
void extractDiskHeader(char *path);
void extractDiskHeaderInfo(char *path);
void extractApploader(char *path);
void extractBootDol(char *path);

void injectDiskHeader(char *sourcePath);
void injectDiskHeaderInfo(char *sourcePath);
void injectApploader(char *sourcePath);

void replaceFile(char *gcmPath, char *localPath);

//globals...
int verboseFlag;	//it's gotta be global, so other functions can see it...

char *filepath;		//path to the file we're working with...
char *filename;		//the name of the file we're working with
FILE *gcmFile;		//the file we're working with

//for working with printing directories
int dirDepth;
GCMFileEntryStruct *lastDir;	//remember the last directory (for absolute paths of files)
int recursiveIndex;				//for the recursive printing...
int listInfoFlag;				//for listing filesize, directory content count, and filetype (d or f)
int listPathFlag;				//for listing full file paths...
char extractRootPath[1024];		//where we're starting to extract from... (directory or file)

#pragma mark -

int main (int argc, char **argv) {
	// start flags declarations...
	
	int hexFlag = 0;
	
	//for extracting:
	char *extractFileFrom = NULL;
	char *extractFileTo = NULL;
	
	char *fsReplacePath = NULL;
	
	verboseFlag = 0;
	
	int showInfoFlag = 1;

	int extractDiskHeaderFlag = 0;
	char *extractDiskHeaderFile = GCM_DISK_HEADER_FILENAME;
	
	int extractDiskHeaderInfoFlag = 0;
	char *extractDiskHeaderInfoFile = GCM_DISK_HEADER_INFO_FILENAME;

	int extractApploaderFlag = 0;
	char *extractApploaderFile = GCM_APPLOADER_FILENAME;
	
	int extractBootDolFlag = 0;
	char *extractBootDolFile = GCM_BOOT_DOL_FILENAME;
	
	int injectDiskHeaderFlag = 0;
	char *injectDiskHeaderFile = GCM_DISK_HEADER_FILENAME;
	
	int injectDiskHeaderInfoFlag = 0;
	char *injectDiskHeaderInfoFile = GCM_DISK_HEADER_INFO_FILENAME;
	
	int injectApploaderFlag = 0;
	char *injectApploaderFile = GCM_APPLOADER_FILENAME;
	
	char *replaceFileLocalPath = NULL;
	char *replaceFileGCMPath = NULL;

	int listFilesFlag = 0;
	listInfoFlag = 0;
	listPathFlag = 0;
	// end flag declarations
	
	//start argument parsing...
	char *currentArg = NULL;

	do {
		currentArg = GET_NEXT_ARG;
		if (!currentArg) {
			//there's no args! uh oh!
			
			//printf("No arguments...\n");
			printUsage();
			exit(0);
		} else if (CHECK_ARG(ARG_VERBOSE)) {
			//turn on verbosity!
			
			verboseFlag++;
			
			verbosePrint("Verbose output ON.");
		} else if (CHECK_ARG(ARG_HELP)) {
			// print extended help
			
			printExtendedUsage();
			exit(0);
		} else if (CHECK_ARG(ARG_INFO)) {
			//they want to see info...
			
			showInfoFlag++;
			
			verbosePrint("Show info flag ON.");
		} else if (CHECK_ARG(ARG_HEX)) {
			//they want hex notation...
			
			hexFlag = 1;
			
			verbosePrint("Hex notation ON.");
		} else if (CHECK_ARG(ARG_REPLACE_FILESYSTEM)) {
			// they want to replace the filesystem
			
			fsReplacePath		= GET_NEXT_ARG;
			
			if (!fsReplacePath) {
				printf("Argument error...\n");
				printUsage();
				exit(1);
			}
		} else if (CHECK_ARG(ARG_EXTRACT)) {
			// extract files...
			// usage: -e <path> <destPath>
			
			extractFileFrom		= GET_NEXT_ARG;
			extractFileTo		= GET_NEXT_ARG;
			
			if (!extractFileFrom || !extractFileTo) {
				//argument error... something was omitted...
				printf("Argument error.\n");
				printUsage();
				exit(1);
			}
		} else if (CHECK_ARG(ARG_EXTRACT_DISK_HEADER)) {
			// extract disk header... (to a file called "boot.bin")
			
			extractDiskHeaderFlag++;
			
			if (PEEK_ARG && strcmp(PEEK_ARG, OPT_FILE) == 0) {
				// if they're specifying a file...
				
				SKIP_NARG(1); //skip that -f we just looked at...
				extractDiskHeaderFile = GET_NEXT_ARG;
			}
		
		} else if (CHECK_ARG(ARG_EXTRACT_DISK_HEADER_INFO)) {
			// extract disk header info... (to a file called "bi2.bin")
			
			extractDiskHeaderInfoFlag++;
			
			if (PEEK_ARG && strcmp(PEEK_ARG, OPT_FILE) == 0) {
				// if they're specifying a file...
				
				SKIP_NARG(1); //skip that -f we just looked at...
				extractDiskHeaderInfoFile = GET_NEXT_ARG;
			}
			
		} else if (CHECK_ARG(ARG_EXTRACT_APPLOADER)) {
			//extract apploader... (to a file called "appldr.bin")
			
			extractApploaderFlag++;
			
			if (PEEK_ARG && strcmp(PEEK_ARG, OPT_FILE) == 0) {
				// if they're specifying a file...
				
				SKIP_NARG(1); //skip that -f we just looked at...
				extractApploaderFile = GET_NEXT_ARG;
			}
		} else if (CHECK_ARG(ARG_EXTRACT_BOOT_DOL)) {
			//extract the boot dol...
			
			extractBootDolFlag++;
			
			if (PEEK_ARG && strcmp(PEEK_ARG, OPT_FILE) == 0) {
				//if they specify a file...
				
				SKIP_NARG(1); //skip that -f
				extractBootDolFile = GET_NEXT_ARG;
			}
		} else if (CHECK_ARG(ARG_INJECT_DISK_HEADER)) {
			//inject the diskheader
			
			injectDiskHeaderFlag++;
			
			if (PEEK_ARG && strcmp(PEEK_ARG, OPT_FILE) == 0) {
				//if they're specifying a file... (otherwise use the default);
				
				SKIP_NARG(1); //skip the -f we just looked at...
				injectDiskHeaderFile = GET_NEXT_ARG;
			}
			
		} else if (CHECK_ARG(ARG_INJECT_DISK_HEADER_INFO)) {
			//inject the diskheaderinfo...
			
			injectDiskHeaderInfoFlag++;
			
			if (PEEK_ARG && strcmp(PEEK_ARG, OPT_FILE) == 0) {
				// if they're specifying a file...
				
				SKIP_NARG(1);
				injectDiskHeaderInfoFile = GET_NEXT_ARG;
			}
		} else if (CHECK_ARG(ARG_INJECT_APPLOADER)) {
			//inject the apploader...
			
			injectApploaderFlag++;
			
			if (PEEK_ARG && strcmp(PEEK_ARG, OPT_FILE) == 0) {
				//if they're specifying a file...
				
				SKIP_NARG(1);
				injectApploaderFile = GET_NEXT_ARG;
			}
		} else if (CHECK_ARG(ARG_REPLACE_FILE)) {
			//they want to replace a file...
			
			replaceFileGCMPath = GET_NEXT_ARG;
			replaceFileLocalPath = GET_NEXT_ARG;
			
			if (!replaceFileGCMPath || !replaceFileLocalPath) {
				printf("Argument error!\n");
				printUsage();
				exit(1);
			}
			
		} else if (CHECK_ARG(ARG_LIST)) {
			// list filesystem
			
			listFilesFlag++; //turn the listFiles flag on.
			
			while(1) {
				if (PEEK_ARG && (strcmp(PEEK_ARG, OPT_FILE_INFO) == 0 || strcmp(PEEK_ARG, OPT_FILE_INFO_SYN) == 0)) {
					SKIP_NARG(1);
					listInfoFlag++;
				} else if (PEEK_ARG && (strcmp(PEEK_ARG, OPT_FULL_PATH) == 0 || strcmp(PEEK_ARG, OPT_FULL_PATH_SYN) == 0)) {
					SKIP_NARG(1);
					listPathFlag++;
				} else {
					break;
				}
			}
		} else {
			// do it to this file... this is the last argument... just ignore the rest...
			
			filepath = currentArg;
			filename = lastPathComponent(filepath);
			
			break;
		}
	} while(*argv);
	//end parsing arguments...
	
	//open the file for reading and start doing read operations!
	openFile("r");

	// print the info...
	if (showInfoFlag) {
		printGCMInfo(hexFlag);
	}
	
	// list the files, if necesary...
	if (listFilesFlag) {
		listFiles();
	}

	// extract files...
	if (extractFileFrom && extractFileTo) {
		//testing recursive extraction...
		
		GCMFileEntryStruct *e = NULL;
		
		if (strcmp(extractFileFrom, "/") == 0) {
			e = GCMGetRootFileEntry(gcmFile);
			printf("root file entry index: %d\n", e->index);
		} else {	
			e = GCMGetFileEntryAtPath(gcmFile, extractFileFrom);
		}
		
		strcpy(extractRootPath, extractFileTo);
		
		recurseFileEntry(e, extractFileEntry);
		
		free(e);
		//extractFile(extractFileFrom, extractFileTo);
	}
	
	
	//extract diskheader
	if (extractDiskHeaderFlag) {
		extractDiskHeader(extractDiskHeaderFile);
	}
	
	//extract diskheaderinfo
	if (extractDiskHeaderInfoFlag) {
		extractDiskHeaderInfo(extractDiskHeaderInfoFile);
	}
	
	//extract apploader
	if (extractApploaderFlag) {
		extractApploader(extractApploaderFile);
	}
	
	//extract main executable DOL
	if (extractBootDolFlag) {
		extractBootDol(extractBootDolFile);
	}
	
	//close the file and open it again for read/write
	closeFile();
	openFile("r+");
	
	//inject the diskheader
	if (injectDiskHeaderFlag) {
		injectDiskHeader(injectDiskHeaderFile);
	}
	
	//inject the diskheaderinfo
	if (injectDiskHeaderInfoFlag) {
		injectDiskHeaderInfo(injectDiskHeaderInfoFile);
	}
	
	//inject the apploader
	if (injectApploaderFlag) {
		injectApploader(injectApploaderFile);
	}
	
	if (replaceFileLocalPath && replaceFileGCMPath) {
		replaceFile(replaceFileGCMPath, replaceFileLocalPath);
	}

	//replace the filesystem
	if (fsReplacePath) {
		GCMReplaceFilesystem(gcmFile, fsReplacePath);
	}
	
	closeFile();
	
	return 0;
}

void openFile(char *mode) {
	/*
	**  opens the GCM file with the mode set to mode
	**	if we were to open it for reading AND writing,
	**	it would error out if we didn't have write permission
	*/
	
	verbosePrint("Opening GCM...");
	
	if (!filepath) {
		printUsage();
		exit(1);
	}
	
	if (!(gcmFile = fopen(filepath, mode))) { //open as r+ so we can inject data, too...
		perror(filepath);
		exit(1);
	}
	
}

void closeFile(void) {
	/*
	**  Closes the GCM for when we're done...
	*/
	
	verbosePrint("Closing GCM...");
	fclose(gcmFile);
}

void verbosePrint(char *s) {
	/*
	**  this prints a string if verbosity is turned on...
	**  this should be turned into a verbosePrintf() so we can accept arguments... but I'm lazy.
	*/
	
	if (verboseFlag) {
		printf("%s\n", s);
	}
}

void listFiles() {
	/*
	**  lists the file system of the GCM...
	*/
	
	dirDepth = 0;
	recursiveIndex = 0;
	GCMFileEntryStruct *r = GCMGetRootFileEntry(gcmFile);
	recurseFileEntry(r, printEntry);
	GCMFreeFileEntryStruct(r);
}

void printGCMInfo(int hexFlag) {
	/*
	**  This just prints all of the relevant info for the ROM
	**  fun fun fun
	*/

	char systemID = GCMGetSystemID(gcmFile);
	
	char *gameID = (char*)malloc(GCM_GAME_ID_LENGTH + 1);
	GCMGetGameID(gcmFile, gameID);
	
	char regionCode = GCMGetRegionCode(gcmFile);
	
	char *makerCode = (char*)malloc(GCM_MAKER_CODE_LENGTH + 1);
	GCMGetMakerCode(gcmFile, makerCode);
	
	char *gameName = (char*)malloc(256);
	GCMGetGameName(gcmFile, gameName);
	
	printf("Filename:        %s\n", filename);
	printf("System ID:       %c (%s)\n", systemID, GCMSystemIDToStr(systemID));
	printf("Game ID:         %s\n", gameID);
	printf("Region:          %c (%s)\n", regionCode, GCMRegionCodeToStr(regionCode));
	printf("Maker Code:      %s (%s)\n", makerCode, GCMMakerCodeToStr(makerCode));
	printf("Game Name:       %s\n", gameName);
	
	char format[256]; //this is for the hexFlag stuff...
	
	//this has to be cleaned up and simplified with a macro or something...
	strcpy(format, "DOL offset:      ");
	strcat(format, (hexFlag) ? "0x%08X" : "%lu");
	strcat(format, "\n");
	printf(format, GCMGetDolOffset(gcmFile));
	
	strcpy(format, "Apploader:       ");
	strcat(format, (hexFlag) ? "0x%08X" : "%lu");
	strcat(format, "\n");
	printf(format, GCM_APPLOADER_OFFSET);
	
	strcpy(format,"ApploaderSize:    ");
	strcat(format, (hexFlag) ? "0x%08X" : "%lu");
	strcat(format, "\n");
	printf(format, GCMGetApploaderSize(gcmFile));
	
	strcpy(format, "FST:             ");
	strcat(format, (hexFlag) ? "0x%08X" : "%lu");
	strcat(format, "\n");
	printf(format, GCMGetFSTOffset(gcmFile));
	
	GCMFileEntryStruct *r = GCMGetRootFileEntry(gcmFile);
	u32 entryCount = r->length;
	printf("File count:      %lu\n", entryCount);
	GCMFreeFileEntryStruct(r);
}

#pragma mark -

void extractFileEntry(GCMFileEntryStruct *e) {
	/*
	**  this gets called from recurseFileEntry()
	**  because of this,  we only need to create a directory if e is a directory
	*/
	
	if (!e) return;
	
	GCMFetchFilenameForFileEntry(gcmFile, e);
	
	//for building the path for where we're making dirs and extracting...
	char s[1024];
	char fp[256] = "";
	
	//now do the actual work...
	if (e->isDir) {
		GCMGetFullPathForFileEntry(gcmFile, e, fp);
	
		strcpy(s, extractRootPath);
		strcat(s, fp);
	
		lastDir = e;
		
		//printf("mkdir %s\n", s);
		
		if (e->index == 0) {
			//if it's the root directory, we're gonna create a directory to extract into named filename.FILES
			strcat(extractRootPath, "/");
			strcat(extractRootPath, filename);
			strcat(extractRootPath, ".FILES");
			
		//	printf("MAKING NEW DIR! %s\n", extractRootPath);
			
			mkdir(extractRootPath, S_IRWXU);
		} else {
			mkdir(s, S_IRWXU);
		}
		
	} else {
		GCMGetFullPathForFileEntry(gcmFile, lastDir, fp);
		strcpy(s, extractRootPath);
		strcat(s, fp);
		strcat(s, "/");
		strcat(s, e->filename);
		
		printf("extracting %s\n", s);
		
		extractFile(e, s);
	}
	//GCMFetchDataForFileEntry(gcmFile, e);
	//DOESN'T WORK!!!!!!!
	return;
}

void extractFile(GCMFileEntryStruct *e, char *dest) {
	/*
	**  extract files from source (in GCM) to dest (in the local filesystem)
	*/
	
	char vstring[1024] = "Extracting ";
	strcat(vstring, e->filename);
	strcat(vstring, " from GCM to ");
	strcat(vstring, dest);
	verbosePrint(vstring);
	
	//check to see if the file was actually found...
	if (!e) {
		printf("File not found (%s)\n", e->filename);
		return;
	}
	
	//this way is fast, but uses up wayyyyyy too much memory.
	//fetch the data
	/*GCMFetchDataForFileEntry(gcmFile, e);
	
	WriteDataToFile(e->data, e->length, dest);
	
	free(e->data);*/
	
	//this thing is wayyyyyyyyy too slow:
	/*FILE *ofile = NULL;
	if (!(ofile = fopen(dest, "w+"))) {
		perror(dest);
		exit(1);
	}
	
	fseek(gcmFile, e->offset, SEEK_SET);
	
	while (ftell(gcmFile) < (e->offset + e->length)) {
		fputc(fgetc(gcmFile), ofile);
	}
	
	fclose(ofile);*/
	
	//this uses buffered reads to copy data in chunks...
	FILE *ofile = NULL;
	if (!(ofile = fopen(dest, "w+"))) {
		perror(dest);
		exit(1);
	}
	
	fseek(gcmFile, e->offset, SEEK_SET);
	
	if (CopyData(gcmFile, ofile, e->length) != e->length) {
		printf("ERROR COPYING DATA!\n");
	}
	
	fclose(ofile);
}

void extractDiskHeader(char *path) {
	/*
	**  extracts the disk header to boot.bin
	*/
	
	verbosePrint("Extracting the disk header...");
	
	//get the data...
	char *buf = (char*)malloc(GCM_DISK_HEADER_LENGTH);
	GCMGetDiskHeader(gcmFile, buf);
		
	WriteDataToFile(buf, GCM_DISK_HEADER_LENGTH, path);
		
	free(buf);
}

void extractDiskHeaderInfo(char *path) {
	/*
	**  extracts the diskheaderinfo to bi2.bin
	*/
	
	verbosePrint("Extracting the disk header info...");
	
	//get the data...
	char *buf = (char*)malloc(GCM_DISK_HEADER_INFO_LENGTH);
	GCMGetDiskHeaderInfo(gcmFile, buf);
	
	WriteDataToFile(buf, GCM_DISK_HEADER_INFO_LENGTH, path);
	
	free(buf);
}

void extractApploader(char *path) {
	/*
	**  extracts the apploader to appldr.bin
	*/
	
	verbosePrint("Extracting the apploader...");
	
	//get the data...
	u32 apploaderLength = GCMGetApploaderSize(gcmFile) + GCM_APPLOADER_CODE_OFFSET;
	char *buf = (char*)malloc(apploaderLength);
	GCMGetApploader(gcmFile, buf);
	
	WriteDataToFile(buf, apploaderLength, path);
	
	free(buf);
}

void extractBootDol(char *path) {
	/*
	**  extracts the boot DOL from the GCM and saves it to path...
	*/
	
	verbosePrint("Extracting the boot DOL...");
	
	if (!path) return;
	
	u32 length = GCMGetBootDolLength(gcmFile);
	char *buf = (char*)malloc(length);
	
	if (GCMGetBootDol(gcmFile, buf) != length) {
		printf("An error occurred when getting the DOL.\n");
		free(buf);
		return;
	}
	
	WriteDataToFile(buf, length, path);
}

#pragma mark -

void injectDiskHeader(char *sourcePath) {
	/*
	**  take a diskHeader (boot.bin) from sourcePath and inject it into gcmFile.
	*/
	
	verbosePrint("Injecting the disk header...");
	
	char *buf = (char*)malloc(GetFilesizeFromPath(sourcePath));
	
	if (ReadDataFromFile(buf, sourcePath) != GCM_DISK_HEADER_LENGTH) {
		printf("This does not appear to be a diskheader (%s)\n", sourcePath);
		free(buf);
		return;
	}
	
	if (GCMPutDiskHeader(gcmFile, buf) != GCM_SUCCESS) {
		printf("An error occurred when writing the disk header! (%d)\n", ferror(gcmFile));
	}
	
	free(buf);
	return;
}

void injectDiskHeaderInfo(char *sourcePath) {
	/*
	**  take a diskHeaderInfo (bi2.bin) from sourcePath and inject it into gcmFile.
	*/
	
	verbosePrint("Injecting the disk header info...");
	
	char *buf = (char*)malloc(GetFilesizeFromPath(sourcePath));
	
	if (ReadDataFromFile(buf, sourcePath) != GCM_DISK_HEADER_INFO_LENGTH) {
		printf("This does not appear to be a diskheaderinfo (%s)\n", sourcePath);
		free(buf);
		return;
	}
	
	if (GCMPutDiskHeaderInfo(gcmFile, buf) != GCM_SUCCESS) {
		printf("An error occurred when writing the disk header info!\n");
	}
	
	free(buf);
	return;
}

void injectApploader(char *sourcePath) {
	/*
	**  this doesn't work, yet... requires some serious shifting of other data...
	*/
	
	verbosePrint("Injecting the apploader...");
	
	//first, load the file into memory...
	
	u32 length = GetFilesizeFromPath(sourcePath);
	char *data = (char*)malloc(length);
	
	if (ReadDataFromFile(sourcePath, data) != length) {
		printf("An error occurred reading the file (%s)", sourcePath);
		free(data);
		return;
	}
}

void replaceFile(char *gcmPath, char *localPath) {
	/*
	**	replace the file at gcmPath with the file at localPath
	**	file sizes must be the same...
	*/

	if (!gcmPath || !localPath) return;

	u32 filesize = GetFilesizeFromPath(localPath);
	GCMFileEntryStruct *e = GCMGetFileEntryAtPath(gcmFile, gcmPath);
	
	if (e->isDir) {
		printf("GCM file path MUST be a file!\n");
		exit(1);
	}
	
	if (filesize != e->length) {
		printf("File sizes of %s and %s do not match!\n", gcmPath, localPath);
		exit(1);
	}
	
	FILE *ifile = NULL;
	
	if (!(ifile = fopen(localPath, "r"))) {
		perror(localPath);
		exit(1);
	}
	
	fseek(gcmFile, e->offset, SEEK_SET);
	CopyData(ifile, gcmFile, filesize);
	
	fclose(ifile);
}

#pragma mark -

void printEntry(GCMFileEntryStruct *e) {
	/*
	**  this prints the entries. Called from the printDirectory function.
	**  This formats the output all nice. Puts the info there if it's supposed to be. etc.
	*/
	
	int j = 0;
	
	char size[100] = "";
	char *path = (char*)malloc(1024);
	char padding[128] = " ";
	
	//get the filename...
	GCMFetchFilenameForFileEntry(gcmFile, e);
	
	strcpy(path, e->filename);
	
	if (listInfoFlag) {
		if (e->isDir) {
			sprintf(size, "(%ld)", (e->length - recursiveIndex - 1));
		} else {
			sprintf(size, "%ld",e->length);
		}
		sprintf(size, "%-14s", size); //this pads everythign nicely...
	}

	
	if (listPathFlag) {
		if (e->isDir) {
			GCMGetFullPathForFileEntry(gcmFile, e, path);
		} else {
			if(dirDepth > 1) { //if we're not at the root-level, show the full path...
				GCMGetFullPathForFileEntry(gcmFile, lastDir, path);
				strcat(path, "/");
			} else {
				strcpy(path, "/");
			}
			
			strcat(path, e->filename);
		}
	} else { //space out the next entry...
		for (j = 0; j < dirDepth; j++) {
			strcat(padding, " ");
		}
	}
	
	if (dirDepth == 0) {
		// if dirDepth == 0, then that means this is the root entry.
		// there's no way of telling that an entry is the root because it 
		// looks a lot like the first entry... if not identical...
		printf("%s /\n", size);
	} else {
		if (e->isDir)
			printf("%s%s%s/\n", size, padding, path);
		else
			printf("%s%s%s\n", size, padding, path);
	}
	
	//free(e->filename);
}

void recurseFileEntry(GCMFileEntryStruct *e, void (*func)(GCMFileEntryStruct *)) {
	/*
	**  Recurse through the file entry... if it's a directory, recurse through all its children
	**  otherwise, it's just gonna perform func on the entry...
	**
	**  one problem is that it uses recursiveIndex to grab the nth file entry... you have to set it before you begin...
	*/
	
	if (!e) {
		printf("ERROR reading entry (entry doesn't exist?!?!)\n");
		return;
	}
	
	(func)(e);
	
	if (e->isDir) {
		dirDepth++;
		GCMFileEntryStruct *next;
		lastDir = e;
		
		for(recursiveIndex = e->index + 1; (unsigned long)recursiveIndex < e->length; recursiveIndex++) {
			next = GCMGetNthFileEntry(gcmFile, recursiveIndex);
			if (next) {
				recurseFileEntry(next, func);
				free(next);
			}
		}
		recursiveIndex--;
		dirDepth--;
		lastDir = GCMGetNthFileEntry(gcmFile, lastDir->offset); //set lastDir to the parent of lastDir...
	}
}

/*void printDirectory(GCMFileEntryStruct *e) {
	if (!e) {
		return;
	}
	
	printEntry(e);
	
	if (e->isDir) {
		dirDepth++;
		GCMFileEntryStruct *next;
		lastDir = e;
		
		//printf("for(%d++; %d < %ld; i++)\n", i, i, e->length);
		
		for(recursiveIndex++; (unsigned long)recursiveIndex < e->length; recursiveIndex++) {
			next = GCMGetNthFileEntry(gcmFile, recursiveIndex);
			if (next) {
				printDirectory(next);
				free(next);
			}
		}
		recursiveIndex--;
		dirDepth--;
		lastDir = GCMGetNthFileEntry(gcmFile, lastDir->offset); //set lastDir to the parent of lastDir...
	}
}*/

void printUsage() {
	printf("GCMTool %s- A utility for working with Nintendo GameCube DVD images.\n\tgcmtool.sourceforge.net\n\n", VERSION);
	printf("Usage:");
	printf("  gcmtool [ options ] <filename>\n\n");
	printf("Use -? for extended usage.\n\n");
}

void printExtendedUsage() {
	printUsage();
	
	printf("    Options:\n");
	PRINT_HELP(ARG_EXTRACT);
	PRINT_HELP(ARG_VERBOSE);
	PRINT_HELP(ARG_INFO);
	PRINT_HELP(ARG_LIST);
	PRINT_HELP(ARG_HELP);
	PRINT_HELP(ARG_HEX);
	PRINT_HELP(ARG_REPLACE_FILESYSTEM);
	PRINT_HELP(ARG_REPLACE_FILE);
	printf("\n");
	printf("  You can add -f <filename> to specify a filename for the following options...\n");
	
	PRINT_HELP(ARG_EXTRACT_DISK_HEADER);
	PRINT_HELP(ARG_EXTRACT_DISK_HEADER_INFO);
	PRINT_HELP(ARG_EXTRACT_APPLOADER);
	PRINT_HELP(ARG_EXTRACT_BOOT_DOL);
	PRINT_HELP(ARG_INJECT_DISK_HEADER);
	PRINT_HELP(ARG_INJECT_DISK_HEADER_INFO);
	PRINT_HELP(ARG_INJECT_APPLOADER);
	PRINT_HELP(ARG_INJECT_BOOT_DOL);
}
