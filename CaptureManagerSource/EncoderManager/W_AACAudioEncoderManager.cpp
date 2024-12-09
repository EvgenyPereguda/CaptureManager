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

#include "W_AACAudioEncoderManager.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../Common/MFHeaders.h"
#include "../Common/InstanceMaker.h"
#include "../Common/Common.h"
#include "../Common/Singleton.h"
#include "EncoderManagerFactory.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "propvarutil.h"
#include <VersionHelpers.h>


namespace CaptureManager
{
	namespace Transform
	{
		namespace Encoder
		{

			static const UINT32 gMaxByteRate = 12000;

			using namespace Core;

			static EncoderInfoData getEncoderInfoData()
			{
				EncoderInfoData lEncoderInfoData;

				lEncoderInfoData.mGUID = __uuidof(AACMFTEncoder);

				lEncoderInfoData.mMediaSubType = W_AACAudioEncoderManager::getMediaSubType();

				lEncoderInfoData.mIsStreaming = TRUE;

				lEncoderInfoData.mMaxBitRate = gMaxByteRate * 8;

				lEncoderInfoData.mMinBitRate = gMaxByteRate * 8;

				return lEncoderInfoData;
			}


			static const InstanceMaker<W_AACAudioEncoderManager, Singleton<EncoderManagerFactory>> staticInstanceMaker(
				getEncoderInfoData());
			
			// IEncoderManager interface

			HRESULT W_AACAudioEncoderManager::enumEncoderMediaTypes(
				IMFMediaType* aPtrUncompressedMediaType,
				EncodingSettings aEncodingSettings,
				REFGUID aRefEncoderCLSID,
				std::vector<CComPtrCustom<IUnknown>> &aRefListOfMediaTypes)
			{
				HRESULT lresult;

				do
				{

					LOG_CHECK_STATE_DESCR(
						(aEncodingSettings.mEncodingMode != EncodingSettings::StreamingCBR)
						,
						E_NOTIMPL);
					
					LOG_CHECK_PTR_MEMORY(aPtrUncompressedMediaType);
					
					CComPtrCustom<IMFMediaType> lMediaType;

					LOG_INVOKE_MF_FUNCTION(MFCreateMediaType, &lMediaType);
										
					LOG_INVOKE_MF_METHOD(CopyAllItems,
						aPtrUncompressedMediaType,
						lMediaType);

					LOG_INVOKE_FUNCTION(checkAndFixInputMediatype, &lMediaType);
					
					CComPtrCustom<IMFTransform> lEncoder;

					LOG_INVOKE_OBJECT_METHOD(lEncoder, CoCreateInstance, aRefEncoderCLSID);
								
					if (aEncodingSettings.mEncodingMode == EncodingSettings::StreamingCBR)
					{
						CComPtrCustom<IMFMediaType> lOutputMediaType;

						LOG_INVOKE_MF_FUNCTION(MFCreateMediaType, &lOutputMediaType);

						LOG_INVOKE_FUNCTION(createOutputMediaType,
							lMediaType,
							lOutputMediaType);

						LOG_INVOKE_MF_METHOD(SetUINT32,
							lOutputMediaType,
							MF_MT_AUDIO_AVG_BYTES_PER_SECOND,
							gMaxByteRate);

						if (IsWindows8OrGreater())
						{
							LOG_INVOKE_MF_METHOD(SetUINT32,
								lOutputMediaType,
								MF_MT_AAC_PAYLOAD_TYPE,
								1);
						}

						LOG_INVOKE_MF_METHOD(SetInputType,
							lEncoder,
							0,
							lMediaType,
							0);

						LOG_INVOKE_MF_METHOD(SetOutputType,
							lEncoder,
							0,
							lOutputMediaType,
							0);

						aRefListOfMediaTypes.push_back(lOutputMediaType.detach());
					}

				} while (false);

				return lresult;
			}

