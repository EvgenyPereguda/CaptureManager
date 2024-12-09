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

#include "../Common/Common.h"
#include "../Common/Singleton.h"
#include "../Common/MFHeaders.h"
#include "../Common/GUIDs.h"
#include "../ConfigManager/ConfigManager.h"
#include "EncoderManager.h"
#include "EncoderManagerFactory.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "IEncoderManager.h"
#include "propvarutil.h"
#include "DMORt.h"
#include "Codecapi.h"

namespace CaptureManager
{
	namespace Transform
	{
		namespace Encoder
		{
			using namespace CaptureManager::Core;

			GUID EncoderManager::getModeGUID(
				EncodingSettings::EncodingMode aEncodingMode)
			{
				switch (aEncodingMode)
				{
				case CaptureManager::EncodingSettings::CBR:
				case CaptureManager::EncodingSettings::VBR:
				case CaptureManager::EncodingSettings::StreamingCBR:
				{
					auto lmap = getModeGUIDCollection();

					for (auto& litem : lmap)
					{
						if (litem.second == aEncodingMode)
							return litem.first;
					}

					return GUID_NULL;

				}
				case CaptureManager::EncodingSettings::NONE:
				default:
					return GUID_NULL;

				}
			}

			HRESULT EncoderManager::fixVBRQUALITY(
				EncodingSettings& aRefEncodingSettings)
			{
				HRESULT lresult;

				do
				{
					if (aRefEncodingSettings.mEncodingMode == EncodingSettings::StreamingCBR)
						aRefEncodingSettings.mAverageBitrateValue = aRefEncodingSettings.mEncodingModeValue;
					else
						if (aRefEncodingSettings.mEncodingMode == EncodingSettings::VBR)
						{
							if (aRefEncodingSettings.mEncodingModeValue <= 3)
								aRefEncodingSettings.mEncodingModeValue = 1;
							else if (aRefEncodingSettings.mEncodingModeValue <= 6)
								aRefEncodingSettings.mEncodingModeValue = 4;
							else if (aRefEncodingSettings.mEncodingModeValue <= 10)
								aRefEncodingSettings.mEncodingModeValue = 8;
							else if (aRefEncodingSettings.mEncodingModeValue <= 13)
								aRefEncodingSettings.mEncodingModeValue = 11;
							else if (aRefEncodingSettings.mEncodingModeValue <= 16)
								aRefEncodingSettings.mEncodingModeValue = 15;
							else if (aRefEncodingSettings.mEncodingModeValue <= 20)
								aRefEncodingSettings.mEncodingModeValue = 18;
							else if (aRefEncodingSettings.mEncodingModeValue <= 22)
								aRefEncodingSettings.mEncodingModeValue = 22;
							else if (aRefEncodingSettings.mEncodingModeValue <= 26)
								aRefEncodingSettings.mEncodingModeValue = 25;
							else if (aRefEncodingSettings.mEncodingModeValue <= 30)
								aRefEncodingSettings.mEncodingModeValue = 29;
							else if (aRefEncodingSettings.mEncodingModeValue <= 33)
								aRefEncodingSettings.mEncodingModeValue = 33;
							else if (aRefEncodingSettings.mEncodingModeValue <= 36)
								aRefEncodingSettings.mEncodingModeValue = 36;
							else if (aRefEncodingSettings.mEncodingModeValue <= 40)
								aRefEncodingSettings.mEncodingModeValue = 40;
							else if (aRefEncodingSettings.mEncodingModeValue <= 45)
								aRefEncodingSettings.mEncodingModeValue = 43;
							else if (aRefEncodingSettings.mEncodingModeValue <= 47)
								aRefEncodingSettings.mEncodingModeValue = 47;
							else if (aRefEncodingSettings.mEncodingModeValue <= 50)
								aRefEncodingSettings.mEncodingModeValue = 50;
							else if (aRefEncodingSettings.mEncodingModeValue <= 54)
								aRefEncodingSettings.mEncodingModeValue = 54;
							else if (aRefEncodingSettings.mEncodingModeValue <= 58)
								aRefEncodingSettings.mEncodingModeValue = 58;
							else if (aRefEncodingSettings.mEncodingModeValue <= 61)
								aRefEncodingSettings.mEncodingModeValue = 61;
							else if (aRefEncodingSettings.mEncodingModeValue <= 65)
								aRefEncodingSettings.mEncodingModeValue = 65;
							else if (aRefEncodingSettings.mEncodingModeValue <= 68)
								aRefEncodingSettings.mEncodingModeValue = 68;
							else if (aRefEncodingSettings.mEncodingModeValue <= 72)
								aRefEncodingSettings.mEncodingModeValue = 72;
							else if (aRefEncodingSettings.mEncodingModeValue <= 75)
								aRefEncodingSettings.mEncodingModeValue = 75;
							else if (aRefEncodingSettings.mEncodingModeValue <= 79)
								aRefEncodingSettings.mEncodingModeValue = 79;
							else if (aRefEncodingSettings.mEncodingModeValue <= 83)
								aRefEncodingSettings.mEncodingModeValue = 83;
							else if (aRefEncodingSettings.mEncodingModeValue <= 86)
								aRefEncodingSettings.mEncodingModeValue = 86;
							else if (aRefEncodingSettings.mEncodingModeValue <= 90)
								aRefEncodingSettings.mEncodingModeValue = 90;
							else if (aRefEncodingSettings.mEncodingModeValue <= 93)
								aRefEncodingSettings.mEncodingModeValue = 93;
							else if (aRefEncodingSettings.mEncodingModeValue <= 97)
								aRefEncodingSettings.mEncodingModeValue = 97;
							else
								aRefEncodingSettings.mEncodingModeValue = 100;
						}

					lresult = S_OK;

				} while (false);
				
				return lresult;
			}

