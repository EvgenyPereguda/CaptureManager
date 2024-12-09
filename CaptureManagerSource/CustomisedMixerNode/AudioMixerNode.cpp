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

#include "AudioMixerNode.h"
#include "../Common/Common.h"
#include "../Common/GUIDs.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include <memory>



namespace CaptureManager
{
	namespace MediaSession
	{
		namespace CustomisedMediaSession
		{
			namespace Mixer
			{
				using namespace CaptureManager::Core;

				static float s_scale = 1.0f;

				AudioMixerNode::AudioMixerNode():
					m_PtrCurrentRelativeVolumes(nullptr),
					m_PtrPrevRelativeVolumes(nullptr)
				{
				}

				AudioMixerNode::~AudioMixerNode()
				{
				}

				HRESULT AudioMixerNode::create(
					DWORD aInputNodeAmount,
					IMFTransform** aPtrPtrTransform)
				{
					HRESULT lresult = E_FAIL;

					do
					{

						CComPtrCustom<IMFTransform> lAudioMixerNode(new AudioMixerNode());										

						std::unique_ptr<DWORD[]> lInputIDStream;

						lInputIDStream.reset(new DWORD[aInputNodeAmount]);

						for (size_t i = 0; i < aInputNodeAmount; i++)
						{
							lInputIDStream[i] = i;
						}
												
						LOG_INVOKE_POINTER_METHOD(lAudioMixerNode, AddInputStreams,
							aInputNodeAmount,
							lInputIDStream.get());
											   
						LOG_INVOKE_QUERY_INTERFACE_METHOD(lAudioMixerNode, aPtrPtrTransform);

					} while (false);

					return lresult;
				}
				
				STDMETHODIMP AudioMixerNode::GetStreamLimits(DWORD* aPtrInputMinimum, DWORD* aPtrInputMaximum,
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

						*aPtrInputMaximum = m_InputStreams.size();

						*aPtrOutputMinimum = 1;

						*aPtrOutputMaximum = 1;

						lresult = S_OK;

					} while (false);

					return lresult;
				}

				STDMETHODIMP AudioMixerNode::GetStreamIDs(DWORD aInputIDArraySize, DWORD* aPtrInputIDs,
					DWORD aOutputIDArraySize, DWORD* aPtrOutputIDs)
				{
					HRESULT lresult = E_FAIL;

					do
					{

						LOG_CHECK_STATE_DESCR(aPtrInputIDs == nullptr ||
							aPtrOutputIDs == nullptr, E_POINTER);

						auto lMaxInputIDArraySize = m_InputStreams.size();

						if (lMaxInputIDArraySize > aInputIDArraySize)
							lMaxInputIDArraySize = aInputIDArraySize;

						for (size_t i = 0; i < lMaxInputIDArraySize; i++)
						{
							aPtrInputIDs[i] = i;
						}

						size_t lMaxOutputIDArraySize = 1;

						if (lMaxOutputIDArraySize > aOutputIDArraySize)
							lMaxOutputIDArraySize = aOutputIDArraySize;

						for (size_t i = 0; i < lMaxOutputIDArraySize; i++)
						{
							aPtrOutputIDs[i] = 0;
						}

						lresult = S_OK;

					} while (false);

					return lresult;
				}

				STDMETHODIMP AudioMixerNode::GetStreamCount(DWORD* aPtrInputStreams, DWORD* aPtrOutputStreams)
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

