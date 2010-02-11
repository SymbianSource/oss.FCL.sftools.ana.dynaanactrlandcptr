/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Common defines for cpp files.
*
*/


#ifndef __ATCOMMONDEFINES_H__
#define __ATCOMMONDEFINES_H__

#include "../inc/stdafx.h"

using namespace std;

// CLI return codes.
// Note! Do not change the values. Used in extension.
struct AT_RETURN_CODE {
	enum RETURN_CODE {
		/* basic */
		OK = 0,							/* All ok. */
		INVALID_ARGUMENT_ERROR = 1,		/* Arguments failty. */
		AT_LIBS_MISSING = 2,			/* Missing at libraries cannot compile hooked apps. */
		CANNOT_FIND_EPOCROOT = 3,		/* Epocroot not set. */
		UNHANDLED_EXCEPTION = 4,		/* General return code when unhandled exception occurs. */
		/* compile errors */
		MAKEFILE_ERROR = 5,				/* Cannot read/create etc makefiles sbs1/2. */
		COMPILE_ERROR = 6,				/* Some error related to compilation of code. */
		UNKNOWN = 7,					/* General compilation error not specific. */
		NO_SUPPORTED_MODULES_ERROR = 8,	/* Cannot find supported modules from project. */
		KERNEL_SIDE_MODULE_ERROR = 9,	/* Cannot find supported modules from project because kernel side. */
		/* Analyze errors */
		WRONG_DATA_FILE_VERSION = 10,	/* Invalid data file version in internal gathering mode. */
		INVALID_DATA_FILE = 11,			/* Data file content invalid. */
		ANALYZE_ERROR = 12,				/* General error in analyze part not specific. */
		EMPTY_DATA_FILE = 13,			/* Data file is empty. */
		SYMBOL_FILE_ERROR = 14,			/* Symbol file content invalid or do not exist. */
		/* building&releasing errors */
		RELEASABLES_ERROR = 20,			/* Cannot find the binaries of module. */
		RESTORE_MODULES_ERROR = 21,		/* Error restoring mmp file to original. */
		CREATING_TEMP_CPP_ERROR = 22,	/* Error creating temporary cpps to store variables to s60 core.*/
		CLEANING_TEMP_ERROR = 23,		/* Error cleaning atool_temp folder from module. */
		READ_MAKEFILE_ERROR = 24,		/* Some error when reading makefile of module / project (sbs1/2). */
		MODIFY_MODULES_ERROR = 25,		/* Error modifying module mmp file. */
		INVALID_MMP_DEFINED = 27,		/* Specified module does not exist in project. */
		/* Attribute file related */
		WRITE_ATTRIBUTES_ERROR = 30,	/* Error writing the cfg file into atool_temp folder (contains arguments).*/
		READ_ATTRIBUTES_ERROR = 31,		/* Error reading hooking related arguments from cfg file. */
		/* User issued exit */
		USER_ISSUED_EXIT = 40,			/* User aborted the run of application. */
		/* HTI */
		HTI_NO_CONNECTION = 50,			/* No hti connection defined */
		HTI_NO_AT_FOLDER = 51,			/* Cannot find analyzetool folder from device. */
		HTI_NO_FILE_DEFINED = 52		/* No file defined to be fetched. */
	};
};

// Debug logging to console
extern bool g_bDebugConsole;
// Debug logging to windows api debug
extern bool g_bDebugDbgView;
// Debug (low level functions).
extern bool g_bDebugLowLevel;

// Temporary directory definition's.
const char AT_TEMP_DIR[] = "atool_temp";
const char AT_TEMP_LST_DIR[] = "atool_temp\\static_lsts\\";

// makefile dir is used in atool.cpp to check will we use sbs2 (in analyze and clear)
#define RAPTOR_MAKEFILE_DIR "atool_temp\\build"

// Atool version number and date
#define ATOOL_VERSION "1.8.0" // NOTE! This version number is written also to temporary cpp file.
#define ATOOL_DATE "1st December 2009"

// Default/min/max call stack sizes
const int AT_ALLOC_CALL_STACK_SIZE_DEFAULT = 40;
const int AT_FREE_CALL_STACK_SIZE_DEFAULT = 0;
const int AT_CALL_STACK_SIZE_MIN = 0;
const int AT_CALL_STACK_SIZE_MAX = 256;