			HRESULT W_AACAudioEncoderManager::getCompressedMediaType(
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
					
					std::vector<CComPtrCustom<IUnknown>> lListOfMediaTypes;

					LOG_INVOKE_FUNCTION(enumEncoderMediaTypes,
						aPtrUncompressedMediaType,
						aEncodingSettings,
						aRefEncoderCLSID,
						lListOfMediaTypes);

					LOG_CHECK_STATE(lListOfMediaTypes.empty());
					
					if (lIndexCompressedMediaType >= lListOfMediaTypes.size())
					{
						LOG_INVOKE_QUERY_INTERFACE_METHOD(lListOfMediaTypes[lListOfMediaTypes.size() - 1], aPtrPtrCompressedMediaType);
					}
					else
					{
						LOG_INVOKE_QUERY_INTERFACE_METHOD(lListOfMediaTypes[lIndexCompressedMediaType], aPtrPtrCompressedMediaType);
					}

				} while (false);

				return lresult;
			}

			HRESULT W_AACAudioEncoderManager::getEncoder(
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
					
					CComPtrCustom<IMFMediaType> lOutputMediaType;

					std::vector<CComPtrCustom<IUnknown>> lListOfMediaTypes;

					LOG_INVOKE_FUNCTION(getCompressedMediaType,
						aPtrUncompressedMediaType,
						aEncodingSettings,
						aRefEncoderCLSID,
						lIndexCompressedMediaType,
						&lOutputMediaType);
										
					CComPtrCustom<IMFMediaType> lMediaType;

					LOG_INVOKE_MF_FUNCTION(MFCreateMediaType, &lMediaType);
					
					LOG_INVOKE_MF_METHOD(CopyAllItems,
						aPtrUncompressedMediaType,
						lMediaType);
					
					LOG_INVOKE_FUNCTION(checkAndFixInputMediatype,&lMediaType);
					
					CComPtrCustom<IMFTransform> lEncoder;

					LOG_INVOKE_OBJECT_METHOD(lEncoder, CoCreateInstance, aRefEncoderCLSID);
					
					LOG_INVOKE_MF_METHOD(SetOutputType,
						lEncoder,
						0,
						lOutputMediaType,
						0);

					LOG_INVOKE_MF_METHOD(SetInputType,
						lEncoder,
						0,
						lMediaType,
						0);

					*aPtrPtrEncoderTransform = lEncoder.Detach();
					
					CComPtrCustom<IMFMediaType> lMediaType1;

					LOG_INVOKE_MF_METHOD(GetInputCurrentType,
						(*aPtrPtrEncoderTransform),
						0,
						&lMediaType1);

				} while (false);

				return lresult;
			}

			// W_AACAudioEncoderManager interface

			GUID W_AACAudioEncoderManager::getMediaSubType()
			{
				return MFAudioFormat_AAC;
			}

