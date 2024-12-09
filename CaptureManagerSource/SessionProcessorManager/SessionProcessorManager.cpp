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

#include "../LogPrintOut/LogPrintOut.h"
#include "SessionProcessorManager.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../Common/MFHeaders.h"
#include "../Common/GUIDs.h"
#include "SessionProcessor.h"
#include "../Common/Common.h"
#include "../CustomisedMediaSession/CustomisedMediaSession.h"
//#include "ISessionCallbackInner.h"
#include "SessionAsyncCallback.h"


namespace CaptureManager
{
	namespace Core
	{

		SessionProcessorManager::SessionProcessorManager():
			mSessionAsyncCallback(new (std::nothrow) SessionAsyncCallback())
		{
		}
		
		SessionProcessorManager::~SessionProcessorManager()
		{
			mSessionProcessorDictionary.clear();
		}

		HRESULT SessionProcessorManager::createSession(
			std::vector<IMFTopologyNode*>& aSourceNodes,
			IUnknown* aPtrISessionCallback,
			SessionDescriptor& aRefSessionDescriptor)
		{

			using namespace CaptureManager::MediaSession::CustomisedMediaSession;

			HRESULT lresult;

			do
			{
				aRefSessionDescriptor = -1;

				LOG_CHECK_STATE(aSourceNodes.empty());

				LOG_CHECK_PTR_MEMORY(aPtrISessionCallback);
				
				CComPtrCustom<IMFTopology> lTopology;

				LOG_INVOKE_MF_FUNCTION(MFCreateTopology,
					&lTopology);

				std::map<IUnknown*, IMFTopologyNode*> lSourceNodesMap;

				for (auto& lSourceNode : aSourceNodes)
				{
					CComPtrCustom<IUnknown> lUnkMediaSource;

					do
					{
						LOG_INVOKE_MF_METHOD(GetUnknown,
							lSourceNode,
							MF_TOPONODE_SOURCE,
							IID_PPV_ARGS(&lUnkMediaSource));

					} while (false);				
					
					if (lUnkMediaSource)
					{
						auto lfindIter = lSourceNodesMap.find(lUnkMediaSource);

						if (lfindIter == lSourceNodesMap.end())
							lSourceNodesMap[lUnkMediaSource] = lSourceNode;
						else
						{

							CComPtrCustom<IMFPresentationDescriptor> lBaseSourcePresentationDescriptor;

							LOG_INVOKE_MF_METHOD(GetUnknown,
								(*lfindIter).second,
								MF_TOPONODE_PRESENTATION_DESCRIPTOR,
								IID_PPV_ARGS(&lBaseSourcePresentationDescriptor));


							CComPtrCustom<IMFPresentationDescriptor> lNextSourcePresentationDescriptor;

							LOG_INVOKE_MF_METHOD(GetUnknown,
								lSourceNode,
								MF_TOPONODE_PRESENTATION_DESCRIPTOR,
								IID_PPV_ARGS(&lNextSourcePresentationDescriptor));


							DWORD lStreamDescriptorCount = 0;

							LOG_INVOKE_MF_METHOD(GetStreamDescriptorCount,
								lNextSourcePresentationDescriptor,
								&lStreamDescriptorCount);

							for (size_t lIndexStream = 0; lIndexStream < lStreamDescriptorCount; lIndexStream++)
							{

								CComPtrCustom<IMFStreamDescriptor> lNextSourceSD;

								BOOL lNextSourceIsSelected = FALSE;

								LOG_INVOKE_MF_METHOD(GetStreamDescriptorByIndex,
									lNextSourcePresentationDescriptor,
									lIndexStream,
									&lNextSourceIsSelected,
									&lNextSourceSD);

								CComPtrCustom<IMFStreamDescriptor> lBaseSourceSD;

								BOOL lBaseSourceIsSelected = FALSE;

								LOG_INVOKE_MF_METHOD(GetStreamDescriptorByIndex,
									lBaseSourcePresentationDescriptor,
									lIndexStream,
									&lBaseSourceIsSelected,
									&lBaseSourceSD);															

								if (
									lBaseSourceIsSelected == FALSE
									&& lNextSourceIsSelected != FALSE 
									&& lNextSourceSD
									)
								{


									LOG_INVOKE_FUNCTION(MediaFoundation::MediaFoundationManager::SelectStream,
										lBaseSourcePresentationDescriptor,
										lIndexStream);

									CComQIPtrCustom<IMFTopologyNode> lDownStreamTopologyNode;

									DWORD lInputIndexOnDownStreamNode = 0;
									do
									{
										LOG_INVOKE_MF_METHOD(GetOutput,
											lSourceNode,
											0,
											&lDownStreamTopologyNode,
											&lInputIndexOnDownStreamNode);

										LOG_INVOKE_MF_METHOD(DisconnectOutput,
											lSourceNode,
											0);										

									} while (false);

									if (lDownStreamTopologyNode)
									{
										LOG_INVOKE_MF_METHOD(ConnectOutput,
											(*lfindIter).second,
											lIndexStream,
											lDownStreamTopologyNode,
											lInputIndexOnDownStreamNode);
									}									
								}
							}
						}
					}
				}

				for (auto& lSourceNodePair: lSourceNodesMap)
				{
					LOG_INVOKE_FUNCTION(enumAndAddOutputTopologyNode,
						lTopology, 
						lSourceNodePair.second);
				}

				LOG_CHECK_STATE_DESCR(FAILED(lresult), lresult);
				
				CComPtrCustom<IMFMediaSession> lSession;

				lSession = new (std::nothrow)CustomisedMediaSession();

				LOG_CHECK_PTR_MEMORY(lSession);
					
				CComQIPtrCustom<ISessionCallbackInner> lISessionCallback;

				lISessionCallback = aPtrISessionCallback;

				CComQIPtrCustom<IMFAsyncCallback> lSessionAsyncCallback = mSessionAsyncCallback;

				CComPtrCustom<SessionProcessor> lSessionProcessor(
					new (std::nothrow) SessionProcessor(
					lSession, 
					mNextSessionDescriptor, 
					lISessionCallback,
					lSessionAsyncCallback));

				LOG_CHECK_PTR_MEMORY(lSessionProcessor);
				
				LOG_INVOKE_MF_METHOD(BeginGetEvent,
					lSession, 
					lSessionProcessor, 
					nullptr);

				LOG_INVOKE_POINTER_METHOD(lSessionProcessor, 
					setTopology,
					lTopology);
				
				aRefSessionDescriptor = mNextSessionDescriptor;

				CComPtrCustom<IUnknown> lUnk;

				LOG_INVOKE_QUERY_INTERFACE_METHOD(lSessionProcessor, &lUnk);

				mSessionProcessorDictionary[mNextSessionDescriptor++] = lUnk;

			} while (false);

			return lresult;
		}

