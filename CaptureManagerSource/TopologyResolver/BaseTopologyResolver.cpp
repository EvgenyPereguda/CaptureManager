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

#include "BaseTopologyResolver.h"
#include "../Common/ComPtrCustom.h"
#include "../Common/ComMassivPtr.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Common.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../Common/MFHeaders.h"
#include "../Common/GUIDs.h"
#include "../CustomisedTeeNode/CustomisedTeeNode.h"



namespace CaptureManager
{
	namespace MediaSession
	{
		namespace CustomisedMediaSession
		{
			using namespace Core;

			HRESULT BaseTopologyResolver::resolveDownStreamConnect(
				IMFTopology* aPtrTopology,
				CollectionOfIDs& aRefCollectionOfIDs,
				IMFTopologyNode* aPtrUpStreamNode,
				IMFMediaType* aPtrUpStreamMediaType,
				IMFTopologyNode* aPtrDownStreamNode,
				DWORD aOutputStreamIndex)
			{
				HRESULT lresult;

				do
				{

					LOG_CHECK_PTR_MEMORY(aPtrUpStreamNode);

					LOG_CHECK_PTR_MEMORY(aPtrUpStreamMediaType);

					LOG_CHECK_PTR_MEMORY(aPtrDownStreamNode);
					
					MF_TOPOLOGY_TYPE lTopologyType;

					LOG_INVOKE_MF_METHOD(GetNodeType,
						aPtrDownStreamNode,
						&lTopologyType);

					CComMassivPtr<IMFMediaType> llOutputDownStreamMediaTypes;

					CComPtrCustom<IMFMediaType> lInputDownStramMediaType;

					switch (lTopologyType)
					{
					case MF_TOPOLOGY_OUTPUT_NODE:
					{

													LOG_INVOKE_FUNCTION(getInputMediaTypeOfMediaSink,
														aPtrTopology,
														aPtrUpStreamNode,
														aPtrDownStreamNode,
														aPtrUpStreamMediaType,
														&lInputDownStramMediaType,
														aOutputStreamIndex);
													
													TOPOID lSinkTopoID;

													LOG_INVOKE_MF_METHOD(GetTopoNodeID,
														aPtrDownStreamNode,
														&lSinkTopoID);
													
													aRefCollectionOfIDs.mListOfSinkIDs.push_back(lSinkTopoID);

					}
						break;
					case MF_TOPOLOGY_SOURCESTREAM_NODE:
					{

														  TOPOID lSourceTopoID;

														  LOG_INVOKE_MF_METHOD(GetTopoNodeID,
															  aPtrDownStreamNode,
															  &lSourceTopoID);
														  
														  aRefCollectionOfIDs.mListOfSourceIDs.push_back(lSourceTopoID);

					}
						break;
					case MF_TOPOLOGY_TRANSFORM_NODE:
					{
														LOG_INVOKE_FUNCTION(getCurrentMediaTypesOfTransform,
															aPtrDownStreamNode,
															aPtrUpStreamMediaType,
														   &lInputDownStramMediaType,
														   llOutputDownStreamMediaTypes.getPtrMassivPtr(),
														   llOutputDownStreamMediaTypes.getRefSizeMassiv());

					}
						break;
					case MF_TOPOLOGY_TEE_NODE:
					{
						LOG_INVOKE_FUNCTION(getCurrentMediaTypesOfTeeNode,
													 aPtrDownStreamNode,
													 aPtrUpStreamMediaType,
													 &lInputDownStramMediaType,
													 llOutputDownStreamMediaTypes.getPtrMassivPtr(),
													 llOutputDownStreamMediaTypes.getRefSizeMassiv());

					}
						break;
					case MF_TOPOLOGY_MAX:
						break;
					default:
						break;
					}

					CComPtrCustom<IMFTopologyNode> lHeadTopologyNode;

					CComPtrCustom<IMFTopologyNode> lTailTopologyNode;

					LOG_INVOKE_FUNCTION(resolveConnection,
						aPtrTopology,
						aPtrUpStreamMediaType,
						lInputDownStramMediaType,
						&lHeadTopologyNode,
						&lTailTopologyNode);
					
					if (!(!lHeadTopologyNode) == !lTailTopologyNode)
					{
						lresult = E_UNEXPECTED;

						break;
					}

					if (lHeadTopologyNode && lTailTopologyNode)
					{
						CComPtrCustom<IMFTopologyNode> lOutputTopologyNode;

						DWORD lInputIndexOnDownstreamNode;

						LOG_INVOKE_MF_METHOD(GetOutput,
							aPtrUpStreamNode,
							aOutputStreamIndex,
							&lOutputTopologyNode,
							&lInputIndexOnDownstreamNode);
						
						LOG_INVOKE_MF_METHOD(DisconnectOutput,
							aPtrUpStreamNode,
							aOutputStreamIndex);
						
						LOG_INVOKE_MF_METHOD(ConnectOutput,
							aPtrUpStreamNode,
							aOutputStreamIndex,
							lHeadTopologyNode,
							0);
						
						LOG_INVOKE_MF_METHOD(ConnectOutput,
							lTailTopologyNode,
							0,
							lOutputTopologyNode,
							lInputIndexOnDownstreamNode);
						
					}
					else
					{
						LOG_CHECK_STATE_DESCR(FAILED(lresult), lresult);

					}

					if (lTopologyType == MF_TOPOLOGY_OUTPUT_NODE)
					{
						break;
					}

					DWORD lOutputNodeCount = 0;

					LOG_INVOKE_MF_METHOD(GetOutputCount,
						aPtrDownStreamNode,
						&lOutputNodeCount);

					UINT32 lIsSwitcherNode(FALSE);

					do
					{
						LOG_INVOKE_MF_METHOD(GetUINT32, aPtrDownStreamNode, CM_SwitcherNode, &lIsSwitcherNode);
					} 
					while (false);

					UINT32 lIsMixerNode(FALSE);

					do
					{
						LOG_INVOKE_MF_METHOD(GetUINT32, aPtrDownStreamNode, CM_MixerNode, &lIsMixerNode);
					} while (false);

					lresult = S_OK;

					if (lIsSwitcherNode == FALSE && lIsMixerNode == FALSE)
						LOG_CHECK_STATE_DESCR(lOutputNodeCount == 0 && lTopologyType != MF_TOPOLOGY_OUTPUT_NODE, MF_E_TOPO_INVALID_OPTIONAL_NODE);
	
					if (lOutputNodeCount == 0)
					{
						break;
					}

					auto lOutputCount = llOutputDownStreamMediaTypes.getSizeMassiv();

					for (UINT32 lIndex = 0; lIndex < lOutputCount; lIndex++)
					{

						CComPtrCustom<IMFTopologyNode> lNextTopologyNode;

						DWORD lInputIndexOnDownstreamNode;

						LOG_INVOKE_MF_METHOD(GetOutput,
							aPtrDownStreamNode,
							lIndex,
							&lNextTopologyNode,
							&lInputIndexOnDownstreamNode);
						
						LOG_INVOKE_FUNCTION(resolveDownStreamConnect,
							aPtrTopology,
							aRefCollectionOfIDs,
							aPtrDownStreamNode,
							llOutputDownStreamMediaTypes[lIndex],
							lNextTopologyNode,
							lIndex);
					}

				} while (false);

				return lresult;
			}
		
