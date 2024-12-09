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

#include "VideoCaptureProcessorProxy.h"
#include "../Common/Common.h"
#include "../Common/MFHeaders.h"
#include "../Common/ComPtrCustom.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "CurrentMediaType.h"
#include "../Common/GUIDs.h"
#include "../MemoryManager/MemoryManager.h"



namespace CaptureManager
{
	namespace COMServer
	{
		using namespace Core;

		using namespace Sources;


		class SourceRequestResult :
			public BaseUnknown < ISourceRequestResult >
		{
		public:
			SourceRequestResult() :
				mStride(0),
				mHeight(0),
				mIsHardware(false),
				mIsBlocked(true),
				mPtrNewFrameCondition(nullptr)
			{}

			DWORD mStreamIndex;

			BOOL mIsKeyFrame;

			INT mStride;

			INT mSourceStride;

			DWORD mHeight;

			DWORD mWidth;

			CComPtrCustom<IMFMediaBuffer> mMediaBuffer;

			CComQIPtrCustom<IMFDXGIDeviceManager> mDeviceManager;

			CComQIPtrCustom<IMFTransform> mMixer;

			CComQIPtrCustom<ID3D11DeviceContext> mImmediateContext;

			CComQIPtrCustom<ID3D11Texture2D> mCaptureTexture;

			CComQIPtrCustom<ID3D11Texture2D> mOutputTexture;

			CComQIPtrCustom<ID3D11Texture2D> mSharedTexture;

			CComPtrCustom<IMFVideoSampleAllocator> mVideoSampleAllocator;

			std::mutex mAccessMutex;

			bool mIsHardware;

			bool mIsBlocked;

			std::condition_variable* mPtrNewFrameCondition;

			HRESULT setHardware(
				IUnknown* aPtrDeviceManager,
				IUnknown* aPtrMixer,
				IUnknown* aPtrImmediateContext,
				IUnknown* aPtrCaptureTexture,
				IUnknown* aPtrOutputTexture,
				IUnknown* aPtrSharedTexture)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrDeviceManager);
					
					LOG_CHECK_PTR_MEMORY(aPtrImmediateContext);

					LOG_CHECK_PTR_MEMORY(aPtrCaptureTexture);

					LOG_CHECK_PTR_MEMORY(aPtrOutputTexture);


					mDeviceManager = aPtrDeviceManager;

					mMixer = aPtrMixer;

					mImmediateContext = aPtrImmediateContext;

					mCaptureTexture = aPtrCaptureTexture;

					mOutputTexture = aPtrOutputTexture;

					mSharedTexture = aPtrSharedTexture;



					LOG_CHECK_PTR_MEMORY(mDeviceManager);
					
					LOG_CHECK_PTR_MEMORY(mImmediateContext);

					LOG_CHECK_PTR_MEMORY(mCaptureTexture);

					LOG_CHECK_PTR_MEMORY(mOutputTexture);


					lresult = S_OK;

					mIsHardware = true;

				} while (false);

