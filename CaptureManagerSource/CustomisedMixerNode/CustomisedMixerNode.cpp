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

#include "CustomisedMixerNode.h"
#include "../Common/MFHeaders.h"
#include "../Common/Common.h"
#include "../Common/GUIDs.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../VideoRendererManager/IMixerStreamPositionControl.h"
#include "MixerWrapper.h"


namespace CaptureManager
{
	namespace MediaSession
	{
		namespace CustomisedMediaSession
		{
			using namespace CaptureManager::Core;

			CustomisedMixerNode::CustomisedMixerNode(
				UINT32 aIndex,
				IMixerWrapper* aPtrIMixerWrapper):
				mIndex(aIndex)
			{
				mMixerWrapper = aPtrIMixerWrapper;
			}

			CustomisedMixerNode::~CustomisedMixerNode(){}
			
			HRESULT CustomisedMixerNode::create(
				DWORD aInputNodeAmount,
				std::vector<CComPtrCustom<IUnknown>>& aRefOutputNodes)
			{
				HRESULT lresult = E_FAIL;

				do
				{				
					CComPtrCustom<IMixerWrapper> lMixerWrapper(new MixerWrapper(aInputNodeAmount));

					for (UINT32 lIndex = 0; lIndex < aInputNodeAmount; lIndex++)
					{
						CComPtrCustom<CustomisedMixerNode> lCustomisedMixerNode(new CustomisedMixerNode(lIndex, lMixerWrapper));
											   
						CComPtrCustom<IMFTransform> lTransform;

						LOG_INVOKE_QUERY_INTERFACE_METHOD(lCustomisedMixerNode, &lTransform);
						
						CComPtrCustom<IMFTopologyNode> lCustomisedMixerTopologyNode;

						LOG_INVOKE_MF_FUNCTION(MFCreateTopologyNode,
							MF_TOPOLOGY_TYPE::MF_TOPOLOGY_TRANSFORM_NODE,
							&lCustomisedMixerTopologyNode);

						LOG_INVOKE_MF_METHOD(SetObject,
							lCustomisedMixerTopologyNode,
							lTransform);

						LOG_INVOKE_MF_METHOD(SetUINT32,
							lCustomisedMixerTopologyNode,
							CM_MixerNode,
							TRUE);						

						CComPtrCustom<IUnknown> lUnknown;

						LOG_INVOKE_QUERY_INTERFACE_METHOD(lCustomisedMixerTopologyNode, &lUnknown);

						aRefOutputNodes.push_back(lUnknown);
					}

					lresult = S_OK;

				} while (false);

				return lresult;
			}
			
			STDMETHODIMP CustomisedMixerNode::GetStreamLimits(DWORD* aPtrInputMinimum, DWORD* aPtrInputMaximum,
				DWORD* aPtrOutputMinimum, DWORD* aPtrOutputMaximum)
			{
				HRESULT lresult = E_FAIL;

				do
				{

					LOG_CHECK_STATE_DESCR(aPtrInputMinimum == NULL ||
						aPtrInputMaximum == NULL ||
						aPtrOutputMinimum == NULL ||
						aPtrOutputMaximum == NULL, E_POINTER);

					*aPtrInputMinimum = 1;

					*aPtrInputMaximum = 1;

					*aPtrOutputMinimum = 1;

					*aPtrOutputMaximum = 1;

					lresult = S_OK;

				} while (false);

				return lresult;
			}

			STDMETHODIMP CustomisedMixerNode::GetStreamIDs(DWORD aInputIDArraySize, DWORD* aPtrInputIDs,
				DWORD aOutputIDArraySize, DWORD* aPtrOutputIDs)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP CustomisedMixerNode::GetStreamCount(DWORD* aPtrInputStreams, DWORD* aPtrOutputStreams)
			{
				HRESULT lresult = E_FAIL;

				do
				{

					LOG_CHECK_STATE_DESCR(aPtrInputStreams == NULL || aPtrOutputStreams == NULL, E_POINTER);

					*aPtrInputStreams = 1;

					*aPtrOutputStreams = 1;

					lresult = S_OK;

				} while (false);

				return lresult;
			}

