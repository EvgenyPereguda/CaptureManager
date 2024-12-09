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

#include "CustomisedMediaSession.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Common.h"
#include "../Common/ComPtrCustom.h"
#include "../Common/ComMassivPtr.h"
#include "../CustomisedMediaPipelineProcessor/CustomisedMediaPipelineProcessorFactory.h"
#include "../TopologyResolver/VideoTopologyResolver.h"
#include "../TopologyResolver/AudioTopologyResolver.h"
#include "../CustomisedPresentationClock/CustomisedPresentationClock.h"
#include "../Common/GUIDs.h"
#include "../Switcher/ISwitcher.h"
#include "../SessionProcessorManager/MediaSinkFinalizeProcessor.h"


namespace CaptureManager
{
	namespace MediaSession
	{
		namespace CustomisedMediaSession
		{
			using namespace Core;
			
			HRESULT initOutputDataBuffer(
				IMFTransform* aPtrTransform,
				MFT_OUTPUT_DATA_BUFFER& aRefOutputBuffer)
			{
				HRESULT lresult;

				MFT_OUTPUT_STREAM_INFO loutputStreamInfo;

				DWORD loutputStreamId = 0;

				CComPtrCustom<IMFSample> lOutputSample;

				CComPtrCustom<IMFMediaBuffer> lMediaBuffer;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrTransform);

					ZeroMemory(&loutputStreamInfo, sizeof(loutputStreamInfo));

					ZeroMemory(&aRefOutputBuffer, sizeof(aRefOutputBuffer));

					LOG_INVOKE_MF_METHOD(GetOutputStreamInfo, aPtrTransform, loutputStreamId, &loutputStreamInfo);
					
					if ((loutputStreamInfo.dwFlags & MFT_OUTPUT_STREAM_PROVIDES_SAMPLES) == 0 &&
						(loutputStreamInfo.dwFlags & MFT_OUTPUT_STREAM_CAN_PROVIDE_SAMPLES) == 0)
					{
						LOG_INVOKE_MF_FUNCTION(MFCreateSample, &lOutputSample);
						
						LOG_INVOKE_MF_FUNCTION(MFCreateMemoryBuffer, loutputStreamInfo.cbSize, &lMediaBuffer);
						
						LOG_INVOKE_MF_METHOD(AddBuffer, lOutputSample, lMediaBuffer);
						
						aRefOutputBuffer.pSample = lOutputSample.Detach();
					}

					aRefOutputBuffer.dwStreamID = loutputStreamId;
				} while (false);

				return lresult;
			}

			
			CustomisedMediaSession::CustomisedMediaSession():
				mFinishBarierState(false),
				mSessionIsInitialised(false),
				mIsSwitchersDetached(false),
				mMediaSessionSate(MediaSessionSate::None),
				mClockStartOffset(0)
			{
				HRESULT lresult;

				do
				{
					LOG_INVOKE_MF_FUNCTION(MFCreateEventQueue,
						&mEventQueue);

					CComPtrCustom<IMFPresentationClock>  lClock;

					LOG_INVOKE_MF_FUNCTION(MFCreatePresentationClock,
						&lClock);

					LOG_INVOKE_FUNCTION(CustomisedPresentationClock::create, lClock, &mClock);
					
					mVideoTopologyResolver = new (std::nothrow)VideoTopologyResolver();

					LOG_CHECK_PTR_MEMORY(mVideoTopologyResolver);

					mVideoTopologyResolver->Release();
					
					mAudioTopologyResolver = new (std::nothrow)AudioTopologyResolver();

					LOG_CHECK_PTR_MEMORY(mAudioTopologyResolver);

					mAudioTopologyResolver->Release();
										
				} while (false);
				
				if (FAILED(lresult))
				{
					throw E_OUTOFMEMORY;
				}
			}

			CustomisedMediaSession::~CustomisedMediaSession()
			{
				HRESULT lresult;
				
				if (mEventQueue)
				{
					do
					{
						LOG_INVOKE_MF_METHOD(Shutdown, mEventQueue);

					} while (false);

					mEventQueue.Release();
				}
			}

			// ISessionSwitcherControl interface

			HRESULT STDMETHODCALLTYPE CustomisedMediaSession::pauseSwitchers()
			{
				HRESULT lresult(E_NOTIMPL);

				do
				{

					WORD lTopoCount;

					LOG_INVOKE_MF_METHOD(GetNodeCount,
						mCurrentTopology,
						&lTopoCount);

					for (WORD lTopoNodeIndex = 0; lTopoNodeIndex < lTopoCount; lTopoNodeIndex++)
					{
						CComPtrCustom<IMFTopologyNode> lTopologyNode;

						LOG_INVOKE_MF_METHOD(GetNode,
							mCurrentTopology,
							lTopoNodeIndex,
							&lTopologyNode);

						MF_TOPOLOGY_TYPE lTopoNodeType;

						LOG_INVOKE_MF_METHOD(GetNodeType,
							lTopologyNode,
							&lTopoNodeType);

						switch (lTopoNodeType)
						{
						case MF_TOPOLOGY_OUTPUT_NODE:
							break;
						case MF_TOPOLOGY_SOURCESTREAM_NODE:
							break;
						case MF_TOPOLOGY_TRANSFORM_NODE:
						{
							UINT32 lState(0);

							do
							{
								LOG_INVOKE_MF_METHOD(GetUINT32, lTopologyNode, CM_SwitcherNode, &lState);

								if (lState != TRUE)
									break;

								CComPtrCustom<IUnknown> lUnkTransform;

								LOG_INVOKE_MF_METHOD(GetObject,
									lTopologyNode,
									&lUnkTransform);

								LOG_CHECK_PTR_MEMORY(lUnkTransform);

								CComPtrCustom<Transform::ISwitcher> lSwitcher;

								LOG_INVOKE_QUERY_INTERFACE_METHOD(lUnkTransform, &lSwitcher);

								LOG_CHECK_PTR_MEMORY(lSwitcher);

								lSwitcher->pause();

							} while (false);

						}
						break;
						case MF_TOPOLOGY_TEE_NODE:
							break;
						case MF_TOPOLOGY_MAX:
							break;
						default:
							break;
						}
					}

					lresult = S_OK;

				} while (false);

				return lresult;
			}

