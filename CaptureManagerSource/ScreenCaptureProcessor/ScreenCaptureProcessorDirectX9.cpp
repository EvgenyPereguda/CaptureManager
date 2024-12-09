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

#include "ScreenCaptureProcessorDirectX9.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
//#include "../DirectXManager/ScreenCaptureDirect3D9Manager.h"
#include "../DirectXManager/Direct3D9ExManager.h"
#include "../Common/MFHeaders.h"
#include "../Common/Common.h"
#include "../DataParser/DataParser.h"
#include "ScreenCaptureProcessorFactory.h"
#include "../Common/InstanceMaker.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Singleton.h"
#include "../ConfigManager/ConfigManager.h"


static CaptureManager::Core::InstanceMaker<CaptureManager::Sources::ScreenCapture::ScreenCaptureProcessorDirectX9,
	CaptureManager::Sources::ScreenCaptureProcessorFactory> staticInstanceMaker(0);

namespace CaptureManager
{
	namespace Sources
	{
		namespace ScreenCapture
		{
			using namespace Core;

			using namespace Core::MediaFoundation;

			using namespace Core::Direct3D9;
						
			ScreenCaptureProcessorDirectX9::ScreenCaptureProcessorDirectX9()				
			{
				ZeroMemory(&mDesktopCoordinates, sizeof(mDesktopCoordinates));
			}
			
			ScreenCaptureProcessorDirectX9::~ScreenCaptureProcessorDirectX9()
			{
			}