			STDMETHODIMP CustomisedMixerNode::GetInputStreamInfo(DWORD aInputStreamID,
				MFT_INPUT_STREAM_INFO* aPtrStreamInfo)
			{
				HRESULT lresult = S_OK;

				do
				{
					std::lock_guard<std::mutex> lock(mMutex);

					LOG_CHECK_PTR_MEMORY(aPtrStreamInfo);

					LOG_CHECK_STATE_DESCR(aInputStreamID != 0, MF_E_INVALIDSTREAMNUMBER);

					aPtrStreamInfo->dwFlags = MFT_INPUT_STREAM_WHOLE_SAMPLES |
						MFT_INPUT_STREAM_SINGLE_SAMPLE_PER_BUFFER;

					aPtrStreamInfo->cbMaxLookahead = 0;

					aPtrStreamInfo->cbAlignment = 0;

					aPtrStreamInfo->hnsMaxLatency = 0;

					aPtrStreamInfo->cbSize = 0;

				} while (false);

				return lresult;
			}

			STDMETHODIMP CustomisedMixerNode::GetOutputStreamInfo(DWORD aOutputStreamID,
				MFT_OUTPUT_STREAM_INFO* aPtrStreamInfo)
			{
				HRESULT lresult = S_OK;

				do
				{
					std::lock_guard<std::mutex> lock(mMutex);

					LOG_CHECK_PTR_MEMORY(aPtrStreamInfo);

					LOG_CHECK_STATE_DESCR(aOutputStreamID != 0, MF_E_INVALIDSTREAMNUMBER);


					aPtrStreamInfo->dwFlags =
						MFT_OUTPUT_STREAM_WHOLE_SAMPLES |
						MFT_OUTPUT_STREAM_SINGLE_SAMPLE_PER_BUFFER |
						MFT_OUTPUT_STREAM_FIXED_SAMPLE_SIZE |
						MFT_OUTPUT_STREAM_PROVIDES_SAMPLES;


					aPtrStreamInfo->cbAlignment = 0;

					aPtrStreamInfo->cbSize = 0;

				} while (false);

				return lresult;
			}

			STDMETHODIMP CustomisedMixerNode::GetInputStreamAttributes(DWORD aInputStreamID,
				IMFAttributes** aPtrPtrAttributes)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP CustomisedMixerNode::GetOutputStreamAttributes(DWORD aOutputStreamID,
				IMFAttributes** aPtrPtrAttributes)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP CustomisedMixerNode::DeleteInputStream(DWORD aStreamID)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP CustomisedMixerNode::AddInputStreams(DWORD aStreams, DWORD* aPtrStreamIDs)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP CustomisedMixerNode::GetInputAvailableType(DWORD aInputStreamID, DWORD aTypeIndex,
				IMFMediaType** aPtrPtrType)
			{
				HRESULT lresult = S_OK;
				CComPtrCustom<IMFMediaType> lMediaType;

				do
				{
					std::lock_guard<std::mutex> lock(mMutex);

					LOG_CHECK_PTR_MEMORY(aPtrPtrType);
					
				} while (false);

				return lresult;
			}

			STDMETHODIMP CustomisedMixerNode::GetOutputAvailableType(DWORD aOutputStreamID, DWORD aTypeIndex,
				IMFMediaType** aPtrPtrType)
			{
				HRESULT lresult = S_OK;
				CComPtrCustom<IMFMediaType> lMediaType;

				do
				{
					std::lock_guard<std::mutex> lock(mMutex);

					LOG_CHECK_PTR_MEMORY(aPtrPtrType);

					LOG_CHECK_PTR_MEMORY(mMixerTransform);

					LOG_INVOKE_MF_METHOD(GetOutputAvailableType, mMixerTransform, aOutputStreamID, aTypeIndex, aPtrPtrType);
									   
				} while (false);

				return lresult;
			}

			STDMETHODIMP CustomisedMixerNode::SetInputType(DWORD aInputStreamID, IMFMediaType* aPtrType,
				DWORD aFlags)
			{
				HRESULT lresult = S_OK;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrType);

					std::lock_guard<std::mutex> lock(mMutex);

					LOG_CHECK_PTR_MEMORY(mMixerWrapper);

