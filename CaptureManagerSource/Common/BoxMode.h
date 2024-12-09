#pragma once

#include "Common.h"

#if defined( _DEBUG_CAPTUREMANAGER )
#define CAPTUREMANAGER_BOX public:
#else
#define CAPTUREMANAGER_BOX protected:
#endif