			HRESULT ScreenCaptureProcessorDirectX9::check()
			{
				HRESULT lresult(E_FAIL);
				
				do
				{
					LOG_INVOKE_OBJECT_METHOD(Singleton<Direct3D9ExManager>::getInstance(), getState);
					
					D3DDISPLAYMODE	lddm;

					D3DPRESENT_PARAMETERS	ld3dpp;
					
					CComPtrCustom<IDirect3D9Ex> lDirect3D;

					lresult = Direct3D9ExManager::Direct3DCreate9Ex(D3D_SDK_VERSION, &lDirect3D);

					LOG_CHECK_PTR_MEMORY(lDirect3D);

					HWND lDesktopHWND = GetDesktopWindow();

					HMONITOR lMonitor = MonitorFromWindow(lDesktopHWND, MONITOR_DEFAULTTONEAREST);

					auto lAdapterCount = lDirect3D->GetAdapterCount();

					UINT l_currentAdapter = D3DADAPTER_DEFAULT;

					for (size_t i = 0; i < lAdapterCount; i++)
					{
						auto lAdapterMonitor = lDirect3D->GetAdapterMonitor(i);

						if (lAdapterMonitor == lMonitor)
						{
							l_currentAdapter = i;

							break;
						}
					}
					
					D3DCAPS9 lcaps;

					LOG_INVOKE_DX9EX_METHOD(GetDeviceCaps, lDirect3D,
						l_currentAdapter, // Означает первичный видеоадаптер.
						D3DDEVTYPE_HAL,   // Задает тип устройства, обычно D3DDEVTYPE_HAL.
						&lcaps);       // Возвращает заполненную структуру D3DCAPS9, которая содержит
									   // информацию о возможностях первичного видеоадаптера.
					
					// Поддерживается аппаратная обработка вершин?
					int lvertexprocessing = 0;

					if (lcaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
					{
						// да, сохраняем в vp флаг поддержки аппаратной
						// обработки вершин.
						lvertexprocessing = D3DCREATE_HARDWARE_VERTEXPROCESSING;
					}
					else
					{
						// Нет, сохраняем в vp флаг использования программной
						// обработки вершин.
						lvertexprocessing = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
					}

					LOG_INVOKE_DX9EX_METHOD(GetAdapterDisplayMode, lDirect3D, l_currentAdapter, &lddm);
					

					ZeroMemory(&ld3dpp, sizeof(D3DPRESENT_PARAMETERS));

					ld3dpp.Windowed = TRUE;
					ld3dpp.Flags = D3DPRESENTFLAG_VIDEO;// D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
					ld3dpp.BackBufferFormat = lddm.Format;
					ld3dpp.BackBufferHeight = lddm.Height;
					ld3dpp.BackBufferWidth = lddm.Width;
					ld3dpp.BackBufferCount = 0;
					ld3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
					ld3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
					ld3dpp.EnableAutoDepthStencil = FALSE;
					ld3dpp.hDeviceWindow = lDesktopHWND;
					ld3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;// D3DPRESENT_INTERVAL_DEFAULT;
					ld3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

					CComPtrCustom<IDirect3DDevice9Ex> lDirect3DDevice;

					lDirect3D->CreateDeviceEx(l_currentAdapter, D3DDEVTYPE_HAL, lDesktopHWND, lvertexprocessing, &ld3dpp, nullptr, &lDirect3DDevice);
					
					LOG_CHECK_PTR_MEMORY(lDirect3DDevice);
										
					lresult = S_OK;

				} while (false);
				
				return lresult;
			}

			HRESULT ScreenCaptureProcessorDirectX9::enumIInnerCaptureProcessor(
				UINT aIndex,
				UINT aOrientation,
				IInnerCaptureProcessor** aPtrPtrIInnerCaptureProcessor)
			{
				HRESULT lresult = E_FAIL;

				do
				{
					LOG_CHECK_STATE(aIndex != 0);

					CComPtrCustom<IInnerCaptureProcessor> lIInnerCaptureProcessor(new (std::nothrow) ScreenCaptureProcessorDirectX9);

					LOG_CHECK_PTR_MEMORY(lIInnerCaptureProcessor);

					LOG_INVOKE_QUERY_INTERFACE_METHOD(lIInnerCaptureProcessor, aPtrPtrIInnerCaptureProcessor);

				} while (false);

				return lresult;
			}


			HRESULT ScreenCaptureProcessorDirectX9::fillVectorScreenCaptureConfigs(
				std::vector<ScreenCaptureConfig>& aRefVectorScreenCaptureConfigs)
			{
				HRESULT lresult;

				int lWidth = GetSystemMetrics(SM_CXSCREEN);

				int lHeight = GetSystemMetrics(SM_CYSCREEN);

				mDesktopSize.cx = lWidth;

				mDesktopSize.cy = lHeight;

				if (mHWNDHandler.misEnable)
				{
					if (mHWNDHandler.mIsWindow)
					{
						RECT lWindowRect;

						auto l_bres = GetWindowRect(mHWNDHandler.mHandler, &lWindowRect);

						if (l_bres != FALSE)
						{

							RECT lClientRect;

							l_bres = GetClientRect(mHWNDHandler.mHandler, &lClientRect);

							lHeight = (((int)::abs(lWindowRect.top - lWindowRect.bottom)) >> 1) << 1;

							int lWindowWidth = (((int)::abs(lWindowRect.right - lWindowRect.left)) >> 1) << 1;

							int lClientWidth = (lClientRect.right >> 1) << 1;

							if (!Singleton<ConfigManager>::getInstance().isWindows10_Or_Greater())
							{
								lWidth = lWindowWidth;
							}
							else
							{
								lWidth = lClientWidth;

								int lBorderThickness = (lWindowWidth - lClientWidth) >> 1;

								lHeight -= lBorderThickness;
							}
						}
					}
					else
					{
						RECT lClientRect;

						auto l_bres = GetClientRect(mHWNDHandler.mHandler, &lClientRect);

						if (l_bres != FALSE)
						{
							lWidth = (lClientRect.right >> 1) << 1;

							lHeight = (lClientRect.bottom >> 1) << 1;
						}
					}
				}

				if (mClipResource.misEnable)
				{
					lWidth = mClipResource.mWidth;

					lHeight = mClipResource.mHeight;
				}

				UINT32 lSizeImage = 0;

				do
				{
					aRefVectorScreenCaptureConfigs.clear();

					BITMAPINFO	lBmpInfo;

					// BMP 32 bpp

					ZeroMemory(&lBmpInfo, sizeof(BITMAPINFO));

					lBmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

					lBmpInfo.bmiHeader.biBitCount = 32;

					lBmpInfo.bmiHeader.biCompression = BI_RGB;

					lBmpInfo.bmiHeader.biWidth = lWidth;

					lBmpInfo.bmiHeader.biHeight = lHeight;

					lBmpInfo.bmiHeader.biPlanes = 1;


					LOG_INVOKE_MF_FUNCTION(MFCalculateImageSize,
						MFVideoFormat_RGB32,
						lBmpInfo.bmiHeader.biWidth,
						lBmpInfo.bmiHeader.biHeight,
						&lSizeImage);

					lBmpInfo.bmiHeader.biSizeImage = lSizeImage;



					ScreenCaptureConfig lScreenCaptureConfig;

					lScreenCaptureConfig.mBitMapInfo = lBmpInfo;


					// 1 fps

					lScreenCaptureConfig.mVideoFPS = 1;

					aRefVectorScreenCaptureConfigs.push_back(lScreenCaptureConfig);


					// 5 fps

					lScreenCaptureConfig.mVideoFPS = 5;

					aRefVectorScreenCaptureConfigs.push_back(lScreenCaptureConfig);


					// 10 fps

					lScreenCaptureConfig.mVideoFPS = 10;

					aRefVectorScreenCaptureConfigs.push_back(lScreenCaptureConfig);


					// 15 fps

					lScreenCaptureConfig.mVideoFPS = 15;

					aRefVectorScreenCaptureConfigs.push_back(lScreenCaptureConfig);


					// 20 fps 

					lScreenCaptureConfig.mVideoFPS = 20;

					aRefVectorScreenCaptureConfigs.push_back(lScreenCaptureConfig);


					// 25 fps 

					lScreenCaptureConfig.mVideoFPS = 25;

					aRefVectorScreenCaptureConfigs.push_back(lScreenCaptureConfig);


					// 30 fps 

					lScreenCaptureConfig.mVideoFPS = 30;

					aRefVectorScreenCaptureConfigs.push_back(lScreenCaptureConfig);


				} while (false);

				return lresult;
			}

			HRESULT ScreenCaptureProcessorDirectX9::releaseResources()
			{
				HRESULT lresult;

				do
				{
					if (!mSurface)
					{
						mSurface.Release();
						mSurface = NULL;
					}
					if (mDirect3DDevice)
					{
						mDirect3DDevice.Release();
						mDirect3DDevice = NULL;
					}
					if (mDirect3D)
					{
						mDirect3D.Release();
						mDirect3D = NULL;
					}

					lresult = S_OK;

				} while (false);

				return lresult;
			}
						
			HRESULT ScreenCaptureProcessorDirectX9::initResources(ScreenCaptureConfig& aScreenCaptureConfig, IMFMediaType* aPtrOutputMediaType)
			{
				HRESULT lresult;

				do
				{
					LOG_INVOKE_OBJECT_METHOD(Singleton<Direct3D9ExManager>::getInstance(), getState);

					//mDesktopCoordinates.right = aScreenCaptureConfig.mBitMapInfo.bmiHeader.biWidth;

					//mDesktopCoordinates.bottom = aScreenCaptureConfig.mBitMapInfo.bmiHeader.biHeight;

					mDesktopCoordinates.right = mDesktopSize.cx;

					mDesktopCoordinates.bottom = mDesktopSize.cy;
										
					D3DDISPLAYMODE	lddm;

					D3DPRESENT_PARAMETERS	ld3dpp;

					CComPtrCustom<IDirect3D9Ex> lDirect3D;

					lresult = Direct3D9ExManager::Direct3DCreate9Ex(D3D_SDK_VERSION, &lDirect3D);

					LOG_CHECK_PTR_MEMORY(lDirect3D);

					HWND lDesktopHWND = GetDesktopWindow();

					if (mHWNDHandler.misEnable)
					{
						lDesktopHWND = mHWNDHandler.mHandler;
					}

					HMONITOR lMonitor = MonitorFromWindow(lDesktopHWND, MONITOR_DEFAULTTONEAREST);

					auto lAdapterCount = lDirect3D->GetAdapterCount();

					UINT l_currentAdapter = D3DADAPTER_DEFAULT;

					for (size_t i = 0; i < lAdapterCount; i++)
					{
						auto lAdapterMonitor = lDirect3D->GetAdapterMonitor(i);

						if (lAdapterMonitor == lMonitor)
						{
							l_currentAdapter = i;

							break;
						}
					}
					
					D3DCAPS9 lcaps;

					LOG_INVOKE_DX9EX_METHOD(GetDeviceCaps, lDirect3D,
						l_currentAdapter, // Означает первичный видеоадаптер.
						D3DDEVTYPE_HAL,   // Задает тип устройства, обычно D3DDEVTYPE_HAL.
						&lcaps);       // Возвращает заполненную структуру D3DCAPS9, которая содержит
									   // информацию о возможностях первичного видеоадаптера.
					
					// Поддерживается аппаратная обработка вершин?
					int lvertexprocessing = 0;

					if (lcaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
					{
						// да, сохраняем в vp флаг поддержки аппаратной
						// обработки вершин.
						lvertexprocessing = D3DCREATE_HARDWARE_VERTEXPROCESSING;
					}
					else
					{
						// Нет, сохраняем в vp флаг использования программной
						// обработки вершин.
						lvertexprocessing = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
					}

					LOG_INVOKE_DX9EX_METHOD(GetAdapterDisplayMode, lDirect3D, l_currentAdapter, &lddm);
					
					ZeroMemory(&ld3dpp, sizeof(D3DPRESENT_PARAMETERS));

					ld3dpp.Windowed = TRUE;
					ld3dpp.Flags = D3DPRESENTFLAG_VIDEO;// D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
					ld3dpp.BackBufferFormat = lddm.Format;
					ld3dpp.BackBufferHeight = aScreenCaptureConfig.mBitMapInfo.bmiHeader.biHeight;
					ld3dpp.BackBufferWidth = aScreenCaptureConfig.mBitMapInfo.bmiHeader.biWidth;
					ld3dpp.BackBufferCount = 0;
					ld3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
					ld3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
					ld3dpp.EnableAutoDepthStencil = FALSE;
					ld3dpp.hDeviceWindow = lDesktopHWND;
					ld3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;// D3DPRESENT_INTERVAL_DEFAULT;
					ld3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

					lDirect3D->CreateDeviceEx(l_currentAdapter, D3DDEVTYPE_HAL, lDesktopHWND, lvertexprocessing, &ld3dpp, nullptr, &mDirect3DDevice);
										
					LOG_CHECK_PTR_MEMORY(mDirect3DDevice);
										
					LOG_INVOKE_DX9EX_METHOD(CreateOffscreenPlainSurface,
						mDirect3DDevice,
						lddm.Width, 
						lddm.Height, 
						D3DFMT_A8R8G8B8,
						D3DPOOL_SYSTEMMEM,
						&mSurface, 
						NULL);

					HDC lHDC;

					mSurface->GetDC(&lHDC);

					ScreenCaptureProcessor::initResources(lHDC);

					mSurface->ReleaseDC(lHDC);
					
					mDirect3D = lDirect3D;

				} while (false);

				if (FAILED(lresult))
				{
					LogPrintOut::getInstance().printOutln(
						LogPrintOut::ERROR_LEVEL,
						__FUNCTIONW__,
						L" Error code: ",
						(HRESULT)lresult);
				}

				return lresult;
			}

			HRESULT ScreenCaptureProcessorDirectX9::grabImage(BYTE* aPtrData)
			{
				HRESULT lresult;

				do
				{

					if (aPtrData == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					LOG_INVOKE_DX9EX_METHOD(GetFrontBufferData, mDirect3DDevice, 0, mSurface);

					D3DLOCKED_RECT	lockedRect;

					HDC lHDC;

					LOG_INVOKE_DX9EX_METHOD(GetDC, mSurface, &lHDC);

					drawOn(mDesktopCoordinates, lHDC);
										
					LOG_INVOKE_DX9EX_METHOD(ReleaseDC, mSurface, lHDC);

					LOG_INVOKE_DX9EX_METHOD(LockRect, mSurface,
						&lockedRect, 
						&mScreenRect, 
						D3DLOCK_NO_DIRTY_UPDATE | 
						D3DLOCK_NOSYSLOCK | 
						D3DLOCK_READONLY);
					
					aPtrData += (mHeight - 1)*mStride;

					LOG_INVOKE_MF_FUNCTION(MFCopyImage,
						aPtrData,
						-mStride,
						(BYTE*)lockedRect.pBits,
						lockedRect.Pitch,
						lockedRect.Pitch,
						mHeight);

					LOG_INVOKE_DX9EX_METHOD(UnlockRect, mSurface);

				} while (false);
				
				return lresult;
			}


		}
	}
}