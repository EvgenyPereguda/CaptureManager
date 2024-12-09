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


#include "VideoTopologyResolver.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Common.h"
#include "../Common/GUIDs.h"
#include "../Common/ComPtrCustom.h"
#include "../Common/ComMassivPtr.h"
#include "../Common/Singleton.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../Common/MFHeaders.h"
#include "../VideoSurfaceCopierManager/VideoSurfaceCopierManager.h"
#include "../VideoSurfaceCopierManager/DirectX11ConvertorTransform.h"
#include "../HorizontMirroring/HorizontMirroring.h"
#include "../CaptureManagerBroker/CaptureManagerConstants.h"
#include "../CustomisedColorConvert/CustomisedColorConvert.h"


namespace CaptureManager
{
	namespace MediaSession
	{
		namespace CustomisedMediaSession
		{
			using namespace Core;
			
			// BaseTopologyResolver implementation

			HRESULT VideoTopologyResolver::addHorizontMirroringConvertorConnection(
				IMFTopology* aPtrTopology,
				IMFMediaType* aPtrUpStreamMediaType,
				IMFTopologyNode* aPtrUpStreamNode,
				DWORD aOutputStreamIndex,
				IMFMediaTypeHandler* aPtrMediaTypeHandler)
			{
				using namespace Transform::Image;

				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrTopology);

					LOG_CHECK_PTR_MEMORY(aPtrUpStreamMediaType);

					LOG_CHECK_PTR_MEMORY(aPtrMediaTypeHandler);

					LOG_CHECK_PTR_MEMORY(aPtrUpStreamNode);
										
					CComPtrCustom<IMFMediaType> lInputMediaType;

					LOG_INVOKE_MF_METHOD(GetCurrentMediaType,
						aPtrMediaTypeHandler,
						&lInputMediaType);
					
					GUID lSubType;

					LOG_INVOKE_MF_METHOD(GetGUID,
						lInputMediaType,
						MF_MT_SUBTYPE,
						&lSubType);
					
					CComPtrCustom<IMFMediaType> lNewInputMediaType;

					LOG_INVOKE_FUNCTION(createMediaType,
						aPtrUpStreamMediaType,
						lSubType,
						&lNewInputMediaType);
					
					LOG_CHECK_PTR_MEMORY(lNewInputMediaType);

					LOG_INVOKE_MF_METHOD(SetCurrentMediaType,
						aPtrMediaTypeHandler,
						lNewInputMediaType);
					
					CComPtrCustom<HorizontMirroring> lHorizontMirroringTransform = new
						(std::nothrow) HorizontMirroring(lSubType);

					LOG_CHECK_PTR_MEMORY(lHorizontMirroringTransform);

					LOG_INVOKE_MF_METHOD(SetInputType,
						lHorizontMirroringTransform,
						0,
						lNewInputMediaType,
						0);

					CComPtrCustom<IMFTopologyNode> lHorizontMirroringTransformTopologyNode;

					LOG_INVOKE_MF_FUNCTION(MFCreateTopologyNode,
						MF_TOPOLOGY_TRANSFORM_NODE,
						&lHorizontMirroringTransformTopologyNode);

					LOG_CHECK_PTR_MEMORY(lHorizontMirroringTransformTopologyNode);

					LOG_INVOKE_MF_METHOD(SetObject,
						lHorizontMirroringTransformTopologyNode,
						lHorizontMirroringTransform);					

					CComPtrCustom<IMFTopologyNode> lMediaSinkTopologyNode;

					DWORD lInputIndexOnDownstreamNode;

					LOG_INVOKE_MF_METHOD(GetOutput,
						aPtrUpStreamNode,
						aOutputStreamIndex,
						&lMediaSinkTopologyNode,
						&lInputIndexOnDownstreamNode);

					LOG_INVOKE_MF_METHOD(DisconnectOutput,
						aPtrUpStreamNode,
						aOutputStreamIndex);

					LOG_INVOKE_MF_METHOD(ConnectOutput,
						aPtrUpStreamNode,
						aOutputStreamIndex,
						lHorizontMirroringTransformTopologyNode,
						0);

					LOG_INVOKE_MF_METHOD(ConnectOutput,
						lHorizontMirroringTransformTopologyNode,
						0,
						lMediaSinkTopologyNode,
						lInputIndexOnDownstreamNode);
					
					LOG_INVOKE_MF_METHOD(AddNode,
						aPtrTopology,
						lHorizontMirroringTransformTopologyNode);

				} while (false);

				return lresult;
			}
				
			HRESULT VideoTopologyResolver::getInputMediaTypeOfMediaSink(
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

					LOG_CHECK_PTR_MEMORY(aPtrTopology);

					LOG_CHECK_PTR_MEMORY(aPtrUpStreamNode);

					LOG_CHECK_PTR_MEMORY(aPtrDownStreamNode);

					LOG_CHECK_PTR_MEMORY(aPtrUpStreamMediaType);

					LOG_CHECK_PTR_MEMORY(aPtrPtrDownStreamMediaType);

					//bool lIsDeviceManager = false;

					//do
					//{
					//	CComPtrCustom<IMFDXGIDeviceManager> lDeviceManager;

					//	LOG_INVOKE_MF_METHOD(GetUnknown, aPtrUpStreamMediaType,
					//		CM_DeviceManager,
					//		IID_PPV_ARGS(&lDeviceManager));

					//	LOG_CHECK_PTR_MEMORY(lDeviceManager);

					//	lIsDeviceManager = true;

					//} while (false);
					
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

					CComPtrCustom<IMFStreamSink> lStreamSink;

					do
					{
						LOG_INVOKE_MF_METHOD(GetStreamSinkById,
							lMediaSink,
							lSinkStreamId,
							&lStreamSink);

					} while (false);

					if (FAILED(lresult))
					{
						LOG_INVOKE_MF_METHOD(AddStreamSink,
							lMediaSink,
							lSinkStreamId,
							nullptr,
							&lStreamSink);
					}
					
					LOG_CHECK_PTR_MEMORY(lStreamSink);

					CComPtrCustom<IMFMediaTypeHandler> lMediaTypeHandler;

					LOG_INVOKE_MF_METHOD(GetMediaTypeHandler,
						lStreamSink,
						&lMediaTypeHandler);
					
					LOG_CHECK_PTR_MEMORY(lMediaTypeHandler);

					UINT32 lValueUINT32 = FALSE;

					do
					{
						LOG_INVOKE_MF_METHOD(GetUINT32,
							aPtrDownStreamNode,
							MF_MT_SAMPLEGRABBER,
							&lValueUINT32);

					} while (false);

					if (SUCCEEDED(lresult))
					{
						if (lValueUINT32 == TRUE)
						{
							do
							{
								LOG_INVOKE_FUNCTION(addHorizontMirroringConvertorConnection,
									aPtrTopology,
									aPtrUpStreamMediaType,
									aPtrUpStreamNode,
									aOutputStreamIndex,
									lMediaTypeHandler);

							} while (false);
						}
					}
					
					CComPtrCustom<IMFGetService> lService;

					do
					{
						LOG_INVOKE_QUERY_INTERFACE_METHOD(lStreamSink, &lService);

					} while (false);

					if (SUCCEEDED(lresult))
					{
						if (lService)
						{
							CComPtrCustom<IMFVideoSampleAllocator> lIMFVideoSampleAllocator;

							lresult = lService->GetService(MR_VIDEO_ACCELERATION_SERVICE, IID_PPV_ARGS(&lIMFVideoSampleAllocator));
													
							if (SUCCEEDED(lresult) && lIMFVideoSampleAllocator != nullptr)
							{
								CComPtrCustom<IMFVideoSampleAllocatorEx> lIMFVideoSampleAllocatorEx;

								lresult = lIMFVideoSampleAllocator->QueryInterface(&lIMFVideoSampleAllocatorEx);

								if (SUCCEEDED(lresult) && lIMFVideoSampleAllocatorEx != nullptr)
								{
									lresult = resolveConnectionVideoSampleAllocatorExMediaSink(
										aPtrTopology,
										lMediaTypeHandler,
										lIMFVideoSampleAllocatorEx,
										aPtrUpStreamNode,
										aOutputStreamIndex,
										aPtrUpStreamMediaType);
								}
								else
								{
									lresult = resolveConnectionVideoSampleAllocatorMediaSink(
										aPtrTopology,
										lMediaTypeHandler,
										lIMFVideoSampleAllocator,
										aPtrUpStreamNode,
										aOutputStreamIndex,
										aPtrUpStreamMediaType);
								}
							}
							else
							{
								CComPtrCustom<IMFVideoSampleAllocatorEx> lIMFVideoSampleAllocatorEx;

								lresult = lService->GetService(MR_VIDEO_ACCELERATION_SERVICE, IID_PPV_ARGS(&lIMFVideoSampleAllocatorEx));

								if (SUCCEEDED(lresult) && lIMFVideoSampleAllocatorEx != nullptr)
								{
									lresult = resolveConnectionVideoSampleAllocatorExMediaSink(
										aPtrTopology,
										lMediaTypeHandler,
										lIMFVideoSampleAllocatorEx,
										aPtrUpStreamNode,
										aOutputStreamIndex,
										aPtrUpStreamMediaType);
								}

							}


							
						}
						else
						{
							lresult = E_POINTER;

							break;
						}
					}

					do
					{

						LOG_INVOKE_MF_METHOD(GetCurrentMediaType,
							lMediaTypeHandler,
							aPtrPtrDownStreamMediaType);

					} while (false);




					//using namespace pugi;

					//xml_document lxmlDoc;

					//auto ldeclNode = lxmlDoc.append_child(node_declaration);

					//ldeclNode.append_attribute(L"version") = L"1.0";

					//xml_node lcommentNode = lxmlDoc.append_child(node_comment);

					//lcommentNode.set_value(L"XML Document of sources");

					//auto lRootXMLElement = lxmlDoc.append_child(L"Sources");
					//DataParser::readMediaType(
					//	*aPtrPtrDownStreamMediaType,
					//	lRootXMLElement);

					//std::wstringstream lwstringstream;

					//lxmlDoc.print(lwstringstream);

					//std::wstring lXMLDocumentString;

					//lXMLDocumentString = lwstringstream.str();

					if (FAILED(lresult))
					{
						UINT32 lvalue = 0;

						do
						{
							LOG_INVOKE_MF_METHOD(GetUINT32,
								aPtrUpStreamMediaType,
								MF_MT_BITCOUNT,
								&lvalue);
						} while (false);

						if (FAILED(lresult))
						{
							LOG_INVOKE_MF_METHOD(SetUINT32,
								aPtrUpStreamMediaType,
								MF_MT_BITCOUNT,
								32);
						}

						LOG_INVOKE_MF_METHOD(SetCurrentMediaType,
							lMediaTypeHandler,
							aPtrUpStreamMediaType);

						LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrUpStreamMediaType, aPtrPtrDownStreamMediaType);

					}
					