			HRESULT STDMETHODCALLTYPE CustomisedMediaSession::resumeSwitchers()
			{
				HRESULT lresult(E_NOTIMPL);

				do
				{
					if (mIsSwitchersDetached)
					{
						std::map<IMFMediaSink*, CComPtrCustom<IMFMediaSink>> lMediaSinks;

						for (auto& lItem : mListOfMediaPipelineProcessors)
						{
							CComPtrCustom<IMediaPipelineProcessorControl> lMediaPipelineProcessorControl;

							lItem->QueryInterface(IID_PPV_ARGS(&lMediaPipelineProcessorControl));

							if (lMediaPipelineProcessorControl)
							{
								for (auto& lswitcherCollection : mSwitcherCollection)
								{
									lresult = lMediaPipelineProcessorControl->checkSwitcherNode(lswitcherCollection.first);

									if (SUCCEEDED(lresult))
									{

										for (auto& lstreamId : lswitcherCollection.second.mListOfSinkIDs)
										{
											lMediaPipelineProcessorControl->registerStreamSink(lstreamId, mClock);


											CComQIPtrCustom<IMFTopologyNode> lOutputTopologyNode;

											LOG_INVOKE_MF_METHOD(GetNodeByID,
												mCurrentTopology,
												lstreamId,
												&lOutputTopologyNode);

											LOG_CHECK_PTR_MEMORY(lOutputTopologyNode);

											MF_TOPOLOGY_TYPE lTopoNodeType;

											LOG_INVOKE_MF_METHOD(GetNodeType,
												lOutputTopologyNode,
												&lTopoNodeType);

											CComPtrCustom<IUnknown> lUnkSinkActivate;

											LOG_INVOKE_MF_METHOD(GetObject,
												lOutputTopologyNode,
												&lUnkSinkActivate);

											LOG_CHECK_PTR_MEMORY(lUnkSinkActivate);

											CComQIPtrCustom<IMFActivate> lSinkActivate = lUnkSinkActivate;

											LOG_CHECK_PTR_MEMORY(lSinkActivate);

											CComPtrCustom<IMFMediaSink> lMediaSink;

											LOG_INVOKE_MF_FUNCTION(ActivateObject,
												lSinkActivate,
												IID_PPV_ARGS(&lMediaSink));

											LOG_CHECK_PTR_MEMORY(lMediaSink);

											auto lfind = lMediaSinks.find(lMediaSink.get());

											if (lfind == lMediaSinks.end())
												lMediaSinks[lMediaSink.get()] = lMediaSink;

										}
									}									
								}
							}
						}
						
						for (auto& lMediaSink : lMediaSinks)
						{
							CComPtrCustom<IMFClockStateSink> lIMFClockStateSink;

							auto lre = lMediaSink.second->QueryInterface(IID_PPV_ARGS(&lIMFClockStateSink));

							if (SUCCEEDED(lre) && lIMFClockStateSink)
							{
								LONGLONG lClockTime = 0;

								MFTIME lSystemTime = 0;

								lre = mClock->GetCorrelatedTime(0, &lClockTime, &lSystemTime);

								lre = lIMFClockStateSink->OnClockStart(lSystemTime, 0);
							}
						}

						mSwitcherCollection.clear();
					}

					WORD lTopoCount;

					LOG_INVOKE_MF_METHOD(GetNodeCount,
						mCurrentTopology,
						&lTopoCount);

					for (WORD lTopoNodeIndex = 0; lTopoNodeIndex < lTopoCount; lTopoNodeIndex++)
					{
						CComPtrCustom<IMFTopologyNode> lTopologyNode;

						LOG_INVOKE_MF_METHOD(GetNode,
							mCurrentTopology,
							lTopoNodeIndex,
							&lTopologyNode);

						MF_TOPOLOGY_TYPE lTopoNodeType;

						LOG_INVOKE_MF_METHOD(GetNodeType,
							lTopologyNode,
							&lTopoNodeType);

						switch (lTopoNodeType)
						{
						case MF_TOPOLOGY_OUTPUT_NODE:
							break;
						case MF_TOPOLOGY_SOURCESTREAM_NODE:
							break;
						case MF_TOPOLOGY_TRANSFORM_NODE:
						{
							UINT32 lState(0);

							do
							{
								LOG_INVOKE_MF_METHOD(GetUINT32, lTopologyNode, CM_SwitcherNode, &lState);

								if (lState != TRUE)
									break;

								CComPtrCustom<IUnknown> lUnkTransform;

								LOG_INVOKE_MF_METHOD(GetObject,
									lTopologyNode,
									&lUnkTransform);

								LOG_CHECK_PTR_MEMORY(lUnkTransform);

								CComPtrCustom<Transform::ISwitcher> lSwitcher;

								LOG_INVOKE_QUERY_INTERFACE_METHOD(lUnkTransform, &lSwitcher);

								LOG_CHECK_PTR_MEMORY(lSwitcher);

								if (mIsSwitchersDetached)
									lSwitcher->reset();
								else
									lSwitcher->resume();								
							} while (false);

						}
						break;
						case MF_TOPOLOGY_TEE_NODE:
							break;
						case MF_TOPOLOGY_MAX:
							break;
						default:
							break;
						}
					}

					mIsSwitchersDetached = false;

					lresult = S_OK;

				} while (false);

				return lresult;
			}
									
			HRESULT STDMETHODCALLTYPE CustomisedMediaSession::detachSwitchers()
			{
				HRESULT lresult(E_NOTIMPL);

				do
				{					
					WORD lTopoCount;

					LOG_INVOKE_MF_METHOD(GetNodeCount,
						mCurrentTopology,
						&lTopoCount);
					
					std::vector<CComPtrCustom<IMFTopologyNode>> lOutputTopologyNodes;

					std::vector<CComPtrCustom<IMFTopologyNode>> lDownStreamTopologyNodes;

					for (WORD lTopoNodeIndex = 0; lTopoNodeIndex < lTopoCount; lTopoNodeIndex++)
					{
						CComPtrCustom<IMFTopologyNode> lTopologyNode;

						LOG_INVOKE_MF_METHOD(GetNode,
							mCurrentTopology,
							lTopoNodeIndex,
							&lTopologyNode);

						MF_TOPOLOGY_TYPE lTopoNodeType;

						LOG_INVOKE_MF_METHOD(GetNodeType,
							lTopologyNode,
							&lTopoNodeType);

						switch (lTopoNodeType)
						{
						case MF_TOPOLOGY_OUTPUT_NODE:
							break;
						case MF_TOPOLOGY_SOURCESTREAM_NODE:
							break;
						case MF_TOPOLOGY_TRANSFORM_NODE:
						{
							UINT32 lState(0);

							do
							{
								LOG_INVOKE_MF_METHOD(GetUINT32, lTopologyNode, CM_SwitcherNode, &lState);

								if (lState != TRUE)
									break;

								DWORD lOutputCount(0);

								LOG_INVOKE_MF_METHOD(GetOutputCount, lTopologyNode, &lOutputCount);

								if (lOutputCount == 1)
								{
									CComPtrCustom<IMFTopologyNode> lDownStreamNode;

									DWORD lInputIndexOnDownStreamNode(0);

									LOG_INVOKE_MF_METHOD(GetOutput, lTopologyNode, 0, &lDownStreamNode, &lInputIndexOnDownStreamNode);

									lDownStreamTopologyNodes.push_back(lDownStreamNode);

									findOutputNodes(
										lOutputTopologyNodes,
										lDownStreamNode);
								}


							} while (false);

						}
						break;
						case MF_TOPOLOGY_TEE_NODE:
							break;
						case MF_TOPOLOGY_MAX:
							break;
						default:
							break;
						}
					}

					std::map<IMFMediaSink*, CComPtrCustom<IMFMediaSink>> lMediaSinks;
					
					for (auto& litem : lOutputTopologyNodes)
					{

						MF_TOPOLOGY_TYPE lTopoNodeType;

						LOG_INVOKE_MF_METHOD(GetNodeType,
							litem,
							&lTopoNodeType);
												
						CComPtrCustom<IUnknown> lUnkSinkActivate;

						LOG_INVOKE_MF_METHOD(GetObject,
							litem,
							&lUnkSinkActivate);

						LOG_CHECK_PTR_MEMORY(lUnkSinkActivate);

						CComQIPtrCustom<IMFActivate> lSinkActivate = lUnkSinkActivate;

						LOG_CHECK_PTR_MEMORY(lSinkActivate);
						
						CComPtrCustom<IMFMediaSink> lMediaSink;

						LOG_INVOKE_MF_FUNCTION(ActivateObject,
							lSinkActivate,
							IID_PPV_ARGS(&lMediaSink));
						
						LOG_CHECK_PTR_MEMORY(lMediaSink);

						auto lfind = lMediaSinks.find(lMediaSink.get());
						
						if (lfind == lMediaSinks.end())
							lMediaSinks[lMediaSink.get()] = lMediaSink;
							
					}
					
					for (auto& lMediaSink : lMediaSinks)
					{
						CComPtrCustom<IMFClockStateSink> lIMFClockStateSink;

						auto lre = lMediaSink.second->QueryInterface(IID_PPV_ARGS(&lIMFClockStateSink));

						if (SUCCEEDED(lre) && lIMFClockStateSink)
						{
							LONGLONG lClockTime = 0;

							MFTIME lSystemTime = 0;

							lre = mClock->GetCorrelatedTime(0, &lClockTime, &lSystemTime);

							lre = lIMFClockStateSink->OnClockStop(lSystemTime);
						}
					}


					CComPtrCustom<IMFCollection> lCollection;

					LOG_INVOKE_MF_FUNCTION(GetOutputNodeCollection,
						mCurrentTopology,
						&lCollection);

					LOG_CHECK_PTR_MEMORY(lCollection);
					
					DWORD lElementCount(0);

					do
					{
						LOG_INVOKE_MF_FUNCTION(GetElementCount,
							lCollection,
							&lElementCount);

					} while (false);

					if (FAILED(lresult))
					{
						lElementCount = 0;
					}
					
					for (DWORD lElementIndex = 0; lElementIndex < lElementCount; lElementIndex++)
					{
						CComPtrCustom<IUnknown> lNodeUnk;

						CComQIPtrCustom<IMFTopologyNode> lNode;

						LOG_INVOKE_MF_FUNCTION(GetElement,
							lCollection,
							lElementIndex,
							&lNodeUnk);

						if (!lNodeUnk)
						{
							continue;
						}

						lNode = lNodeUnk;

						if (!lNode)
						{
							continue;
						}

						CComPtrCustom<IUnknown> lSinkUnk;

						LOG_INVOKE_MF_FUNCTION(GetObject,
							lNode,
							&lSinkUnk);

						if (!lSinkUnk)
						{
							continue;
						}

						CComQIPtrCustom<IMFActivate> lSinkActivate = lSinkUnk;

						if (!lSinkActivate)
						{
							continue;
						}

						CComPtrCustom<IMFMediaSink> lMediaSink;

						LOG_INVOKE_MF_FUNCTION(ActivateObject,
							lSinkActivate,
							IID_PPV_ARGS(&lMediaSink));

						LOG_CHECK_PTR_MEMORY(lMediaSink);

						auto lfindResult = lMediaSinks.find(lMediaSink.get());

						if (lfindResult == lMediaSinks.end())
						{
							continue;
						}

						CComPtrCustom<IMFTopologyNode> lUpstreamNode;

						DWORD lOutputIndexOnUpstreamNode(0);

						lNode->GetInput(0, &lUpstreamNode, &lOutputIndexOnUpstreamNode);

						lUpstreamNode->DisconnectOutput(lOutputIndexOnUpstreamNode);

						mCurrentTopology->RemoveNode(lNode);

						UINT32 lStreamID;

						do
						{

							LOG_INVOKE_MF_FUNCTION(GetUINT32,
								lNode,
								MF_TOPONODE_STREAMID,
								&lStreamID);

						} while (false);

						if (FAILED(lresult))
						{
							continue;
						}

						CComPtrCustom<IMFStreamSink> lStreamSink;

						do
						{
							LOG_INVOKE_MF_METHOD(GetStreamSinkById,
								lMediaSink,
								lStreamID,
								&lStreamSink);

						} while (false);

						if (FAILED(lresult))
						{
							continue;
						}
						
						lresult = lStreamSink->Flush();

						for (auto& litem : mListOfMediaPipelineProcessors)
						{
							CComPtrCustom<IMediaPipelineProcessorControl> lIMediaPipelineProcessorControl;

							LOG_INVOKE_QUERY_INTERFACE_METHOD(litem, &lIMediaPipelineProcessorControl);

							if (lIMediaPipelineProcessorControl)
							{
								lIMediaPipelineProcessorControl->removeStreamSink(lStreamSink);
							}
						}
					}
					
					CComPtrCustom<MediaSinkFinalizeProcessor> lMediaSinkFinalizeProcessor =
						new (std::nothrow)MediaSinkFinalizeProcessor();

					if (!lMediaSinkFinalizeProcessor)
					{
						break;
					}

					for (auto& lMediaSink : lMediaSinks)
					{
						lMediaSinkFinalizeProcessor->finalizeMediaSink(lMediaSink.second);

						lresult = lMediaSink.second->Shutdown();
					}
										
					for (WORD lTopoNodeIndex = 0; lTopoNodeIndex < lTopoCount; lTopoNodeIndex++)
					{
						CComPtrCustom<IMFTopologyNode> lTopologyNode;

						LOG_INVOKE_MF_METHOD(GetNode,
							mCurrentTopology,
							lTopoNodeIndex,
							&lTopologyNode);

						MF_TOPOLOGY_TYPE lTopoNodeType;

						LOG_INVOKE_MF_METHOD(GetNodeType,
							lTopologyNode,
							&lTopoNodeType);

						switch (lTopoNodeType)
						{
						case MF_TOPOLOGY_OUTPUT_NODE:
							break;
						case MF_TOPOLOGY_SOURCESTREAM_NODE:
							break;
						case MF_TOPOLOGY_TRANSFORM_NODE:
						{
							UINT32 lState(0);

							do
							{
								LOG_INVOKE_MF_METHOD(GetUINT32, lTopologyNode, CM_SwitcherNode, &lState);

								if (lState != TRUE)
									break;

								DWORD lOutputCount(0);

								LOG_INVOKE_MF_METHOD(GetOutputCount, lTopologyNode, &lOutputCount);

								if (lOutputCount == 1)
								{
									CComPtrCustom<IMFTopologyNode> lDownStreamNode;

									DWORD lInputIndexOnDownStreamNode(0);

									LOG_INVOKE_MF_METHOD(GetOutput, lTopologyNode, 0, &lDownStreamNode, &lInputIndexOnDownStreamNode);

									LOG_INVOKE_MF_METHOD(DisconnectOutput, lTopologyNode, 0);
									
									releaseNode(lDownStreamNode);
								}


							} while (false);

						}
						break;
						case MF_TOPOLOGY_TEE_NODE:
							break;
						case MF_TOPOLOGY_MAX:
							break;
						default:
							break;
						}
					}

					mIsSwitchersDetached = true;

					mSwitcherCollection.clear();

					lresult = S_OK;

				} while (false);

				return lresult;
			}

