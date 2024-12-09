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

#include "CaptureSource.h"
#include "CaptureStream.h"
#include "SourceOperation.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/ComMassivPtr.h"
#include "../Common/Common.h"
#include "../Common/IInnerGetService.h"
#include "../Common/GUIDs.h"
#include <memory>

EXTERN_GUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_HW_SOURCE,
	0x5e574e59, 0xe28d, 0x41db, 0x93, 0x14, 0x08, 0x9b, 0xfe, 0xd6, 0x95, 0x7c);

namespace CaptureManager
{
	namespace Sources
	{
		using namespace CaptureManager::Core;

		CaptureSource::CaptureSource() :
			mSyncWorkerQueue(MFASYNC_CALLBACK_QUEUE_LONG_FUNCTION),
			mState(SourceState::SourceStateUninitialized)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				LOG_INVOKE_MF_FUNCTION(MFCreateEventQueue, &mEventQueue);

			} while (false);

			if (FAILED(lresult))
				throw;			
		}

		HRESULT CaptureSource::init(CComPtrCustom<IInnerCaptureProcessor> aCaptureProcessor,
			BOOL aIsHardWare)
		{
			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(aCaptureProcessor);

				mCaptureProcessor = aCaptureProcessor;

				LOG_CHECK_PTR_MEMORY(mCaptureProcessor);

				CComPtrCustom<IMediaEvent> lIMediaEvent;

				LOG_INVOKE_QUERY_INTERFACE_METHOD(this, &lIMediaEvent);

				mCaptureProcessor->registerMediaEvent(lIMediaEvent);
				
				LOG_INVOKE_FUNCTION(createPresentationDescriptor,
					aIsHardWare);

				LOG_INVOKE_FUNCTION(initialized);
				
			} while (false);

			return lresult;
		}

		CaptureSource::~CaptureSource()
		{
			mCaptureProcessor.Release();

			mPresentationDescriptor.Release();
				
			mEventQueue.Release();

			if (mSyncWorkerQueue != MFASYNC_CALLBACK_QUEUE_LONG_FUNCTION)
			{
				HRESULT lresult;

				do
				{
					LOG_INVOKE_MF_FUNCTION(MFUnlockWorkQueue, mSyncWorkerQueue);
					
				} while (false);
			}
		}
					
		// IMFMediaSource implementation

		HRESULT STDMETHODCALLTYPE CaptureSource::GetCharacteristics(__RPC__out DWORD * aPtrdwCharacteristics)
		{
			HRESULT lresult;

			do
			{
				std::lock_guard<std::mutex> lLock(mMutex);

				LOG_CHECK_PTR_MEMORY(aPtrdwCharacteristics);
				
				LOG_INVOKE_FUNCTION(checkShutdown);
				
				*aPtrdwCharacteristics = MFMEDIASOURCE_IS_LIVE;

			} while (false);

			return lresult;
		}

		HRESULT STDMETHODCALLTYPE CaptureSource::CreatePresentationDescriptor(
			IMFPresentationDescriptor ** aPtrPtrPresentationDescriptor)
		{
			HRESULT lresult;

			do
			{

				LOG_CHECK_PTR_MEMORY(aPtrPtrPresentationDescriptor);

				LOG_CHECK_STATE_DESCR(!mPresentationDescriptor, MF_E_NOT_INITIALIZED);

				LOG_INVOKE_FUNCTION(checkShutdown);

				LOG_INVOKE_MF_METHOD(Clone, mPresentationDescriptor, aPtrPtrPresentationDescriptor);
									

			} while (false);

			return lresult;
		}

		HRESULT STDMETHODCALLTYPE CaptureSource::Start(
			IMFPresentationDescriptor * aPtrPresentationDescriptor,
			const GUID * aPtrGUIDTimeFormat,
			const PROPVARIANT * aPtrVarStartPosition)
		{
			HRESULT lresult;
				
			do
			{
				std::lock_guard<std::mutex> lLock(mMutex);

				LOG_CHECK_PTR_MEMORY(aPtrVarStartPosition);

				LOG_CHECK_PTR_MEMORY(aPtrPresentationDescriptor);

				LOG_CHECK_STATE_DESCR((aPtrGUIDTimeFormat != NULL) && (*aPtrGUIDTimeFormat != GUID_NULL), MF_E_UNSUPPORTED_TIME_FORMAT);
				
				LOG_CHECK_STATE_DESCR((aPtrVarStartPosition->vt != VT_I8) && (aPtrVarStartPosition->vt != VT_EMPTY), MF_E_UNSUPPORTED_TIME_FORMAT);
				
				LOG_INVOKE_FUNCTION(checkShutdown);

				lresult = isInitialized();

				if (FAILED(lresult))
				{
					LOG_INVOKE_FUNCTION(initialized);
				}
									
				CComPtrCustom<ISourceOperation> lOperation = new (std::nothrow) SourceOperation(
					SourceOperationStart,                   
					aPtrPresentationDescriptor);

				LOG_CHECK_STATE_DESCR(!lOperation, E_OUTOFMEMORY);
					
				CComPtrCustom<IUnknown> lIUnknown;

				LOG_INVOKE_QUERY_INTERFACE_METHOD(lOperation, &lIUnknown);

				LOG_INVOKE_MF_FUNCTION(MFPutWorkItem,
					mSyncWorkerQueue,
					this,                                   
					lIUnknown);				
										
			} while (false);
			
			return lresult;
		}

		HRESULT STDMETHODCALLTYPE CaptureSource::Stop()
		{
			HRESULT lresult;

			do
			{					
				LOG_INVOKE_FUNCTION(checkShutdown);

				LOG_INVOKE_FUNCTION(isInitialized);

				if (mState == SourceState::SourceStateStopped)
					break;

				CComPtrCustom<ISourceOperation> lOperation = new (std::nothrow) SourceOperation(SourceOperationStop);
				
				LOG_CHECK_STATE_DESCR(!lOperation, E_OUTOFMEMORY);

				CComPtrCustom<IUnknown> lIUnknown;

				LOG_INVOKE_QUERY_INTERFACE_METHOD(lOperation, &lIUnknown);

				LOG_INVOKE_MF_FUNCTION(MFPutWorkItem,
					mSyncWorkerQueue,
					this,
					lIUnknown);					

			} while (false);
			
			return lresult;
		}

		HRESULT STDMETHODCALLTYPE CaptureSource::Pause(void)
		{
			HRESULT lresult;

			do
			{
				std::lock_guard<std::mutex> lLock(mMutex);

				LOG_INVOKE_FUNCTION(checkShutdown);

				LOG_INVOKE_FUNCTION(isInitialized);
				
				if (mState != SourceState::SourceStateStarted)
				{
					lresult = S_OK;

					break;
				}
								
				CComPtrCustom<ISourceOperation> lOperation = new (std::nothrow) SourceOperation(SourceOperationPause);

				LOG_CHECK_STATE_DESCR(!lOperation, E_OUTOFMEMORY);

				CComPtrCustom<IUnknown> lIUnknown;

				LOG_INVOKE_QUERY_INTERFACE_METHOD(lOperation, &lIUnknown);

				LOG_INVOKE_MF_FUNCTION(MFPutWorkItem,
					mSyncWorkerQueue,
					this,
					lIUnknown);
								
			} while (false);
			
			return lresult;
		}

		HRESULT STDMETHODCALLTYPE CaptureSource::Shutdown(void)
		{
			HRESULT lresult;

			do
			{
				std::lock_guard<std::mutex> lLock(mMutex);

				LOG_INVOKE_FUNCTION(checkShutdown);

				LOG_INVOKE_FUNCTION(isInitialized);

				auto liter = mStreams.begin();

				auto literEnd = mStreams.end();

				for (; liter != literEnd; liter++)
				{
					LOG_INVOKE_POINTER_METHOD((*liter).second, shutdown);					
				}

				if (!(!mEventQueue))
				{
					LOG_INVOKE_MF_METHOD(Shutdown, mEventQueue);					
				}
					
				mState = SourceState::SourceStateShutdown;

				LOG_INVOKE_FUNCTION(shutdownCapture);
				
				mCaptureProcessor.Release();
				
				mPresentationDescriptor.Release();

				mEventQueue.Release();

			} while (false);
			
			return lresult;
		}

		HRESULT STDMETHODCALLTYPE CaptureSource::GetEvent(
			DWORD aFlags,
			IMFMediaEvent ** aPtrPtrEvent)
		{
			HRESULT lresult;

			do
			{
				std::lock_guard<std::mutex> lLock(mMutex);

				LOG_INVOKE_FUNCTION(checkShutdown);

				LOG_INVOKE_FUNCTION(isInitialized);

				LOG_INVOKE_MF_METHOD(GetEvent, mEventQueue, aFlags, aPtrPtrEvent);
				
			} while (false);
			
			return lresult;
		}

		HRESULT STDMETHODCALLTYPE CaptureSource::BeginGetEvent(
			IMFAsyncCallback *aPtrCallback,
			IUnknown *aPtrUnkState)
		{
			HRESULT lresult;

			do
			{

				std::lock_guard<std::mutex> lLock(mMutex);

				LOG_INVOKE_FUNCTION(checkShutdown);

				LOG_INVOKE_FUNCTION(isInitialized);

				LOG_INVOKE_MF_METHOD(BeginGetEvent, mEventQueue, aPtrCallback, aPtrUnkState);
				
			} while (false);
			
			return lresult;
		}

		HRESULT STDMETHODCALLTYPE CaptureSource::EndGetEvent(
			IMFAsyncResult *aPtrResult,
			_Out_  IMFMediaEvent ** aPtrPtrEvent)
		{
			HRESULT lresult;
				
			do
			{

				std::lock_guard<std::mutex> lLock(mMutex);

				LOG_INVOKE_FUNCTION(checkShutdown);

				LOG_INVOKE_FUNCTION(isInitialized);

				LOG_INVOKE_MF_METHOD(EndGetEvent, mEventQueue, aPtrResult, aPtrPtrEvent);
				
			} while (false);

			return lresult;
		}

		HRESULT STDMETHODCALLTYPE CaptureSource::QueueEvent(MediaEventType aMediaEventType,
			__RPC__in REFGUID aGUIDExtendedType,
			HRESULT aHRStatus,
			const PROPVARIANT * aPtrValue)
		{
			HRESULT lresult;

			do
			{
				std::lock_guard<std::mutex> lLock(mMutex);

				LOG_INVOKE_FUNCTION(checkShutdown);

				LOG_INVOKE_FUNCTION(isInitialized);

				LOG_INVOKE_MF_METHOD(QueueEventParamVar, mEventQueue, 
					aMediaEventType, 
					aGUIDExtendedType, 
					aHRStatus, 
					aPtrValue);
					
			} while (false);

			return lresult;
		}
				
		HRESULT CaptureSource::GetService(
			REFGUID aRefGUIDService,
			REFIID aRefIID,
			LPVOID* aPtrPtrObject)
		{
			HRESULT lresult(MF_E_UNSUPPORTED_SERVICE);

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrPtrObject);

				if (aRefGUIDService == CM_DeviceManager)
				{
					if (aRefIID == __uuidof(IMFDXGIDeviceManager))
					{
						LOG_CHECK_PTR_MEMORY(mCaptureProcessor);

						CComPtrCustom<IInnerGetService> lGetService;

						LOG_INVOKE_QUERY_INTERFACE_METHOD(mCaptureProcessor, &lGetService);

						LOG_CHECK_PTR_MEMORY(lGetService);

						LOG_INVOKE_POINTER_METHOD(lGetService, GetService,
							aRefGUIDService,
							aRefIID,
							aPtrPtrObject);
					}
				}

			} while (false);

			return lresult;
		}

		HRESULT STDMETHODCALLTYPE CaptureSource::GetParameters(__RPC__out DWORD *pdwFlags,
			__RPC__out DWORD *pdwQueue)
		{
			return E_NOTIMPL;
		}

		HRESULT STDMETHODCALLTYPE CaptureSource::Invoke(IMFAsyncResult *pAsyncResult)
		{
			HRESULT lresult;

			do
			{

				CComPtrCustom<IMFAsyncResult> lAsyncResult;
				CComPtrCustom<ISourceOperation> lCommand;
				CComPtrCustom<IUnknown> lState;

				std::lock_guard<std::mutex> lLock(mMutex);

				LOG_INVOKE_FUNCTION(checkShutdown);

				LOG_CHECK_PTR_MEMORY(pAsyncResult);

				lAsyncResult = pAsyncResult;

				LOG_CHECK_PTR_MEMORY(lAsyncResult);

				LOG_INVOKE_MF_METHOD(GetState, lAsyncResult,
					&lState);
				
				LOG_INVOKE_QUERY_INTERFACE_METHOD(lState, &lCommand);

				SourceOperationType lSourceOperationType = SourceOperationType::None;

				LOG_INVOKE_POINTER_METHOD(lCommand, getSourceOperationType, &lSourceOperationType);
				
				switch (lSourceOperationType)
				{
				case SourceOperationStart:
					LOG_INVOKE_FUNCTION(internalStart, lCommand);
					break;
				case SourceOperationStop:
					LOG_INVOKE_FUNCTION(internalStop);
					break;
				case SourceOperationPause:
					LOG_INVOKE_FUNCTION(internalPause);
					break;
				case SourceOperationStreamNeedData:
					LOG_INVOKE_FUNCTION(requestSample, lCommand);
					break;
				case SourceOperationEndOfStream:
					break;
				default:
					break;
				}

			} while (false);
			
			return lresult;
		}

		HRESULT __stdcall CaptureSource::PostEvent(
			DWORD aMediaEventType, 
			REFGUID aGUIDExtendedType, 
			HRESULT aHRStatus, 
			IUnknown* aPtrUnk)
		{
			HRESULT lresult = S_OK;
			do
			{
				LOG_INVOKE_MF_METHOD(QueueEventParamUnk, mEventQueue,
					aMediaEventType,
					aGUIDExtendedType,
					aHRStatus,
					aPtrUnk);

			} while (false);

			return lresult;
		}

		HRESULT STDMETHODCALLTYPE CaptureSource::checkShutdown() const
		{
			HRESULT lresult = S_OK;

			if (mState == SourceState::SourceStateShutdown)
			{
				lresult = MF_E_SHUTDOWN;
			}

			return lresult;
		}

		HRESULT STDMETHODCALLTYPE CaptureSource::internalStart(
			ISourceOperation* aPtrCommand)
		{
			HRESULT lresult;

			do
			{				
				LOG_CHECK_PTR_MEMORY(aPtrCommand);

				CComPtrCustom<IUnknown> lObject;

				CComPtrCustom<IMFPresentationDescriptor> lPresentationDescriptor;
				
				LOG_INVOKE_POINTER_METHOD(aPtrCommand, getObject, &lObject);

				LOG_INVOKE_QUERY_INTERFACE_METHOD(lObject, &lPresentationDescriptor);							

				LOG_INVOKE_FUNCTION(selectStream, lPresentationDescriptor);
				
				mState = SourceState::SourceStateStarted;

				LOG_INVOKE_MF_METHOD(QueueEventParamVar, mEventQueue,
					MESourceStarted,
					GUID_NULL,
					S_OK,
					nullptr);
									
			} while (false);
			
			return lresult;
		}

		HRESULT STDMETHODCALLTYPE CaptureSource::internalPause()
		{
			HRESULT lresult = S_OK;

			do
			{

				mState = SourceState::SourceStatePaused;

				LOG_INVOKE_POINTER_METHOD(mCaptureProcessor, pause);
				
				auto liter = mStreams.begin();

				auto literEnd = mStreams.end();

				for (; liter != literEnd; liter++)
				{					
					BOOL lState = FALSE;

					LOG_INVOKE_POINTER_METHOD((*liter).second, isActive, &lState);										 

					if (lState == TRUE)
					{
						LOG_INVOKE_POINTER_METHOD((*liter).second, pause);
					}
				}
				
			} while (false);

			do
			{
				LOG_INVOKE_MF_METHOD(QueueEventParamVar, mEventQueue,
					MESourcePaused,
					GUID_NULL,
					lresult,
					nullptr);

			} while (false);
			
			return lresult;
		}

		HRESULT STDMETHODCALLTYPE CaptureSource::internalStop()
		{
			HRESULT lresult = S_OK;

			do
			{

				auto liter = mStreams.begin();

				auto literEnd = mStreams.end();

				for (; liter != literEnd; liter++)
				{
					BOOL lState = FALSE;

					LOG_INVOKE_POINTER_METHOD((*liter).second, isActive, &lState);

					if (lState == TRUE)
					{
						LOG_INVOKE_POINTER_METHOD((*liter).second, stop);
					}
				}
													
				mState = SourceState::SourceStateStopped;

			} while (false);

			do
			{
				LOG_INVOKE_MF_METHOD(QueueEventParamVar, mEventQueue,
					MESourceStopped,
					GUID_NULL,
					lresult,
					nullptr);

			} while (false);
			
			return lresult;
		}

		HRESULT STDMETHODCALLTYPE CaptureSource::selectStream(
			IMFPresentationDescriptor *aPtrPresentationDescriptor)
		{
			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrPresentationDescriptor);

				DWORD lStreamDescriptorCount = 0;

				LOG_INVOKE_MF_METHOD(GetStreamDescriptorCount, aPtrPresentationDescriptor,
					&lStreamDescriptorCount);
				
				LOG_INVOKE_FUNCTION(startCapture);
				
				for (DWORD lStreamIndex = 0; lStreamIndex < lStreamDescriptorCount; lStreamIndex++)
				{
					CComPtrCustom<IMFStreamDescriptor> lStreamDescriptor;

					CComPtrCustom<IUnknown> lUnkStream;

					BOOL lselected = FALSE;

					DWORD lstreamId = 0;

					LOG_INVOKE_MF_METHOD(GetStreamDescriptorByIndex, aPtrPresentationDescriptor,
						lStreamIndex, &lselected, &lStreamDescriptor);

					LOG_INVOKE_MF_METHOD(GetStreamIdentifier, lStreamDescriptor,
						&lstreamId);

					BOOL lIsActive;

					LOG_INVOKE_POINTER_METHOD(mStreams[lstreamId], isActive, &lIsActive);
										
					mStreams[lstreamId]->activate(lselected == TRUE);

					lUnkStream = mStreams[lstreamId];

					if (lselected == TRUE)
					{
						MediaEventType lMediaEventType;

						if (lIsActive == TRUE)
						{
							lMediaEventType = MEUpdatedStream;
						}
						else
						{
							lMediaEventType = MENewStream;
						}
						
						LOG_INVOKE_MF_METHOD(QueueEventParamUnk, mEventQueue,
							lMediaEventType,
							GUID_NULL,
							lresult,
							lUnkStream);

						LOG_INVOKE_POINTER_METHOD(mStreams[lstreamId], start);

					}
				}
				
			} while (false);
			
			return lresult;

		}
			
		HRESULT CaptureSource::createPresentationDescriptor(BOOL aIsHardWare)
		{
			HRESULT lresult;

			BSTR lPtrSymbolicLink = nullptr;

			BSTR lPtrFrendlyName = nullptr;
				
			do
			{
				std::lock_guard<std::mutex> lLock(mMutex);

				LOG_CHECK_PTR_MEMORY(mCaptureProcessor);

				UINT32 lStreamAmount;

				LOG_INVOKE_POINTER_METHOD(mCaptureProcessor, getStreamAmount, &lStreamAmount);
				
				UINT32 lStreamDescriptorIndex = 0;
								
				auto lPtrVoid = CoTaskMemAlloc(sizeof(IMFStreamDescriptor*)* lStreamAmount);

				IMFStreamDescriptor ** lStreamDescriptors = (IMFStreamDescriptor**)(lPtrVoid);

				std::unique_ptr<MFT_REGISTER_TYPE_INFO> lptrInfo(new MFT_REGISTER_TYPE_INFO);

				for (decltype(lStreamAmount) lStreamindex = 0; lStreamindex < lStreamAmount; lStreamindex++)
				{

					CComPtrCustom<IMFStreamDescriptor> lStreamDescriptor;

					LOG_INVOKE_FUNCTION(createStreamDescriptor,
						lStreamindex,
						&lStreamDescriptor);

					LOG_CHECK_PTR_MEMORY(lStreamDescriptor);
					
					CComPtrCustom<IMFMediaTypeHandler> lMediaTypeHandler;

					LOG_INVOKE_MF_METHOD(GetMediaTypeHandler,
						lStreamDescriptor,
						&lMediaTypeHandler);

					LOG_CHECK_PTR_MEMORY(lMediaTypeHandler);
					
					GUID lMajorType;

					LOG_INVOKE_MF_METHOD(GetMajorType,
						lMediaTypeHandler,
						&lMajorType);
					
					CComPtrCustom<IMFMediaType> lMediaType;

					LOG_INVOKE_MF_METHOD(GetCurrentMediaType,
						lMediaTypeHandler,
						&lMediaType);

					LOG_CHECK_PTR_MEMORY(lMediaType);
					
					GUID lSubType;

					LOG_INVOKE_MF_METHOD(GetGUID,
						lMediaType,
						MF_MT_SUBTYPE,
						&lSubType);
					
					lptrInfo->guidMajorType = lMajorType;

					lptrInfo->guidSubtype = lSubType;

					lStreamDescriptors[lStreamDescriptorIndex++] = lStreamDescriptor.detach();
					
				}

				if (FAILED(lresult))
				{
					for (decltype(lStreamAmount) lStreamIndex = 0; lStreamIndex < lStreamDescriptorIndex; lStreamIndex++)
					{
						lStreamDescriptors[lStreamIndex]->Release();
					}

					CoTaskMemFree(lStreamDescriptors);

					break;
				}
				
				LOG_INVOKE_MF_FUNCTION(MFCreatePresentationDescriptor,
					lStreamDescriptorIndex,
					lStreamDescriptors,
					&mPresentationDescriptor);

				for (decltype(lStreamAmount) lStreamIndex = 0; lStreamIndex < lStreamDescriptorIndex; lStreamIndex++)
				{
					lStreamDescriptors[lStreamIndex]->Release();
				}

				CoTaskMemFree(lStreamDescriptors);

				LOG_CHECK_PTR_MEMORY(mPresentationDescriptor);

				LOG_INVOKE_MF_METHOD(SelectStream,
					mPresentationDescriptor,
					0);

				LOG_INVOKE_POINTER_METHOD(mCaptureProcessor, getSymbolicLink,
					&lPtrSymbolicLink);
				
				if (lptrInfo->guidMajorType == MFMediaType_Video)
				{
					
					LOG_INVOKE_MF_METHOD(SetString,
						mPresentationDescriptor,
						MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK,
						lPtrSymbolicLink);

					LOG_INVOKE_MF_METHOD(SetUINT32,
						mPresentationDescriptor,
						MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_HW_SOURCE,
						aIsHardWare);

					if (aIsHardWare == TRUE)
						LOG_INVOKE_MF_METHOD(SetGUID,
							mPresentationDescriptor,
							MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_CATEGORY,
							CLSID_VideoInputDeviceCategory);


				}
				else if (lptrInfo->guidMajorType == MFMediaType_Audio)
				{
					LOG_INVOKE_MF_METHOD(SetUINT32,
						mPresentationDescriptor,
						MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_HW_SOURCE,
						FALSE);

					LOG_INVOKE_MF_METHOD(SetString,
						mPresentationDescriptor,
						MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_SYMBOLIC_LINK,
						lPtrSymbolicLink);					
				}

				LOG_INVOKE_MF_METHOD(SetBlob,
					mPresentationDescriptor,
					MF_DEVSOURCE_ATTRIBUTE_MEDIA_TYPE,
					(UINT8*)lptrInfo.release(),
					sizeof(MFT_REGISTER_TYPE_INFO));

				LOG_INVOKE_POINTER_METHOD(mCaptureProcessor, getFrendlyName,
					&lPtrFrendlyName);

				LOG_INVOKE_MF_METHOD(SetString,
					mPresentationDescriptor,
					MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME,
					lPtrFrendlyName);
										
			} while (false);

			if (lPtrSymbolicLink != nullptr)
				SysFreeString(lPtrSymbolicLink);

			if (lPtrFrendlyName != nullptr)
				SysFreeString(lPtrFrendlyName);			
					
			return lresult;
		}

		HRESULT CaptureSource::createStreamDescriptor(
			DWORD aStreamIdentifier,
			IMFStreamDescriptor** aPtrPtrStreamDescriptor)
		{
			HRESULT lresult;

			LPVOID lPtrVoid = nullptr;

			BSTR lStreamNameString = nullptr;

			do
			{

				LOG_CHECK_PTR_MEMORY(aPtrPtrStreamDescriptor);
					
				CComPtrCustom<IMFMediaTypeHandler> lHandler;

				CComMassivPtr<IUnknown> lMediaTypes;

				LOG_INVOKE_POINTER_METHOD(mCaptureProcessor,getMediaTypes,
					aStreamIdentifier,
					&lStreamNameString,
					lMediaTypes.getPtrMassivPtr(),
					lMediaTypes.getPtrSizeMassiv());

				LOG_CHECK_STATE_DESCR(lMediaTypes.getSizeMassiv() == 0, E_FAIL);

				CComMassivPtr<IMFMediaType> lCastMediaTypes;

				UINT32 lMediaTypesCount = 0;
								
				lPtrVoid = CoTaskMemAlloc(sizeof(IMFMediaType*)* lMediaTypes.getSizeMassiv());

				IMFMediaType ** lPtrPtrMediaTypes = (IMFMediaType**)(lPtrVoid);

				for (UINT32 lIndex = 0; lIndex < lMediaTypes.getSizeMassiv(); lIndex++)
				{
					CComPtrCustom<IMFMediaType> lLocalMediaType;

					LOG_INVOKE_QUERY_INTERFACE_METHOD(lMediaTypes[lIndex], &lLocalMediaType);

					lPtrPtrMediaTypes[lMediaTypesCount++] = lLocalMediaType.detach();
				}

				LOG_CHECK_STATE_DESCR(lMediaTypesCount != lMediaTypes.getSizeMassiv(), E_FAIL);

				*lCastMediaTypes.getPtrMassivPtr() = lPtrPtrMediaTypes;

				*lCastMediaTypes.getPtrSizeMassiv() = lMediaTypesCount;

				lPtrVoid = nullptr;
				
				LOG_INVOKE_MF_FUNCTION(MFCreateStreamDescriptor,
						aStreamIdentifier,
						lCastMediaTypes.getSizeMassiv(),
						lCastMediaTypes.getMassivPtr(),
						aPtrPtrStreamDescriptor);

				LOG_CHECK_PTR_MEMORY((*aPtrPtrStreamDescriptor));

				if (lStreamNameString != nullptr)
				{
					(*aPtrPtrStreamDescriptor)->SetString(
						MF_SD_STREAM_NAME,
						lStreamNameString);

					SysFreeString(lStreamNameString);

					lStreamNameString = nullptr;
				}


				LOG_INVOKE_MF_METHOD(GetMediaTypeHandler,
					(*aPtrPtrStreamDescriptor), &lHandler);

				LOG_INVOKE_MF_METHOD(SetCurrentMediaType,
					lHandler, lCastMediaTypes[0]);												
				
				CComPtrCustom<IStreamControl> lCaptureStream = new (std::nothrow) CaptureStream(this, *aPtrPtrStreamDescriptor);

				mStreams[aStreamIdentifier] = lCaptureStream;

			} while (false);

			if (lPtrVoid != nullptr)
				CoTaskMemFree(lPtrVoid);

			if (lStreamNameString != nullptr)
				SysFreeString(lStreamNameString);
				
			return lresult;
		}

		HRESULT CaptureSource::requestSample(ISourceOperation* aPtrCommand)
		{
			HRESULT lresult;

			DWORD lStreamIdentifier(0);

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrCommand);

				LOG_INVOKE_POINTER_METHOD(aPtrCommand, getStreamIdentifier, &lStreamIdentifier);

				if (mState != SourceState::SourceStateStarted)
				{
					if (mState == SourceState::SourceStatePaused)
					{
						do
						{
							CComPtrCustom<IUnknown> lToken;

							LOG_INVOKE_POINTER_METHOD(aPtrCommand, getObject, &lToken);

							LOG_INVOKE_POINTER_METHOD(mStreams[lStreamIdentifier], queueRequestToken, lToken);

						} while (false);
					}

					lresult = S_OK;

					break;
				}

				CComPtrCustom<IUnknown> lUnkSample;

				CComPtrCustom<IMFSample> lSample;

				LOG_INVOKE_POINTER_METHOD(mCaptureProcessor, getNewSample,
					lStreamIdentifier,
					&lUnkSample);

				CComPtrCustom<IUnknown> lToken;

				aPtrCommand->getObject(&lToken);

				if (lUnkSample)
				{
					LOG_INVOKE_QUERY_INTERFACE_METHOD(lUnkSample, &lSample);

					if (lToken)
					{
						LOG_INVOKE_MF_METHOD(SetUnknown, lSample, MFSampleExtension_Token, lToken);
					}

					LOG_INVOKE_POINTER_METHOD(mStreams[lStreamIdentifier], QueueEvent,
						MEMediaSample,
						GUID_NULL,
						S_OK,
						lSample);
				}
				else
				{
					requestNewSample(lStreamIdentifier, lToken);
				}
									
			} while (false);

			if (FAILED(lresult))
			{
				do
				{
					__MIDL___MIDL_itf_mfobjects_0000_0012_0001 lMError = MEError;

					if (lresult == DXGI_ERROR_ACCESS_LOST)
					{
						lMError = MEVideoCaptureDeviceRemoved;
					}

					LOG_INVOKE_POINTER_METHOD(mStreams[lStreamIdentifier], QueueEvent,
						lMError,
						GUID_NULL,
						lresult,
						nullptr);

				} while (false);
			}
			
			return lresult;
		}

		HRESULT CaptureSource::requestNewSample(
			DWORD aStreamIdentifier,
			IUnknown* aPtrToken)
		{
			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(mCaptureProcessor);
				
				CComPtrCustom<ISourceOperation> lOperation = new (std::nothrow) SourceOperation(
					SourceOperationStreamNeedData,
					aStreamIdentifier,
					aPtrToken);

				LOG_CHECK_PTR_MEMORY(lOperation);
								
				CComPtrCustom<IUnknown> lIUnknown;

				LOG_INVOKE_QUERY_INTERFACE_METHOD(lOperation, &lIUnknown);

				LOG_INVOKE_MF_FUNCTION(MFPutWorkItem,
					mSyncWorkerQueue,
					this,
					lIUnknown);
										
			} while (false);
			
			return lresult;
		}
		
		HRESULT CaptureSource::startCapture()
		{
			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(mPresentationDescriptor);

				DWORD lStreamDescriptorCount;

				LOG_INVOKE_MF_METHOD(GetStreamDescriptorCount,
					mPresentationDescriptor,
					&lStreamDescriptorCount);
				
				LOG_CHECK_STATE_DESCR(lStreamDescriptorCount == 0, E_FAIL);

				if (mState != SourceState::SourceStatePaused)
				{
					for (size_t lStreamDescriptorIndex = 0; lStreamDescriptorIndex < lStreamDescriptorCount; lStreamDescriptorIndex++)
					{
						BOOL lSelected = FALSE;

						CComPtrCustom<IMFStreamDescriptor> lStreamDescriptor;

						LOG_INVOKE_MF_METHOD(GetStreamDescriptorByIndex,
							mPresentationDescriptor,
							lStreamDescriptorIndex,
							&lSelected,
							&lStreamDescriptor);

						if (lSelected == FALSE || !lStreamDescriptor)
						{
							continue;
						}

						CComPtrCustom<IMFMediaTypeHandler> lHandler;

						LOG_INVOKE_MF_METHOD(GetMediaTypeHandler,
							lStreamDescriptor,
							&lHandler);

						LOG_CHECK_PTR_MEMORY(lHandler);

						CComPtrCustom<IMFMediaType> lMediaType;

						LOG_INVOKE_MF_METHOD(GetCurrentMediaType,
							lHandler,
							&lMediaType);

						LOG_CHECK_PTR_MEMORY(lMediaType);

						LOG_INVOKE_POINTER_METHOD(mCaptureProcessor, setCurrentMediaType,
							lStreamDescriptorIndex,
							lMediaType);
					}

				}

				LOG_CHECK_STATE_DESCR(FAILED(lresult), lresult);

				LOG_INVOKE_POINTER_METHOD(mCaptureProcessor, start);

			} while (false);
			
			return lresult;
		}

		HRESULT CaptureSource::stopCapture()
		{
			HRESULT lresult;

			do
			{
				LOG_INVOKE_POINTER_METHOD(mCaptureProcessor, stop);

			} while (false);
			
			return lresult;
		}

		HRESULT CaptureSource::shutdownCapture()
		{
			HRESULT lresult;

			do
			{
				LOG_INVOKE_POINTER_METHOD(mCaptureProcessor, shutdown);

			} while (false);

			return lresult;
		}

		HRESULT STDMETHODCALLTYPE CaptureSource::isInitialized() const
		{			
			if (mState == SourceState::SourceStateUninitialized)
			{
				return MF_E_NOT_INITIALIZED;
			}
			else
			{
				return S_OK;
			}
		}

		HRESULT STDMETHODCALLTYPE CaptureSource::initialized()
		{
			HRESULT lresult;

			do
			{				
				LOG_INVOKE_FUNCTION(MediaFoundation::MediaFoundationManager::GetLockWorkQueue, &mSyncWorkerQueue);
				
				mState = SourceState::SourceStateInitialized;

			} while (false);

			return lresult;
		}
			
		HRESULT STDMETHODCALLTYPE CaptureSource::isStarted() const
		{
			if (mState == SourceState::SourceStateStarted)
			{
				return MF_E_NOTACCEPTING;
			}
			else
			{
				return S_OK;
			}
		}

		
	}
}