#pragma once

#include <Unknwnbase.h>
#include <vector>


#ifdef _DEBUG
#define LOG_INVOKE_WINMM_FUNCTION(Function, ...) lresult = CaptureManager::Core::Winmm::WinmmManager::Function(__VA_ARGS__);\
	if (FAILED(lresult))\
				{\
	CaptureManager::LogPrintOut::getInstance().printOutln(\
		CaptureManager::LogPrintOut::ERROR_LEVEL,\
		#Function,\
		L" Error code: ",\
		lresult);\
	CaptureManager::LogPrintOut::getInstance().printOutln(\
		CaptureManager::LogPrintOut::ERROR_LEVEL,\
		L" Line: ", (DWORD)__LINE__, L", Func: ", __FUNCTIONW__,\
		L" Error code: ",\
		lresult);\
		break;\
			}\

#else
#define LOG_INVOKE_WINMM_FUNCTION(Function, ...)lresult = CaptureManager::Core::Winmm::WinmmManager::Function(__VA_ARGS__);\
	if (FAILED(lresult))\
				{\
		break;\
			}\

#endif

namespace CaptureManager
{
	namespace Core
	{
		namespace Winmm
		{
			typedef MMRESULT(WINAPI* timeKillEvent)(
				UINT);

			typedef DWORD(WINAPI *timeGetTime)();

			typedef MMRESULT(WINAPI* timeBeginPeriod)(
				UINT);

			typedef MMRESULT(WINAPI* timeEndPeriod)(
				UINT);

			typedef MMRESULT(WINAPI* timeSetEvent)(
				UINT,
				UINT,
				LPTIMECALLBACK,
				DWORD_PTR,
				UINT);


			typedef MMRESULT(WINAPI* timeGetDevCaps)(
				LPTIMECAPS ptc,
				UINT cbtc
				);

			class WinmmManager
			{
			public:

				static timeKillEvent timeKillEvent;

				static timeGetTime timeGetTime;

				static timeBeginPeriod timeBeginPeriod;

				static timeEndPeriod timeEndPeriod;

				static timeSetEvent timeSetEvent;

				static timeGetDevCaps timeGetDevCaps;
				


				static MMRESULT WINAPI stubtimeKillEvent(
					UINT){
					return E_NOTIMPL;
				}
				
				static DWORD WINAPI stubtimeGetTime(){
					return 0;
				}


				static MMRESULT WINAPI stubtimeBeginPeriod(
					UINT){
					return E_NOTIMPL;
				}


				static MMRESULT WINAPI stubtimeEndPeriod(
					UINT){
					return E_NOTIMPL;
				}

				static MMRESULT WINAPI stubtimeSetEvent(
					UINT,
					UINT,
					LPTIMECALLBACK,
					DWORD_PTR,
					UINT){
					return E_NOTIMPL;
				}

				static MMRESULT WINAPI stubtimeGetDevCaps(
					LPTIMECAPS ptc,
					UINT cbtc
					){
					return E_NOTIMPL;
				}

				HRESULT initialize();

			protected:

				WinmmManager();
				virtual ~WinmmManager();

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