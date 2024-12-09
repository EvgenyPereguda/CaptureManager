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

#include "BaseEncoderManager.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../Common/MFHeaders.h"
#include "../Common/InstanceMaker.h"
#include "../Common/Common.h"
#include "../Common/Singleton.h"
#include "../Common/GUIDs.h"
#include "EncoderManagerFactory.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../ConfigManager/ConfigManager.h"
#include "../Common/ComPtrCustom.h"
#include "propvarutil.h"
#include <wmcodecdsp.h>
#include <VersionHelpers.h>


namespace CaptureManager
{
	namespace Transform
	{
		namespace Encoder
		{
			using namespace Core;

			BaseEncoderManager::BaseEncoderManager()
			{
			}
			
			BaseEncoderManager::~BaseEncoderManager()
			{
			}
			
			HRESULT BaseEncoderManager::modifySubTypeOFMediaType(
				IMFMediaType** aPtrPtrInputMediaType,
				REFGUID aNewSubType)
			{
				HRESULT lresult;

				do
				{

					GUID lMajorType;

					LOG_INVOKE_MF_METHOD(GetGUID,
						(*aPtrPtrInputMediaType),
						MF_MT_MAJOR_TYPE,
						&lMajorType);

					LOG_CHECK_STATE_DESCR(lMajorType != MFMediaType_Video, MF_E_INVALIDMEDIATYPE);

					UINT64 lUINT64Value;

					LOG_INVOKE_MF_METHOD(GetUINT64,
						(*aPtrPtrInputMediaType),
						MF_MT_FRAME_SIZE,
						&lUINT64Value);

					UINT32 lHight = 0, lWidth = 0;

					Unpack2UINT32AsUINT64(lUINT64Value, &lWidth, &lHight);

					LOG_CHECK_STATE_DESCR(lWidth <= 0, MF_E_INVALIDMEDIATYPE);

					BITMAPINFO	lBmpInfo;

					// BMP 32 bpp

					ZeroMemory(&lBmpInfo, sizeof(BITMAPINFO));

					lBmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

					lBmpInfo.bmiHeader.biBitCount = 32;

					lBmpInfo.bmiHeader.biCompression = BI_RGB;

					lBmpInfo.bmiHeader.biWidth = lWidth;

					lBmpInfo.bmiHeader.biHeight = lHight;

					lBmpInfo.bmiHeader.biPlanes = 1;

					UINT32 lSizeImage;

					LOG_INVOKE_MF_FUNCTION(MFCalculateImageSize,
						MFVideoFormat_RGB32,
						lBmpInfo.bmiHeader.biWidth,
						lBmpInfo.bmiHeader.biHeight,
						&lSizeImage);

					lBmpInfo.bmiHeader.biSizeImage = lSizeImage;


					LOG_INVOKE_MF_METHOD(GetUINT64,
						(*aPtrPtrInputMediaType),
						MF_MT_FRAME_RATE,
						&lUINT64Value);

					UINT32 lNumerator = 0, lDenominator = 0;

					Unpack2UINT32AsUINT64(lUINT64Value, &lNumerator, &lDenominator);

					DWORD lBitRate = (lSizeImage * 8 * lNumerator) / lDenominator;

					UINT32 lratioX = 1, lratioY = 1;

					do
					{
						LOG_INVOKE_MF_METHOD(GetUINT64,
							(*aPtrPtrInputMediaType),
							MF_MT_PIXEL_ASPECT_RATIO,
							&lUINT64Value);

					} while (false);


					if (SUCCEEDED(lresult))
					{
						Unpack2UINT32AsUINT64(lUINT64Value, &lratioX, &lratioY);
					}

					CComPtrCustom<IMFVideoMediaType> lVideoMediaType;

					LOG_INVOKE_MF_FUNCTION(MFCreateVideoMediaTypeFromBitMapInfoHeaderEx,
						&lBmpInfo.bmiHeader,                     // video info header to convert
						lBmpInfo.bmiHeader.biSize,               // size of the header structure
						lratioX,                                  // pixel aspect ratio X
						lratioY,                                  // pixel aspect ratio Y
						MFVideoInterlace_Progressive,       // interlace mode 
						0,                                  // video flags
						lNumerator,           // FPS numerator
						lDenominator,          // FPS denominator
						lBitRate,          // max bitrate
						&lVideoMediaType);                           // result - out


					CComPtrCustom<IMFTransform> lColorConvert;

					LOG_INVOKE_OBJECT_METHOD(lColorConvert, CoCreateInstance, __uuidof(CColorConvertDMO));

					LOG_INVOKE_MF_METHOD(SetInputType,
						lColorConvert,
						0,
						lVideoMediaType,
						0);

					DWORD lTypeIndex = 0;

					while (SUCCEEDED(lresult))
					{

						CComPtrCustom<IMFMediaType> lOutputType;

						do
						{
							LOG_INVOKE_MF_METHOD(GetOutputAvailableType,
								lColorConvert,
								0, lTypeIndex++, &lOutputType);

						} while (false);

						if (SUCCEEDED(lresult))
						{
							GUID lSubType;

							LOG_INVOKE_MF_METHOD(GetGUID,
								lOutputType,
								MF_MT_SUBTYPE,
								&lSubType);

							if (lSubType == aNewSubType)
							{
								LONG lstride = 0;

								LOG_INVOKE_MF_FUNCTION(MFGetStrideForBitmapInfoHeader,
									lSubType.Data1,
									lWidth,
									&lstride);

								if (lstride < 0)
									lstride = -lstride;

								lBitRate = (lHight * (UINT32)lstride * 8 * lNumerator) / lDenominator;

								LOG_INVOKE_MF_METHOD(SetUINT32,
									lOutputType,
									MF_MT_AVG_BITRATE,
									lBitRate);

								PROPVARIANT lVarItem;

								LOG_INVOKE_MF_METHOD(GetItem,
									(*aPtrPtrInputMediaType),
									MF_MT_FRAME_RATE,
									&lVarItem);

								LOG_INVOKE_MF_METHOD(SetItem,
									lOutputType,
									MF_MT_FRAME_RATE,
									lVarItem);

								(*aPtrPtrInputMediaType)->Release();

								*aPtrPtrInputMediaType = lOutputType.detach();

								break;
							}
						}
					}

				} while (false);

				return lresult;
			}
		}
	}
}