					if (mIndex == 0)
					{												
						LOG_INVOKE_POINTER_METHOD(mMixerWrapper, initialize, aPtrType);
					}	

					if (!mMixerTransform)
					{
						LOG_INVOKE_POINTER_METHOD(mMixerWrapper, getMixer, &mMixerTransform);						
					}

					LOG_CHECK_PTR_MEMORY(mMixerTransform);

					if (mIndex == 0)
					{
						LOG_INVOKE_MF_METHOD(SetOutputType, mMixerTransform, 0, aPtrType, MFT_SET_TYPE_TEST_ONLY);

						LOG_INVOKE_MF_METHOD(SetOutputType, mMixerTransform, 0, aPtrType, 0);
					}

					LOG_INVOKE_MF_METHOD(SetInputType, mMixerTransform, mIndex, aPtrType, MFT_SET_TYPE_TEST_ONLY);

					LOG_INVOKE_MF_METHOD(SetInputType, mMixerTransform, mIndex, aPtrType, 0);

				} while (false);

				return lresult;
			}

			STDMETHODIMP CustomisedMixerNode::SetOutputType(DWORD aOutputStreamID, IMFMediaType* aPtrType,
				DWORD aFlags)
			{
				HRESULT lresult = S_OK;
				
				do
				{
					std::lock_guard<std::mutex> lock(mMutex);
					
				} while (false);

				return lresult;
			}

			STDMETHODIMP CustomisedMixerNode::GetInputCurrentType(DWORD aInputStreamID, IMFMediaType** aPtrPtrType)
			{
				HRESULT lresult = S_OK;

				do
				{
					std::lock_guard<std::mutex> lock(mMutex);

					LOG_CHECK_PTR_MEMORY(aPtrPtrType);

					LOG_CHECK_PTR_MEMORY(mMixerTransform);

					//if (mIndex == 0)
					//{
					//	LOG_INVOKE_MF_METHOD(GetOutputCurrentType, mMixerTransform, 0, aPtrPtrType);
					//}
					//else
					{
						LOG_INVOKE_MF_METHOD(GetInputCurrentType, mMixerTransform, mIndex, aPtrPtrType);
					}
					
				} while (false);

				return lresult;
			}

			STDMETHODIMP CustomisedMixerNode::GetOutputCurrentType(DWORD aOutputStreamID, IMFMediaType** aPtrPtrType)
			{
				HRESULT lresult = S_OK;

				do
				{
					std::lock_guard<std::mutex> lock(mMutex);

					LOG_CHECK_PTR_MEMORY(aPtrPtrType);

					LOG_CHECK_PTR_MEMORY(mMixerTransform);

					LOG_INVOKE_MF_METHOD(GetOutputCurrentType, mMixerTransform, 0, aPtrPtrType);

				} while (false);

				return lresult;
			}

			STDMETHODIMP CustomisedMixerNode::GetInputStatus(DWORD aInputStreamID, DWORD* aPtrFlags)
			{
				HRESULT lresult = S_OK;

				do
				{
					std::lock_guard<std::mutex> lock(mMutex);

					LOG_CHECK_PTR_MEMORY(aPtrFlags);

				} while (false);

				return lresult;
			}

			STDMETHODIMP CustomisedMixerNode::GetOutputStatus(DWORD* aPtrFlags)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP CustomisedMixerNode::SetOutputBounds(LONGLONG aLowerBound, LONGLONG aUpperBound)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP CustomisedMixerNode::ProcessEvent(DWORD aInputStreamID, IMFMediaEvent* aPtrEvent)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP CustomisedMixerNode::GetAttributes(IMFAttributes** aPtrPtrAttributes)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP CustomisedMixerNode::ProcessMessage(MFT_MESSAGE_TYPE aMessage, ULONG_PTR aParam)
			{
				HRESULT lresult = S_OK;

				do
				{
					std::lock_guard<std::mutex> lock(mMutex);

					LOG_CHECK_PTR_MEMORY(mMixerTransform);

					if (aMessage == MFT_MESSAGE_TYPE::MFT_MESSAGE_COMMAND_FLUSH)
					{
						CComPtrCustom<CaptureManager::Sinks::EVR::IMixerStreamPositionControl> lIMixerStreamPositionControl;

						mMixerTransform->QueryInterface(IID_PPV_ARGS(&lIMixerStreamPositionControl));

						if (lIMixerStreamPositionControl)
							lIMixerStreamPositionControl->flush(mIndex);

						LOG_INVOKE_MF_METHOD(ProcessMessage, mMixerTransform, aMessage, mIndex);
					}
					else
					if (mIndex == 0)
					{
						LOG_INVOKE_MF_METHOD(ProcessMessage, mMixerTransform, aMessage, aParam);
					}
					else
					{
						LOG_INVOKE_MF_METHOD(ProcessMessage, mMixerTransform, aMessage, mIndex);
					}


					

				} while (false);

				return lresult;
			}

