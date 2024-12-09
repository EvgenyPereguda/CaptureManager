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

#include "WMAudioVoiceEncoderManager.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
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

				lEncoderInfoData.mGUID = WMAudioVoiceEncoderManager::getMediaSubType();

				lEncoderInfoData.mMediaSubType = WMAudioVoiceEncoderManager::getMediaSubType();

				lEncoderInfoData.mIsStreaming = FALSE;

				lEncoderInfoData.mMaxBitRate = gMaxBitRate;

				lEncoderInfoData.mMinBitRate = gMinBitRate;

				return lEncoderInfoData;
			}

			static const InstanceMaker<WMAudioVoiceEncoderManager, Singleton<EncoderManagerFactory>> staticInstanceMaker(
				getEncoderInfoData());
			
			// IEncoderManager interface

			HRESULT WMAudioVoiceEncoderManager::enumEncoderMediaTypes(
				IMFMediaType* aPtrUncompressedMediaType,
				EncodingSettings aEncodingSettings,
				REFGUID aRefEncoderCLSID,
				std::vector<CComPtrCustom<IUnknown>> &aRefListOfMediaTypes)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrUncompressedMediaType);
					
					LOG_CHECK_STATE_DESCR(aEncodingSettings.mEncodingMode != EncodingSettings::CBR, E_NOTIMPL);
					
					CComPtrCustom<IMFTransform> lEncoder;

					LOG_INVOKE_OBJECT_METHOD(lEncoder, CoCreateInstance,aRefEncoderCLSID);
					
					DWORD lTypeIndex = 0;

					aRefListOfMediaTypes.clear();

					CComPtrCustom<IMFMediaType> lOutputType;

					while (SUCCEEDED(lresult))
					{
						lresult = lEncoder->GetOutputAvailableType(0, lTypeIndex++, &lOutputType);

						if (SUCCEEDED(lresult))
						{
							aRefListOfMediaTypes.push_back(lOutputType.detach());
						}
					}

					lresult = S_OK;

				} while (false);

				return lresult;
			}

			HRESULT WMAudioVoiceEncoderManager::getCompressedMediaType(
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
					
					LOG_CHECK_STATE_DESCR(aEncodingSettings.mEncodingMode == EncodingSettings::VBR, E_NOTIMPL);
					
					CComPtrCustom<IMFTransform> lEncoder;

					LOG_INVOKE_OBJECT_METHOD(lEncoder, CoCreateInstance,aRefEncoderCLSID);
					
					GUID lMajorType;

					LOG_INVOKE_MF_METHOD(GetGUID,
						aPtrUncompressedMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);
					
					LOG_CHECK_STATE_DESCR(lMajorType != MFMediaType_Audio, MF_E_INVALIDMEDIATYPE);
										
					CComPtrCustom<IMFMediaType> lOutputType;

					LOG_INVOKE_MF_METHOD(GetOutputAvailableType, lEncoder, 0, lIndexCompressedMediaType, &lOutputType);
					
					LOG_INVOKE_MF_METHOD(SetUINT32,
						lOutputType,
						MF_MT_AVG_BITRATE,
						EncodingSettings().mAverageBitrateValue);
					
					*aPtrPtrCompressedMediaType = lOutputType.Detach();

				} while (false);

				return lresult;
			}

			HRESULT WMAudioVoiceEncoderManager::getEncoder(
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
					
					LOG_CHECK_STATE_DESCR(aEncodingSettings.mEncodingMode == EncodingSettings::VBR, E_NOTIMPL);
					
					CComPtrCustom<IMFTransform> lEncoder;

					LOG_INVOKE_OBJECT_METHOD(lEncoder, CoCreateInstance,aRefEncoderCLSID);
					
					GUID lMajorType;

					LOG_INVOKE_MF_METHOD(GetGUID,
						aPtrUncompressedMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);
					
					LOG_CHECK_STATE_DESCR(lMajorType != MFMediaType_Audio, MF_E_INVALIDMEDIATYPE);
					
					CComPtrCustom<IMFMediaType> lOutputType;

					LOG_INVOKE_MF_METHOD(GetOutputAvailableType, lEncoder, 0, lIndexCompressedMediaType, &lOutputType);
					
					CComPtrCustom<IPropertyBag> aPtrPropertyBag;

					LOG_INVOKE_QUERY_INTERFACE_METHOD(lEncoder, &aPtrPropertyBag);
					
					LOG_INVOKE_FUNCTION(setEncodingProperties,
						aPtrPropertyBag,
						aEncodingSettings);

					CComPtrCustom<IPropertyStore> aPtrPropertyStore;

					LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrPropertyBag, &aPtrPropertyStore);
					
					LOG_INVOKE_FUNCTION(setEncodingProperties,
						aPtrPropertyStore,
						aEncodingSettings);
										
					LOG_INVOKE_MF_METHOD(SetOutputType, lEncoder, 0, lOutputType, 0);

					CComPtrCustom<IMFMediaType> lUncompressedAudioMediaType;
					
					LOG_INVOKE_FUNCTION(copyAndModifyInputMediaType, lOutputType, &lUncompressedAudioMediaType);
										
					LOG_INVOKE_MF_METHOD(SetInputType, lEncoder, 0, lUncompressedAudioMediaType, 0);
					
					*aPtrPtrEncoderTransform = lEncoder.Detach();

				} while (false);

				return lresult;
			}

			// WMAudioVoiceEncoderManager implementation

			GUID WMAudioVoiceEncoderManager::getMediaSubType()
			{
				return
				 { 0x0000000A, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71 } };
			}			
			
			HRESULT WMAudioVoiceEncoderManager::setEncodingProperties(
				IPropertyStore* aPtrPropertyStore,
				EncodingSettings aEncodingSettings)
			{
				HRESULT lresult;

				PROPVARIANT var;

				PropVariantInit(&var);

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrPropertyStore);
										
					LOG_INVOKE_FUNCTION(InitPropVariantFromInt32, 3, &var);
					
					LOG_INVOKE_POINTER_METHOD(aPtrPropertyStore, SetValue, MFPKEY_WMAVOICE_ENC_DecoderDelay, var);

					aPtrPropertyStore->Commit();
										
					lresult = S_OK;

					PropVariantClear(&var);

				} while (false);
				return lresult;
			}

			HRESULT WMAudioVoiceEncoderManager::setEncodingProperties(
				IPropertyBag* aPtrIPropertyBag,
				EncodingSettings aEncodingSettings)
			{
				HRESULT lresult;

				VARIANT var;

				VariantInit(&var);

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrIPropertyBag);									

					LOG_INVOKE_FUNCTION(InitVariantFromInt32, 500, &var);
					
					LOG_INVOKE_POINTER_METHOD(aPtrIPropertyBag, Write, g_wszWMACVoiceBuffer, &var);
					
					LOG_INVOKE_FUNCTION(InitVariantFromInt32, 1, &var);
					
					LOG_INVOKE_POINTER_METHOD(aPtrIPropertyBag, Write, g_wszWMACMusicSpeechClassMode, &var);
									
					lresult = S_OK;

					VariantClear(&var);

				} while (false);
				return lresult;
			}
			
			HRESULT WMAudioVoiceEncoderManager::copyAndModifyInputMediaType(
				IMFMediaType* aPtrOriginalInputMediaType,
				IMFMediaType** aPtrPtrCopyAndModifyInputMediaType)
			{
				HRESULT lresult;
				
				UINT32 lBytePerSample = 2;

				UINT32 lNumChannelsValue = 1;
								
				do
				{

					LOG_CHECK_PTR_MEMORY(aPtrOriginalInputMediaType);

					LOG_CHECK_PTR_MEMORY(aPtrPtrCopyAndModifyInputMediaType);

					PROPVARIANT lVarItem;

					GUID lMajorType;

					LOG_INVOKE_MF_METHOD(GetGUID,
						aPtrOriginalInputMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);

					LOG_CHECK_STATE_DESCR(lMajorType != MFMediaType_Audio, MF_E_INVALIDMEDIATYPE);
					
					LOG_INVOKE_MF_FUNCTION(MFCreateMediaType, aPtrPtrCopyAndModifyInputMediaType);
					
					UINT32 lFrequencyValue;

					LOG_INVOKE_MF_METHOD(GetUINT32,
						aPtrOriginalInputMediaType,
						MF_MT_AUDIO_SAMPLES_PER_SECOND,
						&lFrequencyValue);
																														
					LOG_INVOKE_MF_METHOD(SetUINT32,
						(*aPtrPtrCopyAndModifyInputMediaType),
						MF_MT_AUDIO_SAMPLES_PER_SECOND,
						lFrequencyValue);
					
					LOG_INVOKE_MF_METHOD(SetUINT32,
						(*aPtrPtrCopyAndModifyInputMediaType),
						MF_MT_AUDIO_NUM_CHANNELS,
						lNumChannelsValue);
					
					LOG_INVOKE_MF_METHOD(SetUINT32,
						(*aPtrPtrCopyAndModifyInputMediaType),
						MF_MT_AUDIO_BITS_PER_SAMPLE,
						lBytePerSample * 8);

					LOG_INVOKE_MF_METHOD(GetItem,
						aPtrOriginalInputMediaType,
						MF_MT_MAJOR_TYPE,
						&lVarItem);

					LOG_INVOKE_MF_METHOD(SetItem,
						(*aPtrPtrCopyAndModifyInputMediaType),
						MF_MT_MAJOR_TYPE,
						lVarItem);
										
					LOG_INVOKE_MF_METHOD(SetGUID,
						(*aPtrPtrCopyAndModifyInputMediaType),
						MF_MT_SUBTYPE,
						MFAudioFormat_PCM);
										
					LOG_INVOKE_MF_METHOD(SetUINT32,
						(*aPtrPtrCopyAndModifyInputMediaType),
						MF_MT_ALL_SAMPLES_INDEPENDENT,
						TRUE);

					LOG_INVOKE_MF_METHOD(SetUINT32,
						(*aPtrPtrCopyAndModifyInputMediaType),
						MF_MT_AUDIO_AVG_BYTES_PER_SECOND,
						lBytePerSample*lNumChannelsValue*lFrequencyValue);

					LOG_INVOKE_MF_METHOD(SetUINT32,
						(*aPtrPtrCopyAndModifyInputMediaType),
						MF_MT_AUDIO_BLOCK_ALIGNMENT,
						lBytePerSample*lNumChannelsValue);

				} while (false);

				return lresult;
			}

		}
	}
}