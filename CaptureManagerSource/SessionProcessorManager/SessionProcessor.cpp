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

#include "SessionProcessor.h"
#include "SessionAsyncCallback.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "MediaSinkFinalizeProcessor.h"
#include "../CustomisedMediaSession/ISessionSwitcherControl.h"


#include <list>


namespace CaptureManager
{
	namespace Core
	{		
		SessionProcessor::SessionProcessor(IMFMediaSession* aPtrMediaSession,
			SessionDescriptor lSessionDescriptor,
			CComPtrCustom<ISessionCallbackInner>& aRefISessionCallback,
			CComPtrCustom<IMFAsyncCallback>& aRefSessionAsyncCallback) :
			mLocalSession(aPtrMediaSession),
			mSessionDescriptor(lSessionDescriptor),
			mISessionCallback(aRefISessionCallback),
			mSessionAsyncCallback(aRefSessionAsyncCallback)
		{
			mSyncWorkerQueue = MFASYNC_CALLBACK_QUEUE_LONG_FUNCTION;
		}
		
		SessionProcessor::~SessionProcessor()
		{
		}

		HRESULT SessionProcessor::close()
		{
			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(mLocalSession);
				
				std::unique_lock<std::mutex> llock(mMutex);

				if (!mIsStarted)
				{
					lresult = S_OK;
				}
				else
				{
					stopCapture();
				}

				LOG_INVOKE_POINTER_METHOD(mLocalSession, Close);
								
				if ((HRESULT)lresult == S_FALSE)
				{
					break;
				}

				auto lconditionResult = mConditionVariable.wait_for(llock, std::chrono::seconds(5));

				if (lconditionResult == std::cv_status::timeout)
				{

					lresult = CONTEXT_E_SYNCH_TIMEOUT;

					LogPrintOut::getInstance().printOutln(
						LogPrintOut::ERROR_LEVEL, 
						L"SessionProcessor: closeSession - time is out!!! ",
						L" Error code: ", 
						(HRESULT)lresult);
				}
				else
				{

					LogPrintOut::getInstance().printOutln(
						LogPrintOut::INFO_LEVEL, 
						L"SessionProcessor: closeSession - session is closed!!! ",
						L" Error code: ", 
						(HRESULT)lresult);
				}

				LOG_INVOKE_MF_METHOD(Shutdown, mLocalSession);

			} while (false);
			
			return lresult;
		}

		HRESULT SessionProcessor::setTopology(
			IMFTopology* aPtrTopology)
		{
			HRESULT lresult;

			do
			{

				LOG_CHECK_PTR_MEMORY(mLocalSession);

				LOG_CHECK_PTR_MEMORY(aPtrTopology);
				
				std::unique_lock<std::mutex> llock(mMutex);

				LOG_INVOKE_MF_METHOD(SetTopology, 
					mLocalSession, 
					0x1,
					aPtrTopology);
												
				auto lconditionResult = mConditionVariable.wait_for(llock, std::chrono::seconds(15));

				if (lconditionResult == std::cv_status::timeout)
				{
					LogPrintOut::getInstance().printOutln(
						LogPrintOut::ERROR_LEVEL, 
						L"SessionProcessor: setTopology - time is out!!! ",
						L" Error code: ", 
						(HRESULT)lresult);

					lresult = CONTEXT_E_SYNCH_TIMEOUT;
				}
				else
				{

					LogPrintOut::getInstance().printOutln(
						LogPrintOut::INFO_LEVEL, 
						L"SessionProcessor: setTopology - topology is ready!!! ",
						L" Error code: ", 
						(HRESULT)lresult);
				}
				
			} while (false);

			return lresult;
		}

		HRESULT SessionProcessor::startCapture(
			REFGUID aGUIDTimeFormat, 
			const PROPVARIANT *aPtrVarStartPosition)
		{
			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(mLocalSession);

				LOG_CHECK_PTR_MEMORY(aPtrVarStartPosition);
				
				std::unique_lock<std::mutex> llock(mMutex);

				do
				{

					do
					{

						LOG_INVOKE_MF_METHOD(Start,
							mLocalSession,
							&aGUIDTimeFormat,
							aPtrVarStartPosition);

					} while (false);
				

					if (SUCCEEDED(lresult) || lresult == 0xc00d36da)
					{
						auto lconditionResult = mConditionVariable.wait_for(llock, std::chrono::seconds(5));

						if (lconditionResult == std::cv_status::timeout)
						{

							lresult = CONTEXT_E_SYNCH_TIMEOUT;

							LogPrintOut::getInstance().printOutln(
								LogPrintOut::ERROR_LEVEL,
								L"SessionProcessor: startCapture - time is out!!! ",
								L" Error code: ",
								(HRESULT)lresult);
						}
						else
						{

							LogPrintOut::getInstance().printOutln(
								LogPrintOut::INFO_LEVEL,
								L"SessionProcessor: startCapture - capture is started!!! ",
								L" Error code: ",
								(HRESULT)lresult);
						}
					}

				} while (false);
				
			} while (false);