				return lresult;
			}

			HRESULT init(IMFMediaType* aPtrOutputMediaType)
			{
				HRESULT lresult(S_OK);

				do
				{
					if (!mDeviceManager)
					{
						break;
					}

					mMediaBuffer.Release();

					if (mDeviceManager)
					{
						CComPtrCustom<IMFVideoSampleAllocatorEx> lVideoSampleAllocatorEx;

						LOG_INVOKE_MF_FUNCTION(MFCreateVideoSampleAllocatorEx,
							IID_PPV_ARGS(&lVideoSampleAllocatorEx));

						if (lVideoSampleAllocatorEx)
						{
							if (mDeviceManager)
								lVideoSampleAllocatorEx->SetDirectXManager(mDeviceManager);

							mVideoSampleAllocator.Release();


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



							//using namespace pugi;

							//xml_document lxmlDoc;

							//auto ldeclNode = lxmlDoc.append_child(node_declaration);

							//ldeclNode.append_attribute(L"version") = L"1.0";

							//xml_node lcommentNode = lxmlDoc.append_child(node_comment);

							//lcommentNode.set_value(L"XML Document of sources");

							//auto lRootXMLElement = lxmlDoc.append_child(L"Sources");
							//DataParser::readMediaType(
							//	aPtrOutputMediaType,
							//	lRootXMLElement);

							//std::wstringstream lwstringstream;

							//lxmlDoc.print(lwstringstream);

							//std::wstring lXMLDocumentString;

							//lXMLDocumentString = lwstringstream.str();

							LOG_INVOKE_MF_METHOD(InitializeSampleAllocatorEx,
								lVideoSampleAllocatorEx,
								10,
								20,
								inputAttr.get(),
								aPtrOutputMediaType);

							LOG_INVOKE_QUERY_INTERFACE_METHOD(lVideoSampleAllocatorEx, &mVideoSampleAllocator);
						}

						LOG_CHECK_PTR_MEMORY(mVideoSampleAllocator);
					}

					if (mMixer)
					{
						LOG_INVOKE_MF_FUNCTION(MFCreateDXGISurfaceBuffer,
							__uuidof(ID3D11Texture2D),
							mOutputTexture,
							0,
							FALSE,
							&mMediaBuffer);

						LOG_CHECK_PTR_MEMORY(mMediaBuffer);

						CComPtrCustom<IMFSample> lInnerSample;

						LOG_INVOKE_MF_FUNCTION(MFCreateSample,
							&lInnerSample);

						LOG_INVOKE_MF_METHOD(AddBuffer, lInnerSample,
							mMediaBuffer);

						LOG_INVOKE_MF_METHOD(ProcessInput, mMixer, 0, lInnerSample, 0);
					}

					//CComQIPtrCustom<Sinks::EVR::IMixerStreamPositionControl> lIMixerStreamPositionControl;

					//lIMixerStreamPositionControl = mMixer;

					//LOG_CHECK_PTR_MEMORY(lIMixerStreamPositionControl);

					//lIMixerStreamPositionControl->setSrcPosition(0, 0.0f, 1.0f, 0.75, 0.25f);
					//
					//lresult = S_OK;

				} while (false);

				return lresult;
			}

			HRESULT getSample(IMFSample** aPtrPtrSample)
			{
				HRESULT lresult(S_OK);

				do
				{
					if (!mVideoSampleAllocator)
					{
						break;
					}

					CComPtrCustom<IMFSample> lSample;

					LOG_INVOKE_POINTER_METHOD(mVideoSampleAllocator, AllocateSample, &lSample);

					LOG_CHECK_PTR_MEMORY(lSample);

					LOG_INVOKE_QUERY_INTERFACE_METHOD(lSample, aPtrPtrSample);

					if (mMixer)
					{
						MFT_OUTPUT_DATA_BUFFER lBuffer;

						ZeroMemory(&lBuffer, sizeof(lBuffer));

						lBuffer.dwStreamID = 0;

						lBuffer.pSample = lSample;

						DWORD lState(0);

						LOG_INVOKE_MF_METHOD(ProcessOutput, mMixer,
							0,
							1,
							&lBuffer,
							&lState);
					}
					else
					{
						CComPtrCustom<ID3D11Texture2D> lDestSurface;

						CComPtrCustom<IMFMediaBuffer> lDestBuffer;

						LOG_INVOKE_MF_METHOD(GetBufferByIndex, lSample, 0, &lDestBuffer);

						// Get the surface from the buffer.

						CComPtrCustom<IMFDXGIBuffer> lIMFDXGIBuffer;

						LOG_INVOKE_QUERY_INTERFACE_METHOD(lDestBuffer, &lIMFDXGIBuffer);

						LOG_CHECK_PTR_MEMORY(lIMFDXGIBuffer);

						lresult = lIMFDXGIBuffer->GetResource(IID_PPV_ARGS(&lDestSurface));

						if (SUCCEEDED(lresult) && lDestSurface)
						{
							mImmediateContext->CopyResource(lDestSurface, mOutputTexture);
						}
					}

				} while (false);

				return lresult;
			}

			HRESULT getMediaBuffer(IMFMediaBuffer** aPtrPtrMediaBuffer)
			{
				HRESULT lresult(S_OK);

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrPtrMediaBuffer);

					std::lock_guard<std::mutex> lLock(mAccessMutex);

					if (mMediaBuffer)
						LOG_INVOKE_QUERY_INTERFACE_METHOD(mMediaBuffer, aPtrPtrMediaBuffer);
					
					mMediaBuffer.Release();

				} while (false);

				return lresult;
			}


			virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE setData(
				/* [in] */ LPVOID aPtrData,
				/* [in] */ DWORD aByteSize,
				/* [in] */ BOOL aIsKeyFrame)override
			{
				HRESULT lresult(E_FAIL);

				do
				{

					mIsKeyFrame = aIsKeyFrame;

					if (mIsHardware)
					{
						LOG_CHECK_STATE(aPtrData != nullptr);

						CComPtrCustom<ID3D11Device> lDevice;

						mCaptureTexture->GetDevice(&lDevice);

						LOG_CHECK_PTR_MEMORY(lDevice);

						CComPtrCustom<ID3D11DeviceContext> lDeviceContext;

						lDevice->GetImmediateContext(&lDeviceContext);

						LOG_CHECK_PTR_MEMORY(lDeviceContext);

						if (mSharedTexture)
							lDeviceContext->CopyResource(mSharedTexture, mCaptureTexture);

						lresult = S_OK;
					}
					else
					{

						LOG_CHECK_PTR_MEMORY(aPtrData);

						std::lock_guard<std::mutex> lLock(mAccessMutex);

						mMediaBuffer.Release();

						LOG_INVOKE_MF_FUNCTION(MFCreateMemoryBuffer,
							aByteSize,
							&mMediaBuffer);

						BYTE* lPtrBuffer = NULL;

						DWORD lMaxLength;

						DWORD lCurrentLength;

						LOG_INVOKE_MF_METHOD(Lock, mMediaBuffer,
							&lPtrBuffer,
							&lMaxLength,
							&lCurrentLength);

						if (mSourceStride == 0)
						{
							MemoryManager::memcpy(lPtrBuffer, aPtrData, aByteSize);
						}
						else
						{
							BYTE* lPtrByte = (BYTE*)aPtrData;

							if ((mStride * mSourceStride) > 0)
							{
								lPtrByte += (mHeight - 1)*::abs(mStride);
							}

							LOG_INVOKE_MF_FUNCTION(MFCopyImage,
								lPtrBuffer,
								::abs(mStride),
								lPtrByte,
								mSourceStride,
								::abs(mStride),
								mHeight);

						}


						LOG_INVOKE_MF_METHOD(Unlock, mMediaBuffer);

						LOG_INVOKE_MF_METHOD(SetCurrentLength, mMediaBuffer, aByteSize);

						lresult = S_OK;

						if (!mIsBlocked && mPtrNewFrameCondition != nullptr)
						{
							mPtrNewFrameCondition->notify_all();
						}
					}

				} while (false);

				return lresult;
			}

			virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE getStreamIndex(
				/* [out] */ DWORD *aPtrStreamIndex)override
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrStreamIndex);

					*aPtrStreamIndex = mStreamIndex;

					lresult = S_OK;

				} while (false);

				return lresult;
			}

		private:
			virtual ~SourceRequestResult(){}
		};

		VideoCaptureProcessorProxy::VideoCaptureProcessorProxy() :
			CaptureInvoker(AVRT_PRIORITY_AvrtManager::AVRT_PRIORITY_CRITICAL_AvrtManager),
			mDiscontinuity(TRUE),
			mTickSampleTime(0),
			mSampleTime(0),
			mVideoFrameDuration(330000),
			mSleepDuration(5),
			mDeltaTimeDuration(0),
			mPrevTime(0),
			mStreamIndex(0),
			mIsHardware(false),
			mIsDirectX11(false),
			mIsBlocked(true),
			mCaptureProcessorState(CaptureProcessorState::Uninitalized)
		{
		}
				
		HRESULT VideoCaptureProcessorProxy::init(ICaptureProcessor* aPtrICaptureProcessor, InitilaizeCaptureSource* aPtrInitilaizeCaptureSource)
		{
			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrICaptureProcessor);

				LOG_CHECK_PTR_MEMORY(aPtrInitilaizeCaptureSource);

				mICaptureProcessor = aPtrICaptureProcessor;

				LOG_CHECK_PTR_MEMORY(mICaptureProcessor);

				mInitilaizeCaptureSource = aPtrInitilaizeCaptureSource;

				LOG_CHECK_PTR_MEMORY(mInitilaizeCaptureSource);

				mSourceRequestResult = new (std::nothrow) SourceRequestResult();

				LOG_CHECK_PTR_MEMORY(mSourceRequestResult);

				CComPtrCustom<IMFDXGIDeviceManager> lDeviceManager;

				mInitilaizeCaptureSource->getDeviceManager(&lDeviceManager);

				if (lDeviceManager)
				{
					mIsDirectX11 = true;

					mDeviceManager = lDeviceManager;
				}

				CComPtrCustom<IMFTransform> lMixer;

				mInitilaizeCaptureSource->getMixer(&lMixer);



				CComPtrCustom<ID3D11DeviceContext> lImmediateContext;

				mInitilaizeCaptureSource->getImmediateContext(&lImmediateContext);


				CComPtrCustom<ID3D11Texture2D> lCaptureTexture;

				mInitilaizeCaptureSource->getCaptureTexture(&lCaptureTexture);


				CComPtrCustom<ID3D11Texture2D> lOutputTexture;

				mInitilaizeCaptureSource->getOutputTexture(&lOutputTexture);


				CComQIPtrCustom<ID3D11Texture2D> lSharedTexture;

				mInitilaizeCaptureSource->getSharedTexture(&lSharedTexture);




				if (lDeviceManager && lImmediateContext && lCaptureTexture && lOutputTexture)
				{
					((SourceRequestResult*)mSourceRequestResult.get())->setHardware(
						lDeviceManager,
						lMixer,
						lImmediateContext,
						lCaptureTexture,
						lOutputTexture,
						lSharedTexture);

					mIsHardware = true;
				}

				mCaptureProcessorState = Stopped;

				lresult = S_OK;

			} while (false);

			return lresult;
		}

		// IInnerCaptureProcessor methods

		// get friendly name of Processor
		HRESULT VideoCaptureProcessorProxy::getFrendlyName(
			BSTR* aPtrString)
		{
			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrString);

				*aPtrString = SysAllocString(L"Capture Processor");

				lresult = S_OK;

			} while (false);

			return lresult;
		}

		// get synbolicLink of Processor
		HRESULT VideoCaptureProcessorProxy::getSymbolicLink(
			BSTR* aPtrString)
		{
			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrString);

				*aPtrString = SysAllocString(L"Capture Processor");

				lresult = S_OK;

			} while (false);

			return lresult;
		}

		// get supported amount of streams
		HRESULT VideoCaptureProcessorProxy::getStreamAmount(
			UINT32* aPtrStreamAmount)
		{
			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrStreamAmount);

				LOG_CHECK_PTR_MEMORY(mInitilaizeCaptureSource);

				*aPtrStreamAmount = mInitilaizeCaptureSource->mStreams.size();

				lresult = S_OK;

			} while (false);

			return lresult;
		}

		// get supported media types of stream
		HRESULT VideoCaptureProcessorProxy::getMediaTypes(
			UINT32 aStreamIndex,
			BSTR* aPtrStreamNameString,
			IUnknown*** aPtrPtrPtrMediaType,
			UINT32* aPtrMediaTypeCount)
		{
			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrStreamNameString);

				LOG_CHECK_PTR_MEMORY(aPtrPtrPtrMediaType);

				LOG_CHECK_PTR_MEMORY(aPtrMediaTypeCount);

				auto lFindItr = mInitilaizeCaptureSource->mStreams.find(aStreamIndex);

				LOG_CHECK_STATE(lFindItr == mInitilaizeCaptureSource->mStreams.end());

				std::vector<CComPtrCustom<IMFMediaType>> lMediaTypes;

				for (auto& lItem : (*lFindItr).second)
				{
					if (lItem.second)
						lMediaTypes.push_back(lItem.second);
				}

				auto lMediaTypeSize = lMediaTypes.size();

				LOG_CHECK_STATE_DESCR(lMediaTypeSize == 0, E_FAIL);

				decltype(lMediaTypeSize) lMediaTypeCount = 0;

				auto lPtrVoid = CoTaskMemAlloc(sizeof(IUnknown*)* lMediaTypeSize);

				LOG_CHECK_PTR_MEMORY(lPtrVoid);

				IUnknown** lPtrPtrMediaTypes = (IUnknown **)(lPtrVoid);

				LOG_CHECK_PTR_MEMORY(lPtrPtrMediaTypes);

				for (decltype(lMediaTypeSize) lMediaTypeIndex = 0; lMediaTypeIndex < lMediaTypeSize; lMediaTypeIndex++)
				{
					CComPtrCustom<IMFMediaType> lMediaType;

					lMediaType = lMediaTypes[lMediaTypeIndex];

					lPtrPtrMediaTypes[lMediaTypeIndex] = lMediaType.detach();

					lMediaTypeCount++;
				}

				//if (FAILED(lresult))
				//{
				//	for (decltype(lMediaTypeCount) lMediaTypeIndex = 0; lMediaTypeIndex < lMediaTypeCount; lMediaTypeIndex++)
				//	{
				//		lPtrPtrMediaTypes[lMediaTypeIndex]->Release();
				//	}

				//	CoTaskMemFree(lPtrPtrMediaTypes);

				//	break;
				//}

				*aPtrPtrPtrMediaType = lPtrPtrMediaTypes;

				*aPtrMediaTypeCount = lMediaTypeCount;



				lresult = S_OK;

			} while (false);

			return lresult;
		}

		// set supported media type to stream
		HRESULT VideoCaptureProcessorProxy::setCurrentMediaType(
			UINT32 aStreamIndex,
			IUnknown* aPtrMediaType)
		{
			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrMediaType);

				LOG_CHECK_PTR_MEMORY(mICaptureProcessor);

				LOG_CHECK_PTR_MEMORY(mInitilaizeCaptureSource);

				CComQIPtrCustom<IMFMediaType> lMediaType = aPtrMediaType;

				LOG_CHECK_PTR_MEMORY(lMediaType);

				auto lFindItr = mInitilaizeCaptureSource->mStreams.find(aStreamIndex);

				LOG_CHECK_STATE(lFindItr == mInitilaizeCaptureSource->mStreams.end());

				DWORD lMediaTypeIndex = 0;

				for (auto& litem : (*lFindItr).second)
				{
					BOOL l_bResult = FALSE;

					LOG_INVOKE_MF_METHOD(Compare, litem.second,
						lMediaType,
						MF_ATTRIBUTES_MATCH_TYPE::MF_ATTRIBUTES_MATCH_INTERSECTION,
						&l_bResult);

					if (l_bResult == TRUE)
					{
						lMediaTypeIndex = litem.first;

						INT32 lStride(0);

						HRESULT lr = litem.second->GetUINT32(
							MF_MT_DEFAULT_STRIDE,
							((UINT32*)&lStride));

						if (SUCCEEDED(lr))
						{
							if (mSourceRequestResult)
								((SourceRequestResult*)mSourceRequestResult.get())->mStride = lStride;
						}



						UINT32 lWidth = 0;
						UINT32 lHeight = 0;

						lr = MFGetAttributeSize(
							litem.second,
							MF_MT_FRAME_SIZE,
							&lWidth,
							&lHeight);

						if (SUCCEEDED(lr))
						{
							if (mSourceRequestResult)
							{
								((SourceRequestResult*)mSourceRequestResult.get())->mHeight = lHeight;

								((SourceRequestResult*)mSourceRequestResult.get())->mWidth = lWidth;
							}
						}

						mWidth = lWidth;

						mHeight = lHeight;


						lr = litem.second->GetUINT32(
							CM_SourceStride,
							((UINT32*)&lStride));

						if (SUCCEEDED(lr))
						{
							if (mSourceRequestResult)
								((SourceRequestResult*)mSourceRequestResult.get())->mSourceStride = lStride;
						}
						else
						{
							if (mSourceRequestResult)
								((SourceRequestResult*)mSourceRequestResult.get())->mSourceStride = 0;
						}


						UINT32 lNumerator = 0;

						UINT32 lDenomerator = 0;

						lr = MFGetAttributeRatio(
							litem.second,
							MF_MT_FRAME_RATE,
							&lNumerator,
							&lDenomerator);

						if (SUCCEEDED(lr))
						{
							UINT64 lAverTime = 0;

							lr = MediaFoundation::MediaFoundationManager::MFFrameRateToAverageTimePerFrame(
								lNumerator,
								lDenomerator,
								&lAverTime);

							if (SUCCEEDED(lr))
							{
								mVideoFrameDuration = lAverTime;

								mSleepDuration = (mVideoFrameDuration / 10000) / 5;
							}
						}

						break;
					}
				}

				UINT32 lIsDirectCall = FALSE;

				lresult = lMediaType->GetUINT32(
					CM_DIRECT_CALL,
					&lIsDirectCall);
				
				if (SUCCEEDED(lresult))
				{
					mIsBlocked = lIsDirectCall == FALSE;

					if (mSourceRequestResult)
						((SourceRequestResult*)mSourceRequestResult.get())->mIsBlocked = mIsBlocked;


					if (mSourceRequestResult)
						((SourceRequestResult*)mSourceRequestResult.get())->mPtrNewFrameCondition = &mNewFrameCondition;
				}


				CComPtrCustom<CurrentMediaType> lCurrentMediaType(new (std::nothrow)CurrentMediaType);

				LOG_CHECK_PTR_MEMORY(lCurrentMediaType);

				lCurrentMediaType->mStreamIndex = aStreamIndex;

				mStreamIndex = aStreamIndex;

				((SourceRequestResult*)mSourceRequestResult.get())->mStreamIndex = mStreamIndex;

				lCurrentMediaType->mCurrentMediaType = aPtrMediaType;

				lCurrentMediaType->mMediaTypeIndex = lMediaTypeIndex;

				lresult = mICaptureProcessor->setCurrentMediaType(lCurrentMediaType);

				((SourceRequestResult*)mSourceRequestResult.get())->init(lMediaType);

			} while (false);

			return lresult;
		}
		
		// get new sample of stream
		HRESULT VideoCaptureProcessorProxy::getNewSample(
			DWORD aStreamIdentifier,
			IUnknown** ppSample)
		{

			HRESULT lresult = E_FAIL;

			do
			{

				LOG_CHECK_PTR_MEMORY(ppSample);

				LOG_CHECK_STATE(aStreamIdentifier != mStreamIndex);

				MFTIME lSampleTime;

				MFTIME lSampleDuration;

				CComPtrCustom<IMFSample> lSample;

				LOG_INVOKE_MF_FUNCTION(MFCreateSample, &lSample);

				LOG_CHECK_PTR_MEMORY(lSample);

				if (mIsHardware)
				{
					{
						
						std::unique_lock<std::mutex> lLock(mAccessBufferQueueMutex);

						auto lconditionResult = mNewFrameCondition.wait_for(
							lLock,
							std::chrono::seconds(1));

						if (lconditionResult == std::cv_status::timeout)
						{
							break;
						}

						if (mCaptureProcessorState == Stopped)
							break;

						CComPtrCustom<IMFSample> lTempSample;

						((SourceRequestResult*)mSourceRequestResult.get())->getSample(&lTempSample);

						CComPtrCustom<IMFMediaBuffer> lInputBuffer;

						LOG_INVOKE_MF_METHOD(GetBufferByIndex, lTempSample, 0, &lInputBuffer);




						LOG_CHECK_PTR_MEMORY(lSample);

						LOG_INVOKE_MF_METHOD(AddBuffer, lSample, lInputBuffer);

					}
				}
				else
				{
					CComPtrCustom<IMFMediaBuffer> lMediaBuffer;

					{
						std::unique_lock<std::mutex> lLock(mNewFrameMutex);

						auto lconditionResult = mNewFrameCondition.wait_for(
							lLock,
							std::chrono::seconds(1));					
						
						if (lconditionResult == std::cv_status::timeout)
						{
							break;
						}

						if (mCaptureProcessorState == Stopped)
							break;

						((SourceRequestResult*)mSourceRequestResult.get())->getMediaBuffer(&lMediaBuffer);
					}

					if (lMediaBuffer)
						LOG_INVOKE_MF_METHOD(AddBuffer, lSample, lMediaBuffer);
				}

				MFTIME lCurrentTime = MediaFoundation::MediaFoundationManager::MFGetSystemTime();

				lSampleDuration = lCurrentTime - mTickSampleTime;

				mTickSampleTime = lCurrentTime;

				lSampleTime = mSampleTime;

				mSampleTime += lSampleDuration;


				LOG_INVOKE_MF_METHOD(SetSampleDuration, lSample, lSampleDuration);

				LOG_INVOKE_MF_METHOD(SetSampleTime, lSample, lSampleTime);

				LOG_INVOKE_MF_METHOD(SetUINT64, lSample, MFSampleExtension_DeviceTimestamp, lSampleTime);

				LOG_INVOKE_MF_METHOD(SetUINT32, lSample, MFSampleExtension_BottomFieldFirst, TRUE);

				LOG_INVOKE_MF_METHOD(SetUINT32, lSample, MFSampleExtension_CleanPoint, TRUE);

				LOG_INVOKE_MF_METHOD(SetUINT32, lSample, MFSampleExtension_Interlaced, TRUE);

				LOG_INVOKE_MF_METHOD(SetUINT32, lSample, MFSampleExtension_Discontinuity, mDiscontinuity);

				mDiscontinuity = FALSE;

				*ppSample = lSample.Detach();

			} while (false);

			return lresult;
		}

		// start processor
		HRESULT VideoCaptureProcessorProxy::start()
		{
			HRESULT lresult(E_FAIL);

			do
			{
				if (mCaptureProcessorState == Stopped ||
					mCaptureProcessorState == Paused)
				{
					LOG_CHECK_PTR_MEMORY(mICaptureProcessor);

					lresult = mICaptureProcessor->start(
						0,
						GUID_NULL);

					mDeltaTimeDuration = 0;

					if (mCaptureProcessorState == Stopped)
					{
						mSampleTime = 0;
					}

					MFTIME lCurrentTime = MediaFoundation::MediaFoundationManager::MFGetSystemTime();

					mTickSampleTime = lCurrentTime;

					mPrevTime = lCurrentTime;

					mDiscontinuity = TRUE;

					mCaptureProcessorState = Started;

					CaptureInvoker::start();
				}

			} while (false);

			return lresult;
		}

		// stop processor
		HRESULT VideoCaptureProcessorProxy::stop()
		{
			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_PTR_MEMORY(mICaptureProcessor);

				lresult = mICaptureProcessor->stop();

				mCaptureProcessorState = Stopped;

				CaptureInvoker::stop();

				Sleep(100);

			} while (false);

			return lresult;
		}

		// pause processor
		HRESULT VideoCaptureProcessorProxy::pause()
		{
			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_PTR_MEMORY(mICaptureProcessor);

				lresult = mICaptureProcessor->pause();

				mCaptureProcessorState = Paused;

				CaptureInvoker::stop();

			} while (false);

			return lresult;
		}

		// restart processor
		HRESULT VideoCaptureProcessorProxy::restart()
		{
			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_PTR_MEMORY(mICaptureProcessor);

				lresult = mICaptureProcessor->stop();

				lresult = mICaptureProcessor->start(
					0,
					GUID_NULL);

			} while (false);

			return lresult;
		}

		// shutdown processor
		HRESULT VideoCaptureProcessorProxy::shutdown()
		{
			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_PTR_MEMORY(mICaptureProcessor);

				stop();

				lresult = mICaptureProcessor->shutdown();

				mCaptureProcessorState = Shutdown;

			} while (false);

			return lresult;
		}

		HRESULT VideoCaptureProcessorProxy::registerMediaEvent(IUnknown* pUnkIMediaEvent)
		{
			return E_NOTIMPL;
		}





		// IInnerGetService
		HRESULT STDMETHODCALLTYPE VideoCaptureProcessorProxy::GetService(
			REFGUID aRefGUIDService,
			REFIID aRefIID,
			LPVOID* aPtrPtrObject)
		{
			HRESULT lresult(E_NOTIMPL);

			do
			{
				if (!mIsDirectX11)
					break;

				LOG_CHECK_PTR_MEMORY(aPtrPtrObject);

				if (aRefGUIDService == CM_DeviceManager)
				{
					if (aRefIID == __uuidof(IMFDXGIDeviceManager))
					{
						LOG_INVOKE_WIDE_QUERY_INTERFACE_METHOD(mDeviceManager,
							aRefIID,
							aPtrPtrObject);
					}
				}

			} while (false);

			return lresult;
		}
			

		// CaptureInvoker implementation

		HRESULT STDMETHODCALLTYPE VideoCaptureProcessorProxy::invoke()
		{

			if (SUCCEEDED(execute()))
				return S_OK;

			auto lCurrentTime = MediaFoundation::MediaFoundationManager::MFGetSystemTime();

			auto ldif = lCurrentTime - mPrevTime;

			if ((ldif + mDeltaTimeDuration) >= mVideoFrameDuration)
			{
				mPrevTime = lCurrentTime;

				mDeltaTimeDuration = (ldif + mDeltaTimeDuration) - mVideoFrameDuration;

				update();
			}
			else
				std::this_thread::sleep_for(std::chrono::milliseconds(mSleepDuration));

			return S_OK;
		}
		
		void VideoCaptureProcessorProxy::update()
		{
			HRESULT lresult(E_FAIL);

			do
			{


				std::lock_guard<std::mutex> lLock(mAccessBufferQueueMutex);

				LOG_CHECK_PTR_MEMORY(mSourceRequestResult);

				LOG_INVOKE_POINTER_METHOD(mICaptureProcessor, sourceRequest,
					mSourceRequestResult);

				if (mIsBlocked)
					mNewFrameCondition.notify_all();

			} while (false);

		}
		
		HRESULT VideoCaptureProcessorProxy::execute()
		{
			HRESULT lresult(E_NOTIMPL);

			do
			{

			} while (false);

			return lresult;
		}

		VideoCaptureProcessorProxy::~VideoCaptureProcessorProxy()
		{
		}
	}
}