					LOG_CHECK_PTR_MEMORY((*aPtrPtrDownStreamMediaType));
					
				} while (false);

				return lresult;
			}

			HRESULT VideoTopologyResolver::resolveConnection(
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
					
					lresult = resolveDirectX11ConvertorConnection(
						aPtrTopology,
						aPtrUpStreamMediaType,
						aPtrDownStreamMediaType,
						aPtrPtrHeadTopologyNode,
						aPtrPtrTailTopologyNode);
					
					if (FAILED(lresult) || lresult == S_FALSE)
					{

						lresult = resolveFrameConvertorConnection(
							aPtrTopology,
							aPtrUpStreamMediaType,
							aPtrDownStreamMediaType,
							aPtrPtrHeadTopologyNode,
							aPtrPtrTailTopologyNode);

						if (FAILED(lresult) || lresult == S_FALSE)
						{

							LOG_INVOKE_FUNCTION(resolveSubTypeVideoConnection,
								aPtrTopology,
								aPtrUpStreamMediaType,
								aPtrDownStreamMediaType,
								aPtrPtrHeadTopologyNode,
								aPtrPtrTailTopologyNode);
						}
					}

				} while (false);
				
				return lresult;

			}

			HRESULT VideoTopologyResolver::resolveInputType(
				IMFTransform* aPtrTransform,
				IMFMediaType* aPtrUpStreamMediaType)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrUpStreamMediaType);

					LOG_CHECK_PTR_MEMORY(aPtrTransform);

					GUID lMajorType;

					LOG_INVOKE_MF_METHOD(GetGUID, aPtrUpStreamMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);

					LOG_CHECK_STATE(lMajorType != MFMediaType_Video);														

					do
					{
						LOG_INVOKE_FUNCTION(resolveColorConvertionInputType,
							aPtrTransform,
							aPtrUpStreamMediaType);
												
					} while (false);

					if (SUCCEEDED(lresult))
						break;

					LOG_INVOKE_FUNCTION(resolveDecompressionVideoInputType,
						aPtrTransform,
						aPtrUpStreamMediaType);
					
				} while (false);

				return lresult;

			}

			HRESULT VideoTopologyResolver::resolveConnectionDirect3DDeviceManager9MediaSink(
				IMFTopology* aPtrTopology,
				IMFMediaTypeHandler* aPtrMediaTypeHandler,
				IDirect3DDeviceManager9*  aPtrIDirect3DDeviceManager9,
				IMFTopologyNode* aPtrUpStreamNode,
				DWORD aOutputStreamIndex,
				IMFMediaType* aPtrUpStreamMediaType)
			{
				using namespace Transform;
				using namespace Transform::Image;

				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrMediaTypeHandler);

					LOG_CHECK_PTR_MEMORY(aPtrIDirect3DDeviceManager9);

					LOG_CHECK_PTR_MEMORY(aPtrUpStreamNode);

					LOG_CHECK_PTR_MEMORY(aPtrUpStreamMediaType);
					
					CComPtrCustom<IMFMediaType> lMediaType;
					
					do
					{
						LOG_INVOKE_MF_METHOD(GetCurrentMediaType,
							aPtrMediaTypeHandler,
							&lMediaType);

					} while (false);

					if (FAILED(lresult))
					{
						LOG_INVOKE_FUNCTION(createMediaType,
							aPtrUpStreamMediaType,
							MFVideoFormat_RGB32,
							&lMediaType);
						
						LOG_CHECK_PTR_MEMORY(lMediaType);

						LOG_INVOKE_MF_METHOD(SetCurrentMediaType,
							aPtrMediaTypeHandler,
							lMediaType);
						
					}
					else
					{
						lMediaType = aPtrUpStreamMediaType;
					}


					LOG_CHECK_PTR_MEMORY(lMediaType);

					CComPtrCustom<IUnknown> lUnknown;

					LOG_INVOKE_FUNCTION(Singleton<VideoSurfaceCopierManager>::getInstance().createVideoSurfaceCopier,
						aPtrIDirect3DDeviceManager9,
						lMediaType,
						&lUnknown);

					LOG_CHECK_PTR_MEMORY(lUnknown);

					CComPtrCustom<IMFTransform> lDirect3DSurface9Transform;

					LOG_INVOKE_QUERY_INTERFACE_METHOD(lUnknown, &lDirect3DSurface9Transform);

					LOG_CHECK_PTR_MEMORY(lDirect3DSurface9Transform);
					
					CComPtrCustom<IMFTopologyNode> lDirect3DSurface9TransformTopologyNode;

					LOG_INVOKE_MF_FUNCTION(MFCreateTopologyNode,
						MF_TOPOLOGY_TRANSFORM_NODE,
						&lDirect3DSurface9TransformTopologyNode);
					
					LOG_CHECK_PTR_MEMORY(lDirect3DSurface9TransformTopologyNode);

					LOG_INVOKE_MF_METHOD(SetObject,
						lDirect3DSurface9TransformTopologyNode,
						lDirect3DSurface9Transform);
										
					GUID lSubType;

					LOG_INVOKE_MF_METHOD(GetGUID,
						lMediaType,
						MF_MT_SUBTYPE,
						&lSubType);
										
					CComPtrCustom<IMFTopologyNode> lMediaSinkTopologyNode;

					DWORD lInputIndexOnDownstreamNode;

					LOG_INVOKE_MF_METHOD(GetOutput,
						aPtrUpStreamNode,
						aOutputStreamIndex,
						&lMediaSinkTopologyNode,
						&lInputIndexOnDownstreamNode);
					
					LOG_INVOKE_MF_METHOD(DisconnectOutput,
						aPtrUpStreamNode,
						aOutputStreamIndex);
					
					LOG_INVOKE_MF_METHOD(ConnectOutput,
						aPtrUpStreamNode,
						aOutputStreamIndex,
						lDirect3DSurface9TransformTopologyNode,
						0);
					
					LOG_INVOKE_MF_METHOD(ConnectOutput,
						lDirect3DSurface9TransformTopologyNode,
						0,
						lMediaSinkTopologyNode,
						lInputIndexOnDownstreamNode);
					
					LOG_INVOKE_MF_METHOD(AddNode,
						aPtrTopology,
						lDirect3DSurface9TransformTopologyNode);
										
				} while (false);

				return lresult;
			}

			HRESULT VideoTopologyResolver::resolveConnectionVideoSampleAllocatorExMediaSink(
				IMFTopology* aPtrTopology,
				IMFMediaTypeHandler* aPtrMediaTypeHandler,
				IMFVideoSampleAllocatorEx*  aPtrIMFVideoSampleAllocatorEx,
				IMFTopologyNode* aPtrUpStreamNode,
				DWORD aOutputStreamIndex,
				IMFMediaType* aPtrUpStreamMediaType)
			{
				using namespace Transform;
				using namespace Transform::Image;

				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrMediaTypeHandler);

					LOG_CHECK_PTR_MEMORY(aPtrIMFVideoSampleAllocatorEx);

					LOG_CHECK_PTR_MEMORY(aPtrUpStreamNode);

					LOG_CHECK_PTR_MEMORY(aPtrUpStreamMediaType);

					CComPtrCustom<IMFMediaType> lMediaType;

					do
					{
						LOG_INVOKE_MF_METHOD(SetCurrentMediaType,
							aPtrMediaTypeHandler,
							aPtrUpStreamMediaType);

					} while (false);
					
					if (FAILED(lresult))
					{

						CComPtrCustom<IMFMediaType> lFirstOutputMediaType;

						lresult = findSuitableDecoderMediaType(
							aPtrUpStreamMediaType,
							aPtrIMFVideoSampleAllocatorEx,
							&lFirstOutputMediaType,
							aPtrMediaTypeHandler);

						if (FAILED(lresult))
						{
							do
							{
								LOG_INVOKE_FUNCTION(createMediaType,
									aPtrUpStreamMediaType,
									//MFVideoFormat_NV12,
									MFVideoFormat_RGB32,
									&lMediaType);

							} while (false);

							if (FAILED(lresult) && lFirstOutputMediaType)
							{
								LOG_INVOKE_FUNCTION(createMediaType,
									lFirstOutputMediaType,
									MFVideoFormat_NV12,
									//MFVideoFormat_RGB32,
									&lMediaType);
							}


							LOG_CHECK_PTR_MEMORY(lMediaType);

							lresult = findSuitableColorMediaType(
								lMediaType,
								aPtrIMFVideoSampleAllocatorEx,
								aPtrMediaTypeHandler);
						}
						else
						{
							lMediaType = lFirstOutputMediaType;
						}

					}
					else
					{
						lMediaType = aPtrUpStreamMediaType;
					}

					if (FAILED(lresult))
					{
						lMediaType.Release();

						LOG_INVOKE_FUNCTION(createMediaType,
							aPtrUpStreamMediaType,
							//MFVideoFormat_NV12,
							 MFVideoFormat_RGB32,
							&lMediaType);
					}

					LOG_CHECK_PTR_MEMORY(lMediaType);

					LOG_INVOKE_MF_METHOD(SetCurrentMediaType,
						aPtrMediaTypeHandler,
						lMediaType);

					CComPtrCustom<IMFAttributes> inputAttr;
					
					LOG_INVOKE_MF_FUNCTION(MFCreateAttributes,
						&inputAttr, 3);

					LOG_INVOKE_MF_METHOD(SetUINT32,
						inputAttr,
						MF_SA_BUFFERS_PER_SAMPLE, 1);
					LOG_INVOKE_MF_METHOD(SetUINT32, 
						inputAttr,
						MF_SA_D3D11_USAGE, 
						D3D11_USAGE_DEFAULT);
					LOG_INVOKE_MF_METHOD(SetUINT32, 
						inputAttr,
						MF_SA_D3D11_BINDFLAGS,
						D3D11_BIND_RENDER_TARGET
						);

					do
					{
						LOG_INVOKE_MF_METHOD(InitializeSampleAllocatorEx,
							aPtrIMFVideoSampleAllocatorEx,
							3,
							50,
							inputAttr.get(),
							lMediaType.get());

					} while (false);

					if (FAILED(lresult))
					{
						LOG_INVOKE_POINTER_METHOD(aPtrIMFVideoSampleAllocatorEx, 
							InitializeSampleAllocator,
							5, 
							lMediaType.get());
					}

					CComPtrCustom<IUnknown> lUnknown;

					LOG_INVOKE_FUNCTION(Singleton<VideoSurfaceCopierManager>::getInstance().createVideoSurfaceCopier,
						aPtrIMFVideoSampleAllocatorEx,
						lMediaType,
						&lUnknown);

					LOG_CHECK_PTR_MEMORY(lUnknown);

					CComPtrCustom<IMFTransform> lDirect3DSurface9Transform;

					LOG_INVOKE_QUERY_INTERFACE_METHOD(lUnknown, &lDirect3DSurface9Transform);

					LOG_CHECK_PTR_MEMORY(lDirect3DSurface9Transform);

					CComPtrCustom<IMFTopologyNode> lDirect3DSurface9TransformTopologyNode;

					LOG_INVOKE_MF_FUNCTION(MFCreateTopologyNode,
						MF_TOPOLOGY_TRANSFORM_NODE,
						&lDirect3DSurface9TransformTopologyNode);

					LOG_CHECK_PTR_MEMORY(lDirect3DSurface9TransformTopologyNode);

					LOG_INVOKE_MF_METHOD(SetObject,
						lDirect3DSurface9TransformTopologyNode,
						lDirect3DSurface9Transform);

					GUID lSubType;

					LOG_INVOKE_MF_METHOD(GetGUID,
						lMediaType,
						MF_MT_SUBTYPE,
						&lSubType);
					
					CComPtrCustom<IMFTopologyNode> lMediaSinkTopologyNode;

					DWORD lInputIndexOnDownstreamNode;

					LOG_INVOKE_MF_METHOD(GetOutput,
						aPtrUpStreamNode,
						aOutputStreamIndex,
						&lMediaSinkTopologyNode,
						&lInputIndexOnDownstreamNode);

					LOG_INVOKE_MF_METHOD(DisconnectOutput,
						aPtrUpStreamNode,
						aOutputStreamIndex);

					LOG_INVOKE_MF_METHOD(ConnectOutput,
						aPtrUpStreamNode,
						aOutputStreamIndex,
						lDirect3DSurface9TransformTopologyNode,
						0);

					LOG_INVOKE_MF_METHOD(ConnectOutput,
						lDirect3DSurface9TransformTopologyNode,
						0,
						lMediaSinkTopologyNode,
						lInputIndexOnDownstreamNode);

					LOG_INVOKE_MF_METHOD(AddNode,
						aPtrTopology,
						lDirect3DSurface9TransformTopologyNode);


				} while (false);

				return lresult;
			}

			HRESULT VideoTopologyResolver::resolveConnectionVideoSampleAllocatorMediaSink(
				IMFTopology* aPtrTopology,
				IMFMediaTypeHandler* aPtrMediaTypeHandler,
				IMFVideoSampleAllocator*  aPtrIMFVideoSampleAllocator,
				IMFTopologyNode* aPtrUpStreamNode,
				DWORD aOutputStreamIndex,
				IMFMediaType* aPtrUpStreamMediaType)
			{
				using namespace Transform;
				using namespace Transform::Image;

				HRESULT lresult;

				do
				{

					LOG_CHECK_PTR_MEMORY(aPtrMediaTypeHandler);

					LOG_CHECK_PTR_MEMORY(aPtrIMFVideoSampleAllocator);

					LOG_CHECK_PTR_MEMORY(aPtrUpStreamNode);

					LOG_CHECK_PTR_MEMORY(aPtrUpStreamMediaType);

					CComPtrCustom<IMFMediaType> lMediaType;

					//LogPrintOut::getInstance().printOutln(\
					//	LogPrintOut::ERROR_LEVEL, 
					//	__FUNCTIONW__, 
					//	L" lXMLDocumentString: ", 
					//	lXMLDocumentString.c_str());

					do
					{
						LOG_INVOKE_MF_METHOD(SetCurrentMediaType,
							aPtrMediaTypeHandler,
							aPtrUpStreamMediaType);						
					} while (false);

					if (FAILED(lresult))
					{

						CComPtrCustom<IMFMediaType> lFirstOutputMediaType;

						lresult = findSuitableDecoderMediaType(
							aPtrUpStreamMediaType,
							aPtrIMFVideoSampleAllocator,
							&lFirstOutputMediaType,
							aPtrMediaTypeHandler);

						if (FAILED(lresult))
						{
							do
							{
								LOG_INVOKE_FUNCTION(createMediaType,
									aPtrUpStreamMediaType,
									//MFVideoFormat_NV12,
									MFVideoFormat_RGB32,
									&lMediaType);

							} while (false);

							if (FAILED(lresult) && lFirstOutputMediaType)
							{
								LOG_INVOKE_FUNCTION(createMediaType,
									lFirstOutputMediaType,
									//MFVideoFormat_NV12,
									MFVideoFormat_RGB32,
									&lMediaType);
							}


							LOG_CHECK_PTR_MEMORY(lMediaType);

							lresult = findSuitableColorMediaType(
								lMediaType,
								aPtrIMFVideoSampleAllocator,
								aPtrMediaTypeHandler);
						}			

					}

					LOG_CHECK_STATE_DESCR(FAILED(lresult), lresult);

					lMediaType.Release();

					aPtrMediaTypeHandler->GetCurrentMediaType(&lMediaType);
					
					do
					{
						LOG_INVOKE_POINTER_METHOD(aPtrIMFVideoSampleAllocator, InitializeSampleAllocator,
							5,
							lMediaType);

					} while (false);

					if (FAILED(lresult))
					{
						lMediaType.Release();

						LOG_INVOKE_FUNCTION(createMediaType,
							aPtrUpStreamMediaType,
							//MFVideoFormat_AYUV,
							//MFVideoFormat_NV12,
							MFVideoFormat_RGB32,
							&lMediaType);



						LOG_CHECK_PTR_MEMORY(lMediaType);

						LOG_INVOKE_MF_METHOD(SetCurrentMediaType,
							aPtrMediaTypeHandler,
							lMediaType);

						LOG_INVOKE_POINTER_METHOD(aPtrIMFVideoSampleAllocator, InitializeSampleAllocator,
							5,
							lMediaType);
					}

					CComPtrCustom<IUnknown> lUnknown;

					LOG_INVOKE_FUNCTION(Singleton<VideoSurfaceCopierManager>::getInstance().createVideoSurfaceCopier,
						aPtrIMFVideoSampleAllocator,
						lMediaType,
						&lUnknown);

					LOG_CHECK_PTR_MEMORY(lUnknown);

					CComPtrCustom<IMFTransform> lDirect3DSurface9Transform;

					LOG_INVOKE_QUERY_INTERFACE_METHOD(lUnknown, &lDirect3DSurface9Transform);

					LOG_CHECK_PTR_MEMORY(lDirect3DSurface9Transform);

					CComPtrCustom<IMFTopologyNode> lDirect3DSurface9TransformTopologyNode;

					LOG_INVOKE_MF_FUNCTION(MFCreateTopologyNode,
						MF_TOPOLOGY_TRANSFORM_NODE,
						&lDirect3DSurface9TransformTopologyNode);

					LOG_CHECK_PTR_MEMORY(lDirect3DSurface9TransformTopologyNode);

					LOG_INVOKE_MF_METHOD(SetObject,
						lDirect3DSurface9TransformTopologyNode,
						lDirect3DSurface9Transform);

					GUID lSubType;

					LOG_INVOKE_MF_METHOD(GetGUID,
						lMediaType,
						MF_MT_SUBTYPE,
						&lSubType);

					CComPtrCustom<IMFTopologyNode> lMediaSinkTopologyNode;

					DWORD lInputIndexOnDownstreamNode;

					LOG_INVOKE_MF_METHOD(GetOutput,
						aPtrUpStreamNode,
						aOutputStreamIndex,
						&lMediaSinkTopologyNode,
						&lInputIndexOnDownstreamNode);

					LOG_INVOKE_MF_METHOD(DisconnectOutput,
						aPtrUpStreamNode,
						aOutputStreamIndex);

					LOG_INVOKE_MF_METHOD(ConnectOutput,
						aPtrUpStreamNode,
						aOutputStreamIndex,
						lDirect3DSurface9TransformTopologyNode,
						0);

					LOG_INVOKE_MF_METHOD(ConnectOutput,
						lDirect3DSurface9TransformTopologyNode,
						0,
						lMediaSinkTopologyNode,
						lInputIndexOnDownstreamNode);

					LOG_INVOKE_MF_METHOD(AddNode,
						aPtrTopology,
						lDirect3DSurface9TransformTopologyNode);


				} while (false);

				return lresult;
			}
			
			HRESULT VideoTopologyResolver::createMediaType(
				IMFMediaType* aPtrUpStreamMediaType,
				GUID aMFVideoFormat,
				IMFMediaType** aPtrPtrMediaSinkMediaType)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrUpStreamMediaType);

					LOG_CHECK_PTR_MEMORY(aPtrPtrMediaSinkMediaType);

					CComPtrCustom<IMFMediaType> lnewMediaType;

					LOG_INVOKE_MF_FUNCTION(MFCreateMediaType,
						&lnewMediaType);
					
					LOG_CHECK_PTR_MEMORY(lnewMediaType);

					LOG_INVOKE_MF_METHOD(SetGUID,
						lnewMediaType,
						MF_MT_MAJOR_TYPE,
						MFMediaType_Video);
					
					LOG_INVOKE_MF_METHOD(SetGUID,
						lnewMediaType,
						MF_MT_SUBTYPE,
						aMFVideoFormat);
					
					LOG_INVOKE_MF_METHOD(SetUINT32,
						lnewMediaType,
						MF_MT_INTERLACE_MODE,
						MFVideoInterlace_Progressive);
					
					LOG_INVOKE_MF_METHOD(SetUINT32,
						lnewMediaType,
						MF_MT_ALL_SAMPLES_INDEPENDENT,
						TRUE);
					
					PROPVARIANT lVarItem;

					LOG_INVOKE_MF_METHOD(GetItem,
						aPtrUpStreamMediaType,
						MF_MT_FRAME_SIZE,
						&lVarItem);

					if (aMFVideoFormat == MFVideoFormat_NV12)
					{
						lVarItem.hVal.HighPart = (lVarItem.hVal.HighPart >> 1) << 1;

						lVarItem.hVal.LowPart = (lVarItem.hVal.LowPart >> 1) << 1;
					}
					
					LOG_INVOKE_MF_METHOD(SetItem,
						lnewMediaType,
						MF_MT_FRAME_SIZE,
						lVarItem);
					
					LOG_INVOKE_MF_METHOD(GetItem,
						aPtrUpStreamMediaType,
						MF_MT_FRAME_RATE,
						&lVarItem);
					
					LOG_INVOKE_MF_METHOD(SetItem,
						lnewMediaType,
						MF_MT_FRAME_RATE,
						lVarItem);
					
					LOG_INVOKE_MF_METHOD(GetItem,
						aPtrUpStreamMediaType,
						MF_MT_PIXEL_ASPECT_RATIO,
						&lVarItem);
					
					LOG_INVOKE_MF_METHOD(SetItem,
						lnewMediaType,
						MF_MT_PIXEL_ASPECT_RATIO,
						lVarItem);
					
					UINT32 lWidthInPixels = 0;

					UINT32 lHeightInPixels = 0;

					LOG_INVOKE_FUNCTION(MediaFoundation::MediaFoundationManager::GetAttributeSize,
						lnewMediaType,
						MF_MT_FRAME_SIZE,
						lWidthInPixels,
						lHeightInPixels);
					
					do
					{
						LONG lStride = 0;

						LOG_INVOKE_MF_FUNCTION(MFGetStrideForBitmapInfoHeader,
							aMFVideoFormat.Data1,
							lWidthInPixels,
							&lStride);
						
						LOG_INVOKE_MF_METHOD(SetUINT32,
							lnewMediaType,
							MF_MT_DEFAULT_STRIDE,
							(UINT32)lStride);

					} while (false);

					lresult = S_OK;
										
					*aPtrPtrMediaSinkMediaType = lnewMediaType.detach();

				} while (false);

				return lresult;
			}

			HRESULT VideoTopologyResolver::resolveSubTypeVideoConnection(
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

					LOG_INVOKE_MF_METHOD(GetGUID,aPtrUpStreamMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);
					
					LOG_CHECK_STATE(lMajorType != MFMediaType_Video);
					
					LOG_INVOKE_MF_METHOD(GetGUID,
						aPtrDownStreamMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);
					
					LOG_CHECK_STATE(lMajorType != MFMediaType_Video);

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

					do
					{
						LOG_INVOKE_FUNCTION(resolveColorConvertionVideoConnection,
							aPtrTopology,
							aPtrUpStreamMediaType,
							aPtrDownStreamMediaType,
							aPtrPtrHeadTopologyNode,
							aPtrPtrTailTopologyNode);

					} while (false);

					if (SUCCEEDED(lresult))
						break;
										
					LOG_INVOKE_FUNCTION(resolveDecompressionVideoConnection,
						aPtrTopology,
						aPtrUpStreamMediaType,
						aPtrDownStreamMediaType,
						aPtrPtrHeadTopologyNode,
						aPtrPtrTailTopologyNode);
					
				} while (false);

				return lresult;
			}

			HRESULT VideoTopologyResolver::resolveResizeVideoConnection(
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
					
					LOG_CHECK_STATE(lMajorType != MFMediaType_Video);
					
					LOG_INVOKE_MF_METHOD(GetGUID,aPtrDownStreamMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);
					
					LOG_CHECK_STATE(lMajorType != MFMediaType_Video);

					BOOL lCompareResult = FALSE;

					PROPVARIANT lPtrVarItem;

					LOG_INVOKE_MF_METHOD(GetItem,
						aPtrDownStreamMediaType,
						MF_MT_FRAME_SIZE,
						&lPtrVarItem);									

					LOG_INVOKE_MF_METHOD(CompareItem,
						aPtrUpStreamMediaType,
						MF_MT_FRAME_SIZE,
						lPtrVarItem,
						&lCompareResult);
					
					if (lCompareResult == TRUE)
					{
						lresult = S_FALSE;

						break;
					}

					CComPtrCustom<IMFTransform> lResizeTransform;

					LOG_INVOKE_OBJECT_METHOD(lResizeTransform, CoCreateInstance, __uuidof(CResizerDMO));
					
					LOG_CHECK_PTR_MEMORY(lResizeTransform);

					CComPtrCustom<IMFTopologyNode> lNewDownStreamTopologyNode;

					CComPtrCustom<IMFTopologyNode> lNewUpStreamTopologyNode;

					CComPtrCustom<IMFMediaType> lNewOutputType;

					CComPtrCustom<IMFMediaType> lNewInputType;

					do
					{						
						LOG_INVOKE_MF_METHOD(SetOutputType, lResizeTransform, 0, aPtrDownStreamMediaType, 0);

					} while (false);

					if (FAILED(lresult))
					{
						do
						{
							LOG_INVOKE_MF_METHOD(SetInputType, lResizeTransform, 0, aPtrUpStreamMediaType, 0);

						} while (false);

						if (FAILED(lresult))
						{

							LOG_INVOKE_MF_FUNCTION(MFCreateMediaType,&lNewOutputType);
							
							LOG_INVOKE_MF_FUNCTION(MFCreateMediaType,&lNewInputType);
							
							LOG_INVOKE_MF_METHOD(CopyAllItems,
								aPtrDownStreamMediaType,
								lNewOutputType);
							
							LOG_INVOKE_MF_METHOD(CopyAllItems,
								aPtrUpStreamMediaType,
								lNewInputType);
							
							LOG_INVOKE_MF_METHOD(SetGUID,lNewOutputType,
								MF_MT_SUBTYPE,
								MFVideoFormat_IYUV);
							
							LOG_INVOKE_MF_METHOD(SetGUID,lNewInputType,
								MF_MT_SUBTYPE,
								MFVideoFormat_IYUV);
							
							LOG_INVOKE_MF_METHOD(SetOutputType, lResizeTransform, 0, lNewOutputType, 0);

							LOG_INVOKE_MF_METHOD(SetInputType, lResizeTransform, 0, lNewInputType, 0);
							
							LOG_INVOKE_FUNCTION(resolveSubTypeVideoConnection,
								aPtrTopology,
								lNewOutputType,
								aPtrDownStreamMediaType,
								&lNewDownStreamTopologyNode,
								aPtrPtrTailTopologyNode);
							
							LOG_INVOKE_FUNCTION(resolveSubTypeVideoConnection,
								aPtrTopology,
								aPtrUpStreamMediaType,
								lNewInputType,
								aPtrPtrHeadTopologyNode,
								&lNewUpStreamTopologyNode);
							
						}
						else
						{

							GUID lSubType;

							LOG_INVOKE_MF_METHOD(GetGUID,aPtrUpStreamMediaType,
								MF_MT_SUBTYPE,
								&lSubType);
							
							LOG_INVOKE_MF_FUNCTION(MFCreateMediaType,&lNewOutputType);
							
							LOG_INVOKE_MF_METHOD(CopyAllItems,
								aPtrDownStreamMediaType,
								lNewOutputType);
							
							LOG_INVOKE_MF_METHOD(SetGUID,lNewOutputType,
								MF_MT_SUBTYPE,
								lSubType);

							LOG_INVOKE_MF_METHOD(SetOutputType, lResizeTransform, 0, lNewOutputType, 0);

							LOG_INVOKE_MF_METHOD(SetInputType, lResizeTransform, 0, aPtrUpStreamMediaType, 0);
							
							LOG_INVOKE_FUNCTION(resolveSubTypeVideoConnection,
								aPtrTopology,
								lNewOutputType,
								aPtrDownStreamMediaType,
								&lNewDownStreamTopologyNode,
								aPtrPtrTailTopologyNode);
						}

					}
					else
					{
						do
						{
							LOG_INVOKE_MF_METHOD(SetInputType, lResizeTransform, 0, aPtrUpStreamMediaType, 0);
						} while (false);
						
						if (FAILED(lresult))
						{

							GUID lSubType;

							LOG_INVOKE_MF_METHOD(GetGUID,aPtrDownStreamMediaType,
								MF_MT_SUBTYPE,
								&lSubType);
							
							LOG_INVOKE_MF_FUNCTION(MFCreateMediaType,&lNewInputType);
							
							LOG_INVOKE_MF_METHOD(CopyAllItems,
								aPtrUpStreamMediaType,
								lNewInputType);
							
							LOG_INVOKE_MF_METHOD(SetGUID,lNewInputType,
								MF_MT_SUBTYPE,
								lSubType);

							LOG_INVOKE_MF_METHOD(SetInputType, lResizeTransform, 0, lNewInputType, 0);

							LOG_INVOKE_MF_METHOD(SetOutputType, lResizeTransform, 0, aPtrDownStreamMediaType, 0);
							
							LOG_INVOKE_FUNCTION(resolveSubTypeVideoConnection,
								aPtrTopology,
								aPtrUpStreamMediaType,
								lNewInputType,
								aPtrPtrHeadTopologyNode,
								&lNewUpStreamTopologyNode);
						}

					}

					CComPtrCustom<IMFTopologyNode> lResizeTopologyNode;

					LOG_INVOKE_MF_FUNCTION(MFCreateTopologyNode,
						MF_TOPOLOGY_TRANSFORM_NODE,
						&lResizeTopologyNode);
					
					LOG_INVOKE_MF_METHOD(AddNode,
						aPtrTopology,
						lResizeTopologyNode);
					
					LOG_INVOKE_MF_METHOD(SetObject,
						lResizeTopologyNode,
						lResizeTransform);

					if (lNewDownStreamTopologyNode && lNewUpStreamTopologyNode)
					{
						LOG_INVOKE_MF_METHOD(ConnectOutput,
							lNewUpStreamTopologyNode,
							0,
							lResizeTopologyNode,
							0);

						LOG_INVOKE_MF_METHOD(ConnectOutput,
							lResizeTopologyNode,
							0,
							lNewDownStreamTopologyNode,
							0);

						break;
					}
					else if (lNewDownStreamTopologyNode)
					{

						*aPtrPtrHeadTopologyNode = lResizeTopologyNode;

						(*aPtrPtrHeadTopologyNode)->AddRef();

						LOG_INVOKE_MF_METHOD(ConnectOutput,
							lResizeTopologyNode,
							0,
							lNewDownStreamTopologyNode,
							0);

						break;

					}
					else if (lNewUpStreamTopologyNode)
					{

						*aPtrPtrTailTopologyNode = lResizeTopologyNode;

						(*aPtrPtrTailTopologyNode)->AddRef();

						LOG_INVOKE_MF_METHOD(ConnectOutput,
							lNewUpStreamTopologyNode,
							0,
							lResizeTopologyNode,
							0);

						break;

					}

					*aPtrPtrHeadTopologyNode = lResizeTopologyNode;

					(*aPtrPtrHeadTopologyNode)->AddRef();

					*aPtrPtrTailTopologyNode = lResizeTopologyNode.detach();


				} while (false);

				return lresult;
			}

			HRESULT VideoTopologyResolver::resolveFrameRateVideoConnection(
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

					LOG_INVOKE_MF_METHOD(GetGUID,aPtrUpStreamMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);
					
					LOG_CHECK_STATE(lMajorType != MFMediaType_Video);

					LOG_INVOKE_MF_METHOD(GetGUID,aPtrDownStreamMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);

					LOG_CHECK_STATE(lMajorType != MFMediaType_Video);

					BOOL lCompareResult = FALSE;

					PROPVARIANT lPtrVarItem;

					LOG_INVOKE_MF_METHOD(GetItem,aPtrDownStreamMediaType,
						MF_MT_FRAME_RATE,
						&lPtrVarItem);

					LOG_INVOKE_MF_METHOD(CompareItem,aPtrUpStreamMediaType,
						MF_MT_FRAME_RATE,
						lPtrVarItem,
						&lCompareResult);

					if (lCompareResult == TRUE)
					{
						lresult = S_FALSE;

						break;
					}

					CComPtrCustom<IMFTransform> lRateConvertTransform;

					LOG_INVOKE_OBJECT_METHOD(lRateConvertTransform, CoCreateInstance, __uuidof(CFrameRateConvertDmo));
					
					LOG_CHECK_PTR_MEMORY(lRateConvertTransform);

					CComPtrCustom<IMFTopologyNode> lNewDownStreamTopologyNode;

					CComPtrCustom<IMFTopologyNode> lNewUpStreamTopologyNode;

					CComPtrCustom<IMFMediaType> lNewOutputType;

					CComPtrCustom<IMFMediaType> lNewInputType;

					do
					{

						LOG_INVOKE_MF_METHOD(SetOutputType, lRateConvertTransform, 0, aPtrDownStreamMediaType, 0);

					} while (false);

					if (FAILED(lresult))
					{
						do
						{

							LOG_INVOKE_MF_METHOD(SetInputType, lRateConvertTransform, 0, aPtrUpStreamMediaType, 0);

						} while (false);

						if (FAILED(lresult))
						{

							LOG_INVOKE_MF_FUNCTION(MFCreateMediaType,&lNewOutputType);

							LOG_INVOKE_MF_FUNCTION(MFCreateMediaType,&lNewInputType);
							
							LOG_INVOKE_MF_METHOD(CopyAllItems,
								aPtrDownStreamMediaType,
								lNewOutputType);
							
							LOG_INVOKE_MF_METHOD(CopyAllItems,
								aPtrUpStreamMediaType,
								lNewInputType);
							
							LOG_INVOKE_MF_METHOD(SetGUID,lNewOutputType,
								MF_MT_SUBTYPE,
								MFVideoFormat_IYUV);
							
							LOG_INVOKE_MF_METHOD(SetGUID,lNewInputType,
								MF_MT_SUBTYPE,
								MFVideoFormat_IYUV);

							LOG_INVOKE_MF_METHOD(SetInputType, lRateConvertTransform, 0, lNewInputType, 0);
							
							LOG_INVOKE_MF_METHOD(SetOutputType, lRateConvertTransform, 0, lNewOutputType, 0);
							
							LOG_INVOKE_FUNCTION(resolveSubTypeVideoConnection,
								aPtrTopology,
								lNewOutputType,
								aPtrDownStreamMediaType,
								&lNewDownStreamTopologyNode,
								aPtrPtrTailTopologyNode);
							
							LOG_INVOKE_FUNCTION(resolveSubTypeVideoConnection,
								aPtrTopology,
								aPtrUpStreamMediaType,
								lNewInputType,
								aPtrPtrHeadTopologyNode,
								&lNewUpStreamTopologyNode);							
						}
						else
						{

							GUID lSubType;

							LOG_INVOKE_MF_METHOD(GetGUID,aPtrUpStreamMediaType,
								MF_MT_SUBTYPE,
								&lSubType);
							
							LOG_INVOKE_MF_FUNCTION(MFCreateMediaType,&lNewOutputType);
							
							LOG_INVOKE_MF_METHOD(CopyAllItems,
								aPtrDownStreamMediaType,
								lNewOutputType);
							
							LOG_INVOKE_MF_METHOD(SetGUID,lNewOutputType,
								MF_MT_SUBTYPE,
								lSubType);
							
							LOG_INVOKE_MF_METHOD(SetOutputType, lRateConvertTransform, 0, lNewOutputType, 0);
							
							LOG_INVOKE_MF_METHOD(SetInputType, lRateConvertTransform, 0, aPtrUpStreamMediaType, 0);
							
							LOG_INVOKE_FUNCTION(resolveSubTypeVideoConnection,
								aPtrTopology,
								lNewOutputType,
								aPtrDownStreamMediaType,
								&lNewDownStreamTopologyNode,
								aPtrPtrTailTopologyNode);
						}

					}
					else
					{
						do
						{
							LOG_INVOKE_MF_METHOD(SetInputType, lRateConvertTransform, 0, aPtrUpStreamMediaType, 0);

						} while (false);

						if (FAILED(lresult))
						{

							GUID lSubType;

							LOG_INVOKE_MF_METHOD(GetGUID,aPtrDownStreamMediaType,
								MF_MT_SUBTYPE,
								&lSubType);

							LOG_INVOKE_MF_FUNCTION(MFCreateMediaType,&lNewInputType);
							
							LOG_INVOKE_MF_METHOD(CopyAllItems,
								aPtrUpStreamMediaType,
								lNewInputType);
							
							LOG_INVOKE_MF_METHOD(SetGUID,lNewInputType,
								MF_MT_SUBTYPE,
								lSubType);
							
							LOG_INVOKE_MF_METHOD(SetInputType, lRateConvertTransform, 0, lNewInputType, 0);
							
							LOG_INVOKE_MF_METHOD(SetOutputType, lRateConvertTransform, 0, aPtrDownStreamMediaType, 0);
							
							LOG_INVOKE_FUNCTION(resolveColorConvertionVideoConnection,
								aPtrTopology,
								aPtrUpStreamMediaType,
								lNewInputType,
								aPtrPtrHeadTopologyNode,
								&lNewUpStreamTopologyNode);
						}
					}

					CComPtrCustom<IMFTopologyNode> lRateConvertTopologyNode;

					LOG_INVOKE_MF_FUNCTION(MFCreateTopologyNode,
						MF_TOPOLOGY_TRANSFORM_NODE,
						&lRateConvertTopologyNode);
					
					LOG_INVOKE_MF_METHOD(AddNode,
						aPtrTopology,
						lRateConvertTopologyNode);
					
					LOG_INVOKE_MF_METHOD(SetObject,
						lRateConvertTopologyNode,
						lRateConvertTransform);
					
					if (lNewDownStreamTopologyNode && lNewUpStreamTopologyNode)
					{
						LOG_INVOKE_MF_METHOD(ConnectOutput,
							lNewUpStreamTopologyNode,
							0,
							lRateConvertTopologyNode,
							0);
						
						LOG_INVOKE_MF_METHOD(ConnectOutput,
							lRateConvertTopologyNode,
							0,
							lNewDownStreamTopologyNode,
							0);

						break;
					}
					else if (lNewDownStreamTopologyNode)
					{

						*aPtrPtrHeadTopologyNode = lRateConvertTopologyNode;

						(*aPtrPtrHeadTopologyNode)->AddRef();

						LOG_INVOKE_MF_METHOD(ConnectOutput,
							lRateConvertTopologyNode,
							0,
							lNewDownStreamTopologyNode,
							0);

						break;

					}
					else if (lNewUpStreamTopologyNode)
					{

						*aPtrPtrTailTopologyNode = lRateConvertTopologyNode;

						(*aPtrPtrTailTopologyNode)->AddRef();

						LOG_INVOKE_MF_METHOD(ConnectOutput,
							lNewUpStreamTopologyNode,
							0,
							lRateConvertTopologyNode,
							0);

						break;

					}

					*aPtrPtrHeadTopologyNode = lRateConvertTopologyNode;

					(*aPtrPtrHeadTopologyNode)->AddRef();

					*aPtrPtrTailTopologyNode = lRateConvertTopologyNode.detach();
					
				} while (false);

				return lresult;
			}

			HRESULT VideoTopologyResolver::resolveFrameRateAndResizeVideoConnection(
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

					LOG_INVOKE_MF_METHOD(GetGUID,aPtrUpStreamMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);
										
					LOG_CHECK_STATE(lMajorType != MFMediaType_Video);

					LOG_INVOKE_MF_METHOD(GetGUID,aPtrDownStreamMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);

					LOG_CHECK_STATE(lMajorType != MFMediaType_Video);
					
					BOOL lCompareResult = FALSE;

					PROPVARIANT lFrameRateVarItem;

					LOG_INVOKE_MF_METHOD(GetItem,aPtrDownStreamMediaType,
						MF_MT_FRAME_RATE,
						&lFrameRateVarItem);
					
					LOG_INVOKE_MF_METHOD(CompareItem,aPtrUpStreamMediaType,
						MF_MT_FRAME_RATE,
						lFrameRateVarItem,
						&lCompareResult);
					
					if (lCompareResult == TRUE)
					{
						lresult = S_FALSE;

						break;
					}

					PROPVARIANT lFrameSizeVarItem;

					LOG_INVOKE_MF_METHOD(GetItem,aPtrDownStreamMediaType,
						MF_MT_FRAME_SIZE,
						&lFrameSizeVarItem);
					
					LOG_INVOKE_MF_METHOD(CompareItem,aPtrUpStreamMediaType,
						MF_MT_FRAME_SIZE,
						lFrameSizeVarItem,
						&lCompareResult);
					
					if (lCompareResult == TRUE)
					{
						lresult = S_FALSE;

						break;
					}

					CComPtrCustom<IMFTopologyNode> lTempUpStreamTopologyNode;

					CComPtrCustom<IMFTopologyNode> lTempDownStreamTopologyNode;

					CComPtrCustom<IMFMediaType> lTempMediaType;


					LOG_INVOKE_MF_FUNCTION(MFCreateMediaType,&lTempMediaType);
					
					LOG_INVOKE_MF_METHOD(CopyAllItems,
						aPtrDownStreamMediaType,
						lTempMediaType);
					
					LOG_INVOKE_MF_METHOD(GetItem,aPtrUpStreamMediaType,
						MF_MT_FRAME_RATE,
						&lFrameRateVarItem);
					
					LOG_INVOKE_MF_METHOD(SetItem,lTempMediaType,
						MF_MT_FRAME_RATE,
						lFrameRateVarItem);
					
					LOG_INVOKE_MF_METHOD(SetGUID,lTempMediaType,
						MF_MT_SUBTYPE,
						MFVideoFormat_IYUV);
					
					LOG_INVOKE_FUNCTION(resolveResizeVideoConnection,
						aPtrTopology,
						aPtrUpStreamMediaType,
						lTempMediaType,
						aPtrPtrHeadTopologyNode,
						&lTempDownStreamTopologyNode);
					
					LOG_INVOKE_FUNCTION(resolveFrameRateVideoConnection,
						aPtrTopology,
						lTempMediaType,
						aPtrDownStreamMediaType,
						&lTempUpStreamTopologyNode,
						aPtrPtrTailTopologyNode);
					
					LOG_INVOKE_MF_METHOD(ConnectOutput,
						lTempDownStreamTopologyNode,
						0,
						lTempUpStreamTopologyNode,
						0);
					
				} while (false);

				return lresult;
			}

			HRESULT VideoTopologyResolver::resolveColorConvertionVideoConnection(
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

					LOG_INVOKE_MF_METHOD(GetGUID,aPtrUpStreamMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);
					
					LOG_CHECK_STATE(lMajorType != MFMediaType_Video);
					
					LOG_INVOKE_MF_METHOD(GetGUID,aPtrDownStreamMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);

					LOG_CHECK_STATE(lMajorType != MFMediaType_Video);

					BOOL lCompareResult = FALSE;

					PROPVARIANT lSubTypeVarItem;

					LOG_INVOKE_MF_METHOD(GetItem,aPtrDownStreamMediaType,
						MF_MT_SUBTYPE,
						&lSubTypeVarItem);

					LOG_INVOKE_MF_METHOD(CompareItem,aPtrUpStreamMediaType,
						MF_MT_SUBTYPE,
						lSubTypeVarItem,
						&lCompareResult);
					
					if (lCompareResult == TRUE)
					{
						lresult = S_FALSE;

						break;
					}


					GUID lUpSubType;

					LOG_INVOKE_MF_METHOD(GetGUID, aPtrUpStreamMediaType,
						MF_MT_SUBTYPE,
						&lUpSubType);

					CComPtrCustom<IMFMediaType> lTempInputType;

					bool lIsSubTypI420 = lUpSubType.Data1 == 0x30323449;

					lTempInputType = aPtrUpStreamMediaType;

					if (lUpSubType == MFVideoFormat_ARGB32)
					{
						lTempInputType.Release();

						LOG_INVOKE_MF_FUNCTION(MFCreateMediaType, &lTempInputType);

						LOG_CHECK_PTR_MEMORY(lTempInputType);

						aPtrUpStreamMediaType->CopyAllItems(lTempInputType);

						LOG_INVOKE_MF_METHOD(SetGUID, lTempInputType,
							MF_MT_SUBTYPE,
							MFVideoFormat_RGB32);
					}


					

					GUID lDownSubType;

					LOG_INVOKE_MF_METHOD(GetGUID, aPtrDownStreamMediaType,
						MF_MT_SUBTYPE,
						&lDownSubType);

					bool lIsSubTypAYUV = lDownSubType.Data1 == D3DFORMAT('VUYA');

					CComPtrCustom<IMFTopologyNode> lTemplColorConverterTopologyNode;

					if (lIsSubTypI420 && lIsSubTypAYUV)
					{
						CComPtrCustom<IMFTransform> lColorConverterTransform;

						LOG_INVOKE_OBJECT_METHOD(lColorConverterTransform, CoCreateInstance, __uuidof(CColorConvertDMO));

						LOG_CHECK_PTR_MEMORY(lColorConverterTransform);

						LOG_INVOKE_MF_METHOD(SetInputType, lColorConverterTransform, 0, lTempInputType, 0);
						
						DWORD lTypeIndex = 0;

						while (SUCCEEDED(lresult))
						{

							CComPtrCustom<IMFMediaType> lOutputType;

							lresult = lColorConverterTransform->GetOutputAvailableType(0, lTypeIndex++, &lOutputType);

							if (SUCCEEDED(lresult) && lOutputType)
							{
								GUID lTempSubType;

								LOG_INVOKE_MF_METHOD(GetGUID, lOutputType,
									MF_MT_SUBTYPE,
									&lTempSubType);
								
								if (lTempSubType.Data1 == D3DFMT_YUY2)
								{

									PROPVARIANT lPtrVarItem;

									LOG_INVOKE_MF_METHOD(GetItem, lTempInputType,
										MF_MT_FRAME_RATE,
										&lPtrVarItem);

									LOG_INVOKE_MF_METHOD(SetItem, lOutputType,
										MF_MT_FRAME_RATE,
										lPtrVarItem);

									lresult = lColorConverterTransform->SetOutputType(0,
										lOutputType, 0);

									lTempInputType = lOutputType;

									break;
								}
							}
						}

						LOG_INVOKE_MF_FUNCTION(MFCreateTopologyNode,
							MF_TOPOLOGY_TRANSFORM_NODE,
							&lTemplColorConverterTopologyNode);

						LOG_INVOKE_MF_METHOD(SetObject,
							lTemplColorConverterTopologyNode,
							lColorConverterTransform);

						LOG_INVOKE_MF_METHOD(AddNode,
							aPtrTopology,
							lTemplColorConverterTopologyNode);
					}
					
					CComPtrCustom<IMFTransform> lColorConverterTransform;

					LOG_INVOKE_OBJECT_METHOD(lColorConverterTransform, CoCreateInstance, __uuidof(CColorConvertDMO));
					
					LOG_CHECK_PTR_MEMORY(lColorConverterTransform);
					
					//using namespace pugi;

					//xml_document lxmlDoc;

					//auto ldeclNode = lxmlDoc.append_child(node_declaration);

					//ldeclNode.append_attribute(L"version") = L"1.0";

					//xml_node lcommentNode = lxmlDoc.append_child(node_comment);

					//lcommentNode.set_value(L"XML Document of sources");

					//auto lRootXMLElement = lxmlDoc.append_child(L"Sources");
					//DataParser::readMediaType(
					//	aPtrDownStreamMediaType,
					//	lRootXMLElement);

					//std::wstringstream lwstringstream;

					//lxmlDoc.print(lwstringstream);

					//std::wstring lXMLDocumentString;

					//lXMLDocumentString = lwstringstream.str();

					LOG_INVOKE_MF_METHOD(SetInputType, lColorConverterTransform, 0, lTempInputType, 0);
										
					PROPVARIANT lFrameSizeVarItem;

					LOG_INVOKE_MF_METHOD(GetItem, lTempInputType,
						MF_MT_FRAME_SIZE,
						&lFrameSizeVarItem);
					
					LOG_INVOKE_MF_METHOD(SetItem,aPtrDownStreamMediaType,
						MF_MT_FRAME_SIZE,
						lFrameSizeVarItem);
					do
					{
						LOG_INVOKE_MF_METHOD(SetOutputType, lColorConverterTransform, 0, aPtrDownStreamMediaType, 0);

					} while (false);

					//if (FAILED(lresult))
					//{
					//	lColorConverterTransform.Release();

					//	LOG_INVOKE_FUNCTION(CustomisedColorConvert::create,
					//		lTempInputType, 
					//		aPtrDownStreamMediaType, 
					//		&lColorConverterTransform);
					//	
					//	LOG_CHECK_PTR_MEMORY(lColorConverterTransform);
					//}

					LOG_CHECK_STATE_DESCR(FAILED(lresult), lresult);
					
					CComPtrCustom<IMFTopologyNode> llColorConverterTopologyNode;

					LOG_INVOKE_MF_FUNCTION(MFCreateTopologyNode,
						MF_TOPOLOGY_TRANSFORM_NODE,
						&llColorConverterTopologyNode);
					
					LOG_INVOKE_MF_METHOD(SetObject,
						llColorConverterTopologyNode,
						lColorConverterTransform);
					
					LOG_INVOKE_MF_METHOD(AddNode,
						aPtrTopology,
						llColorConverterTopologyNode);
					
					if (lTemplColorConverterTopologyNode)
					{
						lTemplColorConverterTopologyNode->ConnectOutput(0, llColorConverterTopologyNode, 0);

						*aPtrPtrHeadTopologyNode = lTemplColorConverterTopologyNode.detach();
					}
					else
					{
						*aPtrPtrHeadTopologyNode = llColorConverterTopologyNode;

						(*aPtrPtrHeadTopologyNode)->AddRef();
					}
					
					*aPtrPtrTailTopologyNode = llColorConverterTopologyNode.detach();

				} while (false);

				return lresult;
			}

			HRESULT VideoTopologyResolver::resolveColorConvertionInputType(
					IMFTransform* aPtrTransform,
					IMFMediaType* aPtrUpStreamMediaType)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrUpStreamMediaType);

					LOG_CHECK_PTR_MEMORY(aPtrTransform);

					GUID lMajorType;

					LOG_INVOKE_MF_METHOD(GetGUID, aPtrUpStreamMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);

					LOG_CHECK_STATE(lMajorType != MFMediaType_Video);

					do
					{
						CComPtrCustom<IMFTransform> lColorConverterTransform;

						LOG_INVOKE_OBJECT_METHOD(lColorConverterTransform, CoCreateInstance, __uuidof(CColorConvertDMO));

						LOG_CHECK_PTR_MEMORY(lColorConverterTransform);

						LOG_INVOKE_MF_METHOD(SetInputType, lColorConverterTransform, 0, aPtrUpStreamMediaType, 0);


						DWORD lTypeIndex = 0;

						while (SUCCEEDED(lresult))
						{

							CComPtrCustom<IMFMediaType> lOutputType;

							lresult = lColorConverterTransform->GetOutputAvailableType(0, lTypeIndex++, &lOutputType);

							if (SUCCEEDED(lresult))
							{

								PROPVARIANT lPtrVarItem;

								LOG_INVOKE_MF_METHOD(GetItem, aPtrUpStreamMediaType,
									MF_MT_FRAME_RATE,
									&lPtrVarItem);

								LOG_INVOKE_MF_METHOD(SetItem, lOutputType,
									MF_MT_FRAME_RATE,
									lPtrVarItem);

								lresult = aPtrTransform->SetInputType(
									0,
									lOutputType,
									MFT_SET_TYPE_TEST_ONLY);

								if (SUCCEEDED(lresult))
								{
									lresult = aPtrTransform->SetInputType(
										0,
										lOutputType,
										0);

									break;
								}

								lresult = S_OK;
							}
						}

					} while (false);
					
				} while (false);

				return lresult;

			}

			HRESULT VideoTopologyResolver::resolveDecompressionVideoInputType(
				IMFTransform* aPtrTransform,
				IMFMediaType* aPtrUpStreamMediaType)
			{
				HRESULT lresult;

				do
				{

					LOG_CHECK_PTR_MEMORY(aPtrTransform);

					LOG_CHECK_PTR_MEMORY(aPtrUpStreamMediaType);
					
					GUID lMajorType;

					LOG_INVOKE_MF_METHOD(GetGUID, aPtrUpStreamMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);

					LOG_CHECK_STATE(lMajorType != MFMediaType_Video);
					
					GUID lUpStreamSubType;

					LOG_INVOKE_MF_METHOD(GetGUID,
						aPtrUpStreamMediaType,
						MF_MT_SUBTYPE,
						&lUpStreamSubType);

					DWORD lTypeIndex = 0;

					CComMassivPtr<IMFActivate> lDecoderList;

					UINT32 lFlags = MFT_ENUM_FLAG_SYNCMFT;// MFT_ENUM_FLAG_ALL;
					

					MFT_REGISTER_TYPE_INFO lUpStreamMFtypeInfo;

					lUpStreamMFtypeInfo.guidMajorType = MFMediaType_Video;

					lUpStreamMFtypeInfo.guidSubtype = lUpStreamSubType;

					bool lcreateTempColorConvertor = false;


					while (SUCCEEDED(lresult))
					{

						CComPtrCustom<IMFMediaType> lInputType;

						LOG_INVOKE_MF_METHOD(GetInputAvailableType,
							aPtrTransform,
							0,
							lTypeIndex++,
							&lInputType);

						LOG_CHECK_PTR_MEMORY(lInputType);

						GUID lDownStreamSubType;

						LOG_INVOKE_MF_METHOD(GetGUID,
							lInputType,
							MF_MT_SUBTYPE,
							&lDownStreamSubType);



						MFT_REGISTER_TYPE_INFO lDownStreamMFtypeInfo;

						lDownStreamMFtypeInfo.guidMajorType = MFMediaType_Video;

						lDownStreamMFtypeInfo.guidSubtype = lDownStreamSubType;


						
						LOG_INVOKE_MF_FUNCTION(MFTEnumEx,
							MFT_CATEGORY_VIDEO_DECODER,
							lFlags,
							&lUpStreamMFtypeInfo,
							&lDownStreamMFtypeInfo,
							lDecoderList.getPtrMassivPtr(),
							lDecoderList.getPtrSizeMassiv());
						
						if (lDecoderList.getSizeMassiv() > 0)
							break;
					}
					
					auto lDecoderCount = lDecoderList.getSizeMassiv();
					
					if (lDecoderList.getSizeMassiv() == 0)
					{
						LOG_INVOKE_MF_FUNCTION(MFTEnumEx,
							MFT_CATEGORY_VIDEO_DECODER,
							lFlags,
							&lUpStreamMFtypeInfo,
							nullptr,
							lDecoderList.getPtrMassivPtr(),
							lDecoderList.getPtrSizeMassiv());

						LOG_CHECK_STATE_DESCR(lDecoderList.getSizeMassiv() == 0, MF_E_TOPO_CODEC_NOT_FOUND);

						lDecoderCount = lDecoderList.getSizeMassiv();

						lcreateTempColorConvertor = true;
					}


					for (decltype(lDecoderCount) lDecoderTreansformIndex = 0; lDecoderTreansformIndex < lDecoderCount; lDecoderTreansformIndex++)
					{
						CComPtrCustom<IMFTransform> lDecoderTransform;

						LOG_INVOKE_MF_METHOD(ActivateObject,
							lDecoderList[lDecoderTreansformIndex],
							IID_PPV_ARGS(&lDecoderTransform));

						if (FAILED(lresult))
						{
							continue;
						}

						LOG_INVOKE_MF_METHOD(SetInputType,
							lDecoderTransform,
							0,
							aPtrUpStreamMediaType,
							0);

						DWORD lOutputIndex = 0;

						do
						{

							CComPtrCustom<IMFMediaType> lOutputMediaType;

							LOG_INVOKE_MF_METHOD(GetOutputAvailableType,
								lDecoderTransform,
								0,
								lOutputIndex++,
								&lOutputMediaType);

							if (lcreateTempColorConvertor)
							{


								PROPVARIANT lPtrVarItem;

								LOG_INVOKE_MF_METHOD(GetItem, aPtrUpStreamMediaType,
									MF_MT_FRAME_RATE,
									&lPtrVarItem);

								LOG_INVOKE_MF_METHOD(SetItem, lOutputMediaType,
									MF_MT_FRAME_RATE,
									lPtrVarItem);

								lresult = resolveColorConvertionInputType(
									aPtrTransform,
									lOutputMediaType);
							}
							else
							{


								PROPVARIANT lPtrVarItem;

								LOG_INVOKE_MF_METHOD(GetItem, aPtrUpStreamMediaType,
									MF_MT_FRAME_RATE,
									&lPtrVarItem);

								LOG_INVOKE_MF_METHOD(SetItem, lOutputMediaType,
									MF_MT_FRAME_RATE,
									lPtrVarItem);

								lresult = aPtrTransform->SetInputType(
									0,
									lOutputMediaType,
									MFT_SET_TYPE_TEST_ONLY);

								if (SUCCEEDED(lresult))
								{

									lresult = aPtrTransform->SetInputType(
										0,
										lOutputMediaType,
										0);
								}

							}

							if (FAILED(lresult))
								lresult = S_OK;
							else
								break;

							//using namespace pugi;

							//xml_document lxmlDoc;

							//auto ldeclNode = lxmlDoc.append_child(node_declaration);

							//ldeclNode.append_attribute(L"version") = L"1.0";

							//xml_node lcommentNode = lxmlDoc.append_child(node_comment);

							//lcommentNode.set_value(L"XML Document of sources");

							//auto lRootXMLElement = lxmlDoc.append_child(L"Sources");
							//DataParser::readMediaType(
							//	lOutputMediaType,
							//	lRootXMLElement);

							//std::wstringstream lwstringstream;

							//lxmlDoc.print(lwstringstream);

							//std::wstring lXMLDocumentString;

							//lXMLDocumentString = lwstringstream.str();

						} while (SUCCEEDED(lresult));
							
						if (SUCCEEDED(lresult))
							break;
					}

					
				} while (false);

				return lresult;
			}

			HRESULT VideoTopologyResolver::resolveDecompressionVideoConnection(
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

					LOG_INVOKE_MF_METHOD(GetGUID,aPtrUpStreamMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);
					
					LOG_CHECK_STATE(lMajorType != MFMediaType_Video);
					
					LOG_INVOKE_MF_METHOD(GetGUID,aPtrDownStreamMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);

					LOG_CHECK_STATE(lMajorType != MFMediaType_Video);

					BOOL lCompareResult = FALSE;

					PROPVARIANT lPtrVarItem;

					LOG_INVOKE_MF_METHOD(GetItem,aPtrDownStreamMediaType,
						MF_MT_SUBTYPE,
						&lPtrVarItem);

					LOG_INVOKE_MF_METHOD(CompareItem,aPtrUpStreamMediaType,
						MF_MT_SUBTYPE,
						lPtrVarItem,
						&lCompareResult);

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

					lUpStreamMFtypeInfo.guidMajorType = MFMediaType_Video;

					lUpStreamMFtypeInfo.guidSubtype = lUpStreamSubType;


					MFT_REGISTER_TYPE_INFO lDownStreamMFtypeInfo;

					lDownStreamMFtypeInfo.guidMajorType = MFMediaType_Video;

					lDownStreamMFtypeInfo.guidSubtype = lDownStreamSubType;


					CComMassivPtr<IMFActivate> lDecoderList;

					UINT32 lFlags = MFT_ENUM_FLAG_SYNCMFT;// MFT_ENUM_FLAG_ALL;

					LOG_INVOKE_MF_FUNCTION(MFTEnumEx,
						MFT_CATEGORY_VIDEO_DECODER,
						lFlags,
						&lUpStreamMFtypeInfo,
						&lDownStreamMFtypeInfo,
						lDecoderList.getPtrMassivPtr(),
						lDecoderList.getPtrSizeMassiv());
					
					bool lcreateTempColorConvertor = false;

					if (lDecoderList.getSizeMassiv() == 0)
					{
						LOG_INVOKE_MF_FUNCTION(MFTEnumEx,
							MFT_CATEGORY_VIDEO_DECODER,
							lFlags,
							&lUpStreamMFtypeInfo,
							nullptr,
							lDecoderList.getPtrMassivPtr(),
							lDecoderList.getPtrSizeMassiv());
						
						LOG_CHECK_STATE_DESCR(lDecoderList.getSizeMassiv() == 0, MF_E_TOPO_CODEC_NOT_FOUND);

						lcreateTempColorConvertor = true;
					}

					auto lDecoderCount = lDecoderList.getSizeMassiv();

					CComPtrCustom<IMFTransform> lDecoderTransform;

					CComPtrCustom<IMFMediaType> lOutputMediaType;

					CComPtrCustom<IMFTopologyNode> lColorConvertorTopologyNode;

					for (decltype(lDecoderCount) lDecoderTreansformIndex = 0; lDecoderTreansformIndex < lDecoderCount; lDecoderTreansformIndex++)
					{
						lDecoderTransform.Release();

						LOG_INVOKE_MF_METHOD(ActivateObject,
							lDecoderList[lDecoderTreansformIndex],
							IID_PPV_ARGS(&lDecoderTransform));

						if (FAILED(lresult))
						{
							continue;
						}




						CComPtrCustom<IMFMediaType> l_UpStreamMediaType;

						LOG_INVOKE_MF_FUNCTION(MFCreateMediaType,
							&l_UpStreamMediaType);

						LOG_CHECK_PTR_MEMORY(l_UpStreamMediaType);

						LOG_INVOKE_MF_METHOD(CopyAllItems,
							aPtrUpStreamMediaType,
							l_UpStreamMediaType);

						LOG_INVOKE_MF_METHOD(SetInputType,
							lDecoderTransform,
							0,
							l_UpStreamMediaType,
							0);

						//using namespace pugi;

						//xml_document lxmlDoc;

						//auto ldeclNode = lxmlDoc.append_child(node_declaration);

						//ldeclNode.append_attribute(L"version") = L"1.0";

						//xml_node lcommentNode = lxmlDoc.append_child(node_comment);

						//lcommentNode.set_value(L"XML Document of sources");

						//auto lRootXMLElement = lxmlDoc.append_child(L"Sources");
						//DataParser::readMediaType(
						//	l_UpStreamMediaType,
						//	lRootXMLElement);

						//std::wstringstream lwstringstream;

						//lxmlDoc.print(lwstringstream);

						//std::wstring lXMLDocumentString;

						//lXMLDocumentString = lwstringstream.str();
						
						DWORD lOutputIndex = 0;

						do
						{

							lOutputMediaType.Release();

							LOG_INVOKE_MF_METHOD(GetOutputAvailableType,
								lDecoderTransform,
								0,
								lOutputIndex++,
								&lOutputMediaType);
							
							if (lcreateTempColorConvertor)
							{
								LOG_INVOKE_FUNCTION(resolveColorConvertionVideoConnection,
									aPtrTopology,
									lOutputMediaType,
									aPtrDownStreamMediaType,
									&lColorConvertorTopologyNode,
									aPtrPtrTailTopologyNode);
							}
							else
							{
								GUID lSubTypeOutputMediaType;

								LOG_INVOKE_MF_METHOD(GetGUID,
									lOutputMediaType,
									MF_MT_SUBTYPE,
									&lSubTypeOutputMediaType);
																
								if (lDownStreamSubType != lSubTypeOutputMediaType)
								{
									lresult = MF_E_INVALIDTYPE;
								}
							}

						} while (FAILED(lresult));

						if (SUCCEEDED(lresult))
						{
							do
							{
								LOG_INVOKE_MF_METHOD(SetOutputType,
									lDecoderTransform,
									0,
									lOutputMediaType,
									0);



								//CComPtrCustom<IMFMediaType> l_UpStreamMediaType;

								//LOG_INVOKE_MF_FUNCTION(MFCreateMediaType,
								//	&l_UpStreamMediaType);

								//LOG_CHECK_PTR_MEMORY(l_UpStreamMediaType);

								//LOG_INVOKE_MF_METHOD(CopyAllItems,
								//	aPtrUpStreamMediaType,
								//	l_UpStreamMediaType);

								//using namespace pugi;

								//xml_document lxmlDoc;

								//auto ldeclNode = lxmlDoc.append_child(node_declaration);

								//ldeclNode.append_attribute(L"version") = L"1.0";

								//xml_node lcommentNode = lxmlDoc.append_child(node_comment);

								//lcommentNode.set_value(L"XML Document of sources");

								//auto lRootXMLElement = lxmlDoc.append_child(L"Sources");
								//DataParser::readMediaType(
								//	l_UpStreamMediaType,
								//	lRootXMLElement);

								//std::wstringstream lwstringstream;

								//lxmlDoc.print(lwstringstream);

								//std::wstring lXMLDocumentString;

								//lXMLDocumentString = lwstringstream.str();

								//LOG_INVOKE_MF_METHOD(SetInputType,
								//	lDecoderTransform,
								//	0,
								//	l_UpStreamMediaType,
								//	0);


							} while (false);

							if (FAILED(lresult))
							{
								continue;
							}

							break;
						}
					}

					LOG_CHECK_STATE_DESCR(FAILED(lresult), lresult);
					
					CComPtrCustom<IMFTopologyNode> lDecoderTopologyNode;

					LOG_INVOKE_MF_FUNCTION(MFCreateTopologyNode,
						MF_TOPOLOGY_TRANSFORM_NODE,
						&lDecoderTopologyNode);
					
					LOG_INVOKE_MF_METHOD(AddNode,
						aPtrTopology,
						lDecoderTopologyNode);
					
					LOG_INVOKE_MF_METHOD(SetObject,
						lDecoderTopologyNode,
						lDecoderTransform);
					
					*aPtrPtrHeadTopologyNode = lDecoderTopologyNode;

					(*aPtrPtrHeadTopologyNode)->AddRef();

					if (lColorConvertorTopologyNode)
					{
						LOG_INVOKE_MF_METHOD(ConnectOutput,
							lDecoderTopologyNode,
							0,
							lColorConvertorTopologyNode,
							0);
					}
					else
					{
						*aPtrPtrTailTopologyNode = lDecoderTopologyNode.detach();
					}

				} while (false);

				return lresult;
			}
			
			HRESULT VideoTopologyResolver::checkDemandForVideoResizer(
				IMFMediaType* aPtrUpStreamMediaType,
				IMFMediaType* aPtrDownStreamMediaType)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrUpStreamMediaType);

					LOG_CHECK_PTR_MEMORY(aPtrDownStreamMediaType);
					
					GUID lMajorType;

					LOG_INVOKE_MF_METHOD(GetGUID,aPtrUpStreamMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);
					
					LOG_CHECK_STATE(lMajorType != MFMediaType_Video);
					
					LOG_INVOKE_MF_METHOD(GetGUID,aPtrDownStreamMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);
					
					LOG_CHECK_STATE(lMajorType != MFMediaType_Video);

					BOOL lCompareResult = FALSE;

					PROPVARIANT lPtrVarItem;
					
					LOG_INVOKE_MF_METHOD(GetItem,aPtrDownStreamMediaType,
						MF_MT_FRAME_SIZE,
						&lPtrVarItem);
					
					LOG_INVOKE_MF_METHOD(CompareItem,aPtrUpStreamMediaType,
						MF_MT_FRAME_SIZE,
						lPtrVarItem,
						&lCompareResult);
					
					if (lCompareResult == TRUE)
					{
						lresult = S_FALSE;

						break;
					}

				} while (false);

				return lresult;
			}

			HRESULT VideoTopologyResolver::checkDemandForFrameRateConvertor(
				IMFMediaType* aPtrUpStreamMediaType,
				IMFMediaType* aPtrDownStreamMediaType)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrUpStreamMediaType);

					LOG_CHECK_PTR_MEMORY(aPtrDownStreamMediaType);
					
					GUID lMajorType;

					LOG_INVOKE_MF_METHOD(GetGUID,aPtrUpStreamMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);
					
					LOG_CHECK_STATE(lMajorType != MFMediaType_Video);

					LOG_INVOKE_MF_METHOD(GetGUID,aPtrDownStreamMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);

					LOG_CHECK_STATE(lMajorType != MFMediaType_Video);

					BOOL lCompareResult = FALSE;


					//using namespace pugi;

					//xml_document lxmlDoc;

					//auto ldeclNode = lxmlDoc.append_child(node_declaration);

					//ldeclNode.append_attribute(L"version") = L"1.0";

					//xml_node lcommentNode = lxmlDoc.append_child(node_comment);

					//lcommentNode.set_value(L"XML Document of sources");

					//auto lRootXMLElement = lxmlDoc.append_child(L"Sources");
					//DataParser::readMediaType(
					//	aPtrUpStreamMediaType,
					//	lRootXMLElement);

					//std::wstringstream lwstringstream;

					//lxmlDoc.print(lwstringstream);

					//std::wstring lXMLDocumentString;

					//lXMLDocumentString = lwstringstream.str();



					PROPVARIANT lPtrVarItem;

					LOG_INVOKE_MF_METHOD(GetItem, aPtrUpStreamMediaType,
						MF_MT_FRAME_RATE,
						&lPtrVarItem);
					
					LOG_INVOKE_MF_METHOD(CompareItem, aPtrDownStreamMediaType,
						MF_MT_FRAME_RATE,
						lPtrVarItem,
						&lCompareResult);

					if (lCompareResult == TRUE)
					{
						lresult = S_FALSE;

						break;
					}

				} while (false);

				return lresult;
			}

			HRESULT VideoTopologyResolver::resolveFrameConvertorConnection(
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

					LOG_INVOKE_MF_METHOD(GetGUID,aPtrUpStreamMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);
					
					LOG_CHECK_STATE(lMajorType != MFMediaType_Video);

					LOG_INVOKE_MF_METHOD(GetGUID,aPtrDownStreamMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);

					LOG_CHECK_STATE(lMajorType != MFMediaType_Video);

					LOG_INVOKE_FUNCTION(checkDemandForVideoResizer,
						aPtrUpStreamMediaType,
						aPtrDownStreamMediaType);

					decltype(lresult) lresultVideoResizer = lresult;
					
					LOG_INVOKE_FUNCTION(checkDemandForFrameRateConvertor,
						aPtrUpStreamMediaType,
						aPtrDownStreamMediaType);

					decltype(lresult) lresultFrameRateConvertor = lresult;
					
					if (lresultVideoResizer == S_OK && lresultFrameRateConvertor == S_OK)
					{
						LOG_INVOKE_FUNCTION(resolveFrameRateAndResizeVideoConnection,
							aPtrTopology,
							aPtrUpStreamMediaType,
							aPtrDownStreamMediaType,
							aPtrPtrHeadTopologyNode,
							aPtrPtrTailTopologyNode);
					}
					else if (lresultVideoResizer == S_OK && lresultFrameRateConvertor == S_FALSE)
					{
						LOG_INVOKE_FUNCTION(resolveResizeVideoConnection,
							aPtrTopology,
							aPtrUpStreamMediaType,
							aPtrDownStreamMediaType,
							aPtrPtrHeadTopologyNode,
							aPtrPtrTailTopologyNode);
					}
					else if (lresultVideoResizer == S_FALSE && lresultFrameRateConvertor == S_OK)
					{
						LOG_INVOKE_FUNCTION(resolveFrameRateVideoConnection,
							aPtrTopology,
							aPtrUpStreamMediaType,
							aPtrDownStreamMediaType,
							aPtrPtrHeadTopologyNode,
							aPtrPtrTailTopologyNode);
					}
					else if (lresultVideoResizer == S_FALSE && lresultFrameRateConvertor == S_FALSE)
					{
						lresult = S_FALSE;
					}

				} while (false);

				return lresult;
			}

			HRESULT VideoTopologyResolver::findSuitableDecoderMediaType(
				IMFMediaType* aPtrUpStreamMediaType,
				IMFVideoSampleAllocator*  aPtrIMFVideoSampleAllocator,
				IMFMediaType** aPtrPtrFirstOutputMediaType,
				IMFMediaTypeHandler* aPtrMediaTypeHandler)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrUpStreamMediaType);

					LOG_CHECK_PTR_MEMORY(aPtrIMFVideoSampleAllocator);

					LOG_CHECK_PTR_MEMORY(aPtrUpStreamMediaType);

					LOG_CHECK_PTR_MEMORY(aPtrPtrFirstOutputMediaType);
					
					GUID lMajorType;

					LOG_INVOKE_MF_METHOD(GetGUID, aPtrUpStreamMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);

					LOG_CHECK_STATE(lMajorType != MFMediaType_Video);
					
					GUID lUpStreamSubType;

					LOG_INVOKE_MF_METHOD(GetGUID,
						aPtrUpStreamMediaType,
						MF_MT_SUBTYPE,
						&lUpStreamSubType);
					
					MFT_REGISTER_TYPE_INFO lUpStreamMFtypeInfo;

					lUpStreamMFtypeInfo.guidMajorType = MFMediaType_Video;

					lUpStreamMFtypeInfo.guidSubtype = lUpStreamSubType;

					
					CComMassivPtr<IMFActivate> lDecoderList;

					UINT32 lFlags = MFT_ENUM_FLAG_SYNCMFT;
					 //MFT_ENUM_FLAG_ALL;


					LOG_INVOKE_MF_FUNCTION(MFTEnumEx,
						MFT_CATEGORY_VIDEO_DECODER,
						lFlags,
						&lUpStreamMFtypeInfo,
						nullptr,
						lDecoderList.getPtrMassivPtr(),
						lDecoderList.getPtrSizeMassiv());

					LOG_CHECK_STATE_DESCR(lDecoderList.getSizeMassiv() == 0, MF_E_TOPO_CODEC_NOT_FOUND);
					
					auto lDecoderCount = lDecoderList.getSizeMassiv();

					CComPtrCustom<IMFTransform> lDecoderTransform;

					CComPtrCustom<IMFMediaType> lFirstOutputMediaType;
					
					for (decltype(lDecoderCount) lDecoderTreansformIndex = 0; lDecoderTreansformIndex < lDecoderCount; lDecoderTreansformIndex++)
					{
						lDecoderTransform.Release();

						LOG_INVOKE_MF_METHOD(ActivateObject,
							lDecoderList[lDecoderTreansformIndex],
							IID_PPV_ARGS(&lDecoderTransform));

						if (FAILED(lresult))
						{
							continue;
						}



						CComPtrCustom<IMFMediaType> l_UpStreamMediaType;

						LOG_INVOKE_MF_FUNCTION(MFCreateMediaType,
							&l_UpStreamMediaType);

						LOG_CHECK_PTR_MEMORY(l_UpStreamMediaType);

						LOG_INVOKE_MF_METHOD(CopyAllItems,
							aPtrUpStreamMediaType,
							l_UpStreamMediaType);


						//{

						//	using namespace pugi;

						//	xml_document lxmlDoc;

						//	auto ldeclNode = lxmlDoc.append_child(node_declaration);

						//	ldeclNode.append_attribute(L"version") = L"1.0";

						//	xml_node lcommentNode = lxmlDoc.append_child(node_comment);

						//	lcommentNode.set_value(L"XML Document of sources");

						//	auto lRootXMLElement = lxmlDoc.append_child(L"Sources");
						//	DataParser::readMediaType(
						//		aPtrUpStreamMediaType,
						//		lRootXMLElement);

						//	std::wstringstream lwstringstream;

						//	lxmlDoc.print(lwstringstream);

						//	std::wstring lXMLDocumentString;

						//	lXMLDocumentString = lwstringstream.str();
						//}
												
						LOG_INVOKE_MF_METHOD(SetInputType,
							lDecoderTransform,
							0,
							l_UpStreamMediaType,
							0);

						//{

						//	using namespace pugi;

						//	xml_document lxmlDoc;

						//	auto ldeclNode = lxmlDoc.append_child(node_declaration);

						//	ldeclNode.append_attribute(L"version") = L"1.0";

						//	xml_node lcommentNode = lxmlDoc.append_child(node_comment);

						//	lcommentNode.set_value(L"XML Document of sources");

						//	auto lRootXMLElement = lxmlDoc.append_child(L"Sources");
						//	DataParser::readMediaType(
						//		aPtrUpStreamMediaType,
						//		lRootXMLElement);

						//	std::wstringstream lwstringstream;

						//	lxmlDoc.print(lwstringstream);

						//	std::wstring lXMLDocumentString;

						//	lXMLDocumentString = lwstringstream.str();
						//}

						DWORD lOutputIndex = 0;

						do
						{

							CComPtrCustom<IMFMediaType> lOutputMediaType;

							LOG_INVOKE_MF_METHOD(GetOutputAvailableType,
								lDecoderTransform,
								0,
								lOutputIndex++,
								&lOutputMediaType);

							if (!lFirstOutputMediaType)
								lFirstOutputMediaType = lOutputMediaType;

							//using namespace pugi;

							//xml_document lxmlDoc;

							//auto ldeclNode = lxmlDoc.append_child(node_declaration);

							//ldeclNode.append_attribute(L"version") = L"1.0";

							//xml_node lcommentNode = lxmlDoc.append_child(node_comment);

							//lcommentNode.set_value(L"XML Document of sources");

							//auto lRootXMLElement = lxmlDoc.append_child(L"Sources");
							//DataParser::readMediaType(
							//	lOutputMediaType,
							//	lRootXMLElement);

							//std::wstringstream lwstringstream;

							//lxmlDoc.print(lwstringstream);

							//std::wstring lXMLDocumentString;

							//lXMLDocumentString = lwstringstream.str();

							lresult = aPtrMediaTypeHandler->SetCurrentMediaType(
								lOutputMediaType);

							if (SUCCEEDED(lresult))
							{
								lresult = aPtrIMFVideoSampleAllocator->InitializeSampleAllocator(
									5,
									lOutputMediaType);

								if (SUCCEEDED(lresult))
								{
									aPtrIMFVideoSampleAllocator->UninitializeSampleAllocator();

									LOG_INVOKE_QUERY_INTERFACE_METHOD(lOutputMediaType, aPtrPtrFirstOutputMediaType);

									break;
								}
							}

						} while (FAILED(lresult));

						if (SUCCEEDED(lresult))
						{
							break;
						}
					}	

					if (FAILED(lresult) && lFirstOutputMediaType)
					{
						auto ltempresult = lresult;

						LOG_INVOKE_QUERY_INTERFACE_METHOD(lFirstOutputMediaType, aPtrPtrFirstOutputMediaType);

						lresult = ltempresult;
					}

				} while (false);

				return lresult;
			}

			HRESULT VideoTopologyResolver::findSuitableColorMediaType(
				IMFMediaType* aPtrUpStreamMediaType,
				IMFVideoSampleAllocator*  aPtrIMFVideoSampleAllocator,
				IMFMediaTypeHandler* aPtrMediaTypeHandler)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrUpStreamMediaType);

					LOG_CHECK_PTR_MEMORY(aPtrIMFVideoSampleAllocator);

					LOG_CHECK_PTR_MEMORY(aPtrMediaTypeHandler);

					GUID lMajorType;

					LOG_INVOKE_MF_METHOD(GetGUID, aPtrUpStreamMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);

					LOG_CHECK_STATE(lMajorType != MFMediaType_Video);

					do
					{
						CComPtrCustom<IMFTransform> lColorConverterTransform;

						LOG_INVOKE_OBJECT_METHOD(lColorConverterTransform, CoCreateInstance, __uuidof(CColorConvertDMO));

						LOG_CHECK_PTR_MEMORY(lColorConverterTransform);

						LOG_INVOKE_MF_METHOD(SetInputType, lColorConverterTransform, 0, aPtrUpStreamMediaType, 0);


						DWORD lTypeIndex = 0;

						while (SUCCEEDED(lresult))
						{

							CComPtrCustom<IMFMediaType> lOutputType;

							lresult = lColorConverterTransform->GetOutputAvailableType(0, lTypeIndex++, &lOutputType);

							if (SUCCEEDED(lresult))
							{

								PROPVARIANT lPtrVarItem;

								LOG_INVOKE_MF_METHOD(GetItem, aPtrUpStreamMediaType,
									MF_MT_FRAME_RATE,
									&lPtrVarItem);

								LOG_INVOKE_MF_METHOD(SetItem, lOutputType,
									MF_MT_FRAME_RATE,
									lPtrVarItem);

								lresult = aPtrMediaTypeHandler->SetCurrentMediaType(
									lOutputType);
								
								if (SUCCEEDED(lresult))
								{
									lresult = aPtrIMFVideoSampleAllocator->InitializeSampleAllocator(
										5,
										lOutputType);

									if (SUCCEEDED(lresult))
									{
										aPtrIMFVideoSampleAllocator->UninitializeSampleAllocator();

										break;
									}
								}

								lresult = S_OK;
							}
						}

					} while (false);

				} while (false);

				return lresult;
			}
			
			HRESULT VideoTopologyResolver::checkDemandForDirectX11Convertor(
				IMFMediaType* aPtrUpStreamMediaType,
				IMFMediaType* aPtrDownStreamMediaType)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrUpStreamMediaType);

					LOG_CHECK_PTR_MEMORY(aPtrDownStreamMediaType);

					GUID lMajorType;

					LOG_INVOKE_MF_METHOD(GetGUID, aPtrUpStreamMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);

					LOG_CHECK_STATE(lMajorType != MFMediaType_Video);

					LOG_INVOKE_MF_METHOD(GetGUID, aPtrDownStreamMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);

					LOG_CHECK_STATE(lMajorType != MFMediaType_Video);
					

					//using namespace pugi;

					//xml_document lxmlDoc;

					//auto ldeclNode = lxmlDoc.append_child(node_declaration);

					//ldeclNode.append_attribute(L"version") = L"1.0";

					//xml_node lcommentNode = lxmlDoc.append_child(node_comment);

					//lcommentNode.set_value(L"XML Document of sources");

					//auto lRootXMLElement = lxmlDoc.append_child(L"Sources");
					//DataParser::readMediaType(
					//	aPtrUpStreamMediaType,
					//	lRootXMLElement);

					//std::wstringstream lwstringstream;

					//lxmlDoc.print(lwstringstream);

					//std::wstring lXMLDocumentString;

					//lXMLDocumentString = lwstringstream.str();														

					bool lIsUpStreamDeviceManager = false;

					bool lIsDownStreamDeviceManager = false;

					CComPtrCustom<IMFDXGIDeviceManager> lUpStreamDeviceManager;

					do
					{
						LOG_INVOKE_MF_METHOD(GetUnknown, aPtrUpStreamMediaType,
							CM_DeviceManager,
							IID_PPV_ARGS(&lUpStreamDeviceManager));
						
						lIsUpStreamDeviceManager = true;

					} while (false);

					CComPtrCustom<IMFDXGIDeviceManager> lDownStreamDeviceManager;

					do
					{
						LOG_INVOKE_MF_METHOD(GetUnknown, aPtrDownStreamMediaType,
							CM_DeviceManager,
							IID_PPV_ARGS(&lDownStreamDeviceManager));

						lIsDownStreamDeviceManager = true;

					} while (false);

					if (!lIsUpStreamDeviceManager &&
						!lIsDownStreamDeviceManager)
					{
						lresult = S_FALSE;

						break;
					}

					if (lIsUpStreamDeviceManager &&
						!lIsDownStreamDeviceManager)
					{
						lresult = S_OK;

						break;
					}

					if (!lIsUpStreamDeviceManager &&
						lIsDownStreamDeviceManager)
					{
						lresult = S_OK;

						break;
					}

					if (lIsUpStreamDeviceManager &&
						lIsDownStreamDeviceManager)
					{
						if (lUpStreamDeviceManager == lDownStreamDeviceManager)
						{
							lresult = S_FALSE;

							break;
						}
						else
						{
							lresult = S_OK;

							break;
						}
					}

				} while (false);

				return lresult;
			}
			
			HRESULT VideoTopologyResolver::resolveDirectX11ConvertorConnection(
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

					LOG_INVOKE_MF_METHOD(GetGUID, aPtrUpStreamMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);

					LOG_CHECK_STATE(lMajorType != MFMediaType_Video);

					LOG_INVOKE_MF_METHOD(GetGUID, aPtrDownStreamMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);

					LOG_CHECK_STATE(lMajorType != MFMediaType_Video);

					LOG_INVOKE_FUNCTION(checkDemandForDirectX11Convertor,
						aPtrUpStreamMediaType,
						aPtrDownStreamMediaType);

					if (lresult == S_FALSE)
						break;

					if (lresult == S_OK)
					{
						CComPtrCustom<IMFTransform> lDirectX11ConvertorTransform(new Transform::VideoSampleAllocator::DirectX11ConvertorTransform());
						
						LOG_CHECK_PTR_MEMORY(lDirectX11ConvertorTransform);

						LOG_INVOKE_MF_METHOD(SetInputType, lDirectX11ConvertorTransform, 0, aPtrUpStreamMediaType, 0);
						
						LOG_INVOKE_MF_METHOD(SetOutputType, lDirectX11ConvertorTransform, 0, aPtrDownStreamMediaType, 0);

						CComPtrCustom<IMFTopologyNode> lDirectX11ConvertorTransformTopologyNode;

						LOG_INVOKE_MF_FUNCTION(MFCreateTopologyNode,
							MF_TOPOLOGY_TRANSFORM_NODE,
							&lDirectX11ConvertorTransformTopologyNode);

						LOG_INVOKE_MF_METHOD(SetObject,
							lDirectX11ConvertorTransformTopologyNode,
							lDirectX11ConvertorTransform);

						LOG_INVOKE_MF_METHOD(AddNode,
							aPtrTopology,
							lDirectX11ConvertorTransformTopologyNode);
						
						LOG_INVOKE_QUERY_INTERFACE_METHOD(lDirectX11ConvertorTransformTopologyNode, aPtrPtrHeadTopologyNode);



						CComPtrCustom<IMFMediaType> l_UpStreamMediaType;

						LOG_INVOKE_MF_FUNCTION(MFCreateMediaType,
							&l_UpStreamMediaType);

						LOG_CHECK_PTR_MEMORY(l_UpStreamMediaType);

						LOG_INVOKE_MF_METHOD(CopyAllItems,
							aPtrUpStreamMediaType,
							l_UpStreamMediaType);

						do
						{

							LOG_INVOKE_MF_METHOD(DeleteItem,
								l_UpStreamMediaType,
								CM_DeviceManager);

						} while (false);


						CComPtrCustom<IMFTopologyNode> lHeadTopologyNode;

						CComPtrCustom<IMFTopologyNode> lTailTopologyNode;

						LOG_INVOKE_FUNCTION(resolveConnection,
							aPtrTopology,
							l_UpStreamMediaType,
							aPtrDownStreamMediaType,
							&lHeadTopologyNode,
							&lTailTopologyNode);

						if (!(!lHeadTopologyNode) == !lTailTopologyNode)
						{
							lresult = E_UNEXPECTED;

							break;
						}

						if (lHeadTopologyNode && lTailTopologyNode)
						{
							LOG_INVOKE_MF_METHOD(ConnectOutput,
								lDirectX11ConvertorTransformTopologyNode,
								0,
								lHeadTopologyNode,
								0);

							*aPtrPtrTailTopologyNode = lTailTopologyNode.detach();
						}
						else
						{
							*aPtrPtrTailTopologyNode = lDirectX11ConvertorTransformTopologyNode.detach();
						}
					}
				} while (false);

				return lresult;
			}
		}
	}
}