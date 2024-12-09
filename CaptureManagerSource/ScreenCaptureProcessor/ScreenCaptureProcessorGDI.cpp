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

#include "ScreenCaptureProcessorGDI.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../Common/MFHeaders.h"
#include "../DataParser/DataParser.h"
#include "ScreenCaptureProcessorFactory.h"
#include "../Common/InstanceMaker.h"
#include "../Common/Common.h"
#include "../Common/Singleton.h"
#include "../ConfigManager/ConfigManager.h"


static CaptureManager::Core::InstanceMaker<CaptureManager::Sources::ScreenCapture::ScreenCaptureProcessorGDI,
	CaptureManager::Sources::ScreenCaptureProcessorFactory> staticInstanceMaker(1);

namespace CaptureManager
{
	namespace Sources
	{
		namespace ScreenCapture
		{
			using namespace CaptureManager::Core;

			using namespace CaptureManager::Core::MediaFoundation;
					
			ScreenCaptureProcessorGDI::ScreenCaptureProcessorGDI():
				mPtrBits(nullptr),
				mDesktopHWND(nullptr),
				mIsMultiDisplay(FALSE)
			{
				ZeroMemory(&mDesktopCoordinates, sizeof(mDesktopCoordinates));

				fillVectorScreenCaptureConfigs(mVectorScreenCaptureConfigs);

				mIsFlipStride = TRUE;
			}

			ScreenCaptureProcessorGDI::ScreenCaptureProcessorGDI(RECT aWorkArea) :
				mPtrBits(nullptr),
				mDesktopHWND(nullptr),
				mIsMultiDisplay(TRUE)
			{
				ZeroMemory(&mDesktopCoordinates, sizeof(mDesktopCoordinates));

				mWorkArea = aWorkArea;

				mDesktopCoordinates = mWorkArea;

				fillVectorScreenCaptureConfigs(mVectorScreenCaptureConfigs);

				mIsFlipStride = TRUE;
			}

			HRESULT ScreenCaptureProcessorGDI::check()
			{
				return S_OK;
			}

			HRESULT ScreenCaptureProcessorGDI::enumIInnerCaptureProcessor(
				UINT aIndex,
				UINT aOrientation,
				IInnerCaptureProcessor** aPtrPtrIInnerCaptureProcessor)
			{
				HRESULT lresult = E_FAIL;

				do
				{
					LOG_CHECK_STATE(aIndex != 0);

					CComPtrCustom<IInnerCaptureProcessor> lIInnerCaptureProcessor(new (std::nothrow) ScreenCaptureProcessorGDI);

					LOG_CHECK_PTR_MEMORY(lIInnerCaptureProcessor);

					LOG_INVOKE_QUERY_INTERFACE_METHOD(lIInnerCaptureProcessor, aPtrPtrIInnerCaptureProcessor);

				} while (false);

				return lresult;
			}

			HRESULT ScreenCaptureProcessorGDI::init(
				const std::wstring& aRefDeviceName)
			{
				HRESULT lresult = E_FAIL;

				do
				{
					mOutputName = aRefDeviceName;

					lresult = S_OK;

				} while (false);

				return lresult;
			}
			


			std::wstring ScreenCaptureProcessorGDI::getSymbolicLink()
			{
				if (mOutputName.empty())
					return ScreenCaptureProcessor::getSymbolicLink();
				else
					return ScreenCaptureProcessor::getSymbolicLink() + mOutputName;
			}

			std::wstring ScreenCaptureProcessorGDI::getFrendlyName()
			{
				if (mOutputName.empty())
				{
					return ScreenCaptureProcessor::getFrendlyName();
				}
				else
				{
					auto lpos = mOutputName.find(L"D");

					std::wstring llocalDeviceName = mOutputName;

					if (lpos != std::wstring::npos)
					{
						llocalDeviceName = mOutputName.substr(lpos, mOutputName.size());

					}

					if (!mDeviceName.empty())
					{
						llocalDeviceName += L" - " + mDeviceName;
					}

					return ScreenCaptureProcessor::getFrendlyName() + L" (" + llocalDeviceName + L")";
				}
			}


