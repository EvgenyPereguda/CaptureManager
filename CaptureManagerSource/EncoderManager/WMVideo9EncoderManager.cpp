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

#include "WMVideo9EncoderManager.h"
#include <Mediaobj.h>
#include "DMORt.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../DMOManager/DMOManager.h"
#include "../Common/MFHeaders.h"
#include "../Common/InstanceMaker.h"
#include "../Common/Common.h"
#include "../Common/Singleton.h"
#include "EncoderManagerFactory.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "propvarutil.h"


namespace CaptureManager
{
	namespace Transform
	{
		namespace Encoder
		{

			static const UINT32 gMaxBitRate(0);

			static const UINT32 gMinBitRate(0);

			using namespace Core;

			static EncoderInfoData getEncoderInfoData()
			{
				EncoderInfoData lEncoderInfoData;

				lEncoderInfoData.mGUID = WMVideo9EncoderManager::getMediaSubType();

				lEncoderInfoData.mMediaSubType = WMVideo9EncoderManager::getMediaSubType();

				lEncoderInfoData.mIsStreaming = FALSE;

				lEncoderInfoData.mMaxBitRate = gMaxBitRate;

				lEncoderInfoData.mMinBitRate = gMinBitRate;

				return lEncoderInfoData;
			}

			static const InstanceMaker<WMVideo9EncoderManager, Singleton<EncoderManagerFactory>> staticInstanceMaker(
				getEncoderInfoData());

			// IEncoderManager interface

			HRESULT WMVideo9EncoderManager::enumEncoderMediaTypes(
				IMFMediaType* aPtrUncompressedMediaType,
				EncodingSettings aEncodingSettings,
				REFGUID aRefEncoderCLSID,
				std::vector<CComPtrCustom<IUnknown>> &aRefListOfMediaTypes)
			{
				HRESULT lresult;

				do
				{

					LOG_CHECK_STATE_DESCR(aEncodingSettings.mEncodingMode == EncodingSettings::CBR, E_NOTIMPL);
					
					LOG_CHECK_PTR_MEMORY(aPtrUncompressedMediaType);
					
					CComPtrCustom<IMFMediaType> lMediaType;

					LOG_INVOKE_MF_FUNCTION(MFCreateMediaType, &lMediaType);
					
					do
					{
						LOG_INVOKE_FUNCTION(copyInputMediaType,
							aPtrUncompressedMediaType,
							lMediaType);

					} while (false);

					if (FAILED(lresult))
					{
						lMediaType.Release();

						LOG_INVOKE_MF_FUNCTION(MFCreateMediaType, &lMediaType);

						aPtrUncompressedMediaType->CopyAllItems(lMediaType);

						LOG_INVOKE_FUNCTION(modifySubTypeOFMediaType,
							&lMediaType,
							MFVideoFormat_RGB24);
					}
					
					CComPtrCustom<IMFTransform> lEncoder;

					LOG_INVOKE_OBJECT_METHOD(lEncoder, CoCreateInstance,aRefEncoderCLSID);
					
					CComPtrCustom<IPropertyStore> aPtrPropertyStore;

					LOG_INVOKE_QUERY_INTERFACE_METHOD(lEncoder, &aPtrPropertyStore);
					
					LOG_INVOKE_FUNCTION(setEncodingProperties,
						aPtrPropertyStore,
						aEncodingSettings);
					
					do
					{
						LOG_INVOKE_MF_METHOD(SetInputType,
							lEncoder,
							0,
							lMediaType,
							0);

					} while (false);


					if (FAILED(lresult))
					{
						LOG_INVOKE_MF_METHOD(SetGUID,
							lMediaType,
							MF_MT_SUBTYPE,
							MFVideoFormat_RGB24);
						
						LOG_INVOKE_MF_METHOD(SetInputType,
							lEncoder,
							0,
							lMediaType,
							0);
					}
					
					DWORD lTypeIndex = 0;

					aRefListOfMediaTypes.clear();

					while (SUCCEEDED(lresult))
					{
						CComPtrCustom<IMFMediaType> lOutputType;

						lresult = lEncoder->GetOutputAvailableType(0, lTypeIndex++, &lOutputType);

						if (SUCCEEDED(lresult))
						{
							aRefListOfMediaTypes.push_back(lOutputType.detach());
						}
					}

					if (aRefListOfMediaTypes.empty())
					{
						break;
					}

					lresult = S_OK;

				} while (false);

				return lresult;
			}

			HRESULT WMVideo9EncoderManager::getCompressedMediaType(
				IMFMediaType* aPtrUncompressedMediaType,
				EncodingSettings aEncodingSettings,
				REFGUID aRefEncoderCLSID,
				DWORD lIndexCompressedMediaType,
				IMFMediaType** aPtrPtrCompressedMediaType)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrUncompressedMediaType);
					
