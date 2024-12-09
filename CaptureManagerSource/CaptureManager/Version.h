#pragma once

#include "VersionBuild.h"
#include "../Common/Macros.h"

#define VER_FILE_DESCRIPTION_STR    "Video and audio sources capturing library"
#define VER_FILE_VERSION            VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_BUILD
#define VER_FILE_VERSION_STR        STRINGIZE(VERSION_MAJOR)        \
	"." STRINGIZE(VERSION_MINOR)    \
	"." STRINGIZE(VERSION_PATCH) \
	"." STRINGIZE(VERSION_BUILD)
#define VER_PRODUCT_VERSION_STR        STRINGIZE(VERSION_MAJOR)        \
	"." STRINGIZE(VERSION_MINOR)    \
	"." STRINGIZE(VERSION_PATCH) \
	" " STRINGIZE(ADDITIONAL_LABEL)
#define VER_PRODUCTNAME_STR         "Capture Manager"
#define VER_PRODUCT_VERSION         VER_FILE_VERSION
#define VER_ORIGINAL_FILENAME_STR   "CaptureManager.dll"
#define VER_INTERNAL_NAME_STR       VER_ORIGINAL_FILENAME_STR
#define VER_COPYRIGHT_STR           "Copyright (C) 2016"
#ifdef _DEBUG
#define VER_VER_DEBUG             VS_FF_DEBUG
#else
#define VER_VER_DEBUG             0
#endif
#define VER_FILEOS                  VOS_NT_WINDOWS32
#define VER_FILEFLAGS               VER_VER_DEBUG
#define VER_FILETYPE                VFT_DLL