		// Stop all devices
		HRESULT SessionProcessorManager::closeAllSessions()
		{
			HRESULT lresult(S_OK);

			std::map<SessionDescriptor, CComPtrCustom<IUnknown>>::iterator itr = mSessionProcessorDictionary.begin();

			for (; itr != mSessionProcessorDictionary.end(); itr++)
			{
				CComQIPtrCustom<ISessionProcessor> lSessionProcessor;

				lSessionProcessor = (*itr).second;

				if (lSessionProcessor)
				{
					auto resultStop = lSessionProcessor->stopCapture();

					resultStop = lSessionProcessor->close();

					if (FAILED(resultStop))
						lresult = resultStop;
				}
			}

			mSessionProcessorDictionary.clear();
			
			return lresult;
		}

		HRESULT SessionProcessorManager::closeSession(
			SessionDescriptor& aRefSessionDescriptor)
		{
			HRESULT lresult(MF_E_OUT_OF_RANGE);

			std::map<SessionDescriptor, CComPtrCustom<IUnknown>>::iterator itr = mSessionProcessorDictionary.find(aRefSessionDescriptor);

			if(itr != mSessionProcessorDictionary.end())
			{
				CComQIPtrCustom<ISessionProcessor> lSessionProcessor;

				lSessionProcessor = (*itr).second;

				if (lSessionProcessor)
				{
					lresult = S_OK;

					auto resultStop = lSessionProcessor->stopCapture();

					resultStop = lSessionProcessor->close();

					if (FAILED(resultStop))
						lresult = resultStop;
				}

				mSessionProcessorDictionary.erase(itr);
			}
					
			return lresult;
		}

		HRESULT SessionProcessorManager::startSession(
			SessionDescriptor& aRefSessionDescriptor, 
			LONGLONG aStartPosition, 
			const GUID aGUIDTimeFormat)
		{
			HRESULT lresult(MF_E_OUT_OF_RANGE);

			std::map<SessionDescriptor, CComPtrCustom<IUnknown>>::iterator itr = mSessionProcessorDictionary.find(aRefSessionDescriptor);

			if (itr != mSessionProcessorDictionary.end())
			{
				PROPVARIANT var;

				PropVariantInit(&var);

				if (aStartPosition != 0)
					var.hVal.QuadPart = aStartPosition;


				CComQIPtrCustom<ISessionProcessor> lSessionProcessor;

				lSessionProcessor = (*itr).second;

				if (lSessionProcessor)
				{
					lresult = lSessionProcessor->startCapture(aGUIDTimeFormat, &var);
				}

				PropVariantClear(&var);
			}

			if (FAILED(lresult))
			{
				mSessionProcessorDictionary.erase(itr);
			}
			
			return lresult;
		}