			HRESULT STDMETHODCALLTYPE CustomisedMediaSession::notifyToBeginSwitcherStreaming(
				/* [in] */ IMFTopologyNode *aPtrTopologyNode)
			{
				HRESULT lresult(E_NOTIMPL);

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrTopologyNode);
					
					MF_TOPOLOGY_TYPE lTopoNodeType;

					LOG_INVOKE_MF_METHOD(GetNodeType,
						aPtrTopologyNode,
						&lTopoNodeType);

					switch (lTopoNodeType)
					{
					case MF_TOPOLOGY_OUTPUT_NODE:
						break;
					case MF_TOPOLOGY_SOURCESTREAM_NODE:
						break;
					case MF_TOPOLOGY_TRANSFORM_NODE:
					{

						CComQIPtrCustom<IMFTransform> lTransform;

						CComPtrCustom<IUnknown> lUnkTransform;

						LOG_INVOKE_MF_METHOD(GetObject,
							aPtrTopologyNode,
							&lUnkTransform);

						lTransform = lUnkTransform;

						LOG_CHECK_PTR_MEMORY(lTransform);

						LOG_INVOKE_MF_METHOD(ProcessMessage, lTransform, MFT_MESSAGE_NOTIFY_BEGIN_STREAMING, NULL);
					}
					break;
					case MF_TOPOLOGY_TEE_NODE:
						break;
					case MF_TOPOLOGY_MAX:
						break;
					default:
						break;
					}
					
					DWORD lOutputCount = 0;

					aPtrTopologyNode->GetOutputCount(&lOutputCount);
					