// What version of data file contains timestamps in dll's.
const int AT_DLL_TIMESTAMP_DATA_VERSION = 2;

// Constant compatibility string in temporary cpp.
// Api version ; current version.
#define ATOOL_COMPATIBILITY_STRING "1.7.4;1.8.0"

// Datafile version
#define AT_DATA_FILE_VERSION "DATA_FILE_VERSION 11"

// Build summary constants
const string AT_BUILD_SUMMARY_HEADER = "\n###  AnalyzeTool  ###\n### Build Summary ###\n\n";
const string AT_BUILD_SUMMARY_INSTRUMENTED_BUILD_COMPLETE = "Instrumented build complete: ";
const string AT_BUILD_SUMMARY_TARGET = "\tTarget: ";
const string AT_BUILD_SUMMARY_FAILED = "Build failed: ";
const string AT_BUILD_SUMMARY_ERRORS = "\tErrors: ";
const string AT_BUILD_SUMMARY_DATA_FILE_NAME = "\tInternal data gathering filename: ";
const string AT_BUILD_SUMMARY_NORMAL_BUILD_COMPLETE = "Normal build complete: ";
const string AT_BUILD_SUMMARY_STATIC_LIBRARY = "\tModule is static library.";
const string AT_BUILD_SUMMARY_UNSUPPORTED_TARGET_TYPE = "\tModule has unsupported target type.";
const string AT_BUILD_SUMMARY_UNSUPPORTED_COMPILE_DEFINITION = "\tModule defined unsupported compile definition.";
const string AT_BUILD_SUMMARY_BUILD_TYPE = "Build type: ";
const string AT_BUILD_SUMMARY_BUILD_TARGET = "Build target: ";
const string AT_BUILD_SUMMARY_BUILD_PLATFORM = "Build platform: ";
const string AT_BUILD_SUMMARY_BUILD_VARIANT = "Build variant: ";
const string AT_BUILD_SUMMARY_LOGGING_MODE = "Data gathering mode: ";
const string AT_BUILD_SUMMARY_ALLOC_CALL_STACK_SIZE = "Allocation call stack size: ";
const string AT_BUILD_SUMMARY_FREE_CALL_STACK_SIZE = "Free call stack size: ";
const string AT_BUILD_SUMMARY_S60 = "monitored internal";
const string AT_BUILD_SUMMARY_XTI = "monitored external";
const string AT_BUILD_SUMMARY_XTI_FAST = "external";

const string AT_UNSUPPORTED_TARGET_TYPE = "\tModule has unsupported target type.";
const string AT_UNSUPPORTED_COMPILE_DEFINITION = "\tModule defined unsupported compile definition.";

// List of locations under epocroot where analyzetool.h can be found.
// Add all possible to this list so core version information can be read
// from file.
const string AT_CORE_INCLUDE_FILE_WITH_VERSION_NUMBER[] = {
	"epoc32\\include\\domain\\osextensions\\analyzetool\\analyzetool.h",
	"epoc32\\include\\oem\\analyzetool\\analyzetool.h",
	"epoc32\\include\\platform\\analyzetool\\analyzetool.h"
};
const string AT_CORE_VERSION_NUMBER_TAG = "ANALYZETOOL_CORE_VERSION_FOR_CLE";

// Temporary cpp name
const char AT_TEMP_CPP_LOWER_START[] = "0a1b2c3d_atool_temp_";
const char AT_TEMP_CPP_LOWER_END[] = ".cpp";

const char TAB_CHAR_VALUE = 9;
const char SPACE_CHAR_VALUE = 32;

// Bldmake error msg
#define AT_BLDMAKE_ERROR "AnalyzeTool : bldmake bldfiles error.\n"

// File copy/move/etc... message definitions
#define AT_MSG "AnalyzeTool : "
#define AT_MSG_SYSTEM_CALL "AnalyzeTool : System call, "
#define AT_FILE_TO " to "

// -c messages
#define AT_CLEANING_DONE "AnalyzeTool : Cleaning done."
#define AT_CLEANING_NOTHING_FOUND "AnalyzeTool : Nothing found to clean."

