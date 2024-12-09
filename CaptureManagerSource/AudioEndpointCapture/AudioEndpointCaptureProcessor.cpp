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

#include "AudioEndpointCaptureProcessor.h"
#include "../Common/Common.h"
#include "../Common/MFHeaders.h"
#include "../Common/ComPtrCustom.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../MemoryManager/MemoryManager.h"

#include <Functiondiscoverykeys_devpkey.h>
#include <Audioclient.h>
#include <mmdeviceapi.h>

namespace CaptureManager
{
	namespace Sources
	{
		namespace AudioEndpointCapture
		{
			using namespace Core;

			AudioEndpointCaptureProcessor::AudioEndpointCaptureProcessor() :
				CaptureInvoker(AVRT_PRIORITY_AvrtManager::AVRT_PRIORITY_CRITICAL_AvrtManager, L"Pro Audio"),
				//CaptureInvoker(AVRT_PRIORITY_AvrtManager::AVRT_PRIORITY_CRITICAL_AvrtManager, L"Audio"),
				mAudioClient(nullptr),
				mPtrAudioCaptureClient(nullptr),
				mPtrMMDevice(nullptr),
				mState(SourceState::SourceStateUninitialized),	
				mDeltaTimeDuration(0),
				mReleaseAudioClientLock(false),
				mSampleDuration(400000),
				mSilenceDuration(mSampleDuration / 10000),
				mBufferOffset(0),
				mPrevSampleTime(0),
				mSilenceBlock(FALSE),
				mIsSilenceBlock(FALSE),
				mAudioSamplesReadyEvent(NULL),
				mShutdownEvent(NULL),
				mInvokeMode(INVOKEMODE::SLEEP)
			{
			}
			
			// CaptureInvoker implementation

			HRESULT STDMETHODCALLTYPE AudioEndpointCaptureProcessor::starting()
			{
				mPrevTime = MediaFoundation::MediaFoundationManager::MFGetSystemTime();

				mWaitArray[0] = { mShutdownEvent };
				mWaitArray[1] = { mAudioSamplesReadyEvent };

				return S_OK;
			}

			HRESULT STDMETHODCALLTYPE AudioEndpointCaptureProcessor::invoke()
			{
				HRESULT lresult(E_NOTIMPL);

				mReleaseAudioClientLock = true;

				do
				{

					if (mState != SourceState::SourceStateStarted)
					{
						Sleep(10);

						lresult = S_OK;

						break;
					}

					if(mInvokeMode == INVOKEMODE::EVENT)
					{
						DWORD lWaitResult = WaitForMultipleObjects(2, mWaitArray, FALSE, mSilenceDuration);
						switch (lWaitResult)
						{
						case WAIT_OBJECT_0 + 0:
						default:
						{
							goto SHUTDOWN_LABLE;
						}
						break;
						case WAIT_OBJECT_0 + 1:
						{
							UINT32 lNextPacketSize = 0;

							lresult = mPtrAudioCaptureClient->GetNextPacketSize(&lNextPacketSize);

							if (FAILED(lresult))
								lNextPacketSize = 0;

							lresult = S_OK;

							if (lNextPacketSize > 0)
							{
								BYTE* lPtrData;

								UINT32 lNumFramesToRead = 0;

								DWORD lFlags(0);

								lresult = mPtrAudioCaptureClient->GetBuffer(
									&lPtrData,
									&lNumFramesToRead,
									&lFlags,
									NULL,
									NULL
								);

								if (FAILED(lresult))
								{
									lNumFramesToRead = 0;
								}
								else
								{
									if (lFlags & AUDCLNT_BUFFERFLAGS_SILENT)
									{
										mIsSilenceBlock = TRUE;

										mWrittenBufferSize = 0;

										mNewSampleCondition.notify_all();
									}
									else
									{
										if (lNumFramesToRead > 0)
										{
											auto lwrittenByte = lNumFramesToRead * mBlockAlign;

											writeAudioBuffer(lPtrData,
												lwrittenByte);

											mWrittenBufferSize += lwrittenByte;

											mIsSilenceBlock = FALSE;
										}
									}
								}

								lresult = mPtrAudioCaptureClient->ReleaseBuffer(lNumFramesToRead);

							}
							
							if (mWrittenBufferSize >= mExpectedBufferSize)
							{
								mIsSilenceBlock = FALSE;

								mNewSampleCondition.notify_all();

								mPrevTime = MediaFoundation::MediaFoundationManager::MFGetSystemTime();

								mWrittenBufferSize = 0;
							}
						}
						break;

						case WAIT_TIMEOUT:
						{
							mIsSilenceBlock = TRUE;

							mWrittenBufferSize = 0;

							mNewSampleCondition.notify_all();
						}
						break;
											   
						}

					SHUTDOWN_LABLE:
						lresult = S_OK;
						break;
					}
					else if (mInvokeMode == INVOKEMODE::SLEEP)
					{
						auto lCurrentTime = MediaFoundation::MediaFoundationManager::MFGetSystemTime();

						auto ldif = lCurrentTime - mPrevTime;

						if ((ldif + mDeltaTimeDuration) >= mSampleDuration)
						{
							mPrevTime = lCurrentTime;

							mDeltaTimeDuration = (ldif + mDeltaTimeDuration) - mSampleDuration;
							
							mIsSilenceBlock = mSilenceBlock;

							mSilenceBlock = TRUE;

							mNewSampleCondition.notify_all();
						}

						UINT32 lNextPacketSize = 0;

						lresult = mPtrAudioCaptureClient->GetNextPacketSize(&lNextPacketSize);

						if (FAILED(lresult))
							lNextPacketSize = 0;

						lresult = S_OK;

						if (lNextPacketSize > 0)
						{
							BYTE* lPtrData;

							UINT32 lNumFramesToRead = 0;

							DWORD lFlags(0);

							lresult = mPtrAudioCaptureClient->GetBuffer(
								&lPtrData,
								&lNumFramesToRead,
								&lFlags,
								NULL,
								NULL
							);

							if (FAILED(lresult))
							{
								lNumFramesToRead = 0;
							}
							else
							{
								if (lFlags & AUDCLNT_BUFFERFLAGS_SILENT)
								{
									mSilenceBlock = TRUE;
								}
								else
								{
									if (lNumFramesToRead > 0)
									{
										writeAudioBuffer(lPtrData,
											lNumFramesToRead * mBlockAlign);

										mSilenceBlock = FALSE;
									}
								}
							}

							lresult = mPtrAudioCaptureClient->ReleaseBuffer(lNumFramesToRead);
						}

						Sleep(mMillTickTime);
					}
					
					lresult = S_OK;

				} while (false);

				mReleaseAudioClientLock = false;

				mReleaseAudioClientCondition.notify_one();

				return lresult;
			}


