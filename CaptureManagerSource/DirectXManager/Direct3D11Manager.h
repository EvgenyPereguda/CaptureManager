#pragma once

#include <Unknwn.h>

struct IDXGIAdapter;
struct ID3D11Device;
struct ID3D11DeviceContext;
typedef
enum D3D_DRIVER_TYPE D3D_DRIVER_TYPE;
typedef
enum D3D_FEATURE_LEVEL D3D_FEATURE_LEVEL;

namespace CaptureManager
{
	namespace Core
	{
		namespace Direct3D11
		{


#ifdef _DEBUG_CAPTUREMANAGER
#define LOG_INVOKE_DX11_FUNCTION(Function, ...) lresult = Direct3D11::Direct3D11Manager::Function(__VA_ARGS__);\
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
#define LOG_INVOKE_DX11_FUNCTION(Function, ...) {\
	lresult = Direct3D11::Direct3D11Manager::Function(__VA_ARGS__);\
	if (FAILED(lresult))\
					{\
		break;\
				}\
				}\

#endif


#ifdef _DEBUG_CAPTUREMANAGER
#define LOG_INVOKE_DX11_METHOD(Function, ...) lresult = Direct3D11::Direct3D11Manager::Function(__VA_ARGS__);\
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
#define LOG_INVOKE_DX11_METHOD(Function, Object, ...) lresult = Object->Function(__VA_ARGS__);\
	if (FAILED(lresult))break;\

#endif
			
			typedef HRESULT(WINAPI *D3D11CreateDevice)(
				IDXGIAdapter*,
				D3D_DRIVER_TYPE,
				HMODULE,
				UINT,
				D3D_FEATURE_LEVEL*,
				UINT,
				UINT,
				ID3D11Device**,
				D3D_FEATURE_LEVEL*,
				ID3D11DeviceContext**);


			class Direct3D11Manager
			{
			public:

				static D3D11CreateDevice D3D11CreateDevice;


			public:

				HRESULT getState();

			protected:
				Direct3D11Manager();
				~Direct3D11Manager();

			private:

				HRESULT mState;

				HRESULT initFunctions(HMODULE aModule);





				
				static HRESULT WINAPI stubD3D11CreateDevice(
					IDXGIAdapter* aPtrAdapter,
					D3D_DRIVER_TYPE aDriverType,
					HMODULE aSoftware,
					UINT aFlags,
					D3D_FEATURE_LEVEL* aPtrFeatureLevels,
					UINT aFeatureLevels,
					UINT aSDKVersion,
					ID3D11Device** aPtrPtrDevice,
					D3D_FEATURE_LEVEL* aPtrFeatureLevel,
					ID3D11DeviceContext** aPtrPtrImmediateContext)
				{
					return E_NOTIMPL;
				}
			};
		}
	}
}