// Analyzing data without process start or dll load
#define AT_ANALYZE_INSUFFICIENT_LOGGING_DATA "Insufficient logging data to locate code lines for memory addresses.\nLogging has been started after process start."
#define AT_ANALYZE_NO_PROCESS_START "AnalyzeTool : No processes start information found in test run data."
#define AT_ANALYZE_NO_DLL_LOAD "AnalyzeTool : No dll load information found in test run data."
#define AT_ANALYZE_CANNOT_PINPOINT "AnalyzeTool : Memory addresses cannot be pinpointed."
#define AT_ANALYZE_ABNORMAL_EXIT "Abnormal process end"

// Analyzing without map / lst files
#define AT_ANALYZE_MISSING_LST_MAP_FILES "AnalyzeTool : Missing lst or map files, memory addresses shown only to module level.\n"
//#define AT_UNINSTRUMENT_FAILED_IN_ANALYZE_MESSAGE "AnalyzeTool: Error creating map/lst files, leaks shown only to module level.\nRun atool -u to try create lst/map files again after binaries build successfully.\n"

// Mmp error msg
#define INVALID_MMP_ERROR "AnalyzeTool : Error, Specified mmp is invalid.\n"

// Make error msg
#define MAKE_ERROR "AnalyzeTool : Error, Running make.\n"

#define AT_NOT_BEEN_BUILD_ERROR "AnalyzeTool : Error, project has not been build using AnalyzeTool.\n"

// Makefile errors
#define LEVEL1_MAKEFILE_ERROR "AnalyzeTool : Error, reading level 1 makefile.\n"
#define LEVEL2_MAKEFILE_ERROR "AnalyzeTool : Error, reading level 2 makefile.\n"
#define MAKEFILE_ERROR_TIP "AnalyzeTool : Tip, your environment might require you to use -variant [target] argument.\n";

//Variant messages
#define INVALID_VARIANT_ERROR "AnalyzeTool : Error,Specified variant is invalid.\n"
#define USING_DEFAULT_VARIANT_MESSAGE "AnalyzeTool : Note, Using DEFAULT variant.\n"
#define NO_DEFAULT_VARIANT_ERROR "AnalyzeTool : Error, environment does not specify DEFAULT variant (No variant support?).\n"

// Variant dir is the dir where all variants are defined
#define VARIANT_DIR "\\epoc32\\tools\\variant\\"

// Variants defined extension name of the variant must equal filename
#define VARIANT_FILE_EXTENSION ".VAR"

//Max file name length
const int MAX_FILENAME_LENGTH = 1024;

//Max line length when reading lines from text file.2048 should be enought.
const int MAX_LINE_LENGTH = 2048*8;

// ATs project file name to store project attributes
#define AT_PROJECT_ATTRIBUTES_FILE_NAME "project.cfg"
const string AT_PROJECT_ATTRIBUTES_SEPARATOR( ":$:" );

// ATs level1 makefile name
#define AT_LEVEL_1_MAKEFILE_NAME "main.at"

// ATs level2 makefiles extension
#define AT_LEVEL_2_MAKEFILE_EXT "atm"

//Default logging level
const int DEFAULT_LOGGING_LEVEL = 3;

//Minimun value of logging level 
const int MIN_LOGGING_LEVEL = 0;

//Maximun value of logging level
const int MAX_LOGGING_LEVEL = 3;

//Const char represeting dash
const char DASH = '\\';

/* Table of file extension not to delete
 * from atools temp directory when compiling
 * note define all in lowercase
 */
const string TEMP_EXTENSION_NO_DELETE[] = {
	"xml",
	"dat",
	"tmp",
	"cpp",
	"at",
	"atm",
	"cfg"
};

// Allowed characters/digits, other than these will be filtered out when using CATBase FilterString
const char CFILTERSTRING[] = "abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_";

/* Table of unsupported target types
 * (i.e. mmp file syntax targettype in makefile)
 * List of target types which will not be hooked:
*/
const string UNSUPPORTED_TARGET_TYPES[] = {
	"CTL",
	"ECOMIIC",
	"IMPLIB",
	"KDLL",
	"KEXT",
	"KLIB",
	"LDD",
	/* "LIB",  currently have own vector in project.*/
	"MDA",
	"MDL",
	"NONE",
	"NOTIFIER",
	"PDD",
	"RDL",
	"STDLIB",
	"VAR"
};

