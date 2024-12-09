#pragma once

#include <Unknwnbase.h>
#include <vector>


//#ifdef _DEBUG
//#define LOG_INVOKE_AVRT_FUNCTION(Function, ...) lresult = Core::AvrtManager::Function(__VA_ARGS__);\
//	if (FAILED(lresult))\
//					{\
//	LogPrintOut::getInstance().printOutln(\
//		LogPrintOut::ERROR_LEVEL,\
//		#Function,\
//		L" Error code: ",\
//		lresult);\
//	LogPrintOut::getInstance().printOutln(\
//		LogPrintOut::ERROR_LEVEL,\
//		L" Line: ", (DWORD)__LINE__, L", Func: ", __FUNCTIONW__,\
//		L" Error code: ",\
//		lresult);\
//		break;\
//				}\
//
//#else
//#define LOG_INVOKE_AVRT_FUNCTION(Function, ...)lresult = Core::AvrtManager::Function(__VA_ARGS__);\
//	if (FAILED(lresult))\
//					{\
//		break;\
//				}\
//
//#endif

typedef enum _AVRT_PRIORITY_AvrtManager
{
	AVRT_PRIORITY_VERYLOW_AvrtManager = -2,
	AVRT_PRIORITY_LOW_AvrtManager,
	AVRT_PRIORITY_NORMAL_AvrtManager,
	AVRT_PRIORITY_HIGH_AvrtManager,
	AVRT_PRIORITY_CRITICAL_AvrtManager
} AVRT_PRIORITY_AvrtManager;

namespace CaptureManager
{
	namespace Core
	{
		typedef HANDLE(WINAPI *AvSetMmThreadCharacteristicsW)(
			LPCWSTR,
			LPDWORD);

		typedef BOOL(WINAPI *AvRevertMmThreadCharacteristics)(
			HANDLE);

		typedef BOOL(WINAPI *AvSetMmThreadPriority)(
			HANDLE,
			AVRT_PRIORITY_AvrtManager);
		
		class AvrtManager
		{
		public:

			HRESULT initialize();



			static AvSetMmThreadCharacteristicsW AvSetMmThreadCharacteristicsW;
			static AvRevertMmThreadCharacteristics AvRevertMmThreadCharacteristics;
			static AvSetMmThreadPriority AvSetMmThreadPriority;




		protected:
			AvrtManager();
			virtual ~AvrtManager();

		private:

			static HRESULT mResult;

			static std::vector<HMODULE> mModules;

			HRESULT loadLibraries();

			HRESULT initFunctions(HMODULE aModule);

			HRESULT fillPtrFuncCollection();


			static HANDLE WINAPI stubAvSetMmThreadCharacteristicsW(
				LPCWSTR,
				LPDWORD){
				return nullptr;
			}

			static BOOL WINAPI stubAvRevertMmThreadCharacteristics(
				HANDLE){
				return FALSE;
			}

			static BOOL WINAPI stubAvSetMmThreadPriority(
				HANDLE,
				AVRT_PRIORITY_AvrtManager){
				return FALSE;
			}
		};
	}
}