			HRESULT ScreenCaptureProcessorGDI::fillVectorScreenCaptureConfigs(
				std::vector<ScreenCaptureConfig>& aRefVectorScreenCaptureConfigs)
			{
				HRESULT lresult;

				//RECT lRect;

				//GetWindowRect(GetDesktopWindow(), &lRect);

				//int lWidth = lRect.right - lRect.left;

				//int lHeight = lRect.bottom - lRect.top;

				int lWidth = GetSystemMetrics(SM_CXSCREEN);

				int lHeight = GetSystemMetrics(SM_CYSCREEN);

				mMultiDisplayOffset.x = 0;

				mMultiDisplayOffset.y = 0;

				mDesktopHWND = GetDesktopWindow();

				if (mIsMultiDisplay == TRUE)
				{
					lWidth = mWorkArea.right - mWorkArea.left;

					lHeight = mWorkArea.bottom - mWorkArea.top;

					mMultiDisplayOffset.x = mWorkArea.left;

					mMultiDisplayOffset.y = mWorkArea.top;
				}



				mDesktopSize.cx = lWidth;

				mDesktopSize.cy = lHeight;



				//mDesktopSize.cx = lWidth;

				//mDesktopSize.cy = lHeight;

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
									
			HRESULT ScreenCaptureProcessorGDI::initResources(ScreenCaptureConfig& aScreenCaptureConfig, IMFMediaType* aPtrOutputMediaType)
			{
				HRESULT lresult = S_OK;

				do
				{
					LPVOID lLPVOIDBits;

					if (mIsMultiDisplay == FALSE)
					{
						mDesktopCoordinates.right = mDesktopSize.cx;

						mDesktopCoordinates.bottom = mDesktopSize.cy;
					}

					auto lDesktopmBitMapInfo = aScreenCaptureConfig.mBitMapInfo;
					
					int lWidth = mDesktopSize.cx;

					int lHeight = mDesktopSize.cy;

					if (mHWNDHandler.misEnable)
					{

						RECT lClientRect;

						RECT lWindowRect;

						auto l_bClientRes = GetClientRect(mHWNDHandler.mHandler, &lClientRect);

						auto l_bWindowRes = GetWindowRect(mHWNDHandler.mHandler, &lWindowRect);

						if (l_bClientRes != FALSE && l_bWindowRes != FALSE)
						{
							auto lWindowWidth = (int)::abs(lWindowRect.right - lWindowRect.left);

							auto lWindowHeight = (int)::abs(lWindowRect.top - lWindowRect.bottom);



							auto lClientWidth = (int)::abs(lClientRect.right - lClientRect.left);

							auto lClientHeight = (int)::abs(lClientRect.top - lClientRect.bottom);


							auto lBorderWidth = (lWindowWidth - lClientWidth) >> 1;

							auto lTopOffset = (lWindowHeight - lClientHeight - lBorderWidth);


							mHWNDHandler.mBorderWidth = lBorderWidth;

							mHWNDHandler.mTopOffset = lTopOffset;
						}
						
					}
					
					lDesktopmBitMapInfo.bmiHeader.biWidth = lWidth;

					lDesktopmBitMapInfo.bmiHeader.biHeight = lHeight;

					lDesktopmBitMapInfo.bmiHeader.biPlanes = 1;

					UINT32 lSizeImage = 0;

					LOG_INVOKE_MF_FUNCTION(MFCalculateImageSize,
						MFVideoFormat_RGB32,
						lDesktopmBitMapInfo.bmiHeader.biWidth,
						lDesktopmBitMapInfo.bmiHeader.biHeight,
						&lSizeImage);

					lDesktopmBitMapInfo.bmiHeader.biSizeImage = lSizeImage;
					
					if (mDesktopHWND == nullptr)
						mDesktopHWND = GetDesktopWindow();
					else
					{
						mMultiDisplayOffset.x = mWorkArea.left;

						mMultiDisplayOffset.y = mWorkArea.top;
					}

					if (mHWNDHandler.misEnable)
					{
						mDesktopHWND = mHWNDHandler.mHandler;
					}

					mDesktopHDC = GetDC(mDesktopHWND);

					mDesktopCompatibleHDC = CreateCompatibleDC(mDesktopHDC);

					mSecondDesktopCompatibleHDC = CreateCompatibleDC(mDesktopHDC);

					mDestCompatibleHDC = CreateCompatibleDC(mDesktopHDC);






					mDesktopCompatibleHBitmap = CreateDIBSection(mDesktopHDC, &lDesktopmBitMapInfo, DIB_RGB_COLORS, &lLPVOIDBits, NULL, 0);

					if (mDesktopCompatibleHDC == nullptr || mDesktopCompatibleHBitmap == nullptr)
						break;

					SelectObject(mDesktopCompatibleHDC, mDesktopCompatibleHBitmap);

					LOG_CHECK_PTR_MEMORY(lLPVOIDBits);

					mPtrBits = (BYTE *)lLPVOIDBits;



					lLPVOIDBits = nullptr;

					mSecondDesktopCompatibleHBitmap = CreateDIBSection(mDesktopHDC, &lDesktopmBitMapInfo, DIB_RGB_COLORS, &lLPVOIDBits, NULL, 0);

					if (mSecondDesktopCompatibleHDC == nullptr || mSecondDesktopCompatibleHBitmap == nullptr)
						break;
										
					SelectObject(mSecondDesktopCompatibleHDC, mSecondDesktopCompatibleHBitmap);
					
					LOG_CHECK_PTR_MEMORY(lLPVOIDBits);

					mSecondPtrBits = (BYTE *)lLPVOIDBits;

					mDestCompatibleHBitmap = CreateDIBSection(mDesktopHDC, &aScreenCaptureConfig.mBitMapInfo, DIB_RGB_COLORS, &lLPVOIDBits, NULL, 0);

					if (mDestCompatibleHDC == nullptr || mDestCompatibleHBitmap == nullptr)
						break;

					SelectObject(mDestCompatibleHDC, mDestCompatibleHBitmap);

					LOG_CHECK_PTR_MEMORY(lLPVOIDBits);

					mPtrBits = (BYTE *)lLPVOIDBits;
					
					ScreenCaptureProcessor::initResources(mDesktopCompatibleHDC);
										
																				
				} while (false);

				return lresult;
			}

			HRESULT ScreenCaptureProcessorGDI::releaseResources()
			{
				HRESULT lresult;

				do
				{

					if (mDesktopCompatibleHDC)
						DeleteDC(mDesktopCompatibleHDC);

					if (mDesktopCompatibleHBitmap)
						DeleteObject(mDesktopCompatibleHBitmap);

					if (mSecondDesktopCompatibleHDC)
						DeleteDC(mSecondDesktopCompatibleHDC);

					if (mSecondDesktopCompatibleHBitmap)
						DeleteObject(mSecondDesktopCompatibleHBitmap);

					if (mDestCompatibleHDC)
						DeleteDC(mDestCompatibleHDC);

					if (mDestCompatibleHBitmap)
						DeleteObject(mDestCompatibleHBitmap);
					
					ReleaseDC(mDesktopHWND, mDesktopHDC);

					lresult = S_OK;

				} while (false);

				return lresult;
			}
			
			HRESULT ScreenCaptureProcessorGDI::grabImage(BYTE* aPtrData)
			{
				HRESULT lresult;

				do
				{
					int lLeftOffset = 0;
					int lTopOffset = 0;


					int lSrcLeftOffset = 0;
					int lSrcTopOffset = 0;

					if (mIsMultiDisplay == TRUE)
					{
						lSrcLeftOffset = mMultiDisplayOffset.x;
						lSrcTopOffset = mMultiDisplayOffset.y;
					}

					if (mClipResource.misEnable)
					{
						lLeftOffset = mClipResource.mLeft;
						lTopOffset = mClipResource.mTop;
					}

					if (mHWNDHandler.misEnable )
					{

						RECT lWindowRect;
						
						auto l_bWindowRes = GetWindowRect(mHWNDHandler.mHandler, &lWindowRect);


						if (!mHWNDHandler.mIsWindow)
						{
							lWindowRect.left += mHWNDHandler.mBorderWidth;

							lWindowRect.top += mHWNDHandler.mTopOffset;
						}
						else
						{
							if (Singleton<ConfigManager>::getInstance().isWindows10_Or_Greater())
							{
								lWindowRect.left += mHWNDHandler.mBorderWidth;
							}
							else
							{
								lSrcLeftOffset = -mHWNDHandler.mBorderWidth;
							}


							lSrcTopOffset = -mHWNDHandler.mTopOffset;
						}
						
						mDesktopCoordinates = lWindowRect;
					}

					BitBlt(mDesktopCompatibleHDC, 0, 0, mDesktopSize.cx, mDesktopSize.cy, NULL, lSrcLeftOffset, lSrcTopOffset, BLACKNESS);

					BitBlt(mDesktopCompatibleHDC, 0, 0, mDesktopSize.cx, mDesktopSize.cy, mDesktopHDC, lSrcLeftOffset, lSrcTopOffset, SRCCOPY);

					drawOn(mDesktopCoordinates, mDesktopCompatibleHDC);

					BitBlt(mDestCompatibleHDC, 0, 0, mWidth, mHeight, mDesktopCompatibleHDC, lLeftOffset, lTopOffset, SRCCOPY);

					aPtrData += (mHeight - 1) * mStride;
																				
					LOG_INVOKE_MF_FUNCTION(MFCopyImage,
						aPtrData,
						-mStride,
						mPtrBits,
						mStride,
						mStride,
						mHeight);

				} while (false);

				return lresult;
			}
				
			ScreenCaptureProcessorGDI::~ScreenCaptureProcessorGDI()
			{
				releaseResources();
			}
		}
	}
}