/**
* Kernel side target types
*/
const string KERNEL_SIDE_TARGET_TYPES[] = {
	"VAR",
	"LDD",
	"PDD",
	"KEXT"
};

// Kernel mode compile definition
const string KERNEL_MODE_COMPILE_DEFINITION = "__KERNEL_MODE__";

/**
* MMP file changes for AT
* Note "SOURCE            0a1b2c3d_atool_temp_TARGET.cpp" is added also.
*/
const string MMPFILECHANGES[] = {
	"//*** AnalyzeTool changes start ***",
	"SOURCEPATH        atool_temp",
	"LIBRARY           AToolMemoryHook.lib",
	"STATICLIBRARY     AToolStaticLib.lib",
	"OPTION ARMCC --interleave",
	"//*** AnalyzeTool changes end ***"
};

//MMP file changes for AT
//For targettype Dll
const string MMPFILECHANGES_DLL[] = {
	"//*** AnalyzeTool changes start ***",
	"OPTION ARMCC --interleave",
	"//*** AnalyzeTool changes end ***"
};

// Trace file constants definitions
#define LABEL_DATA_FILE_VERSION "DATA_FILE_VERSION"
#define LABEL_PROCESS_START     "PROCESS_START"
#define LABEL_DLL_LOAD          "DLL_LOAD"
#define LABEL_DLL_UNLOAD		"DLL_UNLOAD"
#define LABEL_MEM_LEAK          "MEM_LEAK"
#define LABEL_PROCESS_END       "PROCESS_END"
#define LABEL_ERROR_OCCURED     "ERROR_OCCURED"
#define LABEL_HANDLE_LEAK       "HANDLE_LEAK"
#define LABEL_TEST_START        "TEST_START"
#define LABEL_TEST_END          "TEST_END"
#define LABEL_LOGGING_CANCELLED "LOGGING_CANCELLED"

// AddressToLine related constants

#define LABEL_ABNORMAL "ABNORMAL"
const int FUNCTIONS_OFFSET_IN_MAP_FILE_ARMV5 = 0x8000;
const int FUNCTIONS_OFFSET_IN_GCCE = 0x8000;
const int FUNCTIONS_OFFSET_IN_MAP_FILE_WINSCW = 0x400000;
#define TEXT_NO_HANDLE_LEAKS "No handle leaks."

// Make file constant labels
#define MAKEFILE_TARGETTYPE_STRING "# TargetType "
#define MAKEFILE_BASIC_TARGETTYPE_STRING "# BasicTargetType "
#define MAKEFILE_TARGET_STRING "# Target "
#define MAKEFILE_FEATURE_VARIANT_NAME "# FeatureVariantName "
#define MAKEFILE_FEATURE_VARIANT_UREL_LABEL "# FeatureVariantURELLabel "
#define MAKEFILE_FEATURE_VARIANT_UDEB_LABEL "# FeatureVariantUDEBLabel "
#define UDEB_OPTIMIZATION_LEVEL 0

// Listing file contant labels
#define LST_FILE_SOURCE_FILE_LABEL "Source file:"

/**
* Struct for saving module info from .dat file and number of leaks in module.
*
*/
struct DLL_LOAD_INFO
{
	string sModuleName;
	unsigned long iStartAddress;
	unsigned long iEndAddress;
	int iLeaks;
	unsigned long iPID;
	int iSubTestStartOpenHandles;
	int iSubTestEndOpenHandles;
	// Load/Unload times, note these are micro seconds from 1970 so size is huge.
	unsigned long long iLoadTime;
	unsigned long long iUnloadTime;
	DLL_LOAD_INFO() {
		iStartAddress = 0;
		iEndAddress = 0;
		iLeaks = 0;
		iPID = 0;
		iSubTestStartOpenHandles = 0;
		iSubTestEndOpenHandles = 0;
		iLoadTime = 0;
		iUnloadTime = 0;
	};
};

/**
* Struct for saving symbol information from listing file
*/
struct LINE_IN_FILE
{
	int iLine;
	string sFileName;
	string sFunction;
	string sMangledName;
	string sLstName;
};