					LOG_CHECK_PTR_MEMORY(aPtrPtrCompressedMediaType);
					
					CComPtrCustom<IMFMediaType> lMediaType;

					LOG_INVOKE_MF_FUNCTION(MFCreateMediaType, &lMediaType);
					

					do
					{
						LOG_INVOKE_FUNCTION(copyInputMediaType,
							aPtrUncompressedMediaType,
							lMediaType);

					} while (false);

					if (FAILED(lresult))
					{
						lMediaType.Release();

						LOG_INVOKE_MF_FUNCTION(MFCreateMediaType, &lMediaType);

						aPtrUncompressedMediaType->CopyAllItems(lMediaType);

						LOG_INVOKE_FUNCTION(modifySubTypeOFMediaType,
							&lMediaType,
							MFVideoFormat_RGB24);
					}
					
					CComPtrCustom<IMFTransform> lEncoder;

					LOG_INVOKE_OBJECT_METHOD(lEncoder, CoCreateInstance,aRefEncoderCLSID);
					
					CComPtrCustom<IPropertyStore> aPtrPropertyStore;

					LOG_INVOKE_QUERY_INTERFACE_METHOD(lEncoder, &aPtrPropertyStore);
					
					GUID lMajorType;

					LOG_INVOKE_MF_METHOD(GetGUID,
						lMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);
					
					LOG_CHECK_STATE_DESCR(lMajorType != MFMediaType_Video, MF_E_INVALIDMEDIATYPE);
					
					LOG_INVOKE_FUNCTION(setEncodingProperties,
						aPtrPropertyStore,
						aEncodingSettings);
					
					do
					{
						LOG_INVOKE_MF_METHOD(SetInputType,
							lEncoder,
							0,
							lMediaType,
							0);

					} while (false);


					if (FAILED(lresult))
					{
						LOG_INVOKE_MF_METHOD(SetGUID,
							lMediaType,
							MF_MT_SUBTYPE,
							MFVideoFormat_RGB24);
						
						LOG_INVOKE_MF_METHOD(SetInputType,
							lEncoder,
							0,
							lMediaType,
							0);
					}
					
					CComPtrCustom<IMFMediaType> lOutputType;

					LOG_INVOKE_MF_METHOD(GetOutputAvailableType, lEncoder, 0, lIndexCompressedMediaType, &lOutputType);
					
					LOG_INVOKE_MF_METHOD(SetUINT32,
						lOutputType,
						MF_MT_AVG_BITRATE,
						aEncodingSettings.mAverageBitrateValue);
					
					LOG_INVOKE_FUNCTION(addPrivateData, lEncoder, lOutputType);
					
					*aPtrPtrCompressedMediaType = lOutputType.Detach();

				} while (false);