			if (FAILED(lresult))
			{
				do
				{
					LogPrintOut::getInstance().printOutln(
						LogPrintOut::INFO_LEVEL,
						L"SessionProcessor: startCapture - cannot be started!!! ",
						L" Error code: ",
						(HRESULT)lresult);

					std::unique_lock<std::mutex> llock(mMutex);

					HRESULT lCloseResult = mLocalSession->Close();

					if ((HRESULT)lCloseResult == S_FALSE)
					{
						break;
					}

						auto lconditionResult = mConditionVariable.wait_for(llock, std::chrono::seconds(5));

					if (lconditionResult == std::cv_status::timeout)
					{

						lCloseResult = CONTEXT_E_SYNCH_TIMEOUT;

						LogPrintOut::getInstance().printOutln(
							LogPrintOut::ERROR_LEVEL,
							L"SessionProcessor: closeSession - time is out!!! ",
							L" Error code: ",
							(HRESULT)lCloseResult);
					}
					else
					{

						LogPrintOut::getInstance().printOutln(
							LogPrintOut::INFO_LEVEL,
							L"SessionProcessor: closeSession - session is closed!!! ",
							L" Error code: ",
							(HRESULT)lCloseResult);
					}

					lCloseResult = mLocalSession->Shutdown();

					if (FAILED(lCloseResult))
					{
						LogPrintOut::getInstance().printOutln(
							LogPrintOut::ERROR_LEVEL,
							__FUNCTIONW__,
							L" Error code: ",
							(HRESULT)lCloseResult);
					}

				} while (false);
			}

			