/**
* Struct for saving symbol information from map file
*/
struct MAP_FUNC_INFO
{
	int iAddress;
	int iFuncLength;
	string sMangledName;
	string sWholeLine;
	string sFunctionName;
};

/**
* State that represents the outcome
* of locating memory address code line from module
*/
enum ADDRESS_TO_LINE_STATE {
	no_map_data = 0, /* Map data was missing */
	out_of_function_range, /* Could not locate it to any function */
	mangled_symbol_name,
	only_function_name, /* Only got function name not code line */
	succesfull /* Have line number, function, etc...*/
};

/**
* Struct to store detail data and state when
* locating memory addresses code lines
*/
struct ADDRESS_TO_LINE_ITEM {
	ADDRESS_TO_LINE_STATE eState; /* outcome*/
	string sFileName; /* Filename of address*/
	string sFunctionName; /* Function name of address*/
	string sMangledFunctionName; /* function/Symbol name */
	int iFunctionLineNumber; /* Functions line number */
	int iExactLineNumber; /* Exact line number of address */
};

/**
* Struct to store  memory leaks
* call stack entry data
*/
struct CALL_STACK_ITEM {
	string sAddress; /* Address in string */
	int iAddress; /* Address in number (converted)*/
	int iLocation; /* Location. i.e. corresponds m_vDllLoadModList's index*/
	int iCalculatedLeakAddress;
	bool bBuildUdeb; /* Optimization level */
	string sModuleName; /* Modules name were leak is*/
	ADDRESS_TO_LINE_ITEM addressToLine; /* Data from locating code line for memory address */
};

/**
* Enumerations used in argument structure.
*/
enum MAIN_SWITCH
{
	SWITCH_UNKNOWN = 0,
	SWITCH_ANALYZE = 1,
	SWITCH_HOOK = 2,
	SWITCH_UNHOOK = 3,
	SWITCH_OLD_HOOK = 4,
	SWITCH_PARSE_TRACE = 5,
	SWITCH_CLEAN = 6,
	SWITCH_HTI_GET = 7,
	SWITCH_HTI_DELETE = 8,
	SWITCH_VERSION = 9,
	SWITCH_HELP = 10
};
enum HOOK_SWITCH
{
	HOOK_UNKNOWN = 0,
	HOOK_INTERNAL,
	HOOK_EXTERNAL, 
	HOOK_EXTERNAL_FAST,
	HOOK_EXTENSION_INTERNAL,
	HOOK_EXTENSION_EXTERNAL,
	HOOK_EXTENSION_EXTERNAL_FAST,
	HOOK_EXTENSION_UNINSTRUMENT,
	HOOK_EXTENSION_FAILED,
	HOOK_OLD_EXTENSION_INSTRUMENT,
	HOOK_OLD_EXTENSION_UNINSTRUMENT,
	HOOK_OLD_EXTENSION_FAILED
};

