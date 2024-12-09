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

#include "AudioTopologyResolver.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../Common/MFHeaders.h"
#include "../Common/ComPtrCustom.h"
#include "../Common/ComMassivPtr.h"
#include "../Common/Common.h"
#include "../Common/GUIDs.h"
#include "../CustomisedAudioResampler/CustomisedAudioResampler.h"


namespace CaptureManager
{
	namespace MediaSession
	{
		namespace CustomisedMediaSession
		{
			using namespace Core;

			// BaseTopologyResolver implementation
			
			HRESULT AudioTopologyResolver::getInputMediaTypeOfMediaSink(
				IMFTopology* aPtrTopology,
				IMFTopologyNode* aPtrUpStreamNode,
				IMFTopologyNode* aPtrDownStreamNode,
				IMFMediaType* aPtrUpStreamMediaType,
				IMFMediaType** aPtrPtrDownStreamMediaType,
				UINT32 aOutputStreamIndex)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrUpStreamNode);

					LOG_CHECK_PTR_MEMORY(aPtrDownStreamNode);

					LOG_CHECK_PTR_MEMORY(aPtrUpStreamMediaType);

					LOG_CHECK_PTR_MEMORY(aPtrPtrDownStreamMediaType);

					UINT32 lSinkStreamId = 0;

					LOG_INVOKE_MF_METHOD(GetUINT32,
						aPtrDownStreamNode,
						MF_TOPONODE_STREAMID,
						&lSinkStreamId);

					CComPtrCustom<IUnknown> lIUnknown;

					LOG_INVOKE_MF_METHOD(GetObject,
						aPtrDownStreamNode,
						&lIUnknown);

					CComQIPtrCustom<IMFActivate> lSinkActivate(lIUnknown);

					LOG_CHECK_PTR_MEMORY(lSinkActivate);

					CComPtrCustom<IMFMediaSink> lMediaSink;

					LOG_INVOKE_MF_METHOD(ActivateObject,
						lSinkActivate,
						__uuidof(IMFMediaSink),
						(void**)&lMediaSink);

					LOG_CHECK_PTR_MEMORY(lMediaSink);

					LOG_INVOKE_MF_METHOD(SetUnknown,
						aPtrDownStreamNode,
						CM_Sink,
						lMediaSink);
					
					CComPtrCustom<IMFStreamSink> lStreamSink;

					LOG_INVOKE_MF_METHOD(GetStreamSinkById,
						lMediaSink,
						lSinkStreamId,
						&lStreamSink);

					LOG_CHECK_PTR_MEMORY(lStreamSink);

					CComPtrCustom<IMFMediaTypeHandler> lMediaTypeHandler;

					LOG_INVOKE_MF_METHOD(GetMediaTypeHandler,
						lStreamSink,
						&lMediaTypeHandler);

					LOG_CHECK_PTR_MEMORY(lMediaTypeHandler);

					do
					{
						CComPtrCustom<IMFMediaType> lMediaType;

						LOG_INVOKE_MF_METHOD(GetCurrentMediaType,
							lMediaTypeHandler,
							&lMediaType);

					} while (false);

					CComPtrCustom<IMFMediaType> lDownStreamMediaType;

					lDownStreamMediaType = aPtrUpStreamMediaType;