			return lresult;
		}

		HRESULT SessionProcessor::pauseCapture()
		{
			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(mLocalSession);
				
				std::unique_lock<std::mutex> llock(mMutex);

				LOG_INVOKE_MF_METHOD(Pause,
					mLocalSession);
				
				auto lconditionResult = mConditionVariable.wait_for(llock, std::chrono::seconds(5));

				if (lconditionResult == std::cv_status::timeout)
				{

					lresult = CONTEXT_E_SYNCH_TIMEOUT;

					LogPrintOut::getInstance().printOutln(
						LogPrintOut::ERROR_LEVEL,
						L"SessionProcessor: pauseCapture - time is out!!! ",
						L" Error code: ",
						(HRESULT)lresult);
				}
				else
				{

					LogPrintOut::getInstance().printOutln(
						LogPrintOut::INFO_LEVEL,
						L"SessionProcessor: pauseCapture - capture is executed!!! ",
						L" Error code: ",
						(HRESULT)lresult);
				}
				
			} while (false);

			if (FAILED(lresult))
			{
				do
				{

					std::unique_lock<std::mutex> llock(mMutex);

					HRESULT lCloseResult = mLocalSession->Close();

					if ((HRESULT)lCloseResult == S_FALSE)
					{
						break;
					}

					auto lconditionResult = mConditionVariable.wait_for(llock, std::chrono::seconds(5));

					if (lconditionResult == std::cv_status::timeout)
					{

						lCloseResult = CONTEXT_E_SYNCH_TIMEOUT;

						LogPrintOut::getInstance().printOutln(
							LogPrintOut::ERROR_LEVEL,
							L"SessionProcessor: closeSession - time is out!!! ",
							L" Error code: ",
							(HRESULT)lCloseResult);
					}
					else
					{

						LogPrintOut::getInstance().printOutln(
							LogPrintOut::INFO_LEVEL,
							L"SessionProcessor: closeSession - session is closed!!! ",
							L" Error code: ",
							(HRESULT)lCloseResult);
					}

					lCloseResult = mLocalSession->Shutdown();

					if (FAILED(lCloseResult))
					{
						LogPrintOut::getInstance().printOutln(
							LogPrintOut::ERROR_LEVEL,
							__FUNCTIONW__,
							L" Error code: ",
							(HRESULT)lCloseResult);
					}

				} while (false);
			}



			return lresult;
		}

		HRESULT SessionProcessor::stopCapture()
		{
			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(mLocalSession);

				std::unique_lock<std::mutex> llock(mMutex);
				
				LOG_INVOKE_MF_METHOD(Stop,
					mLocalSession);

				if (lresult == S_FALSE)
					break;
				
				auto lconditionResult = mConditionVariable.wait_for(llock, std::chrono::seconds(15));

				if (lconditionResult == std::cv_status::timeout)
				{

					lresult = CONTEXT_E_SYNCH_TIMEOUT;

					LogPrintOut::getInstance().printOutln(
						LogPrintOut::ERROR_LEVEL, 
						L"SessionProcessor: stopCapture - time is out!!! ",
						L" Error code: ", 
						(HRESULT)lresult);
				}
				else
				{

					LogPrintOut::getInstance().printOutln(
						LogPrintOut::INFO_LEVEL, 
						L"SessionProcessor: stopCapture - capture is stopped!!! ",
						L" Error code: ", 
						(HRESULT)lresult);
				}

				mIsStarted = false;
				
			} while (false);

			return lresult;
		}
		
		HRESULT SessionProcessor::pauseSwitchers()
		{
			using namespace MediaSession::CustomisedMediaSession;

			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(mLocalSession);

				CComPtrCustom<ISessionSwitcherControl> lISessionSwitcherControl;

				std::unique_lock<std::mutex> llock(mMutex);

				LOG_INVOKE_QUERY_INTERFACE_METHOD(mLocalSession, &lISessionSwitcherControl);

				LOG_CHECK_PTR_MEMORY(lISessionSwitcherControl);

				LOG_INVOKE_POINTER_METHOD(lISessionSwitcherControl, pauseSwitchers);


			} while (false);

			return lresult;
		}

		HRESULT SessionProcessor::resumeSwitchers()
		{
			using namespace MediaSession::CustomisedMediaSession;

			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(mLocalSession);

				CComPtrCustom<ISessionSwitcherControl> lISessionSwitcherControl;

				std::unique_lock<std::mutex> llock(mMutex);

				LOG_INVOKE_QUERY_INTERFACE_METHOD(mLocalSession, &lISessionSwitcherControl);

				LOG_CHECK_PTR_MEMORY(lISessionSwitcherControl);

				LOG_INVOKE_POINTER_METHOD(lISessionSwitcherControl, resumeSwitchers);


			} while (false);

			return lresult;
		}

		HRESULT SessionProcessor::detachSwitchers()
		{
			using namespace MediaSession::CustomisedMediaSession;

			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(mLocalSession);

				CComPtrCustom<ISessionSwitcherControl> lISessionSwitcherControl;

				std::unique_lock<std::mutex> llock(mMutex);

				LOG_INVOKE_QUERY_INTERFACE_METHOD(mLocalSession, &lISessionSwitcherControl);

				LOG_CHECK_PTR_MEMORY(lISessionSwitcherControl);

				LOG_INVOKE_POINTER_METHOD(lISessionSwitcherControl, detachSwitchers);


			} while (false);

			return lresult;
		}
				
		HRESULT SessionProcessor::checkToplogyNode(IMFTopologyNode* aPtrTopologyNode)
		{
			HRESULT lresult(E_FAIL);

			do
			{

				LOG_CHECK_PTR_MEMORY(mLocalSession);

				LOG_CHECK_PTR_MEMORY(aPtrTopologyNode);

				TOPOID lTopoID(0);

				aPtrTopologyNode->GetTopoNodeID(&lTopoID);

				CComPtrCustom<IMFTopology> lFullTopology;

				mLocalSession->GetFullTopology(MFSESSION_GETFULLTOPOLOGY_CURRENT, 0, &lFullTopology);

				CComPtrCustom<IMFTopologyNode> lFindTopologyNode;

				lresult = lFullTopology->GetNodeByID(lTopoID, &lFindTopologyNode);

			} while (false);

			return lresult;
		}
		
		HRESULT SessionProcessor::attachSwitcher(
			/* [in] */ IMFTopologyNode *aPtrSwitcherNode,
			/* [in] */ IMFTopologyNode *aPtrDownStreamNode)
		{
			using namespace MediaSession::CustomisedMediaSession;

			HRESULT lresult(E_FAIL);

			do
			{

				LOG_CHECK_PTR_MEMORY(mLocalSession);

				LOG_CHECK_PTR_MEMORY(aPtrSwitcherNode);

				LOG_CHECK_PTR_MEMORY(aPtrDownStreamNode);

				CComPtrCustom<ISessionSwitcherControl> lISessionSwitcherControl;

				LOG_INVOKE_QUERY_INTERFACE_METHOD(mLocalSession, &lISessionSwitcherControl);

				LOG_CHECK_PTR_MEMORY(lISessionSwitcherControl);

				LOG_INVOKE_POINTER_METHOD(lISessionSwitcherControl, attachSwitcher,
					/* [in] */ aPtrSwitcherNode,
					/* [in] */ aPtrDownStreamNode);

			} while (false);

			return lresult;
		}
				
		HRESULT SessionProcessor::fastEncodingPersistence()
		{
			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_PTR_MEMORY(mLocalSession);
				
				CComPtrCustom<IMFTopology> lFullTopology;

				mLocalSession->GetFullTopology(MFSESSION_GETFULLTOPOLOGY_CURRENT, 0, &lFullTopology);

				LOG_INVOKE_FUNCTION(postEncodingUpdate, lFullTopology);

			} while (false);
			
			return lresult;
		}

		STDMETHODIMP SessionProcessor::GetParameters(
			DWORD* aPtrFlags, 
			DWORD* aPtrQueue)
		{
			return E_NOTIMPL;
		}

		STDMETHODIMP SessionProcessor::Invoke(
			IMFAsyncResult* aPtrAsyncResult)
		{
			HRESULT lresult;

			do
			{
				CComPtrCustom<IMFMediaEvent> lEvent;

				std::lock_guard<std::mutex> lLock(mCritSec);
				
				LOG_CHECK_PTR_MEMORY(aPtrAsyncResult);

				LOG_CHECK_PTR_MEMORY(mLocalSession);
				
				LOG_INVOKE_MF_METHOD(EndGetEvent,
					mLocalSession, 
					aPtrAsyncResult, 
					&lEvent);

				auto levent = processMediaEvent(lEvent);

				lresult = std::get<1>(levent);
				
				if (std::get<0>(levent) == STARTED)
				{
					mIsStarted = true;
				}

				if (std::get<0>(levent) == STOPPED)
				{
					mIsStarted = false;
				}

				if (std::get<0>(levent) == CLOSED)
				{
					mIsStarted = false;
				}
								
				if (std::get<0>(levent) != CLOSED)
				{
					LOG_INVOKE_MF_METHOD(BeginGetEvent,
						mLocalSession, 
						this, 
						nullptr);
				}
				
			} while (false);
						
			return lresult;
		}

		std::tuple<SessionProcessor::MediaEvent, HRESULT> SessionProcessor::processMediaEvent(
			CComPtrCustom<IMFMediaEvent>& aMediaEvent)
		{

			MediaEvent levent = OK;

			HRESULT lresult = S_OK;

			HRESULT lresultStatus = S_OK;

			do
			{

				MediaEventType leventType;

				UINT32 TopoStatus = MF_TOPOSTATUS_INVALID;

				LOG_CHECK_PTR_MEMORY(aMediaEvent);
			
				do
				{
					LOG_INVOKE_MF_METHOD(GetType, aMediaEvent, &leventType);

				} while (false);

				if (FAILED(lresult))
				{
					levent = EXECUTION_ERROR;

					break;
				}

				HRESULT lhrStatus = S_OK;

				do
				{
					LOG_INVOKE_MF_METHOD(GetStatus, aMediaEvent, &lhrStatus);

				} while (false);
								
				if(FAILED(lresult))
				{
					levent = EXECUTION_ERROR;

					break;
				}

				lresultStatus = lhrStatus;

				if (FAILED(lresultStatus))
				{
					levent = STATUS_ERROR;

					break;
				}

				if (leventType == MESessionTopologyStatus)
				{
					LOG_INVOKE_MF_METHOD(GetUINT32,
						aMediaEvent,
						MF_EVENT_TOPOLOGY_STATUS, &TopoStatus);

					if (TopoStatus == MF_TOPOSTATUS_READY)
					{
						std::unique_lock<std::mutex> llock(mMutex);

						if (mSessionAsyncCallback)
						{

							CComPtrCustom<
								SessionAsyncCallback::ISessionAsyncCallbackRequest> lSessionAsyncCallbackRequest =
								new (std::nothrow) SessionAsyncCallback::SessionAsyncCallbackRequest(
								SessionCallbackEventCode::ItIsReadyToStart,
								mSessionDescriptor,
								mISessionCallback);

							LOG_CHECK_PTR_MEMORY(lSessionAsyncCallbackRequest);
														
							LOG_INVOKE_MF_FUNCTION(MFPutWorkItem,
								mSyncWorkerQueue,
								mSessionAsyncCallback,
								lSessionAsyncCallbackRequest);
						}

						mConditionVariable.notify_one();
					}

					break;
				}

				if (leventType == MESessionStarted)
				{

					std::unique_lock<std::mutex> llock(mMutex);

					mIsStarted = true;

					if (mSessionAsyncCallback)
					{

						CComPtrCustom<
							SessionAsyncCallback::ISessionAsyncCallbackRequest> lSessionAsyncCallbackRequest =
							new (std::nothrow) SessionAsyncCallback::SessionAsyncCallbackRequest(
							SessionCallbackEventCode::ItIsStarted,
							mSessionDescriptor,
							mISessionCallback);

						LOG_CHECK_PTR_MEMORY(lSessionAsyncCallbackRequest);
					
						LOG_INVOKE_MF_FUNCTION(MFPutWorkItem,
							mSyncWorkerQueue,
							mSessionAsyncCallback,
							lSessionAsyncCallbackRequest);
					}
					
					mConditionVariable.notify_one();

					break;
				}

				if (leventType == MESessionPaused)
				{
					std::unique_lock<std::mutex> llock(mMutex);

					if (mSessionAsyncCallback)
					{

						CComPtrCustom<
							SessionAsyncCallback::ISessionAsyncCallbackRequest> lSessionAsyncCallbackRequest =
							new (std::nothrow) SessionAsyncCallback::SessionAsyncCallbackRequest(
							SessionCallbackEventCode::ItIsPaused,
							mSessionDescriptor,
							mISessionCallback);

						LOG_CHECK_PTR_MEMORY(lSessionAsyncCallbackRequest);

						LOG_INVOKE_MF_FUNCTION(MFPutWorkItem,
							mSyncWorkerQueue,
							mSessionAsyncCallback,
							lSessionAsyncCallbackRequest);
					}

					mConditionVariable.notify_one();

					levent = PAUSED;
				}

				if (leventType == MEStreamSinkStopped)
				{
				}

				if (leventType == MESessionStopped)
				{
					std::lock_guard<std::mutex> llock(mMutex);

					if (mSessionAsyncCallback)
					{

						CComPtrCustom<
							SessionAsyncCallback::ISessionAsyncCallbackRequest> lSessionAsyncCallbackRequest =
							new (std::nothrow) SessionAsyncCallback::SessionAsyncCallbackRequest(
							SessionCallbackEventCode::ItIsStopped,
							mSessionDescriptor,
							mISessionCallback);

						LOG_CHECK_PTR_MEMORY(lSessionAsyncCallbackRequest);

						LOG_INVOKE_MF_FUNCTION(MFPutWorkItem,
							mSyncWorkerQueue,
							mSessionAsyncCallback,
							lSessionAsyncCallbackRequest);
					}

					mConditionVariable.notify_one();

					levent = STOPPED;
				}

				if (leventType == MESessionEnded)
				{
					std::unique_lock<std::mutex> llock(mMutex);

					if (mSessionAsyncCallback)
					{

						CComPtrCustom<
							SessionAsyncCallback::ISessionAsyncCallbackRequest> lSessionAsyncCallbackRequest =
							new (std::nothrow) SessionAsyncCallback::SessionAsyncCallbackRequest(
							SessionCallbackEventCode::ItIsEnded,
							mSessionDescriptor,
							mISessionCallback);

						LOG_CHECK_PTR_MEMORY(lSessionAsyncCallbackRequest);

						LOG_INVOKE_MF_FUNCTION(MFPutWorkItem,
							mSyncWorkerQueue,
							mSessionAsyncCallback,
							lSessionAsyncCallbackRequest);
					}

					mConditionVariable.notify_one();

					levent = ENDED;
				}

				if (leventType == MEStreamSinkStopped)
				{
					std::unique_lock<std::mutex> llock(mMutex);

					levent = ENDED;
				}

				if (leventType == MESessionClosed)
				{
					std::unique_lock<std::mutex> llock(mMutex);

					if (mLocalSession)
					{
						CComPtrCustom<IMFTopology> lFullTopology;

						mLocalSession->GetFullTopology(MFSESSION_GETFULLTOPOLOGY_CURRENT, 0, &lFullTopology);

						lresult = postEncodingUpdate(lFullTopology);
					}

					if (mSessionAsyncCallback)
					{

						CComPtrCustom<
							SessionAsyncCallback::ISessionAsyncCallbackRequest> lSessionAsyncCallbackRequest =
							new (std::nothrow) SessionAsyncCallback::SessionAsyncCallbackRequest(
							SessionCallbackEventCode::ItIsClosed,
							mSessionDescriptor,
							mISessionCallback);

						LOG_CHECK_PTR_MEMORY(lSessionAsyncCallbackRequest);

						LOG_INVOKE_MF_FUNCTION(MFPutWorkItem,
							mSyncWorkerQueue,
							mSessionAsyncCallback,
							lSessionAsyncCallbackRequest);
					}
					
					mConditionVariable.notify_one();

					levent = CLOSED;
				}


				if (leventType == MEVideoCaptureDeviceRemoved)
				{
					std::unique_lock<std::mutex> llock(mMutex);

					if (mSessionAsyncCallback)
					{

						CComPtrCustom<
							SessionAsyncCallback::ISessionAsyncCallbackRequest> lSessionAsyncCallbackRequest =
							new (std::nothrow) SessionAsyncCallback::SessionAsyncCallbackRequest(
							SessionCallbackEventCode::VideoCaptureDeviceRemoved,
							mSessionDescriptor,
							mISessionCallback);

						LOG_CHECK_PTR_MEMORY(lSessionAsyncCallbackRequest);

						LOG_INVOKE_MF_FUNCTION(MFPutWorkItem,
							mSyncWorkerQueue,
							mSessionAsyncCallback,
							lSessionAsyncCallbackRequest);
					}

					mConditionVariable.notify_one();

					levent = OK;
				}


				if (leventType == MEVideoCaptureDevicePreempted)
				{
					std::unique_lock<std::mutex> llock(mMutex);
					
					mConditionVariable.notify_one();

					levent = CLOSED;
				}

				if (leventType == CMSnapTriggerEvent)
				{
					std::unique_lock<std::mutex> llock(mMutex);

					if (mSessionAsyncCallback)
					{

						CComPtrCustom<
							SessionAsyncCallback::ISessionAsyncCallbackRequest> lSessionAsyncCallbackRequest =
							new (std::nothrow) SessionAsyncCallback::SessionAsyncCallbackRequest(
								SessionCallbackEventCode::SnapTrigger,
								mSessionDescriptor,
								mISessionCallback);

						LOG_CHECK_PTR_MEMORY(lSessionAsyncCallbackRequest);

						LOG_INVOKE_MF_FUNCTION(MFPutWorkItem,
							mSyncWorkerQueue,
							mSessionAsyncCallback,
							lSessionAsyncCallbackRequest);
					}

					mConditionVariable.notify_one();

					levent = OK;
				}
				
			} while (false);

			if (levent == EXECUTION_ERROR)
			{
				std::unique_lock<std::mutex> llock(mMutex);

				if (mSessionAsyncCallback)
				{

					CComPtrCustom<
						SessionAsyncCallback::ISessionAsyncCallbackRequest> lSessionAsyncCallbackRequest =
						new (std::nothrow) SessionAsyncCallback::SessionAsyncCallbackRequest(
						SessionCallbackEventCode::Execution_Error,
						mSessionDescriptor,
						mISessionCallback);

					do
					{

						LOG_CHECK_PTR_MEMORY(lSessionAsyncCallbackRequest);

						LOG_INVOKE_MF_FUNCTION(MFPutWorkItem,
							mSyncWorkerQueue,
							mSessionAsyncCallback,
							lSessionAsyncCallbackRequest);

					} while (false);
				}

			//	mConditionVariable.notify_one();
			}

			if (levent == STATUS_ERROR)
			{
				std::unique_lock<std::mutex> llock(mMutex);

				if (mSessionAsyncCallback)
				{

					CComPtrCustom<
						SessionAsyncCallback::ISessionAsyncCallbackRequest> lSessionAsyncCallbackRequest =
						new (std::nothrow) SessionAsyncCallback::SessionAsyncCallbackRequest(
						SessionCallbackEventCode::Status_Error,
						mSessionDescriptor,
						mISessionCallback);
					
					do
					{

						LOG_CHECK_PTR_MEMORY(lSessionAsyncCallbackRequest);

						LOG_INVOKE_MF_FUNCTION(MFPutWorkItem,
							mSyncWorkerQueue,
							mSessionAsyncCallback,
							lSessionAsyncCallbackRequest);

					} while (false);
				}

			//	mConditionVariable.notify_one();
			}

			if (FAILED(lresultStatus))
			{
				lresult = lresultStatus;
			}

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::INFO_LEVEL,
					__FUNCTIONW__,
					L" Error code: ",
					(HRESULT)lresult);
			}

			return std::make_tuple(levent, lresult);
		}
		
		HRESULT SessionProcessor::postEncodingUpdate(
			IMFTopology* aPtrTopology)
		{
			HRESULT lresult;
			
			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrTopology);
				
				CComPtrCustom<IMFCollection> lCollection;

				LOG_INVOKE_MF_FUNCTION(GetOutputNodeCollection,
					aPtrTopology,
					&lCollection);
				
				LOG_CHECK_PTR_MEMORY(lCollection);
				
				DWORD lElementCount;

				LOG_INVOKE_MF_FUNCTION(GetElementCount,
					lCollection,
					&lElementCount);
				
				std::list<CComPtrCustom<IMFMediaSink>> lIMFMediaSinkList;
				
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
					
					auto lfindResult = std::find(lIMFMediaSinkList.begin(),
						lIMFMediaSinkList.end(),
						lMediaSink);

					if (lfindResult == lIMFMediaSinkList.end())
					{
						lIMFMediaSinkList.push_back(lMediaSink);
					}

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
										
					lresult = postEncodingUpdate(
						lStreamSink,
						lNode);

					if (FAILED(lresult))
					{
						continue;
					}							

					lresult = lStreamSink->Flush();
				}

				CComPtrCustom<MediaSinkFinalizeProcessor> lMediaSinkFinalizeProcessor =
					new (std::nothrow)MediaSinkFinalizeProcessor();

				if (!lMediaSinkFinalizeProcessor)
				{
					break;
				}

				auto liter = lIMFMediaSinkList.begin();

				for (; liter != lIMFMediaSinkList.end(); liter++)
				{

					lresult = lMediaSinkFinalizeProcessor->finalizeMediaSink(*liter);

					lresult = (*liter)->Shutdown();

					if (FAILED(lresult))
					{
						continue;
					}
				}

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL,
					__FUNCTIONW__,
					L" Error code: ",
					(HRESULT)lresult);
			}

			return lresult;						
		}

		HRESULT SessionProcessor::postEncodingUpdate(
			IMFStreamSink* aPtrStreamSink,
			IMFTopologyNode* aPtrTopologyNode)
		{
			HRESULT lresult;

			PROPVARIANT lvar;

			PropVariantInit(&lvar);			

			CComPtrCustom<IMFTopologyNode> lUpStreamNode;

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrStreamSink);
				
				LOG_CHECK_PTR_MEMORY(aPtrTopologyNode);
				
				DWORD lOutputIndexOnUpstreamNode;

				LOG_INVOKE_MF_METHOD(GetInput,
					aPtrTopologyNode,
					0,
					&lUpStreamNode,
					&lOutputIndexOnUpstreamNode);
				
				MF_TOPOLOGY_TYPE lTopologyType;

				LOG_INVOKE_MF_METHOD(GetNodeType,
					lUpStreamNode,
					&lTopologyType);
				
				if (lTopologyType == MF_TOPOLOGY_SOURCESTREAM_NODE)
				{
					lresult = S_FALSE;

					break;
				}

				if (lTopologyType != MF_TOPOLOGY_TRANSFORM_NODE)
				{
					lresult = E_FAIL;

					break;
				}
				
				CComPtrCustom<IUnknown> lEncoderUnk;

				LOG_INVOKE_MF_FUNCTION(GetObject,
					lUpStreamNode,
					&lEncoderUnk);
				
				LOG_CHECK_PTR_MEMORY(lEncoderUnk);
				
				CComQIPtrCustom<IMFTransform> lEncoder = lEncoderUnk;

				LOG_CHECK_PTR_MEMORY(lEncoder);
				
				CComPtrCustom<IMFMediaTypeHandler> lMediaTypeHandler;

				LOG_INVOKE_MF_METHOD(GetMediaTypeHandler,
					aPtrStreamSink,
					&lMediaTypeHandler);
				
				LOG_CHECK_PTR_MEMORY(lMediaTypeHandler);
				
				GUID lMajorType = GUID_NULL;

				LOG_INVOKE_MF_METHOD(GetMajorType,
					lMediaTypeHandler,
					&lMajorType);
				
				CComQIPtrCustom<IPropertyStore> lStreamSinkProps = aPtrStreamSink;

				LOG_CHECK_PTR_MEMORY(lStreamSinkProps);
				
				CComQIPtrCustom<IPropertyStore> lEncoderProps = lEncoder;

				LOG_CHECK_PTR_MEMORY(lEncoderProps);							

				if (lMajorType == MFMediaType_Video)
				{

					LOG_INVOKE_POINTER_METHOD(lEncoderProps, GetValue, MFPKEY_BAVG, &lvar);
					
					LOG_INVOKE_POINTER_METHOD(lStreamSinkProps, SetValue, MFPKEY_STAT_BAVG, lvar);
					
					PropVariantClear(&lvar);

					LOG_INVOKE_POINTER_METHOD(lEncoderProps, GetValue, MFPKEY_RAVG, &lvar);
					
					LOG_INVOKE_POINTER_METHOD(lStreamSinkProps, SetValue, MFPKEY_STAT_RAVG, lvar);
					
					PropVariantClear(&lvar);

					LOG_INVOKE_POINTER_METHOD(lEncoderProps, GetValue, MFPKEY_BMAX, &lvar);
					
					LOG_INVOKE_POINTER_METHOD(lStreamSinkProps, SetValue, MFPKEY_STAT_BMAX, lvar);
					
					PropVariantClear(&lvar);

					LOG_INVOKE_POINTER_METHOD(lEncoderProps, GetValue, MFPKEY_RMAX, &lvar);
					
					LOG_INVOKE_POINTER_METHOD(lStreamSinkProps, SetValue, MFPKEY_STAT_RMAX, lvar);

					PropVariantClear(&lvar);

				}
				else if (lMajorType == MFMediaType_Audio)
				{
					PropVariantInit(&lvar);

					LOG_INVOKE_POINTER_METHOD(lEncoderProps, GetValue, MFPKEY_STAT_BAVG, &lvar);
					
					LOG_INVOKE_POINTER_METHOD(lStreamSinkProps, SetValue, MFPKEY_STAT_BAVG, lvar);
					
					PropVariantClear(&lvar);

					LOG_INVOKE_POINTER_METHOD(lEncoderProps, GetValue, MFPKEY_STAT_RAVG, &lvar);
					
					LOG_INVOKE_POINTER_METHOD(lStreamSinkProps, SetValue, MFPKEY_STAT_RAVG, lvar);
					
					PropVariantClear(&lvar);

					LOG_INVOKE_POINTER_METHOD(lEncoderProps, GetValue, MFPKEY_STAT_BMAX, &lvar);
					
					LOG_INVOKE_POINTER_METHOD(lStreamSinkProps, SetValue, MFPKEY_STAT_BMAX, lvar);
					
					PropVariantClear(&lvar);

					LOG_INVOKE_POINTER_METHOD(lEncoderProps, GetValue, MFPKEY_STAT_RMAX, &lvar);
					
					LOG_INVOKE_POINTER_METHOD(lStreamSinkProps, SetValue, MFPKEY_STAT_RMAX, lvar);

					PropVariantClear(&lvar);

					LOG_INVOKE_POINTER_METHOD(lEncoderProps, GetValue, MFPKEY_WMAENC_AVGBYTESPERSEC, &lvar);
					
					LOG_INVOKE_POINTER_METHOD(lStreamSinkProps, SetValue, MFPKEY_WMAENC_AVGBYTESPERSEC, lvar);
					
					PropVariantClear(&lvar);
				}


			} while (false);

			if (FAILED(lresult))
			{
				lresult = postEncodingUpdate(
					aPtrStreamSink,
					lUpStreamNode);
			}
			
			return lresult;
		}

	}
}