					for (size_t lOutputIndex = 0; lOutputIndex < lOutputCount; lOutputIndex++)
					{
						DWORD lInputIndexOnDownstreamNode;

						CComPtrCustom<IMFTopologyNode> lOutputTopologyNode;

						aPtrTopologyNode->GetOutput(
							lOutputIndex,
							&lOutputTopologyNode,
							&lInputIndexOnDownstreamNode);

						if (lOutputTopologyNode)
							notifyToBeginSwitcherStreaming(lOutputTopologyNode);
					}


				} while (false);

				return lresult;
			}

			
			HRESULT STDMETHODCALLTYPE CustomisedMediaSession::attachSwitcher(
				/* [in] */ IMFTopologyNode *aPtrSwitcherNode,
				/* [in] */ IMFTopologyNode *aPtrDownStreamNode)
			{
				HRESULT lresult(E_NOTIMPL);

				do
				{
					enumAndAddOutputTopologyNode(mCurrentTopology, aPtrDownStreamNode);

					CComPtrCustom<IUnknown> lUnkTransform;

					LOG_INVOKE_MF_METHOD(GetObject, aPtrSwitcherNode, &lUnkTransform);

					LOG_CHECK_PTR_MEMORY(lUnkTransform);

					aPtrSwitcherNode->ConnectOutput(0, aPtrDownStreamNode, 0);

					CComPtrCustom<IMFTransform> lTransform;

					LOG_INVOKE_QUERY_INTERFACE_METHOD(lUnkTransform, &lTransform);

					LOG_CHECK_PTR_MEMORY(lTransform);

					CComPtrCustom<Transform::ISwitcher> lSwitcher;

					LOG_INVOKE_QUERY_INTERFACE_METHOD(lUnkTransform, &lSwitcher);

					LOG_CHECK_PTR_MEMORY(lSwitcher);

					lSwitcher->pause();


					CComPtrCustom<IMFMediaType> lMediaType;

					LOG_INVOKE_MF_METHOD(GetOutputCurrentType, lTransform, 0, &lMediaType);

					LOG_CHECK_PTR_MEMORY(lMediaType);

					GUID lMajorType;

					LOG_INVOKE_MF_METHOD(GetMajorType, lMediaType, &lMajorType);

					CollectionOfIDs lCollectionOfIDs;

					if (lMajorType == MFMediaType_Video)
					{
						//CComPtrCustom<IUnknown> lUpStreamDeviceManager;

						//LOG_INVOKE_MF_METHOD(GetUnknown, lMediaType,
						//	CM_DeviceManager,
						//	IID_PPV_ARGS(&lUpStreamDeviceManager));
						
						//using namespace pugi;

						//xml_document lxmlDoc;

						//auto ldeclNode = lxmlDoc.append_child(node_declaration);

						//ldeclNode.append_attribute(L"version") = L"1.0";

						//xml_node lcommentNode = lxmlDoc.append_child(node_comment);

						//lcommentNode.set_value(L"XML Document of sources");

						//auto lRootXMLElement = lxmlDoc.append_child(L"lMediaType");
						//DataParser::readMediaType(
						//	lMediaType,
						//	lRootXMLElement);

						//std::wstringstream lwstringstream;

						//lxmlDoc.print(lwstringstream);

						//std::wstring lXMLDocumentString;

						//lXMLDocumentString = lwstringstream.str();


						//LogPrintOut::getInstance().printOutln(
						//	LogPrintOut::INFO_LEVEL,
						//	L" lMediaType: ",
						//	lXMLDocumentString.c_str());


						LOG_CHECK_PTR_MEMORY(mVideoTopologyResolver);

						LOG_INVOKE_POINTER_METHOD(mVideoTopologyResolver, resolveDownStreamConnect,
							mCurrentTopology,
							lCollectionOfIDs,
							aPtrSwitcherNode,
							lMediaType,
							aPtrDownStreamNode,
							0);

						notifyToBeginSwitcherStreaming(aPtrSwitcherNode);						
					}
					else if (lMajorType == MFMediaType_Audio)
					{						
						LOG_CHECK_PTR_MEMORY(mAudioTopologyResolver);

						LOG_INVOKE_POINTER_METHOD(mAudioTopologyResolver, resolveDownStreamConnect,
							mCurrentTopology,
							lCollectionOfIDs,
							aPtrSwitcherNode,
							lMediaType,
							aPtrDownStreamNode,
							0);
					}
					else
					{
						lresult = E_NOTIMPL;
					}

					LOG_CHECK_STATE(FAILED(lresult));

					if (!mSessionIsInitialised)
					{
						mListOfStreamsCollectionOfID.push_back(lCollectionOfIDs);
					}
					else
					{
						TOPOID lTopoID(0);

						aPtrSwitcherNode->GetTopoNodeID(&lTopoID);

						mSwitcherCollection[lTopoID] = lCollectionOfIDs;
					}

				} while (false);

				return lresult;
			}
			
			// IMFMediaEventGenerator implementation

			HRESULT STDMETHODCALLTYPE CustomisedMediaSession::GetEvent(
				DWORD dwFlags,
				IMFMediaEvent **ppEvent)
			{
				HRESULT lresult(E_NOTIMPL);

				do
				{

				} while (false);

				return lresult;
			}

			HRESULT STDMETHODCALLTYPE CustomisedMediaSession::BeginGetEvent(
				IMFAsyncCallback* aPtrCallback,
				IUnknown* aPtrUnkState)
			{
				std::unique_lock<std::mutex> lLock(mMutex);

				HRESULT lresult(S_OK);

				do
				{
					if (mMediaSessionSate != MediaSessionSate::SessionShutdowned)
					{
						LOG_INVOKE_MF_METHOD(BeginGetEvent, mEventQueue, aPtrCallback, aPtrUnkState);
					}

				} while (false);

				return lresult;
			}

			HRESULT STDMETHODCALLTYPE CustomisedMediaSession::EndGetEvent(
				IMFAsyncResult* aPtrResult,
				IMFMediaEvent** aPtrPtrEvent)
			{
				std::unique_lock<std::mutex> lLock(mMutex);

				HRESULT lresult(S_OK);

				do
				{
					if (mMediaSessionSate != MediaSessionSate::SessionShutdowned)
					{
						LOG_INVOKE_MF_METHOD(EndGetEvent, mEventQueue, aPtrResult, aPtrPtrEvent);
					}

				} while (false);

				return lresult;
			}

			HRESULT STDMETHODCALLTYPE CustomisedMediaSession::QueueEvent(
				MediaEventType aMediaEventType,
				REFGUID aRefGUIDExtendedType,
				HRESULT aHRESULTStatus,
				const PROPVARIANT* aPtrPropVariantValue)
			{
				std::unique_lock<std::mutex> lLock(mMutex);

				HRESULT lresult(S_OK);

				do
				{
					if (mMediaSessionSate != MediaSessionSate::SessionShutdowned)
					{
						LOG_INVOKE_MF_METHOD(QueueEventParamVar,
							mEventQueue,
							aMediaEventType,
							aRefGUIDExtendedType,
							aHRESULTStatus,
							aPtrPropVariantValue);
					}

				} while (false);

				return lresult;
			}

			// IMFMediaSession implementation

			HRESULT STDMETHODCALLTYPE CustomisedMediaSession::SetTopology(
				DWORD aSetTopologyFlags,
				IMFTopology* aPtrTopology)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrTopology);
					
					LOG_INVOKE_FUNCTION(resolveTopology, aPtrTopology);
					
					CComPtrCustom<IMFTopology> lTopology;

					lTopology = aPtrTopology;

					mTopologyQueue.push(lTopology);

					LOG_INVOKE_FUNCTION(queueTopologyReadyEvent,
						aPtrTopology,
						lresult);
					
					if (!mSessionIsInitialised && !mCurrentTopology)
					{
						mCurrentTopology = mTopologyQueue.front();

						mTopologyQueue.pop();
					}

				} while (false);

				return lresult;
			}

			HRESULT STDMETHODCALLTYPE CustomisedMediaSession::ClearTopologies()
			{
				HRESULT lresult(E_NOTIMPL);

				do
				{

				} while (false);

				return lresult;
			}

			HRESULT CustomisedMediaSession::checkInitBarier(IMFMediaStream* aPtrMediaStream)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					std::unique_lock<std::mutex> lLock(mInitBarrierMutex);
										
					if (mInitBarrierCount > 0)
					{
						auto lfineIter = std::find(mMediaStreamList.begin(),
							mMediaStreamList.end(),
							aPtrMediaStream);

						if (lfineIter == mMediaStreamList.end())
						{
							--mInitBarrierCount;

							mMediaStreamList.push_back(aPtrMediaStream);
						}

						if (mInitBarrierCount == 0)
							mInitBarrierCV.notify_all();
						else
							mInitBarrierCV.wait(lLock, [this] { return mInitBarrierCount <= 0; });

						lresult = S_OK;
					}
					else if (mInitBarrierCount == 0)
					{
						lresult = S_OK;
					}
					else if (mInitBarrierCount < 0)
					{
						lresult = E_NOTFOUND;
					}

				} while (false);

				return lresult;
			}

			HRESULT CustomisedMediaSession::checkFinishBarier()
			{
				HRESULT lresult;

				do
				{
					std::lock_guard<std::mutex> lLock(mFinishBarierMutex);	
					
					if (mFinishBarierState)
					{
						lresult = E_ABORT;
					}
					else
					{
						lresult = S_OK;
					}

				} while (false);

				return lresult;
			}
			

			HRESULT CustomisedMediaSession::resetTargetNode(
				IMFTopologyNode* aPtrUpStreamTopologyNode,
				IMFMediaType* aPtrUpStreamMediaType,
				ITopologyResolver* aPtrTopologyResolver)
			{
				HRESULT lresult = S_OK;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrUpStreamTopologyNode);

					LOG_CHECK_PTR_MEMORY(aPtrTopologyResolver);

					LOG_CHECK_PTR_MEMORY(aPtrUpStreamMediaType);

					CComPtrCustom<IMFTopologyNode> lDownStreamTopologyNode;
					
					DWORD lOutputIndex;

					LOG_INVOKE_MF_METHOD(GetOutput, 
						aPtrUpStreamTopologyNode,
						0,
						&lDownStreamTopologyNode,
						&lOutputIndex);

					CComPtrCustom<IMFTopologyNode> lDownTargetStreamNode;

					if (lDownStreamTopologyNode)
					{
						LOG_INVOKE_MF_METHOD(DisconnectOutput, aPtrUpStreamTopologyNode, 0);

						getTargetNode(lDownStreamTopologyNode, &lDownTargetStreamNode);
					}

					if (lDownTargetStreamNode)
					{
						aPtrUpStreamTopologyNode->ConnectOutput(0, lDownTargetStreamNode, lOutputIndex);
					}

					CComPtrCustom<ITopologyResolver> lTopologyResolver;

					LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrTopologyResolver, &lTopologyResolver);

					LOG_CHECK_PTR_MEMORY(lTopologyResolver);

					CollectionOfIDs lCollectionOfIDs;

					LOG_INVOKE_POINTER_METHOD(lTopologyResolver, resolveDownStreamConnect,
						mCurrentTopology,
						lCollectionOfIDs,
						aPtrUpStreamTopologyNode,
						aPtrUpStreamMediaType,
						lDownTargetStreamNode,
						0);
					
				} while (false);

				return lresult;
			}


			LONG CustomisedMediaSession::getInitBarierCount()
			{
				return mInitBarrierCount;
			}

			HRESULT CustomisedMediaSession::notifyToBeginStreaming(
				IMFTopology* aPtrTopology)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrTopology);

					WORD lTopoCount;

					LOG_INVOKE_MF_METHOD(GetNodeCount,
						aPtrTopology,
						&lTopoCount);
					
					for (WORD lTopoNodeIndex = 0; lTopoNodeIndex < lTopoCount; lTopoNodeIndex++)
					{
						CComPtrCustom<IMFTopologyNode> lTopologyNode;

						LOG_INVOKE_MF_METHOD(GetNode,
							aPtrTopology,
							lTopoNodeIndex,
							&lTopologyNode);
						
						MF_TOPOLOGY_TYPE lTopoNodeType;

						LOG_INVOKE_MF_METHOD(GetNodeType,
							lTopologyNode,
							&lTopoNodeType);
						
						switch (lTopoNodeType)
						{
						case MF_TOPOLOGY_OUTPUT_NODE:
							break;
						case MF_TOPOLOGY_SOURCESTREAM_NODE:
							break;
						case MF_TOPOLOGY_TRANSFORM_NODE:
						{

														   CComQIPtrCustom<IMFTransform> lTransform;

														   CComPtrCustom<IUnknown> lUnkTransform;

														  LOG_INVOKE_MF_METHOD(GetObject,
															   lTopologyNode,
															   &lUnkTransform);
														  
														   lTransform = lUnkTransform;

														   LOG_CHECK_PTR_MEMORY(lTransform);

														   LOG_INVOKE_MF_METHOD(ProcessMessage, lTransform, MFT_MESSAGE_NOTIFY_BEGIN_STREAMING, NULL);
						}
							break;
						case MF_TOPOLOGY_TEE_NODE:
							break;
						case MF_TOPOLOGY_MAX:
							break;
						default:
							break;
						}
					}
					
				} while (false);
				
				return lresult;

			}
			
			HRESULT CustomisedMediaSession::notifyToEndStreaming(
				IMFTopology* aPtrTopology)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrTopology);
					
					WORD lTopoCount;

					LOG_INVOKE_MF_METHOD(GetNodeCount,
						aPtrTopology,
						&lTopoCount);
					
					for (WORD lTopoNodeIndex = 0; lTopoNodeIndex < lTopoCount; lTopoNodeIndex++)
					{
						CComPtrCustom<IMFTopologyNode> lTopologyNode;

						LOG_INVOKE_MF_METHOD(GetNode,
							aPtrTopology,
							lTopoNodeIndex,
							&lTopologyNode);

						if (lTopologyNode)
							notifyToEndStreaming(lTopologyNode);
					}

				} while (false);

				return lresult;
			}

			HRESULT CustomisedMediaSession::notifyToEndStreaming(
					IMFTopologyNode* aPtrTopologyNode)
				{
					HRESULT lresult;

					do
					{
						LOG_CHECK_PTR_MEMORY(aPtrTopologyNode);

						MF_TOPOLOGY_TYPE lTopoNodeType;

						LOG_INVOKE_MF_METHOD(GetNodeType,
							aPtrTopologyNode,
							&lTopoNodeType);

						switch (lTopoNodeType)
						{
						case MF_TOPOLOGY_OUTPUT_NODE:
							break;
						case MF_TOPOLOGY_SOURCESTREAM_NODE:
							break;
						case MF_TOPOLOGY_TRANSFORM_NODE:
						{

							CComQIPtrCustom<IMFTransform> lTransform;

							CComPtrCustom<IUnknown> lUnkTransform;

							LOG_INVOKE_MF_METHOD(GetObject,
								aPtrTopologyNode,
								&lUnkTransform);

							lTransform = lUnkTransform;

							LOG_CHECK_PTR_MEMORY(lTransform);

							LOG_INVOKE_MF_METHOD(ProcessMessage, lTransform, MFT_MESSAGE_NOTIFY_END_OF_STREAM, NULL);

							LOG_INVOKE_MF_METHOD(ProcessMessage, lTransform, MFT_MESSAGE_COMMAND_DRAIN, NULL);

							while (true)
							{
								MFT_OUTPUT_DATA_BUFFER loutputDataBuffer;

								LOG_INVOKE_FUNCTION(initOutputDataBuffer,
									lTransform,
									loutputDataBuffer);

								do
								{

									DWORD lprocessOutputStatus = 0;

									LOG_INVOKE_MF_METHOD(ProcessOutput, lTransform,
										0,
										1,
										&loutputDataBuffer,
										&lprocessOutputStatus);
								} while (false);

								if (loutputDataBuffer.pSample != nullptr)
									loutputDataBuffer.pSample->Release();

								if (lresult == MF_E_TRANSFORM_NEED_MORE_INPUT)
								{
									lresult = S_OK;

									break;
								}
							}

							LOG_INVOKE_MF_METHOD(ProcessMessage, lTransform, MFT_MESSAGE_NOTIFY_END_STREAMING, NULL);

						}
						break;
						case MF_TOPOLOGY_TEE_NODE:
							break;
						case MF_TOPOLOGY_MAX:
							break;
						default:
							break;
						}

					} while (false);

					return lresult;
				}

			HRESULT STDMETHODCALLTYPE CustomisedMediaSession::Start(
				const GUID* aConstPtrGUIDTimeFormat,
				const PROPVARIANT* aConstPtrVarStartPosition)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					std::lock_guard<std::mutex> lAccesseLock(mAccesseMutex);

					{
						std::lock_guard<std::mutex> lLock(mFinishBarierMutex);

						mFinishBarierState = false;
					}

					if (!mSessionIsInitialised)
					{

						mSessionIsInitialised = true;

						notifyToBeginStreaming(mCurrentTopology);

						if (mListOfMediaPipelineProcessors.empty())
						{

							CComPtrCustom<IMFPresentationTimeSource> lTimeSource;

							LOG_INVOKE_MF_FUNCTION(MFCreateSystemTimeSource, &lTimeSource);
							
							LOG_CHECK_PTR_MEMORY(lTimeSource);
							
							LOG_INVOKE_MF_METHOD(SetTimeSource,
								mClock,
								lTimeSource);
							
							for (auto lItem : mListOfStreamsCollectionOfID)
							{
								CComPtrCustom<IMediaPipeline> lMediaPipelineProcessor;

								LOG_INVOKE_FUNCTION(
									CustomisedMediaPipelineProcessorFactory::createCustomisedMediaPipelineProcessor,
									this,
									lItem,
									mCurrentTopology,
									mClock,
									&lMediaPipelineProcessor);
								
								mListOfMediaPipelineProcessors.push_back(lMediaPipelineProcessor);
							}

							if (FAILED(lresult))
							{
								break;
							}
						}


						for (auto& litem : mListOfMediaPipelineProcessors)
						{
							LOG_INVOKE_POINTER_METHOD(litem,startSources,
								aConstPtrGUIDTimeFormat,
								aConstPtrVarStartPosition);

						}

						if (FAILED(lresult))
						{
							auto ltempResult = lresult;

							for (auto& litem : mListOfMediaPipelineProcessors)
							{
								do
								{
									LOG_INVOKE_POINTER_METHOD(litem, stopSources);
								} while (false);
							}

							lresult = ltempResult;

							break;
						}

						LOG_INVOKE_MF_METHOD(Start,
							mClock,
							mClockStartOffset);						
					}
					else
					{

						for (auto& litem : mListOfMediaPipelineProcessors)
						{
							LOG_INVOKE_POINTER_METHOD(litem, startSources,
								aConstPtrGUIDTimeFormat,
								aConstPtrVarStartPosition);							
						}
						
						mInitBarrierCount = mMediaStreamList.size();

						mMediaStreamList.clear();

						if (FAILED(lresult))
						{
							auto ltempResult = lresult;

							for (auto& litem : mListOfMediaPipelineProcessors)
							{
								do
								{
									LOG_INVOKE_POINTER_METHOD(litem, stopSources);
								} while (false);
							}

							lresult = ltempResult;

							break;
						}
						
						LOG_INVOKE_MF_METHOD(Start,
							mClock,
							mClockStartOffset);
					}

					if (FAILED(lresult))
					{
						break;
					}

					mMediaSessionSate = MediaSessionSate::SessionStarted;

				} while (false);

				if (FAILED(lresult))
				{
					auto lErrorCode = lresult;

					mSessionIsInitialised = false;

					do
					{
						CComPtrCustom<IMFMediaEvent> lSessionEvent;

						LOG_INVOKE_MF_FUNCTION(MFCreateMediaEvent,
							MEError,
							GUID_NULL,
							lresult,
							nullptr,
							&lSessionEvent);

						LOG_INVOKE_MF_METHOD(QueueEvent, mEventQueue, lSessionEvent);

					} while (false);

					lresult = lErrorCode;
				}
				else
				{
					do
					{						
						CComPtrCustom<IMFMediaEvent> lSessionEvent;

						LOG_INVOKE_MF_FUNCTION(MFCreateMediaEvent,
							MESessionStarted,
							GUID_NULL,
							lresult,
							nullptr,
							&lSessionEvent);
						
						LOG_INVOKE_MF_METHOD(QueueEvent, mEventQueue, lSessionEvent);
						
					} while (false);
				
				}

				return lresult;
			}

			HRESULT STDMETHODCALLTYPE CustomisedMediaSession::Pause()
			{
				HRESULT lresult;

				do
				{
					std::lock_guard<std::mutex> lAccesseLock(mAccesseMutex);

					{
						std::lock_guard<std::mutex> lLock(mFinishBarierMutex);

						mFinishBarierState = false;
					}

					if (mMediaSessionSate == MediaSessionSate::SessionPaused)
					{
						lresult = S_OK;

						break;
					}					
					
					if (mMediaSessionSate != MediaSessionSate::SessionStarted)
					{
						lresult = S_FALSE;

						break;
					}

					mMediaSessionSate = MediaSessionSate::SessionPaused;
					
					for (auto& lItem : mListOfMediaPipelineProcessors)
					{
						lresult = lItem->pauseSources();

						if (FAILED(lresult))
							continue;
					}

					lresult = mClock->Pause();

					if (lresult == 0xc00d9c42)
					{
						lresult = S_OK;
					}

					if (FAILED(lresult))
					{
						break;
					}

					for (auto& lItem : mListOfMediaPipelineProcessors)
					{
						lItem->finishStreamSinkPausing();
					}
										
					mClockStartOffset = PRESENTATION_CURRENT_POSITION;

				} while (false);


				if (FAILED(lresult))
				{

				}
				else
				{
					do
					{
						CComPtrCustom<IMFMediaEvent> lSessionEvent;

						LOG_INVOKE_MF_FUNCTION(MFCreateMediaEvent,
							MESessionPaused,
							GUID_NULL,
							lresult,
							nullptr,
							&lSessionEvent);
						
						LOG_INVOKE_MF_METHOD(QueueEvent, mEventQueue, lSessionEvent);

					} while (false);

				}

								
				return lresult;
			}

			HRESULT STDMETHODCALLTYPE CustomisedMediaSession::Stop()
			{
				HRESULT lresult;

				do
				{
					std::lock_guard<std::mutex> lAccesseLock(mAccesseMutex);

					if (!mSessionIsInitialised)
					{
						lresult = S_FALSE;

						break;
					}

					{
						std::lock_guard<std::mutex> lLock(mFinishBarierMutex);

						mFinishBarierState = true;
					}

					if (mMediaSessionSate != MediaSessionSate::SessionStarted &&
						mMediaSessionSate != MediaSessionSate::SessionPaused)
					{
						lresult = S_FALSE;

						break;
					}

					mMediaSessionSate = MediaSessionSate::SessionStopped;

					mInitBarrierCount = -1;

					for (auto& lItem : mListOfMediaPipelineProcessors)
					{
						lresult = lItem->stopSources();

						if (FAILED(lresult))
							continue;
					}

					lresult = mClock->Stop();

					if (lresult == 0xc00d9c42)
					{
						lresult = S_OK;

						break;
					}

					if (FAILED(lresult))
					{
						break;
					}

					for (auto& lItem : mListOfMediaPipelineProcessors)
					{
						lItem->finishStreamSinkStopping();
					}

					mClockStartOffset = 0;
					
					CComPtrCustom<IMFMediaEvent> lSessionEvent;

					LOG_INVOKE_MF_FUNCTION(MFCreateMediaEvent,
						MESessionStopped,
						GUID_NULL,
						lresult,
						nullptr,
						&lSessionEvent);
					
					LOG_INVOKE_MF_METHOD(QueueEvent, mEventQueue, lSessionEvent);
																				
				} while (false);
				
				return lresult;
			}

			HRESULT STDMETHODCALLTYPE CustomisedMediaSession::Close()
			{
				HRESULT lresult;

				do
				{
					std::lock_guard<std::mutex> lAccesseLock(mAccesseMutex);

					if (mMediaSessionSate == MediaSessionSate::SessionClosed)
					{
						lresult = S_OK;

						break;
					}

					mMediaSessionSate = MediaSessionSate::SessionClosed;
					
					{
						std::lock_guard<std::mutex> lLock(mFinishBarierMutex);

						mFinishBarierState = true;
					}

					if (mSessionIsInitialised)
						notifyToEndStreaming(mCurrentTopology);

					lresult = S_OK;

				} while (false);

				{
					do
					{
						CComPtrCustom<IMFMediaEvent> lSessionEvent;

						LOG_INVOKE_MF_FUNCTION(MFCreateMediaEvent,
							MESessionClosed,
							GUID_NULL,
							lresult,
							nullptr,
							&lSessionEvent);
						
						LOG_INVOKE_MF_METHOD(QueueEvent, mEventQueue, lSessionEvent);
						
					} while (false);

				}
				
				return lresult;
			}

			HRESULT STDMETHODCALLTYPE CustomisedMediaSession::Shutdown()
			{
				HRESULT lresult;

				do
				{
					std::lock_guard<std::mutex> lAccesseLock(mAccesseMutex);

					if (mMediaSessionSate == MediaSessionSate::SessionShutdowned)
					{
						lresult = S_OK;

						break;
					}

					mMediaSessionSate = MediaSessionSate::SessionShutdowned;
										
					CComPtrCustom<IMFCollection> lSourceNodeCollection;

					LOG_INVOKE_MF_METHOD(GetSourceNodeCollection, mCurrentTopology, &lSourceNodeCollection);
					
					LOG_CHECK_PTR_MEMORY(lSourceNodeCollection);
					
					DWORD lSourceNodeCount = 0;

					LOG_INVOKE_MF_METHOD(GetElementCount, lSourceNodeCollection, &lSourceNodeCount);
					
					for (DWORD lSourceIndex = 0; lSourceIndex < lSourceNodeCount; lSourceIndex++)
					{
						CComPtrCustom<IUnknown> lElement;

						LOG_INVOKE_MF_METHOD(GetElement, lSourceNodeCollection, lSourceIndex, &lElement);
						
						CComQIPtrCustom<IMFTopologyNode> lSourceTopologyNode = lElement;

						LOG_CHECK_PTR_MEMORY(lSourceTopologyNode);
												
						CComPtrCustom<IUnknown> lMediaSourceUnknown;

						LOG_INVOKE_MF_METHOD(GetUnknown,
							lSourceTopologyNode,
							MF_TOPONODE_SOURCE,
							__uuidof(IMFMediaSource),
							(void**)&lMediaSourceUnknown);

						LOG_CHECK_PTR_MEMORY(lMediaSourceUnknown);
						
						CComQIPtrCustom<IMFMediaSource> lMediaSource = lMediaSourceUnknown;

						LOG_CHECK_PTR_MEMORY(lMediaSource);
																		
						LOG_INVOKE_MF_METHOD(Shutdown, lMediaSource);					
					}	
					
					CComPtrCustom<IMFCollection> lOutputNodeCollection;

					LOG_INVOKE_MF_METHOD(GetOutputNodeCollection, mCurrentTopology, &lOutputNodeCollection);
					
					LOG_CHECK_PTR_MEMORY(lOutputNodeCollection);
					
					DWORD lSinkNodeCount = 0;

					lOutputNodeCollection->GetElementCount(&lSinkNodeCount);
																									
					for (DWORD lSinkIndex = 0; lSinkIndex < lSinkNodeCount; lSinkIndex++)
					{
						CComPtrCustom<IUnknown> lElement;

						LOG_INVOKE_MF_METHOD(GetElement, lOutputNodeCollection, lSinkIndex, &lElement);
						
						CComQIPtrCustom<IMFTopologyNode> lOutputTopologyNode = lElement;

						LOG_CHECK_PTR_MEMORY(lOutputTopologyNode);
						
						CComPtrCustom<IUnknown> lActivateUnknown;

						LOG_INVOKE_MF_METHOD(GetObject,
							lOutputTopologyNode,
							&lActivateUnknown);
						
						LOG_CHECK_PTR_MEMORY(lActivateUnknown);
						
						CComQIPtrCustom<IMFActivate> lSinkActivate = lActivateUnknown;

						LOG_CHECK_PTR_MEMORY(lSinkActivate);
						
						LOG_INVOKE_POINTER_METHOD(lSinkActivate, ShutdownObject);
					}
					
					if (mEventQueue)
					{
						LOG_INVOKE_MF_METHOD(Shutdown, mEventQueue);

						mEventQueue.Release();
					}

					CComQIPtrCustom<IPresentationClock>  lClock;

					lClock = mClock;

					if (lClock)
						lClock->shutdown();

					mSessionIsInitialised = false;

				} while (false);

				return lresult;
			}

			HRESULT STDMETHODCALLTYPE CustomisedMediaSession::GetClock(
				IMFClock** aPtrPtrClock)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrPtrClock);

					LOG_CHECK_PTR_MEMORY(mClock);
					
					LOG_INVOKE_QUERY_INTERFACE_METHOD(mClock, aPtrPtrClock);
					
				} while (false);

				return lresult;
			}

			HRESULT STDMETHODCALLTYPE CustomisedMediaSession::GetSessionCapabilities(
				DWORD *pdwCaps)
			{
				HRESULT lresult;

				do
				{
					lresult = E_NOTIMPL;
				} while (false);

				return lresult;
			}

			HRESULT STDMETHODCALLTYPE CustomisedMediaSession::GetFullTopology(
				DWORD dwGetFullTopologyFlags,
				TOPOID TopoId,
				IMFTopology **aPtrPtrFullTopology)
			{
				HRESULT lresult;

				do
				{

					LOG_CHECK_PTR_MEMORY(aPtrPtrFullTopology);

					LOG_INVOKE_MF_FUNCTION(MFCreateTopology, aPtrPtrFullTopology);

					LOG_CHECK_PTR_MEMORY(*aPtrPtrFullTopology);

					LOG_INVOKE_POINTER_METHOD((*aPtrPtrFullTopology), CloneFrom, mCurrentTopology);

				} while (false);

				return lresult;
			}
						
			HRESULT CustomisedMediaSession::resolveTopology(IMFTopology* aPtrTopology)
			{
				HRESULT lresult;

				do
				{

					LOG_CHECK_PTR_MEMORY(aPtrTopology);
					
					CComPtrCustom<IMFCollection> lSourceNodeCollection;		

					LOG_INVOKE_MF_METHOD(GetSourceNodeCollection, aPtrTopology, &lSourceNodeCollection);
				
					LOG_CHECK_PTR_MEMORY(lSourceNodeCollection);
					
					DWORD lSourceNodeCount = 0;

					LOG_INVOKE_MF_METHOD(GetElementCount, lSourceNodeCollection, &lSourceNodeCount);
					
					mInitBarrierCount = lSourceNodeCount;

					std::list<TOPOID> lListOfUsedSources;

					for (DWORD lSourceIndex = 0; lSourceIndex < lSourceNodeCount; lSourceIndex++)
					{
						CComPtrCustom<IUnknown> lElement;

						LOG_INVOKE_MF_METHOD(GetElement, lSourceNodeCollection, lSourceIndex, &lElement);
												
						CComQIPtrCustom<IMFTopologyNode> lSourceTopologyNode = lElement;

						LOG_CHECK_PTR_MEMORY(lSourceTopologyNode);
						
						TOPOID lSourceTopoID;

						LOG_INVOKE_MF_METHOD(GetTopoNodeID,
							lSourceTopologyNode,
							&lSourceTopoID);
						
						auto lFindIter = std::find(
							lListOfUsedSources.begin(),
							lListOfUsedSources.end(),
							lSourceTopoID);

						if (lFindIter != lListOfUsedSources.end())
						{
							continue;
						}
																								
						CComPtrCustom<IUnknown> lUnknown;

						LOG_INVOKE_MF_METHOD(GetUnknown,
							lSourceTopologyNode,
							MF_TOPONODE_STREAM_DESCRIPTOR,
							__uuidof(IMFStreamDescriptor),
							(void**)&lUnknown);

						LOG_CHECK_PTR_MEMORY(lUnknown);
						
						CComQIPtrCustom<IMFStreamDescriptor> lStreamDescriptor = lUnknown;

						LOG_CHECK_PTR_MEMORY(lStreamDescriptor);
						
						CComPtrCustom<IMFMediaTypeHandler> lMediaTypeHandler;

						LOG_INVOKE_MF_METHOD(GetMediaTypeHandler,
							lStreamDescriptor,
							&lMediaTypeHandler);
						
						LOG_CHECK_PTR_MEMORY(lMediaTypeHandler);
						
						CComPtrCustom<IMFMediaType> lOutputMediaType;

						LOG_INVOKE_MF_METHOD(GetCurrentMediaType,
							lMediaTypeHandler,
							&lOutputMediaType);
						
						LOG_CHECK_PTR_MEMORY(lOutputMediaType);
						
						LOG_INVOKE_FUNCTION(resolveStreamTopology,
							aPtrTopology,
							lListOfUsedSources,
							lSourceTopologyNode, 
							lOutputMediaType);
					}
					
				} while (false);

				return lresult;
			}
			
			HRESULT CustomisedMediaSession::resolveStreamTopology(
				IMFTopology* aPtrTopology,
				std::list<TOPOID>& aRefListOfUsedSources,
				IMFTopologyNode* aPtrUpStreamTopologyNode,
				IMFMediaType* aPtrUpStreamMediaType)
			{
				HRESULT lresult;

				do
				{

					LOG_CHECK_PTR_MEMORY(aPtrTopology);
					
					LOG_CHECK_PTR_MEMORY(aPtrUpStreamTopologyNode);

					LOG_CHECK_PTR_MEMORY(aPtrUpStreamMediaType);
					
					DWORD lOutputNodeCount = 0;

					LOG_INVOKE_MF_METHOD(GetOutputCount,
						aPtrUpStreamTopologyNode,
						&lOutputNodeCount);
					
					LOG_CHECK_STATE_DESCR(lOutputNodeCount != 1, MF_E_TOPO_INVALID_OPTIONAL_NODE);
					
					DWORD lInputIndexOnDownstreamNode;

					CComPtrCustom<IMFTopologyNode> lDownStreamTopologyNode;

					LOG_INVOKE_MF_METHOD(GetOutput,
						aPtrUpStreamTopologyNode,
						0,
						&lDownStreamTopologyNode,
						&lInputIndexOnDownstreamNode);

					GUID lGUIDMajorType;

					LOG_INVOKE_MF_METHOD(GetMajorType,
						aPtrUpStreamMediaType,
						&lGUIDMajorType);
					
					TOPOID lSourceTopoID;

					LOG_INVOKE_MF_METHOD(GetTopoNodeID,
						aPtrUpStreamTopologyNode,
						&lSourceTopoID);
										
					CollectionOfIDs lCollectionOfIDs;

					lCollectionOfIDs.mListOfSourceIDs.push_back(lSourceTopoID);

					if (lGUIDMajorType == MFMediaType_Video)
					{						
						CComPtrCustom<IMFMediaType> lUpStreamMediaType;

						LOG_INVOKE_MF_FUNCTION(MFCreateMediaType, &lUpStreamMediaType);

						LOG_CHECK_PTR_MEMORY(lUpStreamMediaType);

						LOG_INVOKE_MF_METHOD(CopyAllItems,
							aPtrUpStreamMediaType,
							lUpStreamMediaType);
						
						CComPtrCustom<IMFGetService> lGetService;

						do
						{

							LOG_INVOKE_MF_METHOD(GetUnknown,
								aPtrUpStreamTopologyNode,
								MF_TOPONODE_SOURCE,
								IID_PPV_ARGS(&lGetService));

							LOG_CHECK_PTR_MEMORY(lGetService);

							CComPtrCustom<IMFDXGIDeviceManager> lUpStreamDeviceManager;

							LOG_INVOKE_MF_METHOD(GetService, lGetService, CM_DeviceManager, IID_PPV_ARGS(&lUpStreamDeviceManager));

							LOG_CHECK_PTR_MEMORY(lUpStreamDeviceManager);

							LOG_INVOKE_MF_METHOD(SetUnknown, lUpStreamMediaType,
								CM_DeviceManager,
								lUpStreamDeviceManager);

						} while (false);

						LOG_CHECK_PTR_MEMORY(mVideoTopologyResolver);
						
						LOG_INVOKE_POINTER_METHOD(mVideoTopologyResolver, resolveDownStreamConnect,
							aPtrTopology,
							lCollectionOfIDs,
							aPtrUpStreamTopologyNode,
							lUpStreamMediaType,
							lDownStreamTopologyNode,
							0);
					}
					else if (lGUIDMajorType == MFMediaType_Audio)
					{

						LOG_CHECK_PTR_MEMORY(mAudioTopologyResolver);
						
						LOG_INVOKE_POINTER_METHOD(mAudioTopologyResolver, resolveDownStreamConnect,
							aPtrTopology,
							lCollectionOfIDs,
							aPtrUpStreamTopologyNode,
							aPtrUpStreamMediaType,
							lDownStreamTopologyNode,
							0);					
					}
					else
					{
						lresult = E_FAIL;
					}

					for (auto lItem : lCollectionOfIDs.mListOfSourceIDs)
					{
						aRefListOfUsedSources.push_back(lItem);
					}
					
					mListOfStreamsCollectionOfID.push_back(lCollectionOfIDs);
				
				} while (false);

				return lresult;
			}
			
			HRESULT CustomisedMediaSession::queueTopologyReadyEvent(
				IMFTopology* aPtrTopology,
				HRESULT aResult)
			{
				HRESULT lresult = S_OK;

				PROPVARIANT lvariantStatus;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrTopology);
					
					CComPtrCustom<IMFMediaEvent> lEvent;

					PropVariantInit(&lvariantStatus);

					lvariantStatus.vt = VT_UNKNOWN;

					lvariantStatus.punkVal = aPtrTopology;
				
					LOG_INVOKE_MF_FUNCTION(MFCreateMediaEvent,
						MESessionTopologyStatus,    
						GUID_NULL,                  
						aResult,
						&lvariantStatus,             
						&lEvent);
				
					if (FAILED(aResult))
					{
						lresult = lEvent->SetUINT32(MF_EVENT_TOPOLOGY_STATUS, MF_TOPOSTATUS_INVALID);
					}
					else
					{
						lresult = lEvent->SetUINT32(MF_EVENT_TOPOLOGY_STATUS, MF_TOPOSTATUS_READY);
					}
					
					LOG_INVOKE_MF_METHOD(QueueEvent, mEventQueue, lEvent);

				} while (false);

				return lresult;
			}


			HRESULT CustomisedMediaSession::findOutputNodes(
				std::vector<CComPtrCustom<IMFTopologyNode>>& aOutputTopologyNodes,
				IMFTopologyNode* aPtrDownStreamNode)
			{
				HRESULT lresult = S_OK;

				do
				{


					MF_TOPOLOGY_TYPE lTopoNodeType;

					LOG_INVOKE_MF_METHOD(GetNodeType,
						aPtrDownStreamNode,
						&lTopoNodeType);

					switch (lTopoNodeType)
					{
					case MF_TOPOLOGY_OUTPUT_NODE:
					{
						CComPtrCustom<IMFTopologyNode> lOutputNode;

						lOutputNode = aPtrDownStreamNode;

						aOutputTopologyNodes.push_back(lOutputNode);
					}
						break;
					case MF_TOPOLOGY_SOURCESTREAM_NODE:
						break;
					case MF_TOPOLOGY_TRANSFORM_NODE:
					break;
					case MF_TOPOLOGY_TEE_NODE:
						break;
					case MF_TOPOLOGY_MAX:
						break;
					default:
						break;
					}



					DWORD lOutputCount(0);

					LOG_INVOKE_MF_METHOD(GetOutputCount, aPtrDownStreamNode, &lOutputCount);

					for (size_t i = 0; i < lOutputCount; i++)
					{
						CComPtrCustom<IMFTopologyNode> lDownStreamNode;

						DWORD lInputIndexOnDownStreamNode(0);

						LOG_INVOKE_MF_METHOD(GetOutput, aPtrDownStreamNode, i, &lDownStreamNode, &lInputIndexOnDownStreamNode);
						
						findOutputNodes(
							aOutputTopologyNodes,
							lDownStreamNode);
					}


				} while (false);

				return lresult;
			}

			HRESULT CustomisedMediaSession::releaseNode(IMFTopologyNode* aPtrDownStreamNode)
			{
				HRESULT lresult = S_OK;

				do
				{
					
					notifyToEndStreaming(aPtrDownStreamNode);

					DWORD lOutputCount(0);

					LOG_INVOKE_MF_METHOD(GetOutputCount, aPtrDownStreamNode, &lOutputCount);

					for (size_t i = 0; i < lOutputCount; i++)
					{
						CComPtrCustom<IMFTopologyNode> lDownStreamNode;

						DWORD lInputIndexOnDownStreamNode(0);

						LOG_INVOKE_MF_METHOD(GetOutput, aPtrDownStreamNode, i, &lDownStreamNode, &lInputIndexOnDownStreamNode);

						releaseNode(
							lDownStreamNode);
					}
					
					mCurrentTopology->RemoveNode(aPtrDownStreamNode);

				} while (false);

				return lresult;
			}

			HRESULT CustomisedMediaSession::getTargetNode(IMFTopologyNode* aPtrDownStreamNode, IMFTopologyNode** aPtrPtrTargetStreamNode)
			{
				HRESULT lresult = S_OK;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrDownStreamNode);

					UINT32 lState = FALSE;

					lresult = aPtrDownStreamNode->GetUINT32(CM_TARGET_NODE, &lState);

					if (SUCCEEDED(lresult) && lState != FALSE)
					{
						LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrDownStreamNode, aPtrPtrTargetStreamNode);

						break;
					}


					notifyToEndStreaming(aPtrDownStreamNode);

					DWORD lOutputCount(0);

					LOG_INVOKE_MF_METHOD(GetOutputCount, aPtrDownStreamNode, &lOutputCount);

					for (size_t i = 0; i < lOutputCount; i++)
					{
						CComPtrCustom<IMFTopologyNode> lDownStreamNode;

						DWORD lInputIndexOnDownStreamNode(0);

						LOG_INVOKE_MF_METHOD(GetOutput, aPtrDownStreamNode, i, &lDownStreamNode, &lInputIndexOnDownStreamNode);

						lresult = getTargetNode(
							lDownStreamNode, aPtrPtrTargetStreamNode);

						if (SUCCEEDED(lresult))
						{
							break;
						}
					}

					mCurrentTopology->RemoveNode(aPtrDownStreamNode);

					lresult = S_OK;

				} while (false);

				return lresult;
			}
			
			HRESULT CustomisedMediaSession::enumAndAddOutputTopologyNode(
				IMFTopology* aPtrTopology,
				IMFTopologyNode* aPtrTopologyNode)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrTopology);

					LOG_CHECK_PTR_MEMORY(aPtrTopologyNode);

					LOG_INVOKE_MF_METHOD(AddNode,
						aPtrTopology,
						aPtrTopologyNode);

					LOG_INVOKE_MF_METHOD(SetUINT32,
						aPtrTopologyNode,
						CM_TARGET_NODE,
						TRUE);

					DWORD lRefOutputNodeCount;

					LOG_INVOKE_MF_METHOD(GetOutputCount,
						aPtrTopologyNode,
						&lRefOutputNodeCount);

					for (DWORD lIndex = 0; lIndex < lRefOutputNodeCount; lIndex++)
					{
						DWORD lDownstreamIndex = 0;

						CComPtrCustom<IMFTopologyNode> lChildNode;

						LOG_INVOKE_MF_METHOD(GetOutput,
							aPtrTopologyNode,
							lIndex,
							&lChildNode,
							&lDownstreamIndex);

						LOG_CHECK_PTR_MEMORY(lChildNode);

						LOG_INVOKE_FUNCTION(enumAndAddOutputTopologyNode,
							aPtrTopology,
							lChildNode);
					}

				} while (false);

				return lresult;
			}
		}
	}
}