					if (lresult != S_OK)
					{
						do
						{
							UINT32 lSAROutputNode = 0;

							LOG_INVOKE_MF_METHOD(GetUINT32,
								aPtrDownStreamNode,
								CM_SAROutputNode,
								&lSAROutputNode);

							if (lSAROutputNode != FALSE)
							{
								do
								{
									CComPtrCustom<IMFMediaType> lTempDownStreamMediaType;

									// 32 bits

									{
										lDownStreamMediaType.Release();

										createUncompressedMediaType(16000, 2, 32, &lDownStreamMediaType);

										lresult = lMediaTypeHandler->IsMediaTypeSupported(lDownStreamMediaType, &lTempDownStreamMediaType);

										if (lresult == S_OK)
											break;
									}
									{
										lDownStreamMediaType.Release();

										createUncompressedMediaType(32000, 2, 32, &lDownStreamMediaType);

										lresult = lMediaTypeHandler->IsMediaTypeSupported(lDownStreamMediaType, &lTempDownStreamMediaType);

										if (lresult == S_OK)
											break;
									}
									{
										lDownStreamMediaType.Release();

										createUncompressedMediaType(44100, 2, 32, &lDownStreamMediaType);

										lresult = lMediaTypeHandler->IsMediaTypeSupported(lDownStreamMediaType, &lTempDownStreamMediaType);

										if (lresult == S_OK)
											break;
									}
									{
										lDownStreamMediaType.Release();

										createUncompressedMediaType(48000, 2, 32, &lDownStreamMediaType);

										lresult = lMediaTypeHandler->IsMediaTypeSupported(lDownStreamMediaType, &lTempDownStreamMediaType);

										if (lresult == S_OK)
											break;
									}
									{
										lDownStreamMediaType.Release();

										createUncompressedMediaType(96000, 2, 32, &lDownStreamMediaType);

										lresult = lMediaTypeHandler->IsMediaTypeSupported(lDownStreamMediaType, &lTempDownStreamMediaType);

										if (lresult == S_OK)
											break;
									}

									// 16 bits


									{
										lDownStreamMediaType.Release();

										createUncompressedMediaType(16000, 2, 16, &lDownStreamMediaType);

										lresult = lMediaTypeHandler->IsMediaTypeSupported(lDownStreamMediaType, &lTempDownStreamMediaType);

										if (lresult == S_OK)
											break;
									}
									{
										lDownStreamMediaType.Release();

										createUncompressedMediaType(32000, 2, 16, &lDownStreamMediaType);

										lresult = lMediaTypeHandler->IsMediaTypeSupported(lDownStreamMediaType, &lTempDownStreamMediaType);

										if (lresult == S_OK)
											break;
									}
									{
										lDownStreamMediaType.Release();

										createUncompressedMediaType(44100, 2, 16, &lDownStreamMediaType);

										lresult = lMediaTypeHandler->IsMediaTypeSupported(lDownStreamMediaType, &lTempDownStreamMediaType);

										if (lresult == S_OK)
											break;
									}
									{
										lDownStreamMediaType.Release();

										createUncompressedMediaType(48000, 2, 16, &lDownStreamMediaType);

										lresult = lMediaTypeHandler->IsMediaTypeSupported(lDownStreamMediaType, &lTempDownStreamMediaType);

										if (lresult == S_OK)
											break;
									}
									{
										lDownStreamMediaType.Release();

										createUncompressedMediaType(96000, 2, 16, &lDownStreamMediaType);

										lresult = lMediaTypeHandler->IsMediaTypeSupported(lDownStreamMediaType, &lTempDownStreamMediaType);

										if (lresult == S_OK)
											break;
									}

								} while (false);
							}

						} while (false);
					}

					LOG_CHECK_PTR_MEMORY(lDownStreamMediaType);
				
					LOG_INVOKE_MF_METHOD(SetCurrentMediaType,
						lMediaTypeHandler,
						lDownStreamMediaType);
					
					LOG_INVOKE_QUERY_INTERFACE_METHOD(lDownStreamMediaType, aPtrPtrDownStreamMediaType);

					LOG_CHECK_PTR_MEMORY((*aPtrPtrDownStreamMediaType));

				} while (false);