				STDMETHODIMP AudioMixerNode::GetInputStreamInfo(DWORD aInputStreamID,
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

				STDMETHODIMP AudioMixerNode::GetOutputStreamInfo(DWORD aOutputStreamID,
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

				STDMETHODIMP AudioMixerNode::GetInputStreamAttributes(DWORD aInputStreamID,
					IMFAttributes** aPtrPtrAttributes)
				{
					return E_NOTIMPL;
				}

				STDMETHODIMP AudioMixerNode::GetOutputStreamAttributes(DWORD aOutputStreamID,
					IMFAttributes** aPtrPtrAttributes)
				{
					return E_NOTIMPL;
				}

				STDMETHODIMP AudioMixerNode::DeleteInputStream(DWORD aStreamID)
				{
					return E_NOTIMPL;
				}

				STDMETHODIMP AudioMixerNode::AddInputStreams(DWORD aStreams, DWORD* aPtrStreamIDs)
				{
					HRESULT lresult = E_FAIL;

					do
					{
						LOG_CHECK_STATE_DESCR(
							aPtrStreamIDs == nullptr,
							E_POINTER);

						std::lock_guard<std::mutex> lock(mMutex);		
						
						FLOAT lNomRelativeVolume = s_scale * 1.0f / ((float)aStreams);

						mFirstRelativeVolumes = std::make_unique<RelativeVolumes>();

						mFirstRelativeVolumes->mRelativeVolumes = new float[aStreams];



						mSecondRelativeVolumes = std::make_unique<RelativeVolumes>();

						mSecondRelativeVolumes->mRelativeVolumes = new float[aStreams];


						m_PtrCurrentRelativeVolumes = mFirstRelativeVolumes.get();
						
						m_PtrPrevRelativeVolumes = mSecondRelativeVolumes.get();

												
						for (DWORD i = 0; i < aStreams; i++)
						{
							auto lID = aPtrStreamIDs[i];

							StreamInfo lStreamInfo;

							mFirstRelativeVolumes->mRelativeVolumes[i] = lNomRelativeVolume;

							mSecondRelativeVolumes->mRelativeVolumes[i] = lNomRelativeVolume;

							lStreamInfo.mIndex = i;
							
							m_InputStreams[lID] = lStreamInfo;
						}

						lresult = S_OK;

					} while (false);

					return lresult;
				}

				STDMETHODIMP AudioMixerNode::GetInputAvailableType(DWORD aInputStreamID, DWORD aTypeIndex,
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

				STDMETHODIMP AudioMixerNode::GetOutputAvailableType(DWORD aOutputStreamID, DWORD aTypeIndex,
					IMFMediaType** aPtrPtrType)
				{
					HRESULT lresult = E_FAIL;
					CComPtrCustom<IMFMediaType> lMediaType;

					do
					{
						std::lock_guard<std::mutex> lock(mMutex);

						LOG_CHECK_PTR_MEMORY(aPtrPtrType);

						LOG_CHECK_PTR_MEMORY(mOutputMediaType);
						
						LOG_INVOKE_QUERY_INTERFACE_METHOD(mOutputMediaType, aPtrPtrType);

					} while (false);

					return lresult;
				}

				STDMETHODIMP AudioMixerNode::SetInputType(DWORD aInputStreamID, IMFMediaType* aPtrType,
					DWORD aFlags)
				{
					HRESULT lresult = S_OK;

					do
					{
						LOG_CHECK_PTR_MEMORY(aPtrType);

						std::lock_guard<std::mutex> lock(mMutex);


					} while (false);

					return lresult;
				}

				STDMETHODIMP AudioMixerNode::SetOutputType(DWORD aOutputStreamID, IMFMediaType* aPtrType,
					DWORD aFlags)
				{
					HRESULT lresult = S_OK;

					do
					{
						LOG_CHECK_PTR_MEMORY(aPtrType);

						LOG_CHECK_STATE_DESCR(aOutputStreamID != 0, E_INVALIDARG);

						std::lock_guard<std::mutex> lock(mMutex);

						GUID lMajorType = GUID_NULL;

						LOG_INVOKE_MF_METHOD(GetGUID,
							aPtrType,
							MF_MT_MAJOR_TYPE,
							&lMajorType);

						LOG_CHECK_STATE_DESCR(lMajorType != MFMediaType_Audio, MF_E_INVALIDMEDIATYPE);

						mOutputMediaType.Release();
						

						LOG_INVOKE_MF_FUNCTION(MFCreateMediaType,
							&mOutputMediaType);

						LOG_CHECK_PTR_MEMORY(mOutputMediaType);

						LOG_INVOKE_MF_METHOD(CopyAllItems,
							aPtrType,
							mOutputMediaType);
												
						GUID lSubType;

						LOG_INVOKE_MF_METHOD(GetGUID,
							mOutputMediaType,
							MF_MT_SUBTYPE,
							&lSubType);

						if (lSubType != MFAudioFormat_PCM)
						{
							LOG_INVOKE_MF_METHOD(SetGUID,
								mOutputMediaType,
								MF_MT_SUBTYPE,
								MFAudioFormat_PCM);
						}

						UINT32 lvalue = 0;

						LOG_INVOKE_MF_METHOD(GetUINT32,
							mOutputMediaType,
							MF_MT_AUDIO_BITS_PER_SAMPLE,
							&lvalue);

						if (lvalue != 16)
						{
							LOG_INVOKE_MF_METHOD(SetUINT32,
								mOutputMediaType,
								MF_MT_AUDIO_BITS_PER_SAMPLE,
								16);

							LOG_INVOKE_MF_METHOD(SetUINT32,
								mOutputMediaType,
								MF_MT_AUDIO_VALID_BITS_PER_SAMPLE,
								16);
						}


						LOG_INVOKE_MF_METHOD(GetUINT32,
							mOutputMediaType,
							MF_MT_AUDIO_NUM_CHANNELS,
							&lvalue);

						if (lvalue != 1 && lvalue != 2)
						{
							lvalue = 2;
						}

						LOG_INVOKE_MF_METHOD(SetUINT32,
							mOutputMediaType,
							MF_MT_AUDIO_NUM_CHANNELS,
							lvalue);

						LOG_INVOKE_MF_METHOD(SetUINT32,
							mOutputMediaType,
							MF_MT_AUDIO_CHANNEL_MASK,
							lvalue == 2 ? 0x3 : 0x4);

						LOG_INVOKE_MF_METHOD(SetUINT32,
							mOutputMediaType,
							MF_MT_AUDIO_BLOCK_ALIGNMENT,
							lvalue * 2);

						LOG_INVOKE_MF_METHOD(SetUINT32,
							mOutputMediaType,
							MF_MT_AUDIO_PREFER_WAVEFORMATEX,
							TRUE);


						UINT32 lSamplesPerSecondValue = 0;

						LOG_INVOKE_MF_METHOD(GetUINT32,
							mOutputMediaType,
							MF_MT_AUDIO_SAMPLES_PER_SECOND,
							&lSamplesPerSecondValue);

						LOG_INVOKE_MF_METHOD(SetUINT32,
							mOutputMediaType,
							MF_MT_AUDIO_AVG_BYTES_PER_SECOND,
							lSamplesPerSecondValue * lvalue * 2);

						auto lIter = m_InputStreams.find(0);

						LOG_CHECK_STATE_DESCR(lIter == m_InputStreams.end(), MF_E_INVALIDSTREAMNUMBER);

						(*lIter).second.mInputMediaType = mOutputMediaType;

					} while (false);

					return lresult;
				}

				STDMETHODIMP AudioMixerNode::GetInputCurrentType(DWORD aInputStreamID, IMFMediaType** aPtrPtrType)
				{
					HRESULT lresult = S_OK;

					do
					{
						std::lock_guard<std::mutex> lock(mMutex);

						LOG_CHECK_PTR_MEMORY(aPtrPtrType);

						LOG_CHECK_PTR_MEMORY(mOutputMediaType);

						LOG_INVOKE_QUERY_INTERFACE_METHOD(mOutputMediaType, aPtrPtrType);



						auto lIter = m_InputStreams.find(aInputStreamID);

						LOG_CHECK_STATE_DESCR(lIter == m_InputStreams.end(), MF_E_INVALIDSTREAMNUMBER);

						(*lIter).second.mInputMediaType = mOutputMediaType;
						
					} while (false);

					return lresult;
				}

				STDMETHODIMP AudioMixerNode::GetOutputCurrentType(DWORD aOutputStreamID, IMFMediaType** aPtrPtrType)
				{
					HRESULT lresult = S_OK;

					do
					{
						std::lock_guard<std::mutex> lock(mMutex);

						LOG_CHECK_PTR_MEMORY(aPtrPtrType);

						LOG_CHECK_PTR_MEMORY(mOutputMediaType);

						LOG_INVOKE_QUERY_INTERFACE_METHOD(mOutputMediaType, aPtrPtrType);

					} while (false);

					return lresult;
				}

				STDMETHODIMP AudioMixerNode::GetInputStatus(DWORD aInputStreamID, DWORD* aPtrFlags)
				{
					HRESULT lresult = S_OK;

					do
					{
						std::lock_guard<std::mutex> lock(mMutex);

						LOG_CHECK_PTR_MEMORY(aPtrFlags);

					} while (false);

					return lresult;
				}

				STDMETHODIMP AudioMixerNode::GetOutputStatus(DWORD* aPtrFlags)
				{
					return E_NOTIMPL;
				}

				STDMETHODIMP AudioMixerNode::SetOutputBounds(LONGLONG aLowerBound, LONGLONG aUpperBound)
				{
					return E_NOTIMPL;
				}

				STDMETHODIMP AudioMixerNode::ProcessEvent(DWORD aInputStreamID, IMFMediaEvent* aPtrEvent)
				{
					return E_NOTIMPL;
				}

				STDMETHODIMP AudioMixerNode::GetAttributes(IMFAttributes** aPtrPtrAttributes)
				{
					return E_NOTIMPL;
				}

				STDMETHODIMP AudioMixerNode::ProcessMessage(MFT_MESSAGE_TYPE aMessage, ULONG_PTR aParam)
				{
					HRESULT lresult = S_OK;

					do
					{
						std::lock_guard<std::mutex> lock(mMutex);

						if (aMessage == MFT_MESSAGE_NOTIFY_END_STREAMING)
						{
							auto lIter = m_InputStreams.find(aParam);

							if (lIter != m_InputStreams.end())
							{
								(*lIter).second.mAllowRead = false;

								(*lIter).second.mInputMediaType.Release();

								(*lIter).second.mStreamSamples.clear();
							}
						}
						
					} while (false);

					return lresult;
				}

				STDMETHODIMP AudioMixerNode::ProcessInput(DWORD aInputStreamID, IMFSample* aPtrSample,
					DWORD aFlags)
				{
					HRESULT lresult = S_OK;
					DWORD dwBufferCount = 0;

					do
					{
						LOG_CHECK_PTR_MEMORY(aPtrSample);

						std::lock_guard<std::mutex> lock(mMutex);

						auto lIter = m_InputStreams.find(aInputStreamID);

						LOG_CHECK_STATE_DESCR(lIter == m_InputStreams.end(), MF_E_INVALIDSTREAMNUMBER);

						LOG_CHECK_STATE_DESCR(!(*lIter).second.mInputMediaType, MF_E_NOTACCEPTING);

						LOG_CHECK_STATE_DESCR(!mOutputMediaType, MF_E_NOTACCEPTING);

						if (aInputStreamID > 0)
						{
							if ((*lIter).second.mStreamSamples.size() > 5)
								(*lIter).second.mStreamSamples.erase((*lIter).second.mStreamSamples.begin());

							(*lIter).second.mStreamSamples.push_back(StreamSample(aPtrSample));

							if ((*lIter).second.mStreamSamples.size() > 3)
								(*lIter).second.mAllowRead = true;
						}
						else
						{
							mOutputSample = aPtrSample;
						}
																	   
					} while (false);

					return lresult;
				}

				STDMETHODIMP AudioMixerNode::ProcessOutput(DWORD aFlags, DWORD aOutputBufferCount,
					MFT_OUTPUT_DATA_BUFFER* aPtrOutputSamples, DWORD* aPtrStatus)
				{
					HRESULT lresult = S_OK;

					do
					{
						std::lock_guard<std::mutex> lock(mMutex);

						LOG_CHECK_PTR_MEMORY(aPtrOutputSamples);

						LOG_CHECK_PTR_MEMORY(aPtrStatus);

						LOG_CHECK_STATE_DESCR(aOutputBufferCount != 1 || aFlags != 0, E_INVALIDARG);
						
						LOG_CHECK_STATE_DESCR(!mOutputSample, MF_E_TRANSFORM_NEED_MORE_INPUT);

						process();

						aPtrOutputSamples->pSample = mOutputSample.detach();						

					} while (false);

					return lresult;
				}
											   

				// IAudioMixerNode implements

				STDMETHODIMP AudioMixerNode::setRelativeVolume(
					/* [in] */ DWORD aStreamID,
					/* [in] */ FLOAT aRelativeVolume)
				{
					HRESULT lresult = S_OK;

					do
					{
						if (m_PtrPrevRelativeVolumes == nullptr)
							break;

						auto lstreamCount = m_InputStreams.size();

						aRelativeVolume = log10f(1.0f + aRelativeVolume*9.0f);

						
						float lRelativeVolume = (1.0f - aRelativeVolume)/((float)(lstreamCount - 1));

						lRelativeVolume *= s_scale;

						aRelativeVolume *= s_scale;
											   
						for (DWORD i = 0; i < lstreamCount; i++)
						{
							if (aStreamID == i)
							{
								m_PtrPrevRelativeVolumes->mRelativeVolumes[i] = aRelativeVolume;

								continue;
							}

							m_PtrPrevRelativeVolumes->mRelativeVolumes[i] = lRelativeVolume;
						}

						auto ltempPtr = m_PtrCurrentRelativeVolumes;

						m_PtrCurrentRelativeVolumes = m_PtrPrevRelativeVolumes;

						m_PtrPrevRelativeVolumes = ltempPtr;


					} while (false);

					return lresult;
				}
				
				void AudioMixerNode::process()
				{
					HRESULT lresult = S_OK;

					do
					{
						LOG_CHECK_PTR_MEMORY(mOutputSample);

						if (m_PtrCurrentRelativeVolumes == nullptr)
							break;
						
						DWORD lReferenceTotalLength(0);

						mOutputSample->GetTotalLength(&lReferenceTotalLength);

						CComPtrCustom<IMFMediaBuffer> lReferenceMediaBuffer;

						mOutputSample->GetBufferByIndex(0, &lReferenceMediaBuffer);

						LOG_CHECK_PTR_MEMORY(lReferenceMediaBuffer);

						BYTE * lPtrReferenceBuffer = nullptr;

						SHORT * lPtrShortReferenceBuffer = nullptr;

						DWORD lReferenceMaxLength;

						DWORD lReferenceCurrentLength;

						lReferenceMediaBuffer->Lock(&lPtrReferenceBuffer, &lReferenceMaxLength, &lReferenceCurrentLength);

						lPtrShortReferenceBuffer = (SHORT*)lPtrReferenceBuffer;


						auto lBaseRelativeVolume = m_PtrCurrentRelativeVolumes->mRelativeVolumes[0];


						for (size_t i = 0; i < lReferenceCurrentLength >> 1; i++)
						{
							lPtrShortReferenceBuffer[i] = (SHORT)((float)lPtrShortReferenceBuffer[i] * lBaseRelativeVolume);
						}

						for (auto& litem : m_InputStreams)
						{				
							if (litem.second.mAllowRead)
							{
								float lvolume = m_PtrCurrentRelativeVolumes->mRelativeVolumes[litem.second.mIndex];

								CComPtrCustom<IMFMediaBuffer> lMediaBuffer;

								litem.second.mStreamSamples.front().mSample->GetBufferByIndex(0, &lMediaBuffer);

								auto lPosition = litem.second.mStreamSamples.front().mPosition;

								if (lMediaBuffer)
								{
									BYTE * lPtrBuffer = nullptr;

									SHORT * lPtrShortBuffer = nullptr;

									DWORD lMaxLength;

									DWORD lCurrentLength;

									lMediaBuffer->Lock(&lPtrBuffer, &lMaxLength, &lCurrentLength);

									lPtrShortBuffer = (SHORT*)lPtrBuffer;

									lCurrentLength -= lPosition;

									lPtrShortBuffer += lPosition >> 1;

									DWORD lMinCurrentLength = lReferenceCurrentLength > lCurrentLength ? lCurrentLength : lReferenceCurrentLength;

									for (size_t i = 0; i < lMinCurrentLength >> 1; i++)
									{
										*lPtrShortReferenceBuffer = (SHORT)(
											((float)*lPtrShortBuffer * lvolume) +
											((float)*lPtrShortReferenceBuffer));

										++lPtrShortBuffer;

										++lPtrShortReferenceBuffer;
									}

									lMediaBuffer->Unlock();
									
									if (lReferenceCurrentLength >= lCurrentLength)
									{
										if (lReferenceCurrentLength > lCurrentLength)
										{
											if (litem.second.mStreamSamples.size() > 1)
											{
												DWORD lDiff = lReferenceCurrentLength - lMinCurrentLength;
												
												lMediaBuffer.Release();

												litem.second.mStreamSamples[1].mSample->GetBufferByIndex(0, &lMediaBuffer);

												if (lMediaBuffer)
												{
													DWORD lSecondCurrentLength;

													lMediaBuffer->Lock(&lPtrBuffer, &lMaxLength, &lSecondCurrentLength);

													lPtrShortBuffer = (SHORT*)lPtrBuffer;

													lMinCurrentLength = lDiff > lSecondCurrentLength ? lSecondCurrentLength : lDiff;

													for (size_t i = 0; i < lMinCurrentLength >> 1; i++)
													{
														*lPtrShortReferenceBuffer = (SHORT)(
															((float)*lPtrShortBuffer * lvolume) +
															((float)*lPtrShortReferenceBuffer));

														++lPtrShortBuffer;

														++lPtrShortReferenceBuffer;
													}

													litem.second.mStreamSamples[1].mPosition = lMinCurrentLength;

													lMediaBuffer->Unlock();
												}
											}
										}
										
										litem.second.mStreamSamples.erase(litem.second.mStreamSamples.begin());
									}
									else if(lCurrentLength > lReferenceCurrentLength)
									{
										DWORD lDiff = lPosition + lReferenceCurrentLength;

										litem.second.mStreamSamples.front().mPosition = lDiff;
									}
								}

								litem.second.mAllowRead = !litem.second.mStreamSamples.empty();
							}
						}

						lReferenceMediaBuffer->Unlock();

					} while (false);
				}
			}
		}
	}
}