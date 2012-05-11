/*
**  Macros and #defines for working with commandline arguments and help in GCM tool...
**  Part of GCMTool Project
**  gcmtool.sourceforge.net
**
**  spike@sadistech.com
**  June 12, 2004
*/

#ifndef _GCM_COMMANDLINE_H_
#define _GCM_COMMANDLINE_H_

// utility macros:
#define QQ(s) #s

//standard arguments:
#define ARG_HELP					"-?"
#define ARG_HELP_SYN				"--help"
#define ARG_HELP_OPT				""
#define ARG_HELP_HELP				"Displays this help."

/*
**  commandline args are set up like this:
**  ARG_SOME_COMMAND:		The command option (ie: -f or -ps)
**  ARG_SOME_COMMAND_SYN:   A synonym for SOME_COMMAND (ie: --file or --extract-this-type-of-thing)
**  ARG_SOME_COMMAND_OPT:   The human-readable options for SOME_COMMAND, used in the help output
**  ARG_SOME_COMMAND_HELP:  The description used in the help output for SOME_COMMAND
**
**  Explanation of the macros:
**  ARG_SYN(ARG):			used if you wanna try to grab an argument's synonym (ie: ARG_SYN(SOME_COMMAND) will return SOME_COMMAND_SYN
**  PRINT_HELP(ARG):		An awesome macro that takes the command, the synonym, the options and the help and outputs something formatted nice
**
**  GET_NEXT_ARG:			pops the next arg (used in the main() function) and returns it
**  SKIP_NARG(n):			skips n args (for when you PEEK_ARG and want to pop that one without reading it)
**  CHECK_ARG(ARG):			used for checking if something matches a command or its synonym
**  PEEK_ARG:				returns the next arg without affecting the stack
**  PEEK_NARG(n):			returns the nth next arg without affecting the stack
*/

//macros... although they may be simple...
//these are for getting help and synonyms and stuff
#define ARG_SYN(ARG)		ARG ## _SYN
#define PRINT_HELP(ARG)		printf("\t\033[1m" ARG "%s" ARG ## _SYN " " ARG ## _OPT "\033[m\n\t\t" ARG ## _HELP "\n\n", strcmp("", ARG ## _SYN) == 0 ? "" : ", ");

// these are for the argument parsing engine...
#define GET_NEXT_ARG		*(++argv)
#define SKIP_NARG(n)		*(argv += n)	
#define CHECK_ARG(ARG)		strcmp(ARG, currentArg) == 0 || strcmp(ARG ## _SYN, currentArg) == 0
#define PEEK_ARG			*(argv + 1)
#define PEEK_NARG(n)		*(argv + n)


#endif /* _GCM_COMMANDLINE_H_ */