			std::wstring EncoderManager::getModeTitle(
				EncodingSettings::EncodingMode aEncodingMode)
			{
				std::wstring lresult = L"Unknown encoding mode";

				switch (aEncodingMode)
				{
				case CaptureManager::EncodingSettings::CBR:
					lresult = L"Constant bit rate";
					break;
				case CaptureManager::EncodingSettings::VBR:
					lresult = L"Variable bit rate";
					break;
				case CaptureManager::EncodingSettings::StreamingCBR:
					lresult = L"Streaming Constant bit rate";
					break;
				default:
				case CaptureManager::EncodingSettings::NONE:
					break;
				}

				return lresult;
			}

			EncodingSettings::EncodingMode EncoderManager::getEncodingMode(
				REFGUID aRefModeGUID)
			{
				auto lmap = getModeGUIDCollection();

				auto lfinditer = lmap.find(aRefModeGUID);

				if (lfinditer == lmap.end())
					return EncodingSettings::EncodingMode::NONE;

				return (*lfinditer).second;
			}

			std::map<GUID, EncodingSettings::EncodingMode, EncoderManager::GUIDComparer> EncoderManager::getModeGUIDCollection()
			{
				std::map<GUID, EncodingSettings::EncodingMode, EncoderManager::GUIDComparer> lModeGUIDMap;

				lModeGUIDMap[{ 0xca37e2be, 0xbec0, 0x4b17, { 0x94, 0x6d, 0x44, 0xfb, 0xc1, 0xb3, 0xdf, 0x55 } }] = CaptureManager::EncodingSettings::CBR;

				lModeGUIDMap[{ 0xee8c3745, 0xf45b, 0x42b3, { 0xa8, 0xcc, 0xc7, 0xa6, 0x96, 0x44, 0x9, 0x55 } }] = CaptureManager::EncodingSettings::VBR;

				// {8F6FF1B6-534E-49C0-B2A8-16D534EAF135}
				lModeGUIDMap[{ 0x8f6ff1b6, 0x534e, 0x49c0, { 0xb2, 0xa8, 0x16, 0xd5, 0x34, 0xea, 0xf1, 0x35 } }] = CaptureManager::EncodingSettings::StreamingCBR;
				
				return lModeGUIDMap;
			}