			HRESULT BaseTopologyResolver::getInputMediaTypeOfMediaSink(
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

					//LOG_INVOKE_MF_METHOD(GetCurrentMediaType,
					//	lMediaTypeHandler,
					//	aPtrPtrDownStreamMediaType);
					
					LOG_INVOKE_MF_METHOD(SetCurrentMediaType,
						lMediaTypeHandler,
						aPtrUpStreamMediaType);

					LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrUpStreamMediaType, aPtrPtrDownStreamMediaType);
					
					LOG_CHECK_PTR_MEMORY((*aPtrPtrDownStreamMediaType));

				} while (false);
				
				return lresult;
			}
			
			HRESULT BaseTopologyResolver::getCurrentMediaTypesOfTransform(
				IMFTopologyNode* aPtrDownStreamNode,
				IMFMediaType* aPtrOutputUpStreamMediaType,
				IMFMediaType** aPtrPtrInputDownStreamMediaType,
				IMFMediaType*** aPtrPtrPtrOutputDownStramMediaTypes,
				UINT32& aRefOutputDownStramMediaTypeCount)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrDownStreamNode);

					LOG_CHECK_PTR_MEMORY(aPtrPtrInputDownStreamMediaType);

					LOG_CHECK_PTR_MEMORY(aPtrPtrPtrOutputDownStramMediaTypes);
					
					CComPtrCustom<IUnknown> lUnknown;

					LOG_INVOKE_MF_METHOD(GetObject,
						aPtrDownStreamNode,
						&lUnknown);
					
					CComQIPtrCustom<IMFTransform> lTransform = lUnknown;

					LOG_CHECK_PTR_MEMORY(lTransform);

					do
					{
						LOG_INVOKE_MF_METHOD(GetInputCurrentType,
							lTransform,
							0,
							aPtrPtrInputDownStreamMediaType);

					} while (false);

					if (FAILED(lresult))
					{
						do
						{
							LOG_INVOKE_MF_METHOD(SetInputType,
								lTransform,
								0,
								aPtrOutputUpStreamMediaType,
								0);

						} while (false);

						if (FAILED(lresult))
						{
							LOG_INVOKE_FUNCTION(resolveInputType,
								lTransform,
								aPtrOutputUpStreamMediaType);					
						}


						LOG_INVOKE_MF_METHOD(GetInputCurrentType,
							lTransform,
							0,
							aPtrPtrInputDownStreamMediaType);

						CComPtrCustom<IMFMediaType> lOutputType;
						
						LOG_INVOKE_MF_METHOD(GetOutputAvailableType,
							lTransform,
							0,
							0,
							&lOutputType);


						LOG_INVOKE_MF_METHOD(SetOutputType,
							lTransform,
							0,
							lOutputType,
							0);

					}
					
					DWORD lOutputNodeCount;

					LOG_INVOKE_MF_METHOD(GetOutputCount,
						aPtrDownStreamNode,
						&lOutputNodeCount);
					
					UINT32 lMediaTypesCounts = 0;

					auto lPtrVoid = CoTaskMemAlloc(sizeof(IMFMediaType*)* lOutputNodeCount);

					LOG_CHECK_PTR_MEMORY(lPtrVoid);

					IMFMediaType ** lPtrPtrMediaTypes = (IMFMediaType **)(lPtrVoid);

					for (DWORD lIndex = 0; lIndex < lOutputNodeCount; lIndex++)
					{
						IMFMediaType* lPtrMediaType = nullptr;

						do
						{
							LOG_INVOKE_MF_METHOD(GetOutputCurrentType,
								lTransform,
								lIndex,
								&lPtrMediaType);

						} while (false);

						if (FAILED(lresult))
						{
							lresult = lTransform->GetOutputAvailableType(
								lIndex,
								0,
								&lPtrMediaType);
						}

						if (FAILED(lresult))
							break;
						
						lPtrPtrMediaTypes[lIndex] = lPtrMediaType;

						++lMediaTypesCounts;
					}

					if (FAILED(lresult))
					{
						for (UINT32 lIndex = 0; lIndex < lMediaTypesCounts; lIndex++)
						{
							lPtrPtrMediaTypes[lIndex]->Release();
						}

						CoTaskMemFree(lPtrPtrMediaTypes);

						break;
					}

					aRefOutputDownStramMediaTypeCount = lMediaTypesCounts;

					(*aPtrPtrPtrOutputDownStramMediaTypes) = lPtrPtrMediaTypes;

				} while (false);

				return lresult;
			}

			HRESULT BaseTopologyResolver::getCurrentMediaTypesOfTeeNode(
				IMFTopologyNode* aPtrDownStreamNode,
				IMFMediaType* aPtrOutputUpStreamMediaType,
				IMFMediaType** aPtrPtrInputDownStreamMediaType,
				IMFMediaType*** aPtrPtrPtrOutputDownStramMediaTypes,
				UINT32& aRefOutputDownStramMediaTypeCount)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrDownStreamNode);

					LOG_CHECK_PTR_MEMORY(aPtrPtrInputDownStreamMediaType);

					LOG_CHECK_PTR_MEMORY(aPtrPtrPtrOutputDownStramMediaTypes);
					
					DWORD lOutputNodeCount;

					LOG_INVOKE_MF_METHOD(GetOutputCount,
						aPtrDownStreamNode,
						&lOutputNodeCount);

					CComPtrCustom<ITeeNode> lCustomisedTeeNode;

					lCustomisedTeeNode = new(std::nothrow) CustomisedTeeNode(lOutputNodeCount);

					LOG_CHECK_PTR_MEMORY(lCustomisedTeeNode);
					
					CComPtrCustom<IMFMediaType> lInputDownStreamMediaType;

					LOG_INVOKE_MF_FUNCTION(MFCreateMediaType,
						&lInputDownStreamMediaType);
					
					LOG_INVOKE_MF_METHOD(CopyAllItems,
						aPtrOutputUpStreamMediaType,
						lInputDownStreamMediaType);
					
					UINT32 lMediaTypesCounts = 0;

					auto lPtrVoid = CoTaskMemAlloc(sizeof(IMFMediaType)* lOutputNodeCount);

					LOG_CHECK_PTR_MEMORY(lPtrVoid);

					IMFMediaType ** lPtrPtrMediaTypes = (IMFMediaType **)(lPtrVoid);

					for (DWORD lIndex = 0; lIndex < lOutputNodeCount; lIndex++)
					{
						IMFMediaType* lPtrMediaType = lInputDownStreamMediaType;

						lPtrMediaType->AddRef();

						lPtrPtrMediaTypes[lIndex] = lPtrMediaType;

						++lMediaTypesCounts;
					}

					LOG_INVOKE_MF_METHOD(SetObject,
						aPtrDownStreamNode,
						lCustomisedTeeNode);

					aRefOutputDownStramMediaTypeCount = lMediaTypesCounts;

					(*aPtrPtrPtrOutputDownStramMediaTypes) = lPtrPtrMediaTypes;

					*aPtrPtrInputDownStreamMediaType = lInputDownStreamMediaType.detach();

				} while (false);

				return lresult;
			}
		}
	}
}