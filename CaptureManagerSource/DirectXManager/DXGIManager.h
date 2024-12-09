#pragma once

#include <Unknwn.h>

#include "../Common/Common.h"

struct IMFDXGIBuffer;

namespace CaptureManager
{
	namespace Core
	{
		namespace DXGI
		{

#ifdef _DEBUG_CAPTUREMANAGER
#define LOG_INVOKE_DXGI_FUNCTION(Function, ...) lresult = DXGI::DXGIManager::Function(__VA_ARGS__);\
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
#define LOG_INVOKE_DXGI_FUNCTION(Function, ...) {\
	lresult = DXGI::DXGIManager::Function(__VA_ARGS__);\
	if (FAILED(lresult))\
						{\
		break;\
					}\
						}\

#endif


#ifdef _DEBUG_CAPTUREMANAGER
#define LOG_INVOKE_DXGI_METHOD(Function, ...) lresult = DXGI::DXGIManager::Function(__VA_ARGS__);\
	if (FAILED(lresult))\
							{\
	LogPrintOut::getInstance().printOutln(\
		LogPrintOut::ERROR_LEVEL,\
		L" Line: ", (DWORD)__LINE__, L", Func: ", __FUNCTIONW__,\
		L" Error code: ",\
		lresult);\
		break;\
						}\

#else
#define LOG_INVOKE_DXGI_METHOD(Function, Object, ...) lresult = Object->Function(__VA_ARGS__);\
	if (FAILED(lresult))break;\

#endif


			typedef HRESULT(WINAPI *CreateDXGIFactory1)(
				REFIID, 
				void **);

			class DXGIManager
			{
			public:

				static CreateDXGIFactory1 CreateDXGIFactory1;


			public:

				HRESULT getState();

				static HRESULT STDMETHODCALLTYPE GetResource(
					IMFDXGIBuffer* aPtrIMFDXGIBuffer,
					/* [annotation][in] */
					REFIID riid,
					/* [annotation][out] */
					LPVOID *ppvObject);

			protected:
				DXGIManager();
				virtual ~DXGIManager();

			private:

				HRESULT mState;

				HRESULT initFunctions(HMODULE aModule);


				static HRESULT WINAPI stubCreateDXGIFactory1(
					REFIID,
					void **)
				{
					return E_NOTIMPL;
				}
			};
		}
	}
}