// Hook.
struct ARGUMENTS_HOOK
{
	bool bNoBuild;
	bool bDataFileName; //Internal data gathering filename defined.
	string sDataFileName; //Internal data gathering filename.
	int iBuildSystem; //1 = sbs, 2 = raptor.
	int iLoggingMode; //Just for old parameter parsing. 1=xti, 2=s60, 0=?.
	int iAllocCallStackSize;
	int iFreeCallStackSize;
	string sBuildCmd; //Original build command.
	vector<string> vBuildCmd; //Build command split in vector.
	string sPlatform; //Platform.
	string sBuildType; //Build type (udeb/urel).
	string sFeatureVariant;
	vector<string> vTargetPrograms;
	ARGUMENTS_HOOK()
	{
		bNoBuild = false;
		bDataFileName = false;
		sDataFileName = "";
		iBuildSystem = 0;
		iLoggingMode = 0;
		iAllocCallStackSize = AT_ALLOC_CALL_STACK_SIZE_DEFAULT;
		iFreeCallStackSize = AT_FREE_CALL_STACK_SIZE_DEFAULT;
		sBuildCmd = "";
		sPlatform = "";
		sBuildType = "";
		sFeatureVariant = "";
		vTargetPrograms.clear();
	};
};
// Analyze.
struct ARGUMENTS_ANALYZE
{
	string sDataFile;
	string sOutputFile;
	bool bSymbolFile;
	vector<string> vSymbolFiles;
	int iLoggingLevel;
	ARGUMENTS_ANALYZE()
	{
		sDataFile = "";
		sOutputFile = "";
		bSymbolFile = false;
		iLoggingLevel = 3;
	};
};
//Parsing trace.
struct ARGUMENTS_PARSE
{
	bool bDataFile;
	string sDataFile;
	bool bOutputFile;
	string sOutputFile;
	ARGUMENTS_PARSE()
	{
		bDataFile = false;
		sDataFile = "";
		bOutputFile = false;
		sOutputFile = "";
	};
};
//Hti get file.
struct ARGUMENTS_HTI
{
	string sFile;
	ARGUMENTS_HTI()
	{
		sFile = "";
	};
};
/**
* Struct to store parsed arguments from console.
*/
struct ARGUMENTS
{
	// Base arguments.
	MAIN_SWITCH eMainSwitch; // main switch
	HOOK_SWITCH eHookSwitch;
	bool bHelp; //Show help.
	bool bDebugConsole; //Debug logging to console.
	bool bDebugDbgView; //Debug logging to win api.
	bool bDebugLowLevel; //Debug also low level functions.
	bool bEnableSbs2; //Use raptor.
	ARGUMENTS_HOOK HOOK;
	ARGUMENTS_ANALYZE ANALYZE;
	ARGUMENTS_PARSE PARSE;
	ARGUMENTS_HTI HTI;
	ARGUMENTS()
	{
		eMainSwitch = SWITCH_UNKNOWN;
		eHookSwitch = HOOK_UNKNOWN;
		bHelp = false;
		bDebugConsole = false;
		bDebugDbgView = false;
		bEnableSbs2 = false;
	};
};
// DEBUG MACROS
// constant string for all beginning of all debug messages
const char START_LOG_MESSAGE[] = "*** CLE ";
// Log normal function entry (AAA = class::function )
#define LOG_FUNC_ENTRY( AAA ) { if( g_bDebugDbgView) { stringstream strs; strs << START_LOG_MESSAGE << "ENTRY: " << AAA << endl; OutputDebugString( strs.str().c_str() );  } else if ( g_bDebugConsole ) { stringstream strs; strs << START_LOG_MESSAGE << "ENTRY: " << AAA << endl; cout << strs.str();  } }
// Log normal function exit
#define LOG_FUNC_EXIT( AAA ){ if( g_bDebugDbgView) { stringstream strs; strs << START_LOG_MESSAGE << "EXIT: " << AAA << endl; OutputDebugString( strs.str().c_str() ); } else if ( g_bDebugConsole ) { stringstream strs; strs << START_LOG_MESSAGE << "EXIT: " << AAA << endl; cout << strs.str(); } }
// Log low level function entry
#define LOG_LOW_FUNC_ENTRY( AAA ) { if ( g_bDebugLowLevel ) { if( g_bDebugDbgView) { stringstream strs; strs << START_LOG_MESSAGE << AAA << endl; OutputDebugString( strs.str().c_str() ); } else if ( g_bDebugConsole ) { stringstream strs; strs << START_LOG_MESSAGE << AAA << endl; cout << strs.str(); } } }
// Log low level function exit
#define LOG_LOW_FUNC_EXIT( AAA ) {  if ( g_bDebugLowLevel ) { if( g_bDebugDbgView) { stringstream strs; strs << START_LOG_MESSAGE << AAA << endl; OutputDebugString( strs.str().c_str() ); } else if ( g_bDebugConsole ) { stringstream strs; strs << START_LOG_MESSAGE << AAA << endl; cout << strs.str(); } } }
// Log stringstream (string, int, etc..) i.e. string1 << int1 << "log this"
#define LOG_STRING( AAA ) { if( g_bDebugDbgView) { stringstream strs; strs << START_LOG_MESSAGE << AAA << endl; OutputDebugString( strs.str().c_str() ); } else if ( g_bDebugConsole ) { stringstream strs; strs << START_LOG_MESSAGE << AAA << endl; cout << strs.str(); } }

#endif