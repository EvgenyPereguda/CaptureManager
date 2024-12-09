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

#include "WMAudioEncoderManager.h"
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

				lEncoderInfoData.mGUID = WMAudioEncoderManager::getMediaSubType();

				lEncoderInfoData.mMediaSubType = WMAudioEncoderManager::getMediaSubType();

				lEncoderInfoData.mIsStreaming = FALSE;

				lEncoderInfoData.mMaxBitRate = gMaxBitRate;

				lEncoderInfoData.mMinBitRate = gMinBitRate;

				return lEncoderInfoData;
			}

			static const InstanceMaker<WMAudioEncoderManager, Singleton<EncoderManagerFactory>> staticInstanceMaker(
				getEncoderInfoData());
						
			// IEncoderManager interface

			HRESULT WMAudioEncoderManager::enumEncoderMediaTypes(
				IMFMediaType* aPtrUncompressedMediaType,
				EncodingSettings aEncodingSettings,
				REFGUID aRefEncoderCLSID,
				std::vector<CComPtrCustom<IUnknown>> &aRefListOfMediaTypes)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrUncompressedMediaType);
					
					CComPtrCustom<IMFMediaType> lMediaType;

					LOG_INVOKE_MF_FUNCTION(MFCreateMediaType, &lMediaType);
					
					LOG_INVOKE_FUNCTION(copyAndModifyInputMediaType,
						aPtrUncompressedMediaType,
						&lMediaType);
										
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
							lMediaType.Release();

							LOG_INVOKE_FUNCTION(createStubUncompressedMediaType,
								48000,
								2,
								16,
								&lMediaType);
							
							LOG_INVOKE_MF_METHOD(SetInputType,
								lEncoder,
								0,
								lMediaType,
								0);
						}
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
					
					if (FAILED(lresult) && 
						aEncodingSettings.mEncodingMode == EncodingSettings::VBR && 
						aRefListOfMediaTypes.empty())
					{

						if (aEncodingSettings.mEncodingModeValue >= 100)
							aEncodingSettings.mEncodingModeValue = 90;

						LOG_INVOKE_FUNCTION(setEncodingProperties,
							aPtrPropertyStore,
							aEncodingSettings);
						
						LOG_INVOKE_MF_METHOD(SetInputType,
							lEncoder,
							0,
							lMediaType,
							0);

						lTypeIndex = 0;

						while (SUCCEEDED(lresult))
						{

							CComPtrCustom<IMFMediaType> lOutputType;

							lresult = lEncoder->GetOutputAvailableType(0, lTypeIndex++, &lOutputType);

							if (SUCCEEDED(lresult))
							{
								aRefListOfMediaTypes.push_back(lOutputType.detach());
							}
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
			
			HRESULT WMAudioEncoderManager::getCompressedMediaType(
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
										
					if(aEncodingSettings.mEncodingMode == EncodingSettings::VBR)
					{

						if (aEncodingSettings.mEncodingModeValue <= 30)
							aEncodingSettings.mEncodingModeValue = 25;
						else if (aEncodingSettings.mEncodingModeValue <= 60)
							aEncodingSettings.mEncodingModeValue = 50;
						else if (aEncodingSettings.mEncodingModeValue <= 90)
							aEncodingSettings.mEncodingModeValue = 75;	
						else
							aEncodingSettings.mEncodingModeValue = 90;
					}

					std::vector<CComPtrCustom<IUnknown>> lListOfMediaTypes;

					LOG_INVOKE_FUNCTION(enumEncoderMediaTypes,
						aPtrUncompressedMediaType,
						aEncodingSettings,
						aRefEncoderCLSID,
						lListOfMediaTypes);
					
					LOG_CHECK_STATE_DESCR(lListOfMediaTypes.empty(), E_FAIL);
					
					if(aEncodingSettings.mEncodingMode == EncodingSettings::VBR)
						lListOfMediaTypes[0]->QueryInterface(IID_PPV_ARGS(aPtrPtrCompressedMediaType));
					else
						lListOfMediaTypes[lIndexCompressedMediaType]->QueryInterface(IID_PPV_ARGS(aPtrPtrCompressedMediaType));

					break;
					
					CComPtrCustom<IMFMediaType> lMediaType;

					LOG_INVOKE_MF_FUNCTION(MFCreateMediaType, &lMediaType);
					
					LOG_INVOKE_FUNCTION(copyAndModifyInputMediaType,
						aPtrUncompressedMediaType,
						&lMediaType);
										
					CComPtrCustom<IMFTransform> lEncoder;

					LOG_INVOKE_OBJECT_METHOD(lEncoder, CoCreateInstance,aRefEncoderCLSID);
					
					CComPtrCustom<IPropertyStore> aPtrPropertyStore;

					LOG_INVOKE_QUERY_INTERFACE_METHOD(lEncoder, &aPtrPropertyStore);
					
					GUID lMajorType;

					LOG_INVOKE_MF_METHOD(GetGUID,
						aPtrUncompressedMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);

					LOG_CHECK_STATE_DESCR(lMajorType != MFMediaType_Audio, MF_E_INVALIDMEDIATYPE);
					
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
							lMediaType.Release();

							LOG_INVOKE_FUNCTION(createStubUncompressedMediaType,
								48000,
								2,
								16,
								&lMediaType);
							
							LOG_INVOKE_MF_METHOD(SetInputType,
								lEncoder,
								0,
								lMediaType,
								0);
						}
					}

					LOG_INVOKE_FUNCTION(setEncodingProperties,
						aPtrPropertyStore,
						aEncodingSettings);

					CComPtrCustom<IMFMediaType> lOutputType;

					LOG_INVOKE_MF_METHOD(GetOutputAvailableType, lEncoder, 0, lIndexCompressedMediaType, &lOutputType);
					
					LOG_INVOKE_MF_METHOD(SetUINT32,
						lOutputType,
						MF_MT_AVG_BITRATE,
						aEncodingSettings.mAverageBitrateValue);
					
					*aPtrPtrCompressedMediaType = lOutputType.Detach();

				} while (false);

				return lresult;
			}

			HRESULT WMAudioEncoderManager::getEncoder(
				IMFMediaType* aPtrUncompressedMediaType,
				EncodingSettings aEncodingSettings,
				REFGUID aRefEncoderCLSID,
				DWORD aIndexCompressedMediaType,
				IMFTransform** aPtrPtrEncoderTransform)
			{
				HRESULT lresult;

				do
				{

					LOG_CHECK_PTR_MEMORY(aPtrUncompressedMediaType);
					
					LOG_CHECK_PTR_MEMORY(aPtrPtrEncoderTransform);
					
					CComPtrCustom<IMFMediaType> lOutputType;


					if(aEncodingSettings.mEncodingMode == EncodingSettings::VBR)
					{

						if (aEncodingSettings.mEncodingModeValue <= 30)
							aEncodingSettings.mEncodingModeValue = 25;
						else if (aEncodingSettings.mEncodingModeValue <= 60)
							aEncodingSettings.mEncodingModeValue = 50;
						else if (aEncodingSettings.mEncodingModeValue <= 90)
							aEncodingSettings.mEncodingModeValue = 75;
						else
							aEncodingSettings.mEncodingModeValue = 90;
					}

					std::vector<CComPtrCustom<IUnknown>> lListOfMediaTypes;

					LOG_INVOKE_FUNCTION(enumEncoderMediaTypes,
						aPtrUncompressedMediaType,
						aEncodingSettings,
						aRefEncoderCLSID,
						lListOfMediaTypes);
					
					LOG_CHECK_STATE_DESCR(lListOfMediaTypes.empty(), E_FAIL);
					
					LOG_CHECK_STATE(aIndexCompressedMediaType >= lListOfMediaTypes.size());

					if(aEncodingSettings.mEncodingMode == EncodingSettings::VBR)
					{
						lListOfMediaTypes[0]->QueryInterface(IID_PPV_ARGS(&lOutputType));
					}
					else
						lListOfMediaTypes[aIndexCompressedMediaType]->QueryInterface(IID_PPV_ARGS(&lOutputType));
					
					CComPtrCustom<IMFTransform> lEncoder;

					LOG_INVOKE_OBJECT_METHOD(lEncoder, CoCreateInstance,aRefEncoderCLSID);
										
					CComPtrCustom<IPropertyStore> aPtrPropertyStore;

					LOG_INVOKE_QUERY_INTERFACE_METHOD(lEncoder, &aPtrPropertyStore);
					
					GUID lMajorType;

					LOG_INVOKE_MF_METHOD(GetGUID,
						aPtrUncompressedMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);


					LOG_CHECK_STATE_DESCR(lMajorType != MFMediaType_Audio, MF_E_INVALIDMEDIATYPE);
					
					LOG_INVOKE_FUNCTION(setEncodingProperties,
						aPtrPropertyStore,
						aEncodingSettings);
					
					CComPtrCustom<IMFMediaType> lMediaType;

					LOG_INVOKE_MF_FUNCTION(MFCreateMediaType, &lMediaType);
					
					LOG_INVOKE_FUNCTION(copyAndModifyInputMediaType,
						aPtrUncompressedMediaType,
						&lMediaType);
					
					LOG_INVOKE_MF_METHOD(SetInputType, lEncoder, 0, lMediaType, 0);
					
					LOG_INVOKE_MF_METHOD(SetOutputType, lEncoder, 0, lOutputType, 0);
					
					*aPtrPtrEncoderTransform = lEncoder.Detach();

				} while (false);

				return lresult;
			}

			// WMAudioEncoderManager implementation

			GUID WMAudioEncoderManager::getMediaSubType()
			{
				return 
				{ 0x00000161, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71 } };
			}
			
			HRESULT WMAudioEncoderManager::setEncodingProperties(
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
						
						LOG_INVOKE_FUNCTION(InitPropVariantFromBoolean, VARIANT_TRUE, &var);
						
						LOG_INVOKE_POINTER_METHOD(aPtrPropertyStore, SetValue, MFPKEY_CONSTRAINENCLATENCY, var);
						
						LOG_INVOKE_FUNCTION(InitPropVariantFromUInt32, 100, &var);
						
						LOG_INVOKE_POINTER_METHOD(aPtrPropertyStore, SetValue, MFPKEY_MAXENCLATENCYMS, var);

						PropVariantClear(&var);
						
						break;

					case EncodingSettings::VBR:

						LOG_INVOKE_FUNCTION(InitPropVariantFromBoolean, VARIANT_TRUE, &var);
						
						LOG_INVOKE_POINTER_METHOD(aPtrPropertyStore, SetValue, MFPKEY_VBRENABLED, var);
						
						LOG_INVOKE_POINTER_METHOD(aPtrPropertyStore, SetValue, MFPKEY_CONSTRAIN_ENUMERATED_VBRQUALITY, var);
						
						LOG_INVOKE_FUNCTION(InitPropVariantFromUInt32, aEncodingSettings.mEncodingModeValue, &var);
						
						LOG_INVOKE_POINTER_METHOD(aPtrPropertyStore, SetValue, MFPKEY_DESIRED_VBRQUALITY, var);
									
						LOG_INVOKE_FUNCTION(InitPropVariantFromInt32, 1, &var);
						
						LOG_INVOKE_POINTER_METHOD(aPtrPropertyStore, SetValue, MFPKEY_PASSESUSED, var);
												
						LOG_INVOKE_FUNCTION(InitPropVariantFromBoolean, VARIANT_TRUE, &var);
						
						LOG_INVOKE_POINTER_METHOD(aPtrPropertyStore, SetValue, MFPKEY_CONSTRAINENCLATENCY, var);
						
						LOG_INVOKE_POINTER_METHOD(aPtrPropertyStore, SetValue, MFPKEY_CONSTRAINDECLATENCY, var);
						
						LOG_INVOKE_FUNCTION(InitPropVariantFromUInt32, 100, &var);
						
						LOG_INVOKE_POINTER_METHOD(aPtrPropertyStore, SetValue, MFPKEY_MAXENCLATENCYMS, var);
						
						LOG_INVOKE_POINTER_METHOD(aPtrPropertyStore, SetValue, MFPKEY_MAXDECLATENCYMS, var);

						PropVariantClear(&var);
						
						break;

					default:
						lresult = E_UNEXPECTED;
						break;
					}

				} while (false);

				return lresult;
			}

			HRESULT WMAudioEncoderManager::copyAndModifyInputMediaType(
				IMFMediaType* aPtrOriginalInputMediaType,
				IMFMediaType** aPtrPtrCopyAndModifyInputMediaType)
			{
				HRESULT lresult;

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
					
					UINT32 lFrequencyValue;
					
					LOG_INVOKE_MF_METHOD(GetUINT32,
						aPtrOriginalInputMediaType,
						MF_MT_AUDIO_SAMPLES_PER_SECOND,
						&lFrequencyValue);
					
					if (lFrequencyValue < 44100)
					{
						lFrequencyValue = 48000;
					}

					LOG_INVOKE_MF_METHOD(SetUINT32,
						(*aPtrPtrCopyAndModifyInputMediaType),
						MF_MT_AUDIO_SAMPLES_PER_SECOND,
						lFrequencyValue);
					
					UINT32 lNumChannelsValue;

					LOG_INVOKE_MF_METHOD(GetUINT32,
						aPtrOriginalInputMediaType,
						MF_MT_AUDIO_NUM_CHANNELS,
						&lNumChannelsValue);
					
					if (lNumChannelsValue != 2)
					{
						lNumChannelsValue = 2;
					}

					LOG_INVOKE_MF_METHOD(SetUINT32,
						(*aPtrPtrCopyAndModifyInputMediaType),
						MF_MT_AUDIO_NUM_CHANNELS,
						lNumChannelsValue);
					
					UINT32 lBitsvalue;

					LOG_INVOKE_MF_METHOD(GetUINT32,
						aPtrOriginalInputMediaType,
						MF_MT_AUDIO_BITS_PER_SAMPLE,
						&lBitsvalue);
					
					LOG_INVOKE_MF_METHOD(SetUINT32,
						(*aPtrPtrCopyAndModifyInputMediaType),
						MF_MT_AUDIO_BITS_PER_SAMPLE,
						lBitsvalue);
										
					LOG_INVOKE_MF_METHOD(GetItem,
						aPtrOriginalInputMediaType,
						MF_MT_MAJOR_TYPE,
						&lVarItem);
					
					LOG_INVOKE_MF_METHOD(SetItem,
						(*aPtrPtrCopyAndModifyInputMediaType),
						MF_MT_MAJOR_TYPE,
						lVarItem);
					
					GUID lSubType;

					LOG_INVOKE_MF_METHOD(GetGUID,
						aPtrOriginalInputMediaType,
						MF_MT_SUBTYPE,
						&lSubType);
					
					if (lSubType != MFAudioFormat_PCM && 
						lSubType != MFAudioFormat_Float)
					{
						((*aPtrPtrCopyAndModifyInputMediaType))->Release();
						
						LOG_INVOKE_FUNCTION(createStubUncompressedMediaType,
							lFrequencyValue,
							lNumChannelsValue,
							lBitsvalue,
							aPtrPtrCopyAndModifyInputMediaType);
					}

					LOG_INVOKE_MF_METHOD(SetGUID,
						((*aPtrPtrCopyAndModifyInputMediaType)),
						MF_MT_SUBTYPE,
						lSubType);
					
					UINT32 lvalue;

					LOG_INVOKE_MF_METHOD(GetUINT32,
						aPtrOriginalInputMediaType,
						MF_MT_ALL_SAMPLES_INDEPENDENT,
						&lvalue);

					if (lvalue != 1)
					{
						lvalue = 1;
					}

					LOG_INVOKE_MF_METHOD(SetUINT32,
						(*aPtrPtrCopyAndModifyInputMediaType),
						MF_MT_ALL_SAMPLES_INDEPENDENT,
						lvalue);
										
					LOG_INVOKE_MF_METHOD(SetUINT32,
						(*aPtrPtrCopyAndModifyInputMediaType),
						MF_MT_AUDIO_AVG_BYTES_PER_SECOND,
						(lBitsvalue / 8)*lNumChannelsValue*lFrequencyValue);
					
					LOG_INVOKE_MF_METHOD(SetUINT32,
						(*aPtrPtrCopyAndModifyInputMediaType),
						MF_MT_AUDIO_BLOCK_ALIGNMENT,
						(lBitsvalue / 8)*lNumChannelsValue);					

				} while (false);

				return lresult;
			}
			
			HRESULT WMAudioEncoderManager::createStubUncompressedMediaType(
				UINT32 aSamplePerSecond,
				UINT32 aNumChannels,
				UINT32 aBitsPerSample,
				IMFMediaType** aPtrPtrStubUncompressedMediaType)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrPtrStubUncompressedMediaType);
										
					UINT32 lbytePerSample = aBitsPerSample/8;

					CComPtrCustom<IMFMediaType> lMediaType;

					LOG_INVOKE_MF_FUNCTION(MFCreateMediaType, &lMediaType);
					
					LOG_INVOKE_MF_METHOD(SetGUID,
						lMediaType,
						MF_MT_MAJOR_TYPE,
						MFMediaType_Audio);
					
					LOG_INVOKE_MF_METHOD(SetGUID,
						lMediaType,
						MF_MT_SUBTYPE,
						MFAudioFormat_PCM);
					
					LOG_INVOKE_MF_METHOD(SetUINT32,
						lMediaType,
						MF_MT_ALL_SAMPLES_INDEPENDENT,
						1);
					
					LOG_INVOKE_MF_METHOD(SetUINT32,
						lMediaType,
						MF_MT_AUDIO_AVG_BYTES_PER_SECOND,
						aSamplePerSecond * aNumChannels * lbytePerSample);
					
					LOG_INVOKE_MF_METHOD(SetUINT32,
						lMediaType,
						MF_MT_AUDIO_BITS_PER_SAMPLE,
						aBitsPerSample);
					
					LOG_INVOKE_MF_METHOD(SetUINT32,
						lMediaType,
						MF_MT_AUDIO_BLOCK_ALIGNMENT,
						aNumChannels * lbytePerSample);
					
					LOG_INVOKE_MF_METHOD(SetUINT32,
						lMediaType,
						MF_MT_AUDIO_SAMPLES_PER_SECOND,
						aSamplePerSecond);
					
					LOG_INVOKE_MF_METHOD(SetUINT32,
						lMediaType,
						MF_MT_AUDIO_NUM_CHANNELS,
						aNumChannels);
					
					*aPtrPtrStubUncompressedMediaType = lMediaType.detach();

				} while (false);

				return lresult;
			}
		}
	}
}