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

#include "BasePresenter.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Common.h"
#include "../Common/Singleton.h"

HINSTANCE g_hInst = NULL;

namespace EVRMultiSink
{
	namespace Sinks
	{
		namespace EVR
		{
			using namespace CaptureManager;
			using namespace CaptureManager::Core;

			CBasePresenter::CBasePresenter() :
				mIsShutdown(false),
				mIsStarted(false),
				mHWNDVideo(nullptr)
			{
				mPixelRate.Numerator = 1;

				mPixelRate.Denominator = 1;
			}
			
			CBasePresenter::~CBasePresenter()
			{
			}

			HRESULT CBasePresenter::init(
				UINT32 aImageWidth,
				UINT32 aImageHeight,
				MFRatio aFrameRate)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					std::lock_guard<std::mutex> lLock(mAccessMutex);

					mImageWidth = aImageWidth;
					mImageHeight = aImageHeight;
					
					mFrameRate = aFrameRate;
					
					UINT64 lAverageTimePerFrame;

					LOG_INVOKE_MF_FUNCTION(MFFrameRateToAverageTimePerFrame,
						aFrameRate.Numerator,
						aFrameRate.Denominator,
						&lAverageTimePerFrame);

					mVideoFrameDuration = lAverageTimePerFrame;
					
				} while (false);

				return lresult;
			}

			HRESULT CBasePresenter::createUncompressedVideoType(
				DWORD                fccFormat,  // FOURCC or D3DFORMAT value.     
				UINT32               width,
				UINT32               height,
				MFVideoInterlaceMode interlaceMode,
				const MFRatio&       frameRate,
				const MFRatio&       par,
				IMFMediaType         **ppType
				)
			{
				if (ppType == NULL)
				{
					return E_POINTER;
				}

				GUID    subtype = MFVideoFormat_Base;
				LONG    lStride = 0;
				UINT    cbImage = 0;

				CComPtrCustom<IMFMediaType> pType;

				// Set the subtype GUID from the FOURCC or D3DFORMAT value.
				subtype.Data1 = fccFormat;

				HRESULT hr = MediaFoundation::MediaFoundationManager::MFCreateMediaType(&pType);
				if (FAILED(hr))
				{
					goto done;
				}

				hr = pType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
				if (FAILED(hr))
				{
					goto done;
				}

				hr = pType->SetGUID(MF_MT_SUBTYPE, subtype);
				if (FAILED(hr))
				{
					goto done;
				}

				hr = pType->SetUINT32(MF_MT_INTERLACE_MODE, interlaceMode);
				if (FAILED(hr))
				{
					goto done;
				}

				hr = MFSetAttributeSize(pType, MF_MT_FRAME_SIZE, width, height);
				if (FAILED(hr))
				{
					goto done;
				}
				
				hr = MediaFoundation::MediaFoundationManager::MFGetStrideForBitmapInfoHeader(fccFormat, width, &lStride);

				if (SUCCEEDED(hr))
				{
					// Calculate the default stride value.
					hr = pType->SetUINT32(MF_MT_DEFAULT_STRIDE, UINT32(lStride));
					if (FAILED(hr))
					{
						goto done;
					}
				}

				// Calculate the image size in bytes.
				hr = MediaFoundation::MediaFoundationManager::MFCalculateImageSize(subtype, width, height, &cbImage);
				if (SUCCEEDED(hr))
				{
					hr = pType->SetUINT32(MF_MT_SAMPLE_SIZE, cbImage);
					if (FAILED(hr))
					{
						goto done;
					}
				}

				hr = pType->SetUINT32(MF_MT_FIXED_SIZE_SAMPLES, TRUE);
				if (FAILED(hr))
				{
					goto done;
				}

				hr = pType->SetUINT32(MF_MT_ALL_SAMPLES_INDEPENDENT, TRUE);
				if (FAILED(hr))
				{
					goto done;
				}

				// Frame rate
				hr = MFSetAttributeRatio(pType, MF_MT_FRAME_RATE, frameRate.Numerator,
					frameRate.Denominator);
				if (FAILED(hr))
				{
					goto done;
				}

				// Pixel aspect ratio
				hr = MFSetAttributeRatio(pType, MF_MT_PIXEL_ASPECT_RATIO, par.Numerator,
					par.Denominator);
				if (FAILED(hr))
				{
					goto done;
				}

				// Return the pointer to the caller.

				if (pType)
				{
					hr = pType->QueryInterface(IID_PPV_ARGS(ppType));
				}

			done:
				return hr;
			}

		}
	}
}