#pragma once

#include <Unknwnbase.h>
#include <vector>

#include "../Common/Common.h"


typedef struct _DMOMediaType DMO_MEDIA_TYPE;

#ifdef _DEBUG_CAPTUREMANAGER
#define LOG_INVOKE_DMO_FUNCTION(Function, ...) lresult = DMO::DMOManager::Function(__VA_ARGS__);\
	if (FAILED(lresult))\
				{\
	LogPrintOut::getInstance().printOutln(\
		LogPrintOut::ERROR_LEVEL,\
		#Function,\
		L" Error code: ",\
		lresult);\
	LogPrintOut::getInstance().printOutln(\
		LogPrintOut::ERROR_LEVEL,\
		L" Line: ", (DWORD)__LINE__, L", Func: ", __FUNCTIONW__,\
		L" Error code: ",\
		lresult);\
		break;\
			}\

#else
#define LOG_INVOKE_DMO_FUNCTION(Function, ...)lresult = DMO::DMOManager::Function(__VA_ARGS__);\
	if (FAILED(lresult))\
				{\
		break;\
			}\

#endif

namespace CaptureManager
{
	namespace Core
	{
		namespace DMO
		{
			typedef HRESULT(STDAPICALLTYPE *MoFreeMediaType)(
				DMO_MEDIA_TYPE*);

			class DMOManager
			{
			public:

				static MoFreeMediaType MoFreeMediaType;



				static HRESULT STDAPICALLTYPE stubMoFreeMediaType(
					DMO_MEDIA_TYPE*){
					return E_NOTIMPL;
				}

				HRESULT initialize();

			protected:

				DMOManager();
				virtual ~DMOManager();

			private:

				static HRESULT mResult;

				static std::vector<HMODULE> mModules;

				HRESULT loadLibraries();

				HRESULT initFunctions(HMODULE aModule);

				HRESULT fillPtrFuncCollection();
			};
		}
	}
}