				return lresult;
			}

			HRESULT AudioTopologyResolver::resolveConnection(
				IMFTopology* aPtrTopology,
				IMFMediaType* aPtrUpStreamMediaType,
				IMFMediaType* aPtrDownStreamMediaType,
				IMFTopologyNode** aPtrPtrHeadTopologyNode,
				IMFTopologyNode** aPtrPtrTailTopologyNode)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrUpStreamMediaType);

					LOG_CHECK_PTR_MEMORY(aPtrDownStreamMediaType);

					LOG_CHECK_PTR_MEMORY(aPtrPtrHeadTopologyNode);

					LOG_CHECK_PTR_MEMORY(aPtrPtrTailTopologyNode);
					
					LOG_INVOKE_FUNCTION(checkUncompressionAudioSubTypes,
						aPtrUpStreamMediaType,
						aPtrDownStreamMediaType);
					
					if (lresult == S_OK)
					{
						LOG_INVOKE_FUNCTION(resolveUnCompressionAudioConnection,
							aPtrTopology,
							aPtrUpStreamMediaType,
							aPtrDownStreamMediaType,
							aPtrPtrHeadTopologyNode,
							aPtrPtrTailTopologyNode);
					}
					else if (lresult == S_FALSE) // One or bothe of stream types are not uncompressed
					{

						LOG_INVOKE_FUNCTION(resolveSubTypeAudioConnection,
							aPtrTopology,
							aPtrUpStreamMediaType,
							aPtrDownStreamMediaType,
							aPtrPtrHeadTopologyNode,
							aPtrPtrTailTopologyNode);
					}
					
				} while (false);
				
				return lresult;

			}
			
			HRESULT AudioTopologyResolver::resolveInputType(
				IMFTransform* aPtrTransform,
				IMFMediaType* aPtrUpStreamMediaType)
			{
				return E_NOTIMPL;
			}
			
			HRESULT AudioTopologyResolver::resolveSubTypeAudioConnection(
				IMFTopology* aPtrTopology,
				IMFMediaType* aPtrUpStreamMediaType,
				IMFMediaType* aPtrDownStreamMediaType,
				IMFTopologyNode** aPtrPtrHeadTopologyNode,
				IMFTopologyNode** aPtrPtrTailTopologyNode)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrUpStreamMediaType);

					LOG_CHECK_PTR_MEMORY(aPtrDownStreamMediaType);

					LOG_CHECK_PTR_MEMORY(aPtrPtrHeadTopologyNode);

					LOG_CHECK_PTR_MEMORY(aPtrPtrTailTopologyNode);


					GUID lMajorType;

					LOG_INVOKE_MF_METHOD(GetGUID,
						aPtrUpStreamMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);
					
					LOG_CHECK_STATE(lMajorType != MFMediaType_Audio);
					
					LOG_INVOKE_MF_METHOD(GetGUID, 
						aPtrDownStreamMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);

					LOG_CHECK_STATE(lMajorType != MFMediaType_Audio);
					
					BOOL lCompareResult = FALSE;
					
					LOG_INVOKE_MF_METHOD(Compare,
						aPtrUpStreamMediaType,
						aPtrDownStreamMediaType,
						MF_ATTRIBUTES_MATCH_INTERSECTION,
						&lCompareResult);
					
					if (lCompareResult == TRUE)
					{
						lresult = S_FALSE;

						break;
					}
					
					LOG_INVOKE_FUNCTION(resolveDecompressionAudioConnection,
						aPtrTopology,
						aPtrUpStreamMediaType,
						aPtrDownStreamMediaType,
						aPtrPtrHeadTopologyNode,
						aPtrPtrTailTopologyNode);
										
				} while (false);

				return lresult;
			}
			
			HRESULT AudioTopologyResolver::createAudioResamplerConnection(
				IMFTopology* aPtrTopology,
				IMFMediaType* aPtrUpStreamMediaType,
				IMFMediaType* aPtrDownStreamMediaType,
				IMFTopologyNode** aPtrPtrHeadTopologyNode,
				IMFTopologyNode** aPtrPtrTailTopologyNode)
			{
				using namespace Transform::Audio;

				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrUpStreamMediaType);

					LOG_CHECK_PTR_MEMORY(aPtrDownStreamMediaType);

					LOG_CHECK_PTR_MEMORY(aPtrPtrHeadTopologyNode);

					LOG_CHECK_PTR_MEMORY(aPtrPtrTailTopologyNode);

					GUID lMajorType;

					LOG_INVOKE_MF_METHOD(GetGUID, 
						aPtrUpStreamMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);
					
					LOG_CHECK_STATE(lMajorType != MFMediaType_Audio);
					
					LOG_INVOKE_MF_METHOD(GetGUID, 
						aPtrDownStreamMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);
					
					LOG_CHECK_STATE(lMajorType != MFMediaType_Audio);
					
					CComPtrCustom<IMFTransform> lAudioResamplerTransform(new (std::nothrow)CustomisedAudioResampler);
					
					LOG_CHECK_PTR_MEMORY(lAudioResamplerTransform);
					
					LOG_INVOKE_MF_METHOD(SetInputType, lAudioResamplerTransform,
						0, aPtrUpStreamMediaType, 0);
					
					LOG_INVOKE_MF_METHOD(SetOutputType, lAudioResamplerTransform,
						0, aPtrDownStreamMediaType, 0);

					LOG_INVOKE_MF_METHOD(ProcessMessage, lAudioResamplerTransform,
						MFT_MESSAGE_COMMAND_FLUSH, NULL);

					LOG_INVOKE_MF_METHOD(ProcessMessage, lAudioResamplerTransform,
						MFT_MESSAGE_NOTIFY_BEGIN_STREAMING, NULL);

					LOG_INVOKE_MF_METHOD(ProcessMessage, lAudioResamplerTransform,
						MFT_MESSAGE_NOTIFY_START_OF_STREAM, NULL);
					
					CComPtrCustom<IMFTopologyNode> lResizeTopologyNode;

					LOG_INVOKE_MF_FUNCTION(MFCreateTopologyNode,
						MF_TOPOLOGY_TRANSFORM_NODE,
						&lResizeTopologyNode);

					LOG_CHECK_PTR_MEMORY(lResizeTopologyNode);
					
					LOG_INVOKE_MF_METHOD(AddNode,
						aPtrTopology,
						lResizeTopologyNode);
										
					LOG_INVOKE_MF_METHOD(SetObject,
						lResizeTopologyNode,
						lAudioResamplerTransform);
					
					*aPtrPtrHeadTopologyNode = lResizeTopologyNode;

					(*aPtrPtrHeadTopologyNode)->AddRef();

					*aPtrPtrTailTopologyNode = lResizeTopologyNode.detach();


				} while (false);
				
				return lresult;
			}

			HRESULT AudioTopologyResolver::checkUncompressionAudioSubTypes(
				IMFMediaType* aPtrUpStreamMediaType,
				IMFMediaType* aPtrDownStreamMediaType)
			{

				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrUpStreamMediaType);

					LOG_CHECK_PTR_MEMORY(aPtrDownStreamMediaType);
					
					GUID lMajorType;

					LOG_INVOKE_MF_METHOD(GetGUID,
						aPtrUpStreamMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);
										
					LOG_CHECK_STATE(lMajorType != MFMediaType_Audio);

					LOG_INVOKE_MF_METHOD(GetGUID,
						aPtrDownStreamMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);
					
					LOG_CHECK_STATE(lMajorType != MFMediaType_Audio);

					GUID lSubType;

					LOG_INVOKE_MF_METHOD(GetGUID,
						aPtrDownStreamMediaType,
						MF_MT_SUBTYPE,
						&lSubType);
					
					if (lSubType != MFAudioFormat_PCM &&
						lSubType != MFAudioFormat_Float)
					{
						lresult = S_FALSE;

						break;
					}

					LOG_INVOKE_MF_METHOD(GetGUID,
						aPtrUpStreamMediaType,
						MF_MT_SUBTYPE,
						&lSubType);
					
					if (lSubType != MFAudioFormat_PCM &&
						lSubType != MFAudioFormat_Float)
					{
						lresult = S_FALSE;

						break;
					}

				} while (false);
				
				return lresult;
			}

			HRESULT AudioTopologyResolver::checkDemandForResolvingOfUncompressionAudioSubTypes(
				IMFMediaType* aPtrUpStreamMediaType,
				IMFMediaType* aPtrDownStreamMediaType)
			{

				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrUpStreamMediaType);

					LOG_CHECK_PTR_MEMORY(aPtrDownStreamMediaType);
					
					GUID lMajorType;

					LOG_INVOKE_MF_METHOD(GetGUID,
						aPtrUpStreamMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);
					
					LOG_CHECK_STATE(lMajorType != MFMediaType_Audio);
					
					LOG_INVOKE_MF_METHOD(GetGUID,
						aPtrDownStreamMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);

					LOG_CHECK_STATE(lMajorType != MFMediaType_Audio);
					
					GUID lDownStreamSubType;

					LOG_INVOKE_MF_METHOD(GetGUID,
						aPtrDownStreamMediaType,
						MF_MT_SUBTYPE,
						&lDownStreamSubType);
					
					GUID lUpStreamSubType;


					LOG_INVOKE_MF_METHOD(GetGUID,
						aPtrUpStreamMediaType,
						MF_MT_SUBTYPE,
						&lUpStreamSubType);
					
					UINT32 lDownStreamBitsPerSample;

					LOG_INVOKE_MF_METHOD(GetUINT32,
						aPtrDownStreamMediaType,
						MF_MT_AUDIO_BITS_PER_SAMPLE,
						&lDownStreamBitsPerSample);
					
					UINT32 lUpStreamBitsPerSample;

					LOG_INVOKE_MF_METHOD(GetUINT32,
						aPtrUpStreamMediaType,
						MF_MT_AUDIO_BITS_PER_SAMPLE,
						&lUpStreamBitsPerSample);
					
					if (lDownStreamSubType == lUpStreamSubType &&
						lDownStreamBitsPerSample == lUpStreamBitsPerSample)
						lresult = S_FALSE;
					else
						lresult = S_OK;

				} while (false);
				
				return lresult;
			}

			HRESULT AudioTopologyResolver::checkDemandForAudioResampler(
				IMFMediaType* aPtrUpStreamMediaType,
				IMFMediaType* aPtrDownStreamMediaType)
			{

				HRESULT lresult;

				HRESULT lcheckResult(E_FAIL);

				do
				{

					LOG_CHECK_PTR_MEMORY(aPtrUpStreamMediaType);

					LOG_CHECK_PTR_MEMORY(aPtrDownStreamMediaType);
										
					GUID lMajorType;

					LOG_INVOKE_MF_METHOD(GetGUID,aPtrUpStreamMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);
					
					LOG_CHECK_STATE(lMajorType != MFMediaType_Audio);
					
					LOG_INVOKE_MF_METHOD(GetGUID,aPtrDownStreamMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);
					
					LOG_CHECK_STATE(lMajorType != MFMediaType_Audio);
					
					BOOL lCompareResult = FALSE;

					PROPVARIANT lPtrVarItem;

					LOG_INVOKE_MF_METHOD(GetItem,
						aPtrDownStreamMediaType,
						MF_MT_AUDIO_SAMPLES_PER_SECOND,
						&lPtrVarItem);
					
					LOG_INVOKE_MF_METHOD(CompareItem,
						aPtrUpStreamMediaType,
						MF_MT_AUDIO_SAMPLES_PER_SECOND,
						lPtrVarItem,
						&lCompareResult);
					
					if (lCompareResult == TRUE)
					{
						lcheckResult = S_FALSE;
					}
					else
					{
						lcheckResult = S_OK;

						break;
					}

					PropVariantClear(&lPtrVarItem);

					lCompareResult = FALSE;

					LOG_INVOKE_MF_METHOD(GetItem,
						aPtrDownStreamMediaType,
						MF_MT_AUDIO_NUM_CHANNELS,
						&lPtrVarItem);


					LOG_INVOKE_MF_METHOD(CompareItem,aPtrUpStreamMediaType,
						MF_MT_AUDIO_NUM_CHANNELS,
						lPtrVarItem,
						&lCompareResult);
					
					if (lCompareResult == TRUE)
					{
						lcheckResult = S_FALSE;
					}
					else
					{
						lcheckResult = S_OK;

						break;
					}

					PropVariantClear(&lPtrVarItem);

				} while (false);

				return lcheckResult;
			}

			HRESULT AudioTopologyResolver::resolveUnCompressionAudioConnection(
				IMFTopology* aPtrTopology,
				IMFMediaType* aPtrUpStreamMediaType,
				IMFMediaType* aPtrDownStreamMediaType,
				IMFTopologyNode** aPtrPtrHeadTopologyNode,
				IMFTopologyNode** aPtrPtrTailTopologyNode)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrUpStreamMediaType);

					LOG_CHECK_PTR_MEMORY(aPtrDownStreamMediaType);

					LOG_CHECK_PTR_MEMORY(aPtrPtrHeadTopologyNode);

					LOG_CHECK_PTR_MEMORY(aPtrPtrTailTopologyNode);
					
					CComPtrCustom<IMFMediaType> lUpStreamMediaType;

					LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrUpStreamMediaType, &lUpStreamMediaType);

					LOG_CHECK_PTR_MEMORY(lUpStreamMediaType);
					
					GUID lMajorType;

					LOG_INVOKE_MF_METHOD(GetGUID,
						lUpStreamMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);
					
					LOG_CHECK_STATE(lMajorType != MFMediaType_Audio);
					
					LOG_INVOKE_MF_METHOD(GetGUID,
						aPtrDownStreamMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);

					LOG_CHECK_STATE(lMajorType != MFMediaType_Audio);

					LOG_INVOKE_FUNCTION(checkDemandForResolvingOfUncompressionAudioSubTypes,
						lUpStreamMediaType,
						aPtrDownStreamMediaType);

					auto lResampleresult = checkDemandForAudioResampler(
						lUpStreamMediaType,
						aPtrDownStreamMediaType);

					CComPtrCustom<IMFTopologyNode> lHeadTopologyNode;

					CComPtrCustom<IMFTopologyNode> lTailTopologyNode;

					if (lresult == S_OK ||
						lResampleresult == S_OK)
					{

						CComPtrCustom<IMFMediaType> lModifiedUpStreamMediaType;
						
						LOG_INVOKE_FUNCTION(createAudioResamplerConnection,
							aPtrTopology,
							lUpStreamMediaType,
							aPtrDownStreamMediaType,
							&lHeadTopologyNode,
							&lTailTopologyNode);
					}

					*aPtrPtrHeadTopologyNode = lHeadTopologyNode.detach();

					*aPtrPtrTailTopologyNode = lTailTopologyNode.detach();

				} while (false);

				return lresult;
			}
			
			HRESULT AudioTopologyResolver::resolveDecompressionAudioConnection(
				IMFTopology* aPtrTopology,
				IMFMediaType* aPtrUpStreamMediaType,
				IMFMediaType* aPtrDownStreamMediaType,
				IMFTopologyNode** aPtrPtrHeadTopologyNode,
				IMFTopologyNode** aPtrPtrTailTopologyNode)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrUpStreamMediaType);

					LOG_CHECK_PTR_MEMORY(aPtrDownStreamMediaType);

					LOG_CHECK_PTR_MEMORY(aPtrPtrHeadTopologyNode);

					LOG_CHECK_PTR_MEMORY(aPtrPtrTailTopologyNode);
					
					GUID lMajorType;

					LOG_INVOKE_MF_METHOD(GetGUID,
						aPtrUpStreamMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);
					
					LOG_CHECK_STATE(lMajorType != MFMediaType_Audio);
					
					LOG_INVOKE_MF_METHOD(GetGUID,
						aPtrDownStreamMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);
					
					LOG_CHECK_STATE(lMajorType != MFMediaType_Audio);
					
					BOOL lCompareResult = FALSE;

					PROPVARIANT lPtrVarItem;

					PropVariantInit(&lPtrVarItem);

					LOG_INVOKE_MF_METHOD(GetItem,
						aPtrDownStreamMediaType,
						MF_MT_SUBTYPE,
						&lPtrVarItem);

					do
					{
						LOG_INVOKE_MF_METHOD(CompareItem,
							aPtrUpStreamMediaType,
							MF_MT_SUBTYPE,
							lPtrVarItem,
							&lCompareResult);

					} while (false);

					PropVariantClear(&lPtrVarItem);

					LOG_CHECK_STATE_DESCR(FAILED(lresult), lresult);
					
					if (lCompareResult == TRUE)
					{
						lresult = S_FALSE;

						break;
					}

					GUID lUpStreamSubType;

					LOG_INVOKE_MF_METHOD(GetGUID,
						aPtrUpStreamMediaType,
						MF_MT_SUBTYPE,
						&lUpStreamSubType);
					
					GUID lDownStreamSubType;

					LOG_INVOKE_MF_METHOD(GetGUID,
						aPtrDownStreamMediaType,
						MF_MT_SUBTYPE,
						&lDownStreamSubType);
					
					MFT_REGISTER_TYPE_INFO lUpStreamMFtypeInfo;

					lUpStreamMFtypeInfo.guidMajorType = MFMediaType_Audio;

					lUpStreamMFtypeInfo.guidSubtype = lUpStreamSubType;


					MFT_REGISTER_TYPE_INFO lDownStreamMFtypeInfo;

					lDownStreamMFtypeInfo.guidMajorType = MFMediaType_Audio;

					lDownStreamMFtypeInfo.guidSubtype = lDownStreamSubType;


					CComMassivPtr<IMFActivate> lDecoderList;

					UINT32 lFlags = MFT_ENUM_FLAG_SYNCMFT;

					LOG_INVOKE_MF_FUNCTION(MFTEnumEx,
						MFT_CATEGORY_AUDIO_DECODER,
						lFlags,
						&lUpStreamMFtypeInfo,
						&lDownStreamMFtypeInfo,
						lDecoderList.getPtrMassivPtr(),
						lDecoderList.getPtrSizeMassiv());
					
					bool lcreateTempAudioResampler = false;

					if (lDecoderList.getSizeMassiv() == 0)
					{
						LOG_INVOKE_MF_FUNCTION(MFTEnumEx,
							MFT_CATEGORY_AUDIO_DECODER,
							lFlags,
							nullptr,
							&lDownStreamMFtypeInfo,
							lDecoderList.getPtrMassivPtr(),
							lDecoderList.getPtrSizeMassiv());
						
						LOG_CHECK_STATE_DESCR(lDecoderList.getSizeMassiv() == 0, MF_E_TOPO_CODEC_NOT_FOUND);
						
						lcreateTempAudioResampler = true;
					}

					auto lDecoderCount = lDecoderList.getSizeMassiv();

					CComPtrCustom<IMFTransform> lDecoderTransform;

					CComPtrCustom<IMFMediaType> lOutputAvailableType;

					CComPtrCustom<IMFTopologyNode> lDecoderTopologyNode;

					for (decltype(lDecoderCount) lEncoderTreansformIndex = 0;
						lEncoderTreansformIndex < lDecoderCount;
						lEncoderTreansformIndex++)
					{
						lDecoderTransform.Release();

						do
						{

							LOG_INVOKE_MF_METHOD(ActivateObject,
								lDecoderList[lEncoderTreansformIndex],
								IID_PPV_ARGS(&lDecoderTransform));

						} while (false);

						if (FAILED(lresult))
						{
							continue;
						}
												
						DWORD lInputIndex = 0;

						do
						{
							
							LOG_INVOKE_MF_METHOD(SetInputType,
								lDecoderTransform,
								0,
								aPtrUpStreamMediaType,
								0);
							
							DWORD lOutputTypeIndex = 0;

							do
							{
								lOutputAvailableType.Release();

								if (lcreateTempAudioResampler)
								{
									LOG_INVOKE_MF_METHOD(GetOutputAvailableType,
										lDecoderTransform,
										0,
										0,
										&lOutputAvailableType);
								}
								else
								{

									LOG_INVOKE_MF_METHOD(SetOutputType,
										lDecoderTransform,
										0,
										aPtrDownStreamMediaType,
										0);
									
									lOutputAvailableType = aPtrDownStreamMediaType;

									break;
								}

							} while (SUCCEEDED(lresult));

						} while (false);

					}

					if (FAILED(lresult))
					{
						break;
					}

					LOG_INVOKE_MF_METHOD(SetInputType,
						lDecoderTransform,
						0,
						aPtrUpStreamMediaType,
						0);
					
					LOG_INVOKE_MF_METHOD(SetOutputType,
						lDecoderTransform,
						0,
						lOutputAvailableType,
						0);
					
					LOG_INVOKE_MF_FUNCTION(MFCreateTopologyNode,
						MF_TOPOLOGY_TRANSFORM_NODE,
						&lDecoderTopologyNode);
					
					LOG_INVOKE_MF_METHOD(AddNode,
						aPtrTopology,
						lDecoderTopologyNode);
					
					LOG_INVOKE_MF_METHOD(SetObject,
						lDecoderTopologyNode,
						lDecoderTransform);
					
					LOG_INVOKE_QUERY_INTERFACE_METHOD(lDecoderTopologyNode, aPtrPtrHeadTopologyNode);
										
					if (lcreateTempAudioResampler)
					{
						LOG_INVOKE_FUNCTION(checkUncompressionAudioSubTypes,
							lOutputAvailableType,
							aPtrDownStreamMediaType);

						if (lresult == S_OK)
						{
							CComPtrCustom<IMFTopologyNode> ltempHeadToplogyNode;

							LOG_INVOKE_FUNCTION(resolveUnCompressionAudioConnection,
								aPtrTopology,
								lOutputAvailableType,
								aPtrDownStreamMediaType,
								&ltempHeadToplogyNode,
								aPtrPtrTailTopologyNode);
							
							LOG_INVOKE_MF_METHOD(ConnectOutput,
								lDecoderTopologyNode,
								0,
								ltempHeadToplogyNode,
								0);
						}
						else
						{
							lresult = S_OK;

							*aPtrPtrTailTopologyNode = lDecoderTopologyNode.detach();
						}

					}
					else
					{
						lresult = S_OK;

						*aPtrPtrTailTopologyNode = lDecoderTopologyNode.detach();
					}

				} while (false);

				return lresult;
			}

			HRESULT AudioTopologyResolver::createUncompressedMediaType(
				UINT32 aSamplePerSecond,
				UINT32 aNumChannels,
				UINT32 aBitsPerSample,
				IMFMediaType** aPtrPtrStubUncompressedMediaType)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrPtrStubUncompressedMediaType);

					UINT32 lbytePerSample = aBitsPerSample / 8;

					CComPtrCustom<IMFMediaType> lMediaType;

					LOG_INVOKE_MF_FUNCTION(MFCreateMediaType, &lMediaType);

					LOG_INVOKE_MF_METHOD(SetGUID,
						lMediaType,
						MF_MT_MAJOR_TYPE,
						MFMediaType_Audio);

					LOG_INVOKE_MF_METHOD(SetGUID,
						lMediaType,
						MF_MT_SUBTYPE,
						MFAudioFormat_Float);

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