			HRESULT EncoderManager::getDictionaryOfVideoEncoders(
				std::list<EncoderInfo>& aRefVideoEncoderDic)
			{
				HRESULT lresult;

				do
				{

					aRefVideoEncoderDic.clear();									

					CComPtrCustom<IUnknown> ltestObject;

					if (SUCCEEDED(ltestObject.CoCreateInstance(__uuidof(CWMVXEncMediaObject))))
					{
						EncoderInfo lEncoderInfo;

						lEncoderInfo.mName = L"WMVideo8 Encoder MFT";

						lEncoderInfo.mGUID = __uuidof(CWMVXEncMediaObject);

						aRefVideoEncoderDic.push_back(lEncoderInfo);
					}

					if (SUCCEEDED(ltestObject.CoCreateInstance(__uuidof(CWMV9EncMediaObject))))
					{
						EncoderInfo lEncoderInfo;

						lEncoderInfo.mName = L"WMVideo9 Encoder MFT";

						lEncoderInfo.mGUID = __uuidof(CWMV9EncMediaObject);

						aRefVideoEncoderDic.push_back(lEncoderInfo);
					}

					if (SUCCEEDED(ltestObject.CoCreateInstance(__uuidof(CMSSCEncMediaObject2))))
					{
						EncoderInfo lEncoderInfo;

						lEncoderInfo.mName = L"WMVideo9 Screen Encoder MFT";

						lEncoderInfo.mGUID = __uuidof(CMSSCEncMediaObject2);

						lEncoderInfo.mIsStreaming;

						aRefVideoEncoderDic.push_back(lEncoderInfo);
					}

					if (SUCCEEDED(ltestObject.CoCreateInstance(__uuidof(CMSH264EncoderMFT))))
					{
						EncoderInfo lEncoderInfo;

						lEncoderInfo.mName = L"H264 Microsoft Encoder MFT";

						lEncoderInfo.mGUID = __uuidof(CMSH264EncoderMFT);

						lEncoderInfo.mIsStreaming = TRUE;

						aRefVideoEncoderDic.push_back(lEncoderInfo);
					}
										
					if (Singleton<ConfigManager>::getInstance().isWindows10_With_VP_Or_Greater())
					{
						EncoderInfo lEncoderInfo;

						lEncoderInfo.mName = L"H265 Microsoft Encoder MFT";

						lEncoderInfo.mGUID = MFVideoFormat_HEVC;

						lEncoderInfo.mIsStreaming = TRUE;

						aRefVideoEncoderDic.push_back(lEncoderInfo);
					}

//#if _WIN64
//
//					class DECLSPEC_UUID("F2F84074-8BCA-40BD-9159-E880F673DD3B")
//						CH265EncoderMFTProxy;
//
//					if (SUCCEEDED(ltestObject.CoCreateInstance(__uuidof(CH265EncoderMFTProxy))))
//					{
//						aRefVideoEncoderDic[L"HEVC Encoder MFT"] = __uuidof(CH265EncoderMFTProxy);
//					}
//
//#endif


					//HRESULT hr = S_OK;
					//UINT32 count = 0;

					//IMFActivate **ppActivate = NULL;

					//MFT_REGISTER_TYPE_INFO info = { 0 };

					//info.guidMajorType = MFMediaType_Video;
					//info.guidSubtype = MFVideoFormat_MPEG2;

					//hr = LOG_INVOKE_MF_FUNCTION(MFTEnumEx,
					//	MFT_CATEGORY_VIDEO_ENCODER,
					//	MFT_ENUM_FLAG_ALL,
					//	NULL,       // Input type
					//	&info,      // Output type
					//	&ppActivate,
					//	&count
					//	);

			

					//if (SUCCEEDED(hr) && count == 0)
					//{
					//	hr = MF_E_TOPO_CODEC_NOT_FOUND;
					//}

					//IMFTransform *lg;
					//LPWSTR  ppwszValue;
					//UINT32  pcchLength;


					//ppActivate[0]->GetAllocatedString(MFT_FRIENDLY_NAME_Attribute,
					//	&ppwszValue,
					//	&pcchLength);

					//GUID lGUID;

					//ppActivate[0]->GetGUID(MFT_TRANSFORM_CLSID_Attribute, &lGUID);

					//LPWSTR k;

					//StringFromCLSID(lGUID, &k);

					//"AA243E5D-2F73-48c7-97F7-F6FA17651651"

					//"Intel® Quick Sync Video H.264 Encoder MFT" - "{4BE8D3C0-0515-4A37-AD55-E4BAE19AF471}"

					

					// Create the first encoder in the list.


					//for (UINT32 i = 0; i < count; i++)
					//{
					//	ppActivate[i]->Release();
					//}
					//CoTaskMemFree(ppActivate);


					lresult = S_OK;

				} while (false);
				
				return lresult;
			}

