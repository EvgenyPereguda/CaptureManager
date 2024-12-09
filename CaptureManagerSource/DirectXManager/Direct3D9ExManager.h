#pragma once

#include <Unknwn.h>
#include <d3d9.h>

#include "../Common/Common.h"

struct IDirect3D9;
struct IDirect3D9Ex;
struct IDirect3DDeviceManager9;

namespace CaptureManager
{
	namespace Core
	{
		namespace Direct3D9
		{

#ifdef _DEBUG_CAPTUREMANAGER
#define LOG_INVOKE_DX9EX_FUNCTION(Function, ...) lresult = Direct3D9ExManager::Function(__VA_ARGS__);\
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
#define LOG_INVOKE_DX9EX_FUNCTION(Function, ...) {\
	lresult = Direct3D9ExManager::Function(__VA_ARGS__);\
	if (FAILED(lresult))\
					{\
		break;\
				}\
				}\

#endif


#ifdef _DEBUG_CAPTUREMANAGER
#define LOG_INVOKE_DX9EX_METHOD(Function, ...) lresult = Direct3D9ExManager::Function(__VA_ARGS__);\
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
#define LOG_INVOKE_DX9EX_METHOD(Function, Object, ...) lresult = Object->Function(__VA_ARGS__);\
	if (FAILED(lresult))break;\

#endif

			
			typedef HRESULT(STDAPICALLTYPE *Direct3DCreate9Ex)(
				UINT,
				IDirect3D9Ex**);

			typedef HRESULT(STDAPICALLTYPE *DXVA2CreateDirect3DDeviceManager9)(
				UINT*,
				IDirect3DDeviceManager9**);

			class Direct3D9ExManager
			{
			public:

				static Direct3DCreate9Ex Direct3DCreate9Ex;

				static DXVA2CreateDirect3DDeviceManager9 DXVA2CreateDirect3DDeviceManager9;

			public:


				HRESULT getState();

				static HRESULT GetDeviceCaps(
					IDirect3D9* aPtrIDirect3D9,
					UINT aAdapter,
					D3DDEVTYPE aDeviceType,
					D3DCAPS9* aPtrCaps);

				static HRESULT GetAdapterDisplayMode(
					IDirect3D9* aPtrIDirect3D9,
					UINT aAdapter,
					D3DDISPLAYMODE* aPtrMode);

				static HRESULT CreateDevice(
					IDirect3D9* aPtrIDirect3D9,
					UINT aAdapter,
					D3DDEVTYPE aDeviceType,
					HWND aHWNDFocusWindow,
					DWORD aBehaviorFlags,
					D3DPRESENT_PARAMETERS* aPtrPresentationParameters,
					IDirect3DDevice9** aPtrPtrReturnedDeviceInterface);

				static HRESULT CreateDeviceEx(
					IDirect3D9Ex*		aPtrIDirect3D9Ex,
					UINT                  Adapter,
					D3DDEVTYPE            DeviceType,
					HWND                  hFocusWindow,
					DWORD                 BehaviorFlags,
					D3DPRESENT_PARAMETERS *pPresentationParameters,
					D3DDISPLAYMODEEX      *pFullscreenDisplayMode,
					IDirect3DDevice9Ex    **ppReturnedDeviceInterface
				);

				static HRESULT CreateOffscreenPlainSurface(
					IDirect3DDevice9* aPtrIDirect3DDevice9,
					UINT aWidth,
					UINT aHeight,
					D3DFORMAT aFormat,
					D3DPOOL aPool,
					IDirect3DSurface9** aPtrPtrSurface,
					HANDLE* aPtrSharedHandle);

				static HRESULT GetSwapChain(
					IDirect3DDevice9* aPtrIDirect3DDevice9,
					UINT aSwapChain,
					IDirect3DSwapChain9** aPtrPtrSwapChain);

				static HRESULT Reset(
					IDirect3DDevice9* aPtrIDirect3DDevice9,
					D3DPRESENT_PARAMETERS* aPtrPresentationParameters);

				static HRESULT ColorFill(
					IDirect3DDevice9* aPtrIDirect3DDevice9,
					IDirect3DSurface9* aPtrSurface,
					CONST RECT* aPtrRect,
					D3DCOLOR aColor);

				static HRESULT GetPresentParameters(
					IDirect3DSwapChain9* aPtrIDirect3DSwapChain9,
					D3DPRESENT_PARAMETERS* aPtrPresentationParameters);

				static HRESULT ResetDevice(
					IDirect3DDeviceManager9* aPtrIDirect3DDeviceManager9,
					IDirect3DDevice9* aPtrDirect3DDevice9,
					UINT aResetToken);

				static HRESULT OpenDeviceHandle(
					IDirect3DDeviceManager9* aPtrIDirect3DDeviceManager9,
					HANDLE* aPtrHANDLEDevice);

				static HRESULT CloseDeviceHandle(
					IDirect3DDeviceManager9* aPtrIDirect3DDeviceManager9,
					HANDLE aHANDLEDevice);

				static HRESULT LockDevice(
					IDirect3DDeviceManager9* aPtrIDirect3DDeviceManager9,
					HANDLE aHANDLEDevice,
					IDirect3DDevice9** aPtrPtrDevice,
					BOOL aBlock);

				static HRESULT UnlockDevice(
					IDirect3DDeviceManager9* aPtrIDirect3DDeviceManager9,
					HANDLE aHANDLEDevice,
					BOOL aBlock);

				static HRESULT GetFrontBufferData(
					IDirect3DDevice9* aPtrIDirect3DDevice9,
					UINT aSwapChain,
					IDirect3DSurface9* aPtrDestSurface);

				static HRESULT GetDC(
					IDirect3DSurface9* aPtrDestSurface,
					HDC* aPtrHDC);

				static HRESULT ReleaseDC(
					IDirect3DSurface9* aPtrDestSurface,
					HDC aHDC);

				static HRESULT LockRect(
					IDirect3DSurface9* aPtrDestSurface,
					D3DLOCKED_RECT* aPtrLockedRect,
					CONST RECT* aPtrRect,
					DWORD aFlags);

				static HRESULT UnlockRect(
					IDirect3DSurface9* aPtrDestSurface);

				static HRESULT GetBackBuffer(
					IDirect3DSwapChain9* aPtrIDirect3DSwapChain9,
					UINT aBackBuffer,
					D3DBACKBUFFER_TYPE aType,
					IDirect3DSurface9** aPtrPtrBackBuffer);

			protected:
				Direct3D9ExManager();
				~Direct3D9ExManager();

			private:

				HRESULT mState;

				
				static HRESULT STDAPICALLTYPE stubDirect3DCreate9Ex(
					UINT,
					IDirect3D9Ex**)
				{
					return E_NOTIMPL;
				}


				static HRESULT STDAPICALLTYPE stubDXVA2CreateDirect3DDeviceManager9(
					UINT*,
					IDirect3DDeviceManager9**)
				{
					return E_NOTIMPL;
				}

				HRESULT initFunctions(HMODULE aModule);

				Direct3D9ExManager(
					const Direct3D9ExManager&) = delete;
				Direct3D9ExManager& operator=(
					const Direct3D9ExManager&) = delete;
			};

		}
	}
}

