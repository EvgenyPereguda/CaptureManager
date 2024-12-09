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

#include "W_H265VideoEncoderManager.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../Common/MFHeaders.h"
#include "../Common/InstanceMaker.h"
#include "../Common/Common.h"
#include "../Common/Singleton.h"
#include "../Common/GUIDs.h"
#include "EncoderManagerFactory.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../ConfigManager/ConfigManager.h"
#include "propvarutil.h"
#include <wmcodecdsp.h>
#include <VersionHelpers.h>

#define AVEncH265VProfile_Main_420_8 1

namespace CaptureManager
{
	namespace Transform
	{
		namespace Encoder
		{

			static const UINT32 gMaxBitRate(1 << 25);

			static const UINT32 gMinBitRate(1 << 16);
			
			using namespace Core;

			static EncoderInfoData getEncoderInfoData()
			{
				EncoderInfoData lEncoderInfoData;
				
				lEncoderInfoData.mMediaSubType = W_H265VideoEncoderManager::getMediaSubType();

				lEncoderInfoData.mIsStreaming = TRUE;

				lEncoderInfoData.mMaxBitRate = gMaxBitRate;

				lEncoderInfoData.mMinBitRate = gMinBitRate;

				return lEncoderInfoData;
			}

			static const InstanceMaker<W_H265VideoEncoderManager, Singleton<EncoderManagerFactory>> staticInstanceMaker(
				getEncoderInfoData());

			// IEncoderManager interface

			HRESULT W_H265VideoEncoderManager::enumEncoderMediaTypes(
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
							MFVideoFormat_NV12);
					}
					
					LOG_INVOKE_FUNCTION(checkAndFixInputMediatype,&lMediaType);
					
					CComPtrCustom<IMFTransform> lEncoder;

					do
					{
						LOG_INVOKE_OBJECT_METHOD(lEncoder, CoCreateInstance, aRefEncoderCLSID);

					} while (false);

					if (FAILED(lresult))
					{
						UINT32 count = 0;

						IMFActivate **ppActivate = NULL;

						MFT_REGISTER_TYPE_INFO info = { 0 };

						info.guidMajorType = MFMediaType_Video;
						info.guidSubtype = MFVideoFormat_HEVC;

						lresult = LOG_INVOKE_MF_FUNCTION(MFTEnumEx,
							MFT_CATEGORY_VIDEO_ENCODER,
							MFT_ENUM_FLAG_SYNCMFT,
							NULL,       // Input type
							&info,      // Output type
							&ppActivate,
							&count
							);

						if (SUCCEEDED(lresult) && count > 0)
						{
							for (size_t i = 0; i < count; i++)
							{
								lresult = ppActivate[i]->ActivateObject(IID_PPV_ARGS(&lEncoder));

								ppActivate[i]->Release();

								if (SUCCEEDED(lresult))
									break;

								lEncoder.Release();
							}

						}

						CoTaskMemFree(ppActivate);
					}

					LOG_CHECK_PTR_MEMORY(lEncoder);