			STDMETHODIMP CustomisedMixerNode::ProcessInput(DWORD aInputStreamID, IMFSample* aPtrSample,
				DWORD aFlags)
			{
				HRESULT lresult = S_OK;
				DWORD dwBufferCount = 0;

				do
				{
					std::lock_guard<std::mutex> lock(mMutex);

					LOG_CHECK_PTR_MEMORY(aPtrSample);

					LOG_CHECK_PTR_MEMORY(mMixerTransform);

					LOG_INVOKE_MF_METHOD(ProcessInput,
						mMixerTransform,
						mIndex,
						aPtrSample,
						0);

					if (mIndex == 0)
					{
						MFT_OUTPUT_DATA_BUFFER lBuffer;

						ZeroMemory(&lBuffer, sizeof(lBuffer));

						lBuffer.dwStreamID = 0;

						DWORD lState(0);

						LOG_INVOKE_MF_METHOD(ProcessOutput, 
							mMixerTransform,
							0,
							1,
							&lBuffer,
							&lState);

						CComPtrCustom<IMFSample> lSample(lBuffer.pSample);
												
						mSample = lSample;
					}

				} while (false);

				return lresult;
			}

			STDMETHODIMP CustomisedMixerNode::ProcessOutput(DWORD aFlags, DWORD aOutputBufferCount,
				MFT_OUTPUT_DATA_BUFFER* aPtrOutputSamples, DWORD* aPtrStatus)
			{
				HRESULT lresult = S_OK;

				do
				{
					std::lock_guard<std::mutex> lock(mMutex);

					LOG_CHECK_PTR_MEMORY(aPtrOutputSamples);

					LOG_CHECK_PTR_MEMORY(aPtrStatus);

					LOG_CHECK_STATE_DESCR(aOutputBufferCount != 1 || aFlags != 0, E_INVALIDARG);

					LOG_CHECK_STATE_DESCR(!mSample, MF_E_TRANSFORM_NEED_MORE_INPUT);

					aPtrOutputSamples[0].pSample = mSample.Detach();

					aPtrOutputSamples[0].dwStatus = 0;

					*aPtrStatus = 0;
					
				} while (false);

				return lresult;
			}
			


			// IVideoMixerControl implements

			STDMETHODIMP CustomisedMixerNode::setPosition(
				/* [in] */ FLOAT aLeft,
				/* [in] */ FLOAT aRight,
				/* [in] */ FLOAT aTop,
				/* [in] */ FLOAT aBottom)
			{

				HRESULT lresult(E_FAIL);

				do
				{
					if (mIndex == 0)
						break;

					LOG_CHECK_PTR_MEMORY(mMixerTransform);
					
					CComPtrCustom<Sinks::EVR::IMixerStreamPositionControl> lIMixerStreamPositionControl;

					LOG_INVOKE_QUERY_INTERFACE_METHOD(mMixerTransform, &lIMixerStreamPositionControl);

					LOG_CHECK_PTR_MEMORY(lIMixerStreamPositionControl);

					LOG_INVOKE_POINTER_METHOD(lIMixerStreamPositionControl, setPosition,
						mIndex,
						aLeft,
						aRight,
						aTop,
						aBottom);

				} while (false);

				return lresult;
			}