			HRESULT EncoderManager::getDictionaryOfAudioEncoders(
				std::list<EncoderInfo>& aRefAudioEncoderDic)
			{
				HRESULT lresult;

				do
				{
					aRefAudioEncoderDic.clear();
					
					CComPtrCustom<IUnknown> ltestObject;

					if (SUCCEEDED(ltestObject.CoCreateInstance(__uuidof(CWMAEncMediaObject))))
					{
						EncoderInfo lEncoderInfo;

						lEncoderInfo.mName = L"WMAudio Encoder MFT";

						lEncoderInfo.mGUID = __uuidof(CWMAEncMediaObject);

						lEncoderInfo.mIsStreaming = FALSE;

						aRefAudioEncoderDic.push_back(lEncoderInfo);
					}

					if (SUCCEEDED(ltestObject.CoCreateInstance(__uuidof(CWMSPEncMediaObject2))))
					{
						EncoderInfo lEncoderInfo;

						lEncoderInfo.mName = L"WMSpeech Encoder DMO";

						lEncoderInfo.mGUID = __uuidof(CWMSPEncMediaObject2);

						lEncoderInfo.mIsStreaming = FALSE;

						aRefAudioEncoderDic.push_back(lEncoderInfo);
					}

					if (SUCCEEDED(ltestObject.CoCreateInstance(__uuidof(AACMFTEncoder))))
					{
						EncoderInfo lEncoderInfo;

						lEncoderInfo.mName = L"Microsoft AAC Audio Encoder MFT";

						lEncoderInfo.mGUID = __uuidof(AACMFTEncoder);

						lEncoderInfo.mIsStreaming = TRUE;

						aRefAudioEncoderDic.push_back(lEncoderInfo);
					}
					
					lresult = S_OK;
					
				} while (false);

				return lresult;
			}

			HRESULT EncoderManager::enumEncoderMediaTypes(
				IMFMediaType* aPtrMediaType,
				REFGUID aRefEncoderCLSID,
				std::vector<CComPtrCustom<IUnknown>> &aRefListOfMediaTypes,
				EncodingSettings aEncodingSettings)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrMediaType);

					fixVBRQUALITY(aEncodingSettings);

					std::list<Transform::Encoder::EncoderInfo> lEncoderDic;


					GUID lMajorType;

					LOG_INVOKE_MF_METHOD(GetGUID,
						aPtrMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);
										
					if (lMajorType == MFMediaType_Video)
					{
						lresult = getDictionaryOfVideoEncoders(lEncoderDic);
					}
					else if (lMajorType == MFMediaType_Audio)
					{
						lresult = getDictionaryOfAudioEncoders(lEncoderDic);
					}
					else
					{
						lresult = E_INVALIDARG;
					}

					LOG_CHECK_STATE(FAILED(lresult));
					
					lresult = E_BOUNDS;

					for (auto litem : lEncoderDic)
					{
						if (litem.mGUID == aRefEncoderCLSID)
						{
							lresult = S_OK;

							break;
						}
					}

					LOG_CHECK_STATE_DESCR(FAILED(lresult), E_BOUNDS);
										
					CComPtrCustom<IEncoderManager> lIEncoderManager;

					LOG_INVOKE_FUNCTION(Singleton<EncoderManagerFactory>::getInstance().createEncoderManager,
						aRefEncoderCLSID,
						&lIEncoderManager);
										
					LOG_INVOKE_POINTER_METHOD(lIEncoderManager, enumEncoderMediaTypes,
						aPtrMediaType,
						aEncodingSettings,
						aRefEncoderCLSID,
						aRefListOfMediaTypes);
									
				} while (false);
				