					do
					{
						CComPtrCustom<ICodecAPI> aPtrCodecAPI;

						UINT32 lFrameRate = 15;

						UINT32 lNumerator = 0, lDenominator = 0;

						LOG_INVOKE_FUNCTION(MFGetAttributeRatio, lMediaType, MF_MT_FRAME_RATE, &lNumerator, &lDenominator);

						if (lDenominator > 0 && lNumerator > 0)
							lFrameRate = lNumerator / lDenominator;

						LOG_INVOKE_QUERY_INTERFACE_METHOD(lEncoder, &aPtrCodecAPI);

						LOG_INVOKE_FUNCTION(setEncodingProperties,
							aPtrCodecAPI,
							aEncodingSettings,
							lFrameRate);
												
						if (aEncodingSettings.mEncodingMode == EncodingSettings::StreamingCBR)
						{							
							if (aEncodingSettings.mAverageBitrateValue >= gMaxBitRate)
								aEncodingSettings.mAverageBitrateValue = gMaxBitRate;
							else
								if (aEncodingSettings.mAverageBitrateValue <= gMinBitRate)
									aEncodingSettings.mAverageBitrateValue = gMinBitRate;
							else
							{
								auto l_value = log2(aEncodingSettings.mAverageBitrateValue);

								aEncodingSettings.mAverageBitrateValue = 1 << (int)l_value;
							}
							
							LOG_INVOKE_FUNCTION(setEncodingProperties,
								aPtrCodecAPI,
								aEncodingSettings,
								lFrameRate);

							CComPtrCustom<IMFMediaType> lOutputMediaType;

							LOG_INVOKE_MF_FUNCTION(MFCreateMediaType, &lOutputMediaType);

							LOG_INVOKE_FUNCTION(createOutputMediaType,
								lMediaType,
								lOutputMediaType);

							LOG_INVOKE_MF_METHOD(SetUINT32,
								lOutputMediaType,
								MF_MT_MPEG2_PROFILE,
								AVEncH265VProfile_Main_420_8);

							LOG_INVOKE_MF_METHOD(SetUINT32,
								lOutputMediaType,
								MF_MT_AVG_BITRATE,
								aEncodingSettings.mAverageBitrateValue);

							LOG_INVOKE_MF_METHOD(SetOutputType,
								lEncoder,
								0,
								lOutputMediaType,
								0);

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
								//MFVideoFormat_NV12

								CComPtrCustom<IMFMediaType> lAvalableMediaType;

								LOG_INVOKE_FUNCTION(modifySubTypeOFMediaType,
									&lMediaType,
									MFVideoFormat_NV12);
									
								LOG_INVOKE_MF_METHOD(SetInputType,
									lEncoder,
									0,
									lMediaType,
									0);
							}

							aRefListOfMediaTypes.push_back(lOutputMediaType.detach());						
						}

					} while (false);

					CComQIPtrCustom<IMFShutdown> lShutdown;

					lShutdown = lEncoder;