			STDMETHODIMP CustomisedMixerNode::setSrcPosition(
				/* [in] */ FLOAT aLeft,
				/* [in] */ FLOAT aRight,
				/* [in] */ FLOAT aTop,
				/* [in] */ FLOAT aBottom)
			{

				HRESULT lresult(E_FAIL);

				do
				{
					if (mIndex == 0)
						break;

					LOG_CHECK_PTR_MEMORY(mMixerTransform);

					CComPtrCustom<Sinks::EVR::IMixerStreamPositionControl> lIMixerStreamPositionControl;

					LOG_INVOKE_QUERY_INTERFACE_METHOD(mMixerTransform, &lIMixerStreamPositionControl);

					LOG_CHECK_PTR_MEMORY(lIMixerStreamPositionControl);

					LOG_INVOKE_POINTER_METHOD(lIMixerStreamPositionControl, setSrcPosition,
						mIndex,
						aLeft,
						aRight,
						aTop,
						aBottom);

				} while (false);

				return lresult;
			}

			STDMETHODIMP CustomisedMixerNode::setZOrder(
				/* [in] */ DWORD aZOrder)
			{

				HRESULT lresult(E_FAIL);

				do
				{
					if (mIndex == 0)
						break;

					LOG_CHECK_PTR_MEMORY(mMixerTransform);

					CComPtrCustom<Sinks::EVR::IMixerStreamPositionControl> lIMixerStreamPositionControl;

					LOG_INVOKE_QUERY_INTERFACE_METHOD(mMixerTransform, &lIMixerStreamPositionControl);

					LOG_CHECK_PTR_MEMORY(lIMixerStreamPositionControl);

					LOG_INVOKE_POINTER_METHOD(lIMixerStreamPositionControl, setZOrder,
						mIndex,
						aZOrder);

				} while (false);

				return lresult;
			}

			STDMETHODIMP CustomisedMixerNode::setOpacity(
				/* [in] */ FLOAT aOpacity)
			{

				HRESULT lresult(E_FAIL);

				do
				{
					if (mIndex == 0)
						break;

					LOG_CHECK_PTR_MEMORY(mMixerTransform);

					CComPtrCustom<Sinks::EVR::IMixerStreamPositionControl> lIMixerStreamPositionControl;

					LOG_INVOKE_QUERY_INTERFACE_METHOD(mMixerTransform, &lIMixerStreamPositionControl);

					LOG_CHECK_PTR_MEMORY(lIMixerStreamPositionControl);

					LOG_INVOKE_POINTER_METHOD(lIMixerStreamPositionControl, setOpacity,
						mIndex,
						aOpacity);

				} while (false);

				return lresult;
			}

			STDMETHODIMP CustomisedMixerNode::flush()
			{

				HRESULT lresult(E_FAIL);

				do
				{
					if (mIndex == 0)
						break;

					LOG_CHECK_PTR_MEMORY(mMixerTransform);

					CComPtrCustom<Sinks::EVR::IMixerStreamPositionControl> lIMixerStreamPositionControl;

					LOG_INVOKE_QUERY_INTERFACE_METHOD(mMixerTransform, &lIMixerStreamPositionControl);

					LOG_CHECK_PTR_MEMORY(lIMixerStreamPositionControl);

					LOG_INVOKE_POINTER_METHOD(lIMixerStreamPositionControl, flush,
						mIndex);

				} while (false);

				return lresult;
			}
			


			// IAudioMixerControl implements

			STDMETHODIMP CustomisedMixerNode::setRelativeVolume(
				/* [in] */ FLOAT aRelativeVolume)
			{

				HRESULT lresult(E_FAIL);

				do
				{
					if (mIndex == 0)
						break;

					LOG_CHECK_PTR_MEMORY(mMixerTransform);

					CComPtrCustom<CaptureManager::MediaSession::CustomisedMediaSession::IAudioMixerStreamControl> lAudioMixerStreamControl;

					LOG_INVOKE_QUERY_INTERFACE_METHOD(mMixerTransform, &lAudioMixerStreamControl);

					LOG_CHECK_PTR_MEMORY(lAudioMixerStreamControl);

					LOG_INVOKE_POINTER_METHOD(lAudioMixerStreamControl, setRelativeVolume,
						mIndex,
						aRelativeVolume);

				} while (false);

				return lresult;
			}
		}
	}
}