			HRESULT W_AACAudioEncoderManager::checkAndFixInputMediatype(
				IMFMediaType** aPtrPtrInputMediaType)
			{
				HRESULT lresult;

				do
				{

					GUID lMajorType;

					LOG_INVOKE_MF_METHOD(GetGUID,
						(*aPtrPtrInputMediaType),
						MF_MT_MAJOR_TYPE,
						&lMajorType);
					
					LOG_CHECK_STATE_DESCR(lMajorType != MFMediaType_Audio, MF_E_INVALIDMEDIATYPE);
					
					GUID lSubType;

					LOG_INVOKE_MF_METHOD(GetGUID,
						(*aPtrPtrInputMediaType),
						MF_MT_SUBTYPE,
						&lSubType);

					if (lSubType != MFAudioFormat_PCM)
					{
						LOG_INVOKE_MF_METHOD(SetGUID, 
							(*aPtrPtrInputMediaType),
							MF_MT_SUBTYPE,
							MFAudioFormat_PCM);
					}

					UINT32 lvalue = 0;

					LOG_INVOKE_MF_METHOD(GetUINT32,
						(*aPtrPtrInputMediaType),
						MF_MT_AUDIO_BITS_PER_SAMPLE,
						&lvalue);
					
					if (lvalue != 16)
					{
						LOG_INVOKE_MF_METHOD(SetUINT32,
							(*aPtrPtrInputMediaType),
							MF_MT_AUDIO_BITS_PER_SAMPLE,
							16);
					}

					LOG_INVOKE_MF_METHOD(GetUINT32,
						(*aPtrPtrInputMediaType),
						MF_MT_AUDIO_SAMPLES_PER_SECOND,
						&lvalue);
					
					if (lvalue != 44100 && lvalue != 48000)
					{
						LOG_INVOKE_MF_METHOD(SetUINT32,
							(*aPtrPtrInputMediaType),
							MF_MT_AUDIO_SAMPLES_PER_SECOND,
							48000);
					}
					
					LOG_INVOKE_MF_METHOD(GetUINT32,
						(*aPtrPtrInputMediaType),
						MF_MT_AUDIO_NUM_CHANNELS,
						&lvalue);

					if (lvalue != 1 && lvalue != 2)
					{
						lvalue = 2;
					}

					LOG_INVOKE_MF_METHOD(SetUINT32,
						(*aPtrPtrInputMediaType),
						MF_MT_AUDIO_NUM_CHANNELS,
						lvalue);

					LOG_INVOKE_MF_METHOD(SetUINT32,
						(*aPtrPtrInputMediaType),
						MF_MT_AUDIO_CHANNEL_MASK,
						lvalue == 2 ? 0x3 : 0x4);
					
					LOG_INVOKE_MF_METHOD(SetUINT32,
						(*aPtrPtrInputMediaType),
						MF_MT_AUDIO_BLOCK_ALIGNMENT,
						lvalue * 2);
					
					LOG_INVOKE_MF_METHOD(SetUINT32,
						(*aPtrPtrInputMediaType),
						MF_MT_AUDIO_PREFER_WAVEFORMATEX,
						TRUE);

				} while (false);

				return lresult;
			}

			HRESULT W_AACAudioEncoderManager::createOutputMediaType(
				IMFMediaType* aPtrOriginalInputMediaType,
				IMFMediaType* aPtrOutputMediaType)
			{
				HRESULT lresult;

				do
				{

					LOG_CHECK_PTR_MEMORY(aPtrOriginalInputMediaType);
				
					LOG_CHECK_PTR_MEMORY(aPtrOutputMediaType);
					
					PROPVARIANT lVarItem;

					LOG_INVOKE_MF_METHOD(SetGUID, 
						aPtrOutputMediaType,
						MF_MT_MAJOR_TYPE,
						MFMediaType_Audio);

					LOG_INVOKE_MF_METHOD(SetGUID, 
						aPtrOutputMediaType,
						MF_MT_SUBTYPE,
						MFAudioFormat_AAC);
					
					LOG_INVOKE_MF_METHOD(SetUINT32,
						aPtrOutputMediaType,
						MF_MT_AUDIO_BITS_PER_SAMPLE,
						16);
					
					LOG_INVOKE_MF_METHOD(GetItem,
						aPtrOriginalInputMediaType,
						MF_MT_AUDIO_SAMPLES_PER_SECOND,
						&lVarItem);

					LOG_INVOKE_MF_METHOD(SetItem,
						aPtrOutputMediaType,
						MF_MT_AUDIO_SAMPLES_PER_SECOND,
						lVarItem);
					
					LOG_INVOKE_MF_METHOD(GetItem,
						aPtrOriginalInputMediaType,
						MF_MT_AUDIO_NUM_CHANNELS,
						&lVarItem);
					
					LOG_INVOKE_MF_METHOD(SetItem,
						aPtrOutputMediaType,
						MF_MT_AUDIO_NUM_CHANNELS,
						lVarItem);
										
				} while (false);

				return lresult;
			}
		}
	}
}