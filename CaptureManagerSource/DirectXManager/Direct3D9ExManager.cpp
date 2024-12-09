/*
MIT License

Copyright(c) 2020 Evgeny Pereguda

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "Direct3D9ExManager.h"
#include "../LogPrintOut/LogPrintOut.h"
#include <dxva2api.h>
#include <VersionHelpers.h>

namespace CaptureManager
{
	namespace Core
	{
		namespace Direct3D9
		{
#ifndef CASTFUNCTION
#define CASTFUNCTION(Function){\
		auto lPtrFunc = GetProcAddress(aModule, #Function);\
 if (lPtrFunc != nullptr){\
auto lFunc = (CaptureManager::Core::Direct3D9::Function)(lPtrFunc);\
if (lFunc != nullptr)Function = lFunc;\
     }\
							}\

#endif


			Direct3DCreate9Ex Direct3D9ExManager::Direct3DCreate9Ex = Direct3D9ExManager::stubDirect3DCreate9Ex;

			DXVA2CreateDirect3DDeviceManager9 Direct3D9ExManager::DXVA2CreateDirect3DDeviceManager9
				= Direct3D9ExManager::stubDXVA2CreateDirect3DDeviceManager9;


			Direct3D9ExManager::Direct3D9ExManager()
			{
				HRESULT lresult = E_FAIL;

				do
				{
					//if (IsWindows7OrGreater() && !IsWindows8OrGreater())
					{
						auto lModule = LoadLibrary(L"d3d9.dll");

						if (lModule == nullptr)
						{
							lresult = E_HANDLE;

							break;
						}

						lresult = initFunctions(lModule);

						lModule = LoadLibrary(L"Dxva2.dll");

						if (lModule == nullptr)
						{
							lresult = E_HANDLE;

							break;
						}

						lresult = initFunctions(lModule);
					}

				} while (false);

				mState = lresult;
			}

			Direct3D9ExManager::~Direct3D9ExManager()
			{
			}

			// init collection of pointers on Direct3D9 functions
			HRESULT Direct3D9ExManager::initFunctions(HMODULE aModule)
			{

				HRESULT lresult = E_FAIL;

				do
				{
					CASTFUNCTION(Direct3DCreate9Ex);

					CASTFUNCTION(DXVA2CreateDirect3DDeviceManager9);

					lresult = S_OK;

				} while (false);

				return lresult;
			}

			HRESULT Direct3D9ExManager::getState()
			{
				return mState;
			}

			HRESULT Direct3D9ExManager::GetDeviceCaps(
				IDirect3D9* aPtrIDirect3D9,
				UINT aAdapter,
				D3DDEVTYPE aDeviceType,
				D3DCAPS9* aPtrCaps)
			{
				HRESULT lresult;

				do
				{
					if (aPtrIDirect3D9 == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					if (aPtrCaps == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					lresult = aPtrIDirect3D9->GetDeviceCaps(
						aAdapter,
						aDeviceType,
						aPtrCaps);

				} while (false);

				if (FAILED(lresult))
				{
					LogPrintOut::getInstance().printOutln(
						LogPrintOut::ERROR_LEVEL,
						__FUNCTIONW__,
						L" Error code: ",
						lresult);
				}

				return lresult;
			}

			HRESULT Direct3D9ExManager::GetAdapterDisplayMode(
				IDirect3D9* aPtrIDirect3D9,
				UINT aAdapter,
				D3DDISPLAYMODE* aPtrMode)
			{
				HRESULT lresult;

				do
				{
					if (aPtrIDirect3D9 == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					if (aPtrMode == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					lresult = aPtrIDirect3D9->GetAdapterDisplayMode(
						aAdapter,
						aPtrMode);

				} while (false);

				if (FAILED(lresult))
				{
					LogPrintOut::getInstance().printOutln(
						LogPrintOut::ERROR_LEVEL,
						__FUNCTIONW__,
						L" Error code: ",
						lresult);
				}

				return lresult;
			}

			HRESULT Direct3D9ExManager::ResetDevice(
				IDirect3DDeviceManager9* aPtrIDirect3DDeviceManager9,
				IDirect3DDevice9* aPtrDirect3DDevice9,
				UINT aResetToken)
			{
				HRESULT lresult;

				do
				{
					if (aPtrIDirect3DDeviceManager9 == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					if (aPtrDirect3DDevice9 == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					lresult = aPtrIDirect3DDeviceManager9->ResetDevice(
						aPtrDirect3DDevice9,
						aResetToken);

				} while (false);

				if (FAILED(lresult))
				{
					LogPrintOut::getInstance().printOutln(
						LogPrintOut::ERROR_LEVEL,
						__FUNCTIONW__,
						L" Error code: ",
						lresult);
				}

				return lresult;
			}

			HRESULT Direct3D9ExManager::OpenDeviceHandle(
				IDirect3DDeviceManager9* aPtrIDirect3DDeviceManager9,
				HANDLE* aPtrHANDLEDevice)
			{
				HRESULT lresult;

				do
				{
					if (aPtrIDirect3DDeviceManager9 == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					if (aPtrHANDLEDevice == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					lresult = aPtrIDirect3DDeviceManager9->OpenDeviceHandle(
						aPtrHANDLEDevice);

				} while (false);

				if (FAILED(lresult))
				{
					LogPrintOut::getInstance().printOutln(
						LogPrintOut::ERROR_LEVEL,
						__FUNCTIONW__,
						L" Error code: ",
						lresult);
				}

				return lresult;
			}

			HRESULT Direct3D9ExManager::CloseDeviceHandle(
				IDirect3DDeviceManager9* aPtrIDirect3DDeviceManager9,
				HANDLE aHANDLEDevice)
			{
				HRESULT lresult;

				do
				{
					if (aPtrIDirect3DDeviceManager9 == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					lresult = aPtrIDirect3DDeviceManager9->CloseDeviceHandle(
						aHANDLEDevice);

				} while (false);

				if (FAILED(lresult))
				{
					LogPrintOut::getInstance().printOutln(
						LogPrintOut::ERROR_LEVEL,
						__FUNCTIONW__,
						L" Error code: ",
						lresult);
				}

				return lresult;
			}

			HRESULT Direct3D9ExManager::LockDevice(
				IDirect3DDeviceManager9* aPtrIDirect3DDeviceManager9,
				HANDLE aHANDLEDevice,
				IDirect3DDevice9** aPtrPtrDevice,
				BOOL aBlock)
			{
				HRESULT lresult;

				do
				{
					if (aPtrIDirect3DDeviceManager9 == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					if (aPtrPtrDevice == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					lresult = aPtrIDirect3DDeviceManager9->LockDevice(
						aHANDLEDevice,
						aPtrPtrDevice,
						aBlock);

				} while (false);

				if (FAILED(lresult))
				{
					LogPrintOut::getInstance().printOutln(
						LogPrintOut::ERROR_LEVEL,
						__FUNCTIONW__,
						L" Error code: ",
						lresult);
				}

				return lresult;
			}

			HRESULT Direct3D9ExManager::UnlockDevice(
				IDirect3DDeviceManager9* aPtrIDirect3DDeviceManager9,
				HANDLE aHANDLEDevice,
				BOOL aBlock)
			{
				HRESULT lresult;

				do
				{
					if (aPtrIDirect3DDeviceManager9 == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					lresult = aPtrIDirect3DDeviceManager9->UnlockDevice(
						aHANDLEDevice,
						aBlock);

				} while (false);

				if (FAILED(lresult))
				{
					LogPrintOut::getInstance().printOutln(
						LogPrintOut::ERROR_LEVEL,
						__FUNCTIONW__,
						L" Error code: ",
						lresult);
				}

				return lresult;
			}

			HRESULT Direct3D9ExManager::CreateDevice(
				IDirect3D9* aPtrIDirect3D9,
				UINT aAdapter,
				D3DDEVTYPE aDeviceType,
				HWND aHWNDFocusWindow,
				DWORD aBehaviorFlags,
				D3DPRESENT_PARAMETERS* aPtrPresentationParameters,
				IDirect3DDevice9** aPtrPtrReturnedDeviceInterface)
			{
				HRESULT lresult;

				do
				{
					if (aPtrIDirect3D9 == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					if (aPtrPresentationParameters == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					if (aPtrPtrReturnedDeviceInterface == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					lresult = aPtrIDirect3D9->CreateDevice(
						aAdapter,
						aDeviceType,
						aHWNDFocusWindow,
						aBehaviorFlags,
						aPtrPresentationParameters,
						aPtrPtrReturnedDeviceInterface);

				} while (false);

				if (FAILED(lresult))
				{
					LogPrintOut::getInstance().printOutln(
						LogPrintOut::ERROR_LEVEL,
						__FUNCTIONW__,
						L" Error code: ",
						lresult);
				}

				return lresult;
			}
					   			 
			HRESULT Direct3D9ExManager::CreateDeviceEx(
				IDirect3D9Ex* aPtrIDirect3D9Ex,
				UINT                  Adapter,
				D3DDEVTYPE            DeviceType,
				HWND                  hFocusWindow,
				DWORD                 BehaviorFlags,
				D3DPRESENT_PARAMETERS *pPresentationParameters,
				D3DDISPLAYMODEEX      *pFullscreenDisplayMode,
				IDirect3DDevice9Ex    **ppReturnedDeviceInterface)
			{
				HRESULT lresult;

				do
				{
					if (aPtrIDirect3D9Ex == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					if (pPresentationParameters == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					if (ppReturnedDeviceInterface == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					lresult = aPtrIDirect3D9Ex->CreateDeviceEx(
						Adapter,
						DeviceType,
						hFocusWindow,
						BehaviorFlags,
						pPresentationParameters,
						pFullscreenDisplayMode,
						ppReturnedDeviceInterface);

				} while (false);

				if (FAILED(lresult))
				{
					LogPrintOut::getInstance().printOutln(
						LogPrintOut::ERROR_LEVEL,
						__FUNCTIONW__,
						L" Error code: ",
						lresult);
				}

				return lresult;
			}

			HRESULT Direct3D9ExManager::CreateOffscreenPlainSurface(
				IDirect3DDevice9* aPtrIDirect3DDevice9,
				UINT aWidth,
				UINT aHeight,
				D3DFORMAT aFormat,
				D3DPOOL aPool,
				IDirect3DSurface9** aPtrPtrSurface,
				HANDLE* aPtrSharedHandle)
			{
				HRESULT lresult;

				do
				{
					if (aPtrIDirect3DDevice9 == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					if (aPtrPtrSurface == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					lresult = aPtrIDirect3DDevice9->CreateOffscreenPlainSurface(
						aWidth,
						aHeight,
						aFormat,
						aPool,
						aPtrPtrSurface,
						aPtrSharedHandle);

				} while (false);

				if (FAILED(lresult))
				{
					LogPrintOut::getInstance().printOutln(
						LogPrintOut::ERROR_LEVEL,
						__FUNCTIONW__,
						L" Error code: ",
						lresult);
				}

				return lresult;
			}

			HRESULT Direct3D9ExManager::ColorFill(
				IDirect3DDevice9* aPtrIDirect3DDevice9,
				IDirect3DSurface9* aPtrSurface,
				CONST RECT* aPtrRect,
				D3DCOLOR aColor)
			{
				HRESULT lresult;

				do
				{
					if (aPtrIDirect3DDevice9 == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					if (aPtrSurface == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					lresult = aPtrIDirect3DDevice9->ColorFill(
						aPtrSurface,
						aPtrRect,
						aColor);

				} while (false);

				if (FAILED(lresult))
				{
					LogPrintOut::getInstance().printOutln(
						LogPrintOut::ERROR_LEVEL,
						__FUNCTIONW__,
						L" Error code: ",
						lresult);
				}

				return lresult;
			}

			HRESULT Direct3D9ExManager::GetSwapChain(
				IDirect3DDevice9* aPtrIDirect3DDevice9,
				UINT aSwapChain,
				IDirect3DSwapChain9** aPtrPtrSwapChain)
			{
				HRESULT lresult;

				do
				{
					if (aPtrIDirect3DDevice9 == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					if (aPtrPtrSwapChain == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					lresult = aPtrIDirect3DDevice9->GetSwapChain(
						aSwapChain,
						aPtrPtrSwapChain);

				} while (false);

				if (FAILED(lresult))
				{
					LogPrintOut::getInstance().printOutln(
						LogPrintOut::ERROR_LEVEL,
						__FUNCTIONW__,
						L" Error code: ",
						lresult);
				}

				return lresult;
			}

			HRESULT Direct3D9ExManager::Reset(
				IDirect3DDevice9* aPtrIDirect3DDevice9,
				D3DPRESENT_PARAMETERS* aPtrPresentationParameters)
			{
				HRESULT lresult;

				do
				{
					if (aPtrIDirect3DDevice9 == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					if (aPtrPresentationParameters == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					lresult = aPtrIDirect3DDevice9->Reset(
						aPtrPresentationParameters);

				} while (false);

				if (FAILED(lresult))
				{
					LogPrintOut::getInstance().printOutln(
						LogPrintOut::ERROR_LEVEL,
						__FUNCTIONW__,
						L" Error code: ",
						lresult);
				}

				return lresult;
			}

			HRESULT Direct3D9ExManager::GetBackBuffer(
				IDirect3DSwapChain9* aPtrIDirect3DSwapChain9,
				UINT aBackBuffer,
				D3DBACKBUFFER_TYPE aType,
				IDirect3DSurface9** aPtrPtrBackBuffer)
			{
				HRESULT lresult;

				do
				{
					if (aPtrIDirect3DSwapChain9 == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					if (aPtrPtrBackBuffer == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					lresult = aPtrIDirect3DSwapChain9->GetBackBuffer(
						aBackBuffer,
						aType,
						aPtrPtrBackBuffer);

				} while (false);

				if (FAILED(lresult))
				{
					LogPrintOut::getInstance().printOutln(
						LogPrintOut::ERROR_LEVEL,
						__FUNCTIONW__,
						L" Error code: ",
						lresult);
				}

				return lresult;
			}

			HRESULT Direct3D9ExManager::GetPresentParameters(
				IDirect3DSwapChain9* aPtrIDirect3DSwapChain9,
				D3DPRESENT_PARAMETERS* aPtrPresentationParameters)
			{
				HRESULT lresult;

				do
				{
					if (aPtrIDirect3DSwapChain9 == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					if (aPtrPresentationParameters == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					lresult = aPtrIDirect3DSwapChain9->GetPresentParameters(
						aPtrPresentationParameters);

				} while (false);

				if (FAILED(lresult))
				{
					LogPrintOut::getInstance().printOutln(
						LogPrintOut::ERROR_LEVEL,
						__FUNCTIONW__,
						L" Error code: ",
						lresult);
				}

				return lresult;
			}

			HRESULT Direct3D9ExManager::GetFrontBufferData(
				IDirect3DDevice9* aPtrIDirect3DDevice9,
				UINT aSwapChain,
				IDirect3DSurface9* aPtrDestSurface)
			{
				HRESULT lresult;

				do
				{
					if (aPtrIDirect3DDevice9 == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					if (aPtrDestSurface == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					lresult = aPtrIDirect3DDevice9->GetFrontBufferData(
						aSwapChain,
						aPtrDestSurface);

				} while (false);

				if (FAILED(lresult))
				{
					LogPrintOut::getInstance().printOutln(
						LogPrintOut::ERROR_LEVEL,
						__FUNCTIONW__,
						L" Error code: ",
						lresult);
				}

				return lresult;
			}

			HRESULT Direct3D9ExManager::GetDC(
				IDirect3DSurface9* aPtrDestSurface,
				HDC* aPtrHDC)
			{
				HRESULT lresult;

				do
				{
					if (aPtrDestSurface == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					if (aPtrHDC == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					lresult = aPtrDestSurface->GetDC(aPtrHDC);

				} while (false);

				if (FAILED(lresult))
				{
					LogPrintOut::getInstance().printOutln(
						LogPrintOut::ERROR_LEVEL,
						__FUNCTIONW__,
						L" Error code: ",
						lresult);
				}

				return lresult;
			}

			HRESULT Direct3D9ExManager::ReleaseDC(
				IDirect3DSurface9* aPtrDestSurface,
				HDC aHDC)
			{
				HRESULT lresult;

				do
				{
					if (aPtrDestSurface == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					lresult = aPtrDestSurface->ReleaseDC(aHDC);

				} while (false);

				if (FAILED(lresult))
				{
					LogPrintOut::getInstance().printOutln(
						LogPrintOut::ERROR_LEVEL,
						__FUNCTIONW__,
						L" Error code: ",
						lresult);
				}

				return lresult;
			}

			HRESULT Direct3D9ExManager::LockRect(
				IDirect3DSurface9* aPtrDestSurface,
				D3DLOCKED_RECT* aPtrLockedRect,
				CONST RECT* aPtrRect,
				DWORD aFlags)
			{
				HRESULT lresult;

				do
				{
					if (aPtrDestSurface == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					if (aPtrLockedRect == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					if (aPtrRect == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					lresult = aPtrDestSurface->LockRect(
						aPtrLockedRect,
						aPtrRect,
						aFlags);

				} while (false);

				if (FAILED(lresult))
				{
					LogPrintOut::getInstance().printOutln(
						LogPrintOut::ERROR_LEVEL,
						__FUNCTIONW__,
						L" Error code: ",
						lresult);
				}

				return lresult;
			}

			HRESULT Direct3D9ExManager::UnlockRect(
				IDirect3DSurface9* aPtrDestSurface)
			{
				HRESULT lresult;

				do
				{
					if (aPtrDestSurface == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					lresult = aPtrDestSurface->UnlockRect();

				} while (false);

				if (FAILED(lresult))
				{
					LogPrintOut::getInstance().printOutln(
						LogPrintOut::ERROR_LEVEL,
						__FUNCTIONW__,
						L" Error code: ",
						lresult);
				}

				return lresult;
			}

		}
	}
}