				return lresult;
			}

			HRESULT WMVideo9EncoderManager::getEncoder(
				IMFMediaType* aPtrUncompressedMediaType,
				EncodingSettings aEncodingSettings,
				REFGUID aRefEncoderCLSID,
				DWORD lIndexCompressedMediaType,
				IMFTransform** aPtrPtrEncoderTransform)
			{
				HRESULT lresult;

				do
				{

					LOG_CHECK_PTR_MEMORY(aPtrUncompressedMediaType);
					
					LOG_CHECK_PTR_MEMORY(aPtrPtrEncoderTransform);
					
					CComPtrCustom<IMFMediaType> lMediaType;

					LOG_INVOKE_MF_FUNCTION(MFCreateMediaType, &lMediaType);
					

					do
					{
						LOG_INVOKE_FUNCTION(copyInputMediaType,
							aPtrUncompressedMediaType,
							lMediaType);

					} while (false);

					if (FAILED(lresult))
					{
						lMediaType.Release();

						LOG_INVOKE_MF_FUNCTION(MFCreateMediaType, &lMediaType);

						aPtrUncompressedMediaType->CopyAllItems(lMediaType);

						LOG_INVOKE_FUNCTION(modifySubTypeOFMediaType,
							&lMediaType,
							MFVideoFormat_RGB24);
					}
					
					CComPtrCustom<IMFTransform> lEncoder;

					LOG_INVOKE_OBJECT_METHOD(lEncoder, CoCreateInstance,aRefEncoderCLSID);
					
					CComPtrCustom<IPropertyStore> aPtrPropertyStore;

					LOG_INVOKE_QUERY_INTERFACE_METHOD(lEncoder, &aPtrPropertyStore);
					
					GUID lMajorType;

					LOG_INVOKE_MF_METHOD(GetGUID,
						lMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);
					
					LOG_CHECK_STATE_DESCR(lMajorType != MFMediaType_Video, MF_E_INVALIDMEDIATYPE);
					
					LOG_INVOKE_FUNCTION(setEncodingProperties,
						aPtrPropertyStore,
						aEncodingSettings);

					do
					{
						LOG_INVOKE_MF_METHOD(SetInputType,
							lEncoder,
							0,
							lMediaType,
							0);

					} while (false);
					
					if (FAILED(lresult))
					{
						LOG_INVOKE_MF_METHOD(SetGUID,
							lMediaType,
							MF_MT_SUBTYPE,
							MFVideoFormat_RGB24);
						
						LOG_INVOKE_MF_METHOD(SetInputType,
							lEncoder,
							0,
							lMediaType,
							0);
					}
					
					CComPtrCustom<IMFMediaType> lOutputType;

					LOG_INVOKE_MF_METHOD(GetOutputAvailableType, lEncoder, 0, lIndexCompressedMediaType, &lOutputType);
					
					LOG_INVOKE_MF_METHOD(SetUINT32,
						lOutputType,
						MF_MT_AVG_BITRATE,
						aEncodingSettings.mAverageBitrateValue);
					
					LOG_INVOKE_FUNCTION(addPrivateData, lEncoder, lOutputType);
					
					LOG_INVOKE_MF_METHOD(SetOutputType, lEncoder, 0, lOutputType, 0);
					
					*aPtrPtrEncoderTransform = lEncoder.Detach();

				} while (false);

				return lresult;
			}

			// WMVideo9EncoderManager implementation

			GUID WMVideo9EncoderManager::getMediaSubType()
			{
				return
				{ 0x33564D57, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71 } };
			}

			HRESULT WMVideo9EncoderManager::setEncodingProperties(
				IPropertyStore* aPtrPropertyStore,
				EncodingSettings aEncodingSettings)
			{
				HRESULT lresult;

				PROPVARIANT var;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrPropertyStore);
					
					LOG_CHECK_STATE_DESCR(aEncodingSettings.mEncodingMode == EncodingSettings::NONE, MF_E_NOT_INITIALIZED);
					
					switch (aEncodingSettings.mEncodingMode)
					{
					case EncodingSettings::CBR:

						LOG_INVOKE_FUNCTION(InitPropVariantFromBoolean, FALSE, &var);
						
						LOG_INVOKE_POINTER_METHOD(aPtrPropertyStore, SetValue, MFPKEY_VBRENABLED, var);
						
						LOG_INVOKE_FUNCTION(InitPropVariantFromInt32, aEncodingSettings.mEncodingModeValue, &var);
						
						LOG_INVOKE_POINTER_METHOD(aPtrPropertyStore, SetValue, MFPKEY_VIDEOWINDOW, var);


						PropVariantClear(&var);

						break;

					case EncodingSettings::VBR:

						LOG_INVOKE_FUNCTION(InitPropVariantFromBoolean, VARIANT_TRUE, &var);
						
						LOG_INVOKE_POINTER_METHOD(aPtrPropertyStore, SetValue, MFPKEY_VBRENABLED, var);
						
						LOG_INVOKE_FUNCTION(InitPropVariantFromInt32, 1, &var);
						
						LOG_INVOKE_POINTER_METHOD(aPtrPropertyStore, SetValue, MFPKEY_PASSESUSED, var);
						
						LOG_INVOKE_FUNCTION(InitPropVariantFromInt32, 1000, &var);
						
						LOG_INVOKE_POINTER_METHOD(aPtrPropertyStore, SetValue, MFPKEY_VIDEOWINDOW, var);
						
						LOG_INVOKE_FUNCTION(InitPropVariantFromInt32, aEncodingSettings.mEncodingModeValue, &var);
						
						LOG_INVOKE_POINTER_METHOD(aPtrPropertyStore, SetValue, MFPKEY_VBRQUALITY, var);


						PropVariantClear(&var);

						break;

					default:
						lresult = E_UNEXPECTED;
						break;
					}

				} while (false);
				return lresult;
			}

			HRESULT WMVideo9EncoderManager::addPrivateData(
				IMFTransform* aPtrEncoderTransform,
				IMFMediaType* aPtrMediaType)
			{
				HRESULT lresult;

				std::unique_ptr<BYTE> lData;

				DMO_MEDIA_TYPE lDMO_MEDIA_TYPE = { 0 };

				do
				{

					ULONG ldataLength = 0;

					CComPtrCustom<IWMCodecPrivateData> lPrivData;

					LOG_INVOKE_MF_FUNCTION(MFInitAMMediaTypeFromMFMediaType,
						aPtrMediaType,
						FORMAT_VideoInfo,
						(AM_MEDIA_TYPE*)&lDMO_MEDIA_TYPE
						);
					
					LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrEncoderTransform, &lPrivData);

					LOG_INVOKE_POINTER_METHOD(lPrivData, SetPartialOutputType, &lDMO_MEDIA_TYPE);

					LOG_INVOKE_POINTER_METHOD(lPrivData, GetPrivateData, NULL, &ldataLength);

					lData.reset(new(std::nothrow) BYTE[ldataLength]);

					LOG_CHECK_PTR_MEMORY(lData);

					LOG_INVOKE_POINTER_METHOD(lPrivData, GetPrivateData, lData.get(), &ldataLength);

					LOG_INVOKE_MF_METHOD(SetBlob, aPtrMediaType, MF_MT_USER_DATA, lData.get(), ldataLength);

				} while (false);

				do
				{
					LOG_INVOKE_DMO_FUNCTION(MoFreeMediaType, &lDMO_MEDIA_TYPE);

				} while (false);

				return lresult;
			}


			HRESULT WMVideo9EncoderManager::copyInputMediaType(
				IMFMediaType* aPtrOriginalInputMediaType,
				IMFMediaType* aPtrCopyInputMediaType)
			{
				HRESULT lresult;

				do
				{

					LOG_CHECK_PTR_MEMORY(aPtrOriginalInputMediaType);
					
					LOG_CHECK_PTR_MEMORY(aPtrCopyInputMediaType);
					
					PROPVARIANT lVarItem;

					LOG_INVOKE_MF_METHOD(GetItem,
						aPtrOriginalInputMediaType,
						MF_MT_MAJOR_TYPE,
						&lVarItem);
					
					LOG_INVOKE_MF_METHOD(SetItem,
						aPtrCopyInputMediaType,
						MF_MT_MAJOR_TYPE,
						lVarItem);
					
					LOG_INVOKE_MF_METHOD(GetItem,
						aPtrOriginalInputMediaType,
						MF_MT_SUBTYPE,
						&lVarItem);
					
					LOG_INVOKE_MF_METHOD(SetItem,
						aPtrCopyInputMediaType,
						MF_MT_SUBTYPE,
						lVarItem);
					
					LOG_INVOKE_MF_METHOD(GetItem,
						aPtrOriginalInputMediaType,
						MF_MT_FRAME_RATE,
						&lVarItem);
					
					LOG_INVOKE_MF_METHOD(SetItem,
						aPtrCopyInputMediaType,
						MF_MT_FRAME_RATE,
						lVarItem);
					
					LOG_INVOKE_MF_METHOD(GetItem,
						aPtrOriginalInputMediaType,
						MF_MT_FRAME_SIZE,
						&lVarItem);
					
					LOG_INVOKE_MF_METHOD(SetItem,
						aPtrCopyInputMediaType,
						MF_MT_FRAME_SIZE,
						lVarItem);
					
					LOG_INVOKE_MF_METHOD(GetItem,
						aPtrOriginalInputMediaType,
						MF_MT_INTERLACE_MODE,
						&lVarItem);
					
					LOG_INVOKE_MF_METHOD(SetItem,
						aPtrCopyInputMediaType,
						MF_MT_INTERLACE_MODE,
						lVarItem);

					do
					{
						LOG_INVOKE_MF_METHOD(GetItem,
							aPtrOriginalInputMediaType,
							MF_MT_DEFAULT_STRIDE,
							&lVarItem);

					} while (false);
					
					if (FAILED(lresult))
					{

						LOG_INVOKE_MF_METHOD(GetItem,
							aPtrOriginalInputMediaType,
							MF_MT_FRAME_SIZE,
							&lVarItem);
						
						UINT32 lHigh = 0, lLow = 0;

						DataParser::unpack2UINT32AsUINT64(lVarItem, lHigh, lLow);

						GUID lSubType;

						LOG_INVOKE_MF_METHOD(GetGUID,
							aPtrOriginalInputMediaType,
							MF_MT_SUBTYPE,
							&lSubType);
						
						LONG lstride = 0;

						LOG_INVOKE_MF_FUNCTION(MFGetStrideForBitmapInfoHeader,
							lSubType.Data1,
							lHigh,
							&lstride);
						
						LOG_CHECK_STATE(lstride == 0);
						
						LOG_INVOKE_MF_METHOD(SetUINT32,
							aPtrCopyInputMediaType,
							MF_MT_DEFAULT_STRIDE,
							*((UINT32*)&lstride));
					}
					else
					{
						LOG_INVOKE_MF_METHOD(SetItem,
							aPtrCopyInputMediaType,
							MF_MT_DEFAULT_STRIDE,
							lVarItem);
					}

				} while (false);

				return lresult;
			}

		}
	}
}