					if (lShutdown)
						lShutdown->Shutdown();

				} while (false);

				return lresult;
			}

			HRESULT W_H265VideoEncoderManager::getCompressedMediaType(
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

			HRESULT W_H265VideoEncoderManager::getEncoder(
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
							MFVideoFormat_NV12);
					}

					LOG_INVOKE_FUNCTION(checkAndFixInputMediatype,&lMediaType);
					
					CComPtrCustom<IMFTransform> lEncoder;

					{
						UINT32 count = 0;

						IMFActivate **ppActivate = NULL;

						MFT_REGISTER_TYPE_INFO info = { 0 };

						info.guidMajorType = MFMediaType_Video;
						info.guidSubtype = MFVideoFormat_HEVC;

						lresult = LOG_INVOKE_MF_FUNCTION(MFTEnumEx,
							MFT_CATEGORY_VIDEO_ENCODER,
							MFT_ENUM_FLAG_SYNCMFT,
							NULL,       // Input type
							&info,      // Output type
							&ppActivate,
							&count
							);

						if (SUCCEEDED(lresult) && count > 0)
						{
							for (size_t i = 0; i < count; i++)
							{
								lresult = ppActivate[i]->ActivateObject(IID_PPV_ARGS(&lEncoder));

								ppActivate[i]->Release();

								if (SUCCEEDED(lresult))
									break;

								lEncoder.Release();
							}

						}

						CoTaskMemFree(ppActivate);
					}

					CComPtrCustom<ICodecAPI> aPtrCodecAPI;

					LOG_INVOKE_QUERY_INTERFACE_METHOD(lEncoder, &aPtrCodecAPI);
										

					if (aEncodingSettings.mEncodingMode == EncodingSettings::StreamingCBR)
					{
						UINT32 lBiteRate=0;

						LOG_INVOKE_MF_METHOD(GetUINT32,
							lOutputMediaType,
							MF_MT_AVG_BITRATE,
							&lBiteRate);

						aEncodingSettings.mAverageBitrateValue = lBiteRate;
					}

					UINT32 lFrameRate = 15;

					UINT32 lNumerator = 0, lDenominator = 0;

					LOG_INVOKE_FUNCTION(MFGetAttributeRatio, lMediaType, MF_MT_FRAME_RATE, &lNumerator, &lDenominator);

					if (lDenominator > 0 && lNumerator > 0)
						lFrameRate = lNumerator / lDenominator;

					LOG_INVOKE_FUNCTION(setEncodingProperties,
						aPtrCodecAPI,
						aEncodingSettings,
						lFrameRate);

					LOG_INVOKE_MF_METHOD(SetOutputType,
						lEncoder,
						0,
						lOutputMediaType,
						0);

					LOG_INVOKE_FUNCTION(modifySubTypeOFMediaType,
							&lMediaType,
							MFVideoFormat_NV12);

					LOG_INVOKE_MF_METHOD(SetInputType,
						lEncoder,
						0,
						lMediaType,
						0);
					
					
					*aPtrPtrEncoderTransform = lEncoder.Detach();

				} while (false);

				return lresult;
			}

			// W_H265VideoEncoderManager interface

			W_H265VideoEncoderManager::W_H265VideoEncoderManager()
			{
			}

			GUID W_H265VideoEncoderManager::getMediaSubType()
			{
				return MFVideoFormat_HEVC;
			}
			
			HRESULT W_H265VideoEncoderManager::checkAndFixInputMediatype(
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

					LOG_CHECK_STATE_DESCR(lMajorType != MFMediaType_Video, MF_E_INVALIDMEDIATYPE);
					
					GUID lSubType;

					LOG_INVOKE_MF_METHOD(GetGUID,
						(*aPtrPtrInputMediaType),
						MF_MT_SUBTYPE,
						&lSubType);

					if (lSubType == MFVideoFormat_I420 ||
						lSubType == MFVideoFormat_IYUV ||
						lSubType == MFVideoFormat_NV12 ||
						lSubType == MFVideoFormat_YUY2 ||
						lSubType == MFVideoFormat_YV12)
					{
						break;
					}
					

					UINT64 lUINT64Value;

					LOG_INVOKE_MF_METHOD(GetUINT64,
						(*aPtrPtrInputMediaType),
						MF_MT_FRAME_SIZE,
						&lUINT64Value);
					
					UINT32 lHight = 0, lWidth = 0;

					Unpack2UINT32AsUINT64(lUINT64Value, &lWidth, &lHight);

					LOG_CHECK_STATE_DESCR (lWidth <= 0, MF_E_INVALIDMEDIATYPE);
					
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

							LOG_INVOKE_MF_METHOD(GetGUID,
								lOutputType,
								MF_MT_SUBTYPE,
								&lSubType);
							
							if (lSubType == MFVideoFormat_I420 ||
								lSubType == MFVideoFormat_IYUV ||
								lSubType == MFVideoFormat_NV12 ||
								lSubType == MFVideoFormat_YUY2 ||
								lSubType == MFVideoFormat_YV12)
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

			HRESULT W_H265VideoEncoderManager::copyInputMediaType(
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


					UINT32 lHigh = 0, lLow = 0;

					DataParser::unpack2UINT32AsUINT64(lVarItem, lHigh, lLow);

					if (lHigh % 2 != 0 || lLow % 2 != 0)
					{
						lHigh = (lHigh >> 1) << 1;

						lLow = (lLow >> 1) << 1;

						do
						{

							LOG_INVOKE_FUNCTION(MediaFoundation::MediaFoundationManager::SetAttributeSize,
								aPtrCopyInputMediaType,
								MF_MT_FRAME_SIZE,
								lHigh,
								(lLow >> 1) << 1
							);

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
						} while (false);

					}
					else
					{
						LOG_INVOKE_MF_METHOD(SetItem,
							aPtrCopyInputMediaType,
							MF_MT_FRAME_SIZE,
							lVarItem);
					}

					do
					{
						LOG_INVOKE_MF_METHOD(GetItem,
							aPtrOriginalInputMediaType,
							MF_MT_INTERLACE_MODE,
							&lVarItem);

						LOG_INVOKE_MF_METHOD(SetItem,
							aPtrCopyInputMediaType,
							MF_MT_INTERLACE_MODE,
							lVarItem);

					} while (false);
					
					LOG_INVOKE_MF_METHOD(GetItem,
						aPtrOriginalInputMediaType,
						MF_MT_PIXEL_ASPECT_RATIO,
						&lVarItem);
					
					LOG_INVOKE_MF_METHOD(SetItem,
						aPtrCopyInputMediaType,
						MF_MT_PIXEL_ASPECT_RATIO,
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
						
						LOG_CHECK_STATE_DESCR(lstride == 0, MF_E_INVALIDMEDIATYPE);
						
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

					do
					{

						LOG_INVOKE_MF_METHOD(GetItem,
							aPtrOriginalInputMediaType,
							MF_MT_AVG_BITRATE,
							&lVarItem);

					} while (false);

					if (FAILED(lresult))
					{
						LONG lstride = 0;
						
						LOG_INVOKE_MF_METHOD(GetUINT32,
							aPtrCopyInputMediaType,
							MF_MT_DEFAULT_STRIDE,
							((UINT32*)&lstride));

						lstride = abs(lstride);

						LOG_INVOKE_MF_METHOD(GetItem,
							aPtrOriginalInputMediaType,
							MF_MT_FRAME_SIZE,
							&lVarItem);

						UINT32 lWidth = 0, lHeight = 0;

						LOG_INVOKE_FUNCTION(MFGetAttributeSize,
							aPtrOriginalInputMediaType, MF_MT_FRAME_SIZE, &lWidth, &lHeight);

						UINT32 lFrameSize = lHeight * lstride;


						UINT32 lNumerator = 0, lDenominator = 0;

						LOG_INVOKE_FUNCTION(MFGetAttributeRatio, aPtrOriginalInputMediaType, MF_MT_FRAME_RATE, &lNumerator, &lDenominator);

						UINT32 lBitRate = (lFrameSize * lNumerator * 8) / lDenominator;

						LOG_INVOKE_MF_METHOD(SetUINT32,
							aPtrCopyInputMediaType,
							MF_MT_AVG_BITRATE,
							lBitRate);
					}
					else
					{
						LOG_INVOKE_MF_METHOD(SetItem,
							aPtrCopyInputMediaType,
							MF_MT_AVG_BITRATE,
							lVarItem);
					}

				} while (false);

				return lresult;
			}

			HRESULT W_H265VideoEncoderManager::createOutputMediaType(
				IMFMediaType* aPtrOriginalInputMediaType,
				IMFMediaType* aPtrOutputMediaType)
			{
				HRESULT lresult;

				do
				{

					//using namespace pugi;

					//xml_document lxmlDoc;

					//auto ldeclNode = lxmlDoc.append_child(node_declaration);

					//ldeclNode.append_attribute(L"version") = L"1.0";

					//xml_node lcommentNode = lxmlDoc.append_child(node_comment);

					//lcommentNode.set_value(L"XML Document of sources");

					//auto lRootXMLElement = lxmlDoc.append_child(L"Sources");
					//DataParser::readMediaType(
					//	aPtrOriginalInputMediaType,
					//	lRootXMLElement);

					//std::wstringstream lwstringstream;

					//lxmlDoc.print(lwstringstream);

					////lxmlDoc.save_file("doc.xml");

					//std::wstring lXMLDocumentString;

					//lXMLDocumentString = lwstringstream.str();


					LOG_CHECK_PTR_MEMORY(aPtrOriginalInputMediaType);
					
					LOG_CHECK_PTR_MEMORY(aPtrOutputMediaType);
					
					PROPVARIANT lVarItem;

					LOG_INVOKE_MF_METHOD(SetGUID, 
						aPtrOutputMediaType,
						MF_MT_MAJOR_TYPE,
						MFMediaType_Video);
					
					LOG_INVOKE_MF_METHOD(SetGUID, 
						aPtrOutputMediaType,
						MF_MT_SUBTYPE,
						MFVideoFormat_HEVC);

					LOG_INVOKE_MF_METHOD(GetItem,
						aPtrOriginalInputMediaType,
						MF_MT_FRAME_RATE,
						&lVarItem);

					LOG_INVOKE_MF_METHOD(SetItem,
						aPtrOutputMediaType,
						MF_MT_FRAME_RATE,
						lVarItem);

					LOG_INVOKE_MF_METHOD(GetItem,
						aPtrOriginalInputMediaType,
						MF_MT_FRAME_SIZE,
						&lVarItem);

					LOG_INVOKE_MF_METHOD(SetItem,
						aPtrOutputMediaType,
						MF_MT_FRAME_SIZE,
						lVarItem);

					LOG_INVOKE_MF_METHOD(GetItem,
						aPtrOriginalInputMediaType,
						MF_MT_INTERLACE_MODE,
						&lVarItem);
					
					LOG_INVOKE_MF_METHOD(SetItem,
						aPtrOutputMediaType,
						MF_MT_INTERLACE_MODE,
						lVarItem);
					
					LOG_INVOKE_MF_METHOD(SetUINT32,
						aPtrOutputMediaType,
						MF_MT_AVG_BITRATE,
						1000000);

					LOG_INVOKE_MF_METHOD(GetItem,
						aPtrOriginalInputMediaType,
						MF_MT_PIXEL_ASPECT_RATIO,
						&lVarItem);
					
					LOG_INVOKE_MF_METHOD(SetItem,
						aPtrOutputMediaType,
						MF_MT_PIXEL_ASPECT_RATIO,
						lVarItem);

				} while (false);

				return lresult;
			}

			HRESULT W_H265VideoEncoderManager::setEncodingProperties(
				ICodecAPI* aPtrCodecAPI,
				EncodingSettings aEncodingSettings,
				UINT32 aFrameRate)
			{
				HRESULT lresult;

				VARIANT var;

				do
				{


					LOG_INVOKE_FUNCTION(InitVariantFromUInt32, 0, &var);
					
					LOG_CHECK_PTR_MEMORY(aPtrCodecAPI);

					LOG_CHECK_STATE_DESCR(aEncodingSettings.mEncodingMode == EncodingSettings::NONE, MF_E_NOT_INITIALIZED);
					
					switch (aEncodingSettings.mEncodingMode)
					{
					case EncodingSettings::CBR:

						lresult = E_NOTIMPL;

						break;

					case EncodingSettings::StreamingCBR:

						if (Singleton<ConfigManager>::getInstance().isWindows10_With_VP_Or_Greater())
						{

							LOG_INVOKE_FUNCTION(InitVariantFromUInt32, eAVEncCommonRateControlMode_CBR, &var);

							LOG_INVOKE_POINTER_METHOD(aPtrCodecAPI, SetValue, &CODECAPI_AVEncCommonRateControlMode, &var);

							LOG_INVOKE_FUNCTION(InitVariantFromUInt32, aEncodingSettings.mAverageBitrateValue, &var);

							LOG_INVOKE_POINTER_METHOD(aPtrCodecAPI, SetValue, &CODECAPI_AVEncCommonMeanBitRate, &var);

							//LOG_INVOKE_POINTER_METHOD(aPtrCodecAPI, SetValue, &CODECAPI_AVEncCommonMaxBitRate, &var);


							LOG_INVOKE_FUNCTION(InitVariantFromUInt32, aFrameRate, &var);

							LOG_INVOKE_POINTER_METHOD(aPtrCodecAPI, SetValue, &CODECAPI_AVEncMPVGOPSize, &var);

							

							

							//LOG_INVOKE_FUNCTION(InitVariantFromUInt32, 10, &var);

							//LOG_INVOKE_POINTER_METHOD(aPtrCodecAPI, SetValue, &CODECAPI_AVEncCommonQualityVsSpeed, &var);

							//LOG_INVOKE_FUNCTION(InitVariantFromBoolean, TRUE, &var);

							//LOG_INVOKE_POINTER_METHOD(aPtrCodecAPI, SetValue, &CODECAPI_AVLowLatencyMode, &var);
						}
						else
							lresult = E_NOTIMPL;

						break;

					case EncodingSettings::VBR:
						
						LOG_INVOKE_FUNCTION(InitVariantFromUInt32, eAVEncCommonRateControlMode_Quality, &var);
						
						LOG_INVOKE_POINTER_METHOD(aPtrCodecAPI, SetValue, &CODECAPI_AVEncCommonRateControlMode, &var);

						LOG_INVOKE_FUNCTION(InitVariantFromUInt32, aEncodingSettings.mEncodingModeValue, &var);
						
						LOG_INVOKE_POINTER_METHOD(aPtrCodecAPI, SetValue, &CODECAPI_AVEncCommonQuality, &var);
						
						if (IsWindows8OrGreater())
						{
							LOG_INVOKE_FUNCTION(InitVariantFromBoolean, TRUE, &var);
							
							LOG_INVOKE_POINTER_METHOD(aPtrCodecAPI, SetValue, &CODECAPI_AVLowLatencyMode, &var);						
						}

						break;

					default:
						lresult = E_UNEXPECTED;
						break;
					}

				} while (false);

				return lresult;
			}
		}
	}
}