		HRESULT SessionProcessorManager::pauseSession(
			SessionDescriptor& aRefSessionDescriptor)
		{
			HRESULT lresult(MF_E_OUT_OF_RANGE);

			std::map<SessionDescriptor, CComPtrCustom<IUnknown>>::iterator itr = mSessionProcessorDictionary.find(aRefSessionDescriptor);

			if (itr != mSessionProcessorDictionary.end())
			{
				CComQIPtrCustom<ISessionProcessor> lSessionProcessor;

				lSessionProcessor = (*itr).second;

				if (lSessionProcessor)
				{
					lresult = lSessionProcessor->pauseCapture();
				}
			}
			
			return lresult;
		}

		HRESULT SessionProcessorManager::stopSession(
			SessionDescriptor& aRefSessionDescriptor)
		{
			HRESULT lresult(MF_E_OUT_OF_RANGE);

			std::map<SessionDescriptor, CComPtrCustom<IUnknown>>::iterator itr = mSessionProcessorDictionary.find(aRefSessionDescriptor);

			if (itr != mSessionProcessorDictionary.end())
			{
				CComQIPtrCustom<ISessionProcessor> lSessionProcessor;

				lSessionProcessor = (*itr).second;

				if (lSessionProcessor)
				{
					lresult = lSessionProcessor->stopCapture();
				}
			}
			
			return lresult;
		}
		
		HRESULT SessionProcessorManager::pauseSwitchers(
			SessionDescriptor& aRefSessionDescriptor)
		{
			HRESULT lresult(MF_E_OUT_OF_RANGE);

			std::map<SessionDescriptor, CComPtrCustom<IUnknown>>::iterator itr = mSessionProcessorDictionary.find(aRefSessionDescriptor);

			if (itr != mSessionProcessorDictionary.end())
			{
				CComQIPtrCustom<ISessionProcessor> lSessionProcessor;

				lSessionProcessor = (*itr).second;

				if (lSessionProcessor)
				{
					lresult = lSessionProcessor->pauseSwitchers();
				}
			}

			return lresult;
		}
		
		HRESULT SessionProcessorManager::resumeSwitchers(
			SessionDescriptor& aRefSessionDescriptor)
		{
			HRESULT lresult(MF_E_OUT_OF_RANGE);

			std::map<SessionDescriptor, CComPtrCustom<IUnknown>>::iterator itr = mSessionProcessorDictionary.find(aRefSessionDescriptor);

			if (itr != mSessionProcessorDictionary.end())
			{
				CComQIPtrCustom<ISessionProcessor> lSessionProcessor;

				lSessionProcessor = (*itr).second;

				if (lSessionProcessor)
				{
					lresult = lSessionProcessor->resumeSwitchers();
				}
			}

			return lresult;
		}


		HRESULT SessionProcessorManager::detachSwitchers(
			SessionDescriptor& aRefSessionDescriptor)
		{
			HRESULT lresult(MF_E_OUT_OF_RANGE);

			std::map<SessionDescriptor, CComPtrCustom<IUnknown>>::iterator itr = mSessionProcessorDictionary.find(aRefSessionDescriptor);

			if (itr != mSessionProcessorDictionary.end())
			{
				CComQIPtrCustom<ISessionProcessor> lSessionProcessor;

				lSessionProcessor = (*itr).second;

				if (lSessionProcessor)
				{
					lresult = lSessionProcessor->detachSwitchers();
				}
			}

			return lresult;
		}
		
		HRESULT SessionProcessorManager::attachSwitcher(
			/* [in] */ IUnknown *aPtrSwitcherNode,
			/* [in] */ IUnknown *aPtrDownStreamNode)
		{
			HRESULT lresult(MF_E_OUT_OF_RANGE);

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrSwitcherNode);

				LOG_CHECK_PTR_MEMORY(aPtrDownStreamNode);

				CComPtrCustom<IMFTopologyNode> lIMFTopologyNode;

				LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrSwitcherNode, &lIMFTopologyNode);

				LOG_CHECK_PTR_MEMORY(lIMFTopologyNode);

				CComPtrCustom<IMFTopologyNode> lDownStreamTopologyNode;

				LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrDownStreamNode, &lDownStreamTopologyNode);

				LOG_CHECK_PTR_MEMORY(lDownStreamTopologyNode);
				
				for (auto& litem: mSessionProcessorDictionary)
				{
					CComQIPtrCustom<ISessionProcessor> lSessionProcessor;

					lSessionProcessor = litem.second;

					if (lSessionProcessor)
					{
						lresult = lSessionProcessor->checkToplogyNode(lIMFTopologyNode);

						if (SUCCEEDED(lresult))
						{
							lSessionProcessor->attachSwitcher(
								lIMFTopologyNode,
								lDownStreamTopologyNode);						

							break;
						}
					}
				}

				lresult = S_OK;

			} while (false);

			return lresult;
		}
		
		HRESULT SessionProcessorManager::enumAndAddOutputTopologyNode(
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