			//IInnerCaptureProcessor implementation
			
			HRESULT AudioEndpointCaptureProcessor::getFrendlyName(
				BSTR* aPtrString)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrString);

					std::lock_guard<std::mutex> lLock(mAccessMutex);

					*aPtrString = SysAllocString(mFriendlyName.c_str());

					lresult = S_OK;

				} while (false);

				return lresult;
			}

			HRESULT AudioEndpointCaptureProcessor::getSymbolicLink(
				BSTR* aPtrString)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrString);

					std::lock_guard<std::mutex> lLock(mAccessMutex);

					auto SymbolicLink = std::wstring(L"CaptureManager///Software///Sources///AudioEndpointCapture///") + mSymbolicLink;
					
					*aPtrString = SysAllocString(SymbolicLink.c_str());

					lresult = S_OK;

				} while (false);

				return lresult;
			}

			HRESULT AudioEndpointCaptureProcessor::getStreamAmount(
				UINT32* aPtrStreamAmount)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrStreamAmount);

					std::lock_guard<std::mutex> lLock(mAccessMutex);

					*aPtrStreamAmount = 1;

					lresult = S_OK;

				} while (false);

				return lresult;
			}

			HRESULT AudioEndpointCaptureProcessor::getMediaTypes(
				UINT32 aStreamIndex,
				BSTR* aPtrStreamNameString,
				IUnknown*** aPtrPtrPtrMediaType,
				UINT32* aPtrMediaTypeCount)
			{
				HRESULT lresult;

				do
				{
					std::lock_guard<std::mutex> lLock(mAccessMutex);
					
					LOG_CHECK_PTR_MEMORY(aPtrPtrPtrMediaType);
					
					LOG_CHECK_PTR_MEMORY(aPtrMediaTypeCount);

					LOG_CHECK_STATE(aStreamIndex > 0);
										
					auto lMediaTypeSize = mVectorAudioCaptureConfigs.size();

					LOG_CHECK_STATE(lMediaTypeSize == 0);

					decltype(lMediaTypeSize) lMediaTypeCount = 0;
										
					auto lPtrVoid = CoTaskMemAlloc(sizeof(IUnknown*)* lMediaTypeSize);

					LOG_CHECK_PTR_MEMORY(lPtrVoid);

					IUnknown** lPtrPtrMediaTypes = (IUnknown**)(lPtrVoid);

					for (decltype(lMediaTypeSize) lMediaTypeIndex = 0; lMediaTypeIndex < lMediaTypeSize; lMediaTypeIndex++)
					{
						CComPtrCustom<IMFMediaType> lMediaType;

						LOG_INVOKE_FUNCTION(createAudioMediaType,
							mVectorAudioCaptureConfigs[lMediaTypeIndex],
							&lMediaType);

						lPtrPtrMediaTypes[lMediaTypeIndex] = lMediaType.detach();
						
						lMediaTypeCount++;
					}

					if (FAILED(lresult))
					{
						for (decltype(lMediaTypeCount) lMediaTypeIndex = 0; lMediaTypeIndex < lMediaTypeCount; lMediaTypeIndex++)
						{
							try
							{
								lPtrPtrMediaTypes[lMediaTypeIndex]->Release();
							}
							catch (...)
							{

							}
						}

						CoTaskMemFree(lPtrPtrMediaTypes);

						break;
					}

					*aPtrPtrPtrMediaType = lPtrPtrMediaTypes;

					*aPtrMediaTypeCount = lMediaTypeCount;

				} while (false);

				return lresult;
			}

			HRESULT AudioEndpointCaptureProcessor::setCurrentMediaType(
				UINT32 aStreamIndex,
				IUnknown* aPtrMediaType)
			{
				HRESULT lresult;

				do
				{
					std::lock_guard<std::mutex> lLock(mAccessMutex);

					LOG_CHECK_STATE(aStreamIndex > 0);

					LOG_CHECK_STATE(mState != SourceState::SourceStateInitialized
						&& mState != SourceState::SourceStateStopped);

					LOG_CHECK_PTR_MEMORY(aPtrMediaType);

					CComPtrCustom<IMFMediaType> lCurrentMediaType;

					LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrMediaType, &lCurrentMediaType);

					auto lMediaTypeSize = mVectorAudioCaptureConfigs.size();
										
					BOOL lCompareResult = FALSE;

					decltype(lMediaTypeSize) lSelectedMediaTypeIndex = 0;

					for (decltype(lMediaTypeSize) lMediaTypeIndex = 0; 
						lMediaTypeIndex < lMediaTypeSize; 
						lMediaTypeIndex++)
					{
						CComPtrCustom<IMFMediaType> lMediaType;

						LOG_INVOKE_FUNCTION(createAudioMediaType,
							mVectorAudioCaptureConfigs[lMediaTypeIndex],
							&lMediaType);


						//using namespace pugi;

						//xml_document lxmlDoc;

						//auto ldeclNode = lxmlDoc.append_child(node_declaration);

						//ldeclNode.append_attribute(L"version") = L"1.0";

						//xml_node lcommentNode = lxmlDoc.append_child(node_comment);

						//lcommentNode.set_value(L"XML Document of sources");

						//auto lRootXMLElement = lxmlDoc.append_child(L"mediaType");

						//auto lDownMT = lRootXMLElement.append_child(L"lMediaType");

						//DataParser::readMediaType(
						//	lMediaType,
						//	lDownMT);

						//lDownMT = lRootXMLElement.append_child(L"lCurrentMediaType");

						//DataParser::readMediaType(
						//	lCurrentMediaType,
						//	lDownMT);

						//std::wstringstream lwstringstream;

						//lxmlDoc.print(lwstringstream);

						//std::wstring lXMLDocumentString;

						//lXMLDocumentString = lwstringstream.str();

						
						LOG_INVOKE_MF_METHOD(Compare,
							lCurrentMediaType,
							lMediaType,
							MF_ATTRIBUTES_MATCH_ALL_ITEMS,
							&lCompareResult);
						
						if (lCompareResult == TRUE)
						{
							lSelectedMediaTypeIndex = lMediaTypeIndex;

							break;
						}

					}

					if (FAILED(lresult))
					{
						break;
					}

					LOG_CHECK_STATE(lCompareResult == FALSE)

					mCurrentAudioCaptureConfig = mVectorAudioCaptureConfigs[lSelectedMediaTypeIndex];

				} while (false);

				return lresult;
			}

			HRESULT AudioEndpointCaptureProcessor::getNewSample(
				DWORD aStreamIdentifier,
				IUnknown** aPtrPtrSample)
			{
				HRESULT lresult;

				do
				{

					LOG_CHECK_PTR_MEMORY(aPtrPtrSample);

					LOG_CHECK_STATE(aStreamIdentifier > 0);
					
					CComPtrCustom<IMFSample> lSample;
					
					LOG_INVOKE_MF_FUNCTION(MFCreateSample, &lSample);
										
					LONGLONG lSampleTime = 0;

					LONGLONG lSampleDuration = mSampleDuration;

					CComPtrCustom<IMFMediaBuffer> lMediaBuffer;
										
					{
						std::unique_lock<std::mutex> lLock(mAccessBufferQueueMutex);

						auto lconditionResult = mNewSampleCondition.wait_for(lLock, std::chrono::seconds(4));

						LOG_CHECK_STATE(lconditionResult == std::cv_status::timeout);
						
						if (!popMediaBuffer(
							&lMediaBuffer))
						{
							lMediaBuffer.Release();

							LOG_INVOKE_MF_FUNCTION(MFCreateMemoryBuffer, mExpectedBufferSize, &lMediaBuffer);

							LOG_CHECK_PTR_MEMORY(lMediaBuffer);

							DWORD lMaxLength = 0;

							DWORD lCurrentLength = 0;

							BYTE* lPtrBuffer = nullptr;

							LOG_INVOKE_MF_METHOD(Lock, lMediaBuffer, &lPtrBuffer, &lMaxLength, &lCurrentLength);

							LOG_CHECK_PTR_MEMORY(lPtrBuffer);

							ZeroMemory(lPtrBuffer, lMaxLength);

							LOG_INVOKE_MF_METHOD(Unlock, lMediaBuffer);

							LOG_INVOKE_MF_METHOD(SetCurrentLength, lMediaBuffer, mExpectedBufferSize);
						}

						if (mInvokeMode == INVOKEMODE::EVENT)
						{
							lSampleTime = mPrevSampleTime;

							mPrevSampleTime = lSampleTime + mSampleDuration;
						}
						else
						{
							lSampleTime = MediaFoundation::MediaFoundationManager::MFGetSystemTime();

							if (mPrevSampleTime > 0)
							{
								lSampleDuration = lSampleTime - mPrevSampleTime;
							}

							mPrevSampleTime = lSampleTime;
						}
					}
										
					LOG_INVOKE_MF_METHOD(AddBuffer, lSample, lMediaBuffer);

					LOG_INVOKE_MF_METHOD(SetSampleDuration, lSample, lSampleDuration);

					LOG_INVOKE_MF_METHOD(SetSampleTime, lSample, lSampleTime);

					LOG_INVOKE_MF_METHOD(SetUINT64, lSample, MFSampleExtension_DeviceTimestamp, lSampleTime);
															
					LOG_CHECK_STATE(mState == SourceState::SourceStateStopped);

					*aPtrPtrSample = lSample.Detach();

				} while (false);
				
				return lresult;
			}

			HRESULT AudioEndpointCaptureProcessor::start()
			{
				HRESULT lresult;

				do
				{
					std::lock_guard<std::mutex> lLock(mAccessMutex);

					LOG_CHECK_STATE_DESCR(isUninitialized(), MF_E_AUDIO_RECORDING_DEVICE_INVALIDATED);
					
					LOG_INVOKE_FUNCTION(checkShutdown);
					
					if (mState == SourceState::SourceStateStarted)
					{
						lresult = S_OK;

						break;
					}

					mDeltaTimeDuration = 0;

					mPrevSampleTime = 0;

					clearMediaBuffer();

					if (mState == SourceState::SourceStatePaused)
					{						
						mState = SourceState::SourceStateStarted;

						if (mShutdownEvent != NULL) 
							ResetEvent(mShutdownEvent);

						CaptureInvoker::start();

						LOG_CHECK_PTR_MEMORY(mAudioClient);

						LOG_INVOKE_POINTER_METHOD(mAudioClient, Start);

						lresult = S_OK;

						break;
					}
					
					lresult = initializeAudioClient();

					if (FAILED(lresult))
					{
						mState = SourceState::SourceStateUninitialized;

						break;
					}

					LOG_CHECK_PTR_MEMORY(mAudioClient);
							
					UINT32 lAudioClientBufferSampleSize;

					LOG_INVOKE_POINTER_METHOD(mAudioClient, GetBufferSize, &lAudioClientBufferSampleSize);
										
					mBlockAlign = mCurrentAudioCaptureConfig.mPWAVEFORMATEX->nBlockAlign;

					mSampleDuration = mCurrentAudioCaptureConfig.mDefaultDevicePeriod * 4;

					mSilenceDuration = mSampleDuration / 10000;
										
					mExpectedBufferSize = (UINT32)((((LONGLONG)mCurrentAudioCaptureConfig.mPWAVEFORMATEX->nSamplesPerSec) * ((LONGLONG)mBlockAlign)* mSampleDuration) / 10000000LL);
					
					mWrittenBufferSize = 0;
					
					mMillTickTime = (DWORD)((mCurrentAudioCaptureConfig.mDefaultDevicePeriod >> 1) / 15000LL);
										
					LOG_INVOKE_POINTER_METHOD(mAudioClient,GetService,
						IID_PPV_ARGS(&mPtrAudioCaptureClient));
										
					LOG_INVOKE_FUNCTION(allocateBuffer);

					CaptureInvoker::start();
											
					mState = SourceState::SourceStateStarted;

					LOG_INVOKE_POINTER_METHOD(mAudioClient, Start);

				} while (false);

				return lresult;
			}

			HRESULT AudioEndpointCaptureProcessor::allocateBuffer()
			{
				HRESULT lresult;

				do
				{
					mCurrentMediaBuffer.Release();

					LOG_INVOKE_MF_FUNCTION(MFCreateMemoryBuffer, mExpectedBufferSize, &mCurrentMediaBuffer);

					LOG_CHECK_PTR_MEMORY(mCurrentMediaBuffer);

					LOG_INVOKE_MF_METHOD(SetCurrentLength, mCurrentMediaBuffer, mExpectedBufferSize);

					mBufferOffset = 0;

				} while (false);

				return lresult;
			}

			HRESULT AudioEndpointCaptureProcessor::stop()
			{
				HRESULT lresult;

				do
				{
					std::lock_guard<std::mutex> lLock(mAccessMutex);

					CaptureInvoker::stop([this]() {if (this->mShutdownEvent != NULL) SetEvent(this->mShutdownEvent); });
					
					if (mShutdownEvent)
					{
						CloseHandle(mShutdownEvent);
						mShutdownEvent = NULL;
					}

					if (mAudioSamplesReadyEvent)
					{
						CloseHandle(mAudioSamplesReadyEvent);
						mAudioSamplesReadyEvent = NULL;
					}

					LOG_CHECK_STATE_DESCR(isUninitialized(), MF_E_AUDIO_RECORDING_DEVICE_INVALIDATED);

					LOG_INVOKE_FUNCTION(checkShutdown);
					
					LOG_CHECK_PTR_MEMORY(mAudioClient);

					LOG_INVOKE_POINTER_METHOD(mAudioClient, Stop);

					if (mState != SourceState::SourceStateStarted &&
						mState != SourceState::SourceStatePaused)
					{							
						break;
					}

					mState = SourceState::SourceStateStopped;

					LOG_INVOKE_FUNCTION(releaseAudioClient);
					
					lresult = S_OK;

				} while (false);

				return lresult;
			}

			HRESULT AudioEndpointCaptureProcessor::pause()
			{
				HRESULT lresult;

				do
				{
					std::lock_guard<std::mutex> lLock(mAccessMutex);
					
					LOG_CHECK_STATE_DESCR(isUninitialized(), MF_E_AUDIO_RECORDING_DEVICE_INVALIDATED);

					LOG_INVOKE_FUNCTION(checkShutdown);

					LOG_CHECK_PTR_MEMORY(mAudioClient);

					LOG_INVOKE_POINTER_METHOD(mAudioClient, Stop);

					if (mState != SourceState::SourceStateStarted)
					{						
						break;
					}
										
					CaptureInvoker::stop([this]() {if (this->mShutdownEvent != NULL) SetEvent(this->mShutdownEvent); });
					
					mState = SourceState::SourceStatePaused;
					
				} while (false);


				return lresult;
			}

			HRESULT AudioEndpointCaptureProcessor::restart()
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_STATE_DESCR(isUninitialized(), MF_E_AUDIO_RECORDING_DEVICE_INVALIDATED);

					LOG_INVOKE_FUNCTION(checkShutdown);

					LOG_CHECK_PTR_MEMORY(mAudioClient);

					if (mState != SourceState::SourceStatePaused)
					{
						break;
					}

					LOG_INVOKE_FUNCTION(start);
					
				} while (false);

				return lresult;
			}

			HRESULT AudioEndpointCaptureProcessor::shutdown()
			{
				HRESULT lresult;

				do
				{
					std::lock_guard<std::mutex> lLock(mAccessMutex);

					LOG_CHECK_STATE_DESCR(isUninitialized(), MF_E_AUDIO_RECORDING_DEVICE_INVALIDATED);

					LOG_INVOKE_FUNCTION(checkShutdown);

					CaptureInvoker::stop();
					
					releaseAudioClient();
					
					mState = SourceState::SourceStateShutdown;

					lresult = S_OK;

				} while (false);

				return lresult;
			}

			HRESULT AudioEndpointCaptureProcessor::registerMediaEvent(IUnknown* pUnkIMediaEvent)
			{
				return E_NOTIMPL;
			}

			HRESULT STDMETHODCALLTYPE AudioEndpointCaptureProcessor::checkShutdown() const
			{
				HRESULT lresult = S_OK;

				if (mState == SourceState::SourceStateShutdown)
				{
					lresult = MF_E_SHUTDOWN;
				}

				return lresult;
			}
			
			void AudioEndpointCaptureProcessor::pushMediaBuffer(
				IMFMediaBuffer* aPtrMediaBuffer)
			{				
				std::lock_guard<std::mutex> lLock(mAccessBufferQueueMutex);

				if (mBufferQueue.size() > 4)
				{
					mBufferQueue.front().mPtrMediaBuffer->Release();

					mBufferQueue.pop();
				}

				BufferContainer lBufferContainer;

				lBufferContainer.mPtrMediaBuffer = aPtrMediaBuffer;

				mBufferQueue.push(lBufferContainer);
			}

			bool AudioEndpointCaptureProcessor::popMediaBuffer(
				IMFMediaBuffer** aPtrPtrMediaBuffer)
			{
				bool lboolresult = false;

				HRESULT lresult(E_FAIL);

				do
				{
					if (mBufferQueue.empty())
					{
						break;
					}

					auto lbuff = mBufferQueue.front();

					if (mIsSilenceBlock != FALSE)
					{
						DWORD lMaxLength = 0;

						DWORD lCurrentLength = 0;

						BYTE* lPtrBuffer = nullptr;

						LOG_CHECK_PTR_MEMORY(lbuff.mPtrMediaBuffer);

						LOG_INVOKE_MF_METHOD(Lock, lbuff.mPtrMediaBuffer, &lPtrBuffer, &lMaxLength, &lCurrentLength);

						LOG_CHECK_PTR_MEMORY(lPtrBuffer);

						ZeroMemory(lPtrBuffer, lCurrentLength);

						LOG_INVOKE_MF_METHOD(Unlock, lbuff.mPtrMediaBuffer);
					}	

					*aPtrPtrMediaBuffer = lbuff.mPtrMediaBuffer;

					mBufferQueue.pop();

					lboolresult = true;

				} while (false);

				return lboolresult;
			}

			void AudioEndpointCaptureProcessor::clearMediaBuffer()
			{
				std::lock_guard<std::mutex> lLock(mAccessBufferQueueMutex);

				while (!mBufferQueue.empty())
				{
					mBufferQueue.front().mPtrMediaBuffer->Release();

					mBufferQueue.pop();
				}
			}

			// AudioEndpointCaptureProcessor interface

			HRESULT AudioEndpointCaptureProcessor::init(
				IMMDevice* aPtrMMDevice,
				std::wstring aSymbolicLink)
			{
				HRESULT lresult;

				PROPVARIANT pv;

				PropVariantInit(&pv);

				do
				{
					std::lock_guard<std::mutex> lLock(mAccessMutex);

					LOG_CHECK_PTR_MEMORY(aPtrMMDevice);

					CComPtrCustom<IPropertyStore> lPropertyStore;

					LOG_INVOKE_POINTER_METHOD(aPtrMMDevice, OpenPropertyStore, STGM_READ, &lPropertyStore);
					
					LOG_INVOKE_POINTER_METHOD(lPropertyStore, GetValue, PKEY_Device_FriendlyName, &pv);
					
					LOG_CHECK_STATE(VT_LPWSTR != pv.vt);
					
					mFriendlyName = std::wstring(pv.pwszVal);

					if (aSymbolicLink.empty())
					{
						LPWSTR lPtrStrId;

						lresult = aPtrMMDevice->GetId(&lPtrStrId);

						if (FAILED(lresult))
						{
							break;
						}

						if (lPtrStrId == nullptr)
						{
							lresult = E_POINTER;

							break;
						}

						mSymbolicLink = std::wstring(lPtrStrId);

						CoTaskMemFree(lPtrStrId);

					}
					else
					{
						mSymbolicLink = aSymbolicLink;
					}
					
					LOG_INVOKE_FUNCTION(fillVectorAudioCaptureConfigs, aPtrMMDevice, mVectorAudioCaptureConfigs);
					
					LOG_CHECK_STATE_DESCR(mVectorAudioCaptureConfigs.size() == 0, E_BOUNDS);
					
					mCurrentAudioCaptureConfig = mVectorAudioCaptureConfigs[0];

					mState = SourceState::SourceStateInitialized;

					LOG_INVOKE_FUNCTION(PropVariantClear, &pv);

					LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrMMDevice, &mPtrMMDevice);					

				} while (false);
				
				return lresult;
			}

			bool AudioEndpointCaptureProcessor::isUninitialized()
			{
				return mState == SourceState::SourceStateUninitialized;
			}
			
			HRESULT AudioEndpointCaptureProcessor::createAudioMediaType(
				AudioCaptureConfig aAudioCaptureConfig,
				IMFMediaType** aPtrPtrMediaType)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrPtrMediaType);
					
					CComPtrCustom<IMFAudioMediaType> lAudioMediaType;

					LOG_INVOKE_MF_FUNCTION(MFCreateAudioMediaType,
						aAudioCaptureConfig.mPWAVEFORMATEX,
						&lAudioMediaType);

					LOG_CHECK_PTR_MEMORY(lAudioMediaType);

					CComQIPtrCustom<IMFMediaType> lMediaType = (IUnknown*)lAudioMediaType;
					
					LOG_CHECK_PTR_MEMORY(lMediaType)
					
					*aPtrPtrMediaType = lMediaType.detach();

				} while (false);

				return lresult;
			}

			HRESULT AudioEndpointCaptureProcessor::fillVectorAudioCaptureConfigs(
				IMMDevice* aPtrMMDevice, 
				std::vector<AudioCaptureConfig>& aVectorAudioCaptureConfigs)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrMMDevice);

					CComPtrCustom<IAudioClient> lAudioClient;

					LOG_INVOKE_POINTER_METHOD(aPtrMMDevice, Activate,
						__uuidof(IAudioClient),
						CLSCTX_ALL, NULL,
						(void**)&lAudioClient);
					
					LOG_CHECK_PTR_MEMORY(lAudioClient);
					
					AudioCaptureConfig lAudioCaptureConfig;

					lAudioCaptureConfig.mPWAVEFORMATEX = nullptr;

					REFERENCE_TIME lminTime;

					LOG_INVOKE_POINTER_METHOD(lAudioClient, GetDevicePeriod,
						&lAudioCaptureConfig.mDefaultDevicePeriod, 
						&lminTime);
					
					WAVEFORMATEX* lPtrWAVEFORMATEX = nullptr;

					LOG_INVOKE_POINTER_METHOD(lAudioClient, GetMixFormat, &lPtrWAVEFORMATEX);

					LOG_CHECK_PTR_MEMORY(lPtrWAVEFORMATEX);
					
					lAudioCaptureConfig.mWAVEFORMATEX = *lPtrWAVEFORMATEX;

					lAudioCaptureConfig.mPWAVEFORMATEX = lPtrWAVEFORMATEX;

					if (lPtrWAVEFORMATEX->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
					{
						PWAVEFORMATEXTENSIBLE lPtrEx = reinterpret_cast<PWAVEFORMATEXTENSIBLE>(lPtrWAVEFORMATEX);

						if (IsEqualGUID(KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, lPtrEx->SubFormat))
						{
							lAudioCaptureConfig.mWAVEFORMATEX.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
						}
						else if (IsEqualGUID(KSDATAFORMAT_SUBTYPE_PCM, lPtrEx->SubFormat))
						{
							lAudioCaptureConfig.mWAVEFORMATEX.wFormatTag = WAVE_FORMAT_PCM;
						}
					}

					if (lAudioCaptureConfig.mWAVEFORMATEX.nChannels != 2 && lAudioCaptureConfig.mWAVEFORMATEX.nChannels > 2)
					{
						auto ldiffChannels = lAudioCaptureConfig.mWAVEFORMATEX.nBlockAlign / (2 * lAudioCaptureConfig.mWAVEFORMATEX.wBitsPerSample / 8);

						lAudioCaptureConfig.mWAVEFORMATEX.nChannels = 2;

						lAudioCaptureConfig.mWAVEFORMATEX.nBlockAlign = lAudioCaptureConfig.mWAVEFORMATEX.nChannels * lAudioCaptureConfig.mWAVEFORMATEX.wBitsPerSample / 8;

						lAudioCaptureConfig.mWAVEFORMATEX.nAvgBytesPerSec = lAudioCaptureConfig.mWAVEFORMATEX.nBlockAlign * lAudioCaptureConfig.mWAVEFORMATEX.nSamplesPerSec;

						WAVEFORMATEX* pWAVEFORMATEX = NULL;

						lresult = lAudioClient->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, &lAudioCaptureConfig.mWAVEFORMATEX, &pWAVEFORMATEX);

						if (pWAVEFORMATEX != nullptr)
						{
							if (lAudioCaptureConfig.mPWAVEFORMATEX != nullptr)
								CoTaskMemFree(lAudioCaptureConfig.mPWAVEFORMATEX);

							lAudioCaptureConfig.mWAVEFORMATEX = *pWAVEFORMATEX;

							lAudioCaptureConfig.mPWAVEFORMATEX = pWAVEFORMATEX;

							if (lPtrWAVEFORMATEX->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
							{
								PWAVEFORMATEXTENSIBLE lPtrEx = reinterpret_cast<PWAVEFORMATEXTENSIBLE>(lPtrWAVEFORMATEX);

								if (IsEqualGUID(KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, lPtrEx->SubFormat))
								{
									lAudioCaptureConfig.mWAVEFORMATEX.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
								}
								else if (IsEqualGUID(KSDATAFORMAT_SUBTYPE_PCM, lPtrEx->SubFormat))
								{
									lAudioCaptureConfig.mWAVEFORMATEX.wFormatTag = WAVE_FORMAT_PCM;
								}
							}
						}
					}
										
					aVectorAudioCaptureConfigs.push_back(lAudioCaptureConfig);

				} while (false);

				return lresult;
			}

			HRESULT AudioEndpointCaptureProcessor::initializeAudioClient()
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(mPtrMMDevice);
					
					LOG_INVOKE_POINTER_METHOD(mPtrMMDevice, Activate,
						__uuidof(IAudioClient),
						CLSCTX_ALL, NULL,
						(void**)&mAudioClient);
					
					LOG_CHECK_PTR_MEMORY(mAudioClient);

					do
					{
						//WAVEFORMATEX* pWAVEFORMATEX = NULL;

						//LOG_INVOKE_POINTER_METHOD(mAudioClient, IsFormatSupported,
						//		AUDCLNT_SHAREMODE_SHARED,
						//		mCurrentAudioCaptureConfig.mPWAVEFORMATEX,
						//		&pWAVEFORMATEX
						//	);

						LOG_INVOKE_POINTER_METHOD(mAudioClient, Initialize,
							AUDCLNT_SHAREMODE_SHARED,
							AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_LOOPBACK,
							0,
							0,
							mCurrentAudioCaptureConfig.mPWAVEFORMATEX,
							0
						);
												
						mShutdownEvent = CreateEventEx(NULL, NULL, 0, EVENT_MODIFY_STATE | SYNCHRONIZE);

						LOG_CHECK_STATE_DESCR(mShutdownEvent == NULL, E_NOT_VALID_STATE);

						mAudioSamplesReadyEvent = CreateEventEx(NULL, NULL, 0, EVENT_MODIFY_STATE | SYNCHRONIZE);

						LOG_CHECK_STATE_DESCR(mAudioSamplesReadyEvent == NULL, E_NOT_VALID_STATE);

						LOG_INVOKE_POINTER_METHOD(mAudioClient, SetEventHandle, mAudioSamplesReadyEvent);

						mInvokeMode = INVOKEMODE::EVENT;

					} while (false);

					if (FAILED(lresult))
					{
						if (lresult == AUDCLNT_E_UNSUPPORTED_FORMAT)
						{

						}
						else
						{

							LOG_INVOKE_POINTER_METHOD(mAudioClient, Initialize,
								AUDCLNT_SHAREMODE_SHARED,
								AUDCLNT_STREAMFLAGS_LOOPBACK,
								0,
								0,
								mCurrentAudioCaptureConfig.mPWAVEFORMATEX,
								0
							);

							mInvokeMode = INVOKEMODE::SLEEP;
						}
					}
					
				} while (false);

				return lresult;
			}

			HRESULT AudioEndpointCaptureProcessor::releaseAudioClient()
			{
				std::unique_lock<std::mutex> lLock(mReleaseAudioClientMutex);

				mReleaseAudioClientCondition.wait(
					lLock,
					[this]{return !mReleaseAudioClientLock; });

				HRESULT lresult = S_OK;

				if (mPtrAudioCaptureClient != nullptr)
					mPtrAudioCaptureClient->Release();

				if (mAudioClient != nullptr)
					mAudioClient->Release();

				mAudioClient = nullptr;

				mPtrAudioCaptureClient = nullptr;

				return lresult;
			}

			void AudioEndpointCaptureProcessor::writeAudioBuffer(
				BYTE* aPtrData,
				UINT32 aBufferByteLength)
			{			
				HRESULT lresult;

				do
				{					
					BYTE* lPtrBuffer = nullptr;

					LOG_CHECK_PTR_MEMORY(mCurrentMediaBuffer);

					LOG_INVOKE_MF_METHOD(Lock, mCurrentMediaBuffer, &lPtrBuffer, nullptr, nullptr);
					
					LOG_CHECK_PTR_MEMORY(lPtrBuffer);

					auto lavalableSize = mExpectedBufferSize - mBufferOffset;

					auto lmaxCount = lavalableSize >= aBufferByteLength ? aBufferByteLength : lavalableSize;
					
					if (aPtrData != nullptr)
						MemoryManager::memcpy(lPtrBuffer + mBufferOffset, aPtrData, lmaxCount);

					LOG_INVOKE_MF_METHOD(Unlock, mCurrentMediaBuffer);

					mBufferOffset += lmaxCount;

					if (mBufferOffset >= mExpectedBufferSize)
					{
						pushMediaBuffer(mCurrentMediaBuffer.detach());

						allocateBuffer();

						if (aBufferByteLength > lmaxCount)
						{
							writeAudioBuffer(aPtrData + lmaxCount, aBufferByteLength - lmaxCount);
						}
					}

				} while (false);
			}

			AudioEndpointCaptureProcessor::~AudioEndpointCaptureProcessor()
			{
				releaseAudioClient();

				if (mPtrMMDevice != nullptr)
					mPtrMMDevice->Release();

				for (auto lItem : mVectorAudioCaptureConfigs)
				{
					if (lItem.mPWAVEFORMATEX != nullptr)
						CoTaskMemFree(lItem.mPWAVEFORMATEX);
				}

				mVectorAudioCaptureConfigs.clear();
			}
		}
	}
}