				return lresult;
			}



			HRESULT EncoderManager::getMaxMinBitRate(
				REFGUID aRefEncoderCLSID,
				UINT32& aRefMaxBitRate,
				UINT32& aRefMinBitRate)
			{
				HRESULT lresult;

				do
				{
					LOG_INVOKE_FUNCTION(Singleton<EncoderManagerFactory>::getInstance().getMaxMinBitRate,
						aRefEncoderCLSID,
						aRefMaxBitRate,
						aRefMinBitRate);
				} while (false);

				return lresult;
			}
						
			HRESULT EncoderManager::getCompressedMediaType(
				IMFMediaType* aPtrUncompressedMediaType,
				EncodingSettings aEncodingSettings,
				REFGUID aRefGUIDuidCategory,
				REFGUID aRefEncoderCLSID,
				DWORD lIndexCompressedMediaType,
				IMFMediaType** aPtrPtrCompressedMediaType)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrUncompressedMediaType);

					fixVBRQUALITY(aEncodingSettings);

					LOG_CHECK_PTR_MEMORY(aPtrPtrCompressedMediaType);

					std::list<Transform::Encoder::EncoderInfo> lAudioEncoderDic;
					
					LOG_INVOKE_FUNCTION(getDictionaryOfAudioEncoders, lAudioEncoderDic);
					
					lresult = E_BOUNDS;

					for (auto litem : lAudioEncoderDic)
					{
						if (litem.mGUID == aRefEncoderCLSID)
						{
							lresult = S_OK;

							break;
						}
					}

					if (FAILED(lresult))
					{

						std::list<Transform::Encoder::EncoderInfo> lVideoEncoderDic;

						LOG_INVOKE_FUNCTION(getDictionaryOfVideoEncoders, lVideoEncoderDic);
						
						lresult = E_BOUNDS;

						for (auto litem : lVideoEncoderDic)
						{
							if (litem.mGUID == aRefEncoderCLSID)
							{
								lresult = S_OK;

								break;
							}
						}
					}

					LOG_CHECK_STATE(FAILED(lresult));

					CComPtrCustom<IEncoderManager> lIEncoderManager;

					LOG_INVOKE_FUNCTION(Singleton<EncoderManagerFactory>::getInstance().createEncoderManager,
						aRefEncoderCLSID,
						&lIEncoderManager);
					
					LOG_INVOKE_POINTER_METHOD(lIEncoderManager, getCompressedMediaType,
						aPtrUncompressedMediaType,
						aEncodingSettings,
						aRefEncoderCLSID,
						lIndexCompressedMediaType,
						aPtrPtrCompressedMediaType);					

				} while (false);

				return lresult;
			}

			HRESULT EncoderManager::createEncoderTopologyNode(
				IMFMediaType* aPtrUncompressedMediaType,
				EncodingSettings aEncodingSettings,
				REFGUID aRefEncoderCLSID,
				DWORD lIndexCompressedMediaType,
				IMFTopologyNode** aPtrPtrEncoderTopologyNode)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrUncompressedMediaType);

					LOG_CHECK_PTR_MEMORY(aPtrPtrEncoderTopologyNode);
					
					fixVBRQUALITY(aEncodingSettings);

					std::list<Transform::Encoder::EncoderInfo> lAudioEncoderDic;

					LOG_INVOKE_FUNCTION(getDictionaryOfAudioEncoders, lAudioEncoderDic);
					
					lresult = E_BOUNDS;

					for (auto litem : lAudioEncoderDic)
					{
						if (litem.mGUID == aRefEncoderCLSID)
						{
							lresult = S_OK;

							break;
						}
					}

					if (FAILED(lresult))
					{

						std::list<Transform::Encoder::EncoderInfo> lVideoEncoderDic;

						LOG_INVOKE_FUNCTION(getDictionaryOfVideoEncoders, lVideoEncoderDic);
						
						lresult = E_BOUNDS;

						for (auto litem : lVideoEncoderDic)
						{
							if (litem.mGUID == aRefEncoderCLSID)
							{
								lresult = S_OK;

								break;
							}
						}
					}

					LOG_CHECK_STATE(FAILED(lresult));

					CComPtrCustom<IEncoderManager> lIEncoderManager;

					LOG_INVOKE_FUNCTION(Singleton<EncoderManagerFactory>::getInstance().createEncoderManager,
						aRefEncoderCLSID,
						&lIEncoderManager);
										
					CComPtrCustom<IMFTransform> lEncoderTransform;

					LOG_INVOKE_POINTER_METHOD(lIEncoderManager, getEncoder,
						aPtrUncompressedMediaType,
						aEncodingSettings,
						aRefEncoderCLSID,
						lIndexCompressedMediaType,
						&lEncoderTransform);

					LOG_CHECK_PTR_MEMORY(lEncoderTransform);

					CComPtrCustom<IMFTopologyNode> lEncoderNode;

					LOG_INVOKE_MF_FUNCTION(MFCreateTopologyNode, MF_TOPOLOGY_TRANSFORM_NODE, &lEncoderNode);
					
					LOG_INVOKE_MF_METHOD(SetObject, lEncoderNode, lEncoderTransform);

					LOG_INVOKE_MF_METHOD(SetUINT32, lEncoderNode, CM_HEADER, TRUE);
										
					*aPtrPtrEncoderTopologyNode = lEncoderNode.Detach();

				} while (false);
				
				return lresult;
			}
		}
	}
}