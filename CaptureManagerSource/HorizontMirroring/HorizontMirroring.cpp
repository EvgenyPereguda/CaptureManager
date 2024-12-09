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

#include "HorizontMirroring.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../Common/MFHeaders.h"
#include "../Common/Common.h"
#include "../LogPrintOut/LogPrintOut.h"

namespace CaptureManager
{
	namespace Transform
	{
		namespace Image
		{
			using namespace CaptureManager::Core;

			HorizontMirroring::HorizontMirroring(const GUID aProcessedSubType) :
				mProcessedSubType(aProcessedSubType),
				mProcessor(new(std::nothrow) Processor())
			{
				HRESULT lresult;

				do
				{
					LOG_INVOKE_MF_FUNCTION(MFCreateMediaType, 
						&mInputMediaType);
					
					LOG_INVOKE_MF_FUNCTION(MFCreateMediaType, 
						&mOutputMediaType);
					
					LOG_INVOKE_MF_METHOD(SetGUID,
						mInputMediaType, MF_MT_MAJOR_TYPE, MFMediaType_Video);
					
					LOG_INVOKE_MF_METHOD(SetGUID, 
						mInputMediaType, MF_MT_SUBTYPE, mProcessedSubType);
					
					LOG_INVOKE_MF_METHOD(SetUINT32,
						mInputMediaType, MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);
					
					LOG_INVOKE_MF_METHOD(CopyAllItems, mInputMediaType, mOutputMediaType);

					LOG_INVOKE_MF_FUNCTION(MFCreateSampleCopierMFT,
						&mSampleCopierMFT);

					LOG_CHECK_PTR_MEMORY(mSampleCopierMFT);
					
				} while (false);
			}


			HRESULT HorizontMirroring::initOutputDataBuffer(
				IMFTransform* aPtrTransform,
				DWORD lCurrentLength,
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
						LOG_INVOKE_MF_FUNCTION(MFCreateSample,
							&lOutputSample);
						
						LOG_INVOKE_MF_FUNCTION(MFCreateMemoryBuffer,
							lCurrentLength,
							&lMediaBuffer);
						
						LOG_INVOKE_MF_METHOD(AddBuffer, 
							lOutputSample,
							lMediaBuffer);
						
						aRefOutputBuffer.pSample = lOutputSample.Detach();
					}
					else
					{
						lresult = S_OK;
					}

					aRefOutputBuffer.dwStreamID = loutputStreamId;

				} while (false);

				return lresult;
			}

			HorizontMirroring::~HorizontMirroring()
			{
			}

			STDMETHODIMP HorizontMirroring::GetStreamLimits(DWORD* aPtrInputMinimum, DWORD* aPtrInputMaximum,
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

			STDMETHODIMP HorizontMirroring::GetStreamIDs(DWORD aInputIDArraySize, DWORD* aPtrInputIDs,
				DWORD aOutputIDArraySize, DWORD* aPtrOutputIDs)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP HorizontMirroring::GetStreamCount(DWORD* aPtrInputStreams, DWORD* aPtrOutputStreams)
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

			STDMETHODIMP HorizontMirroring::GetInputStreamInfo(DWORD aInputStreamID,
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

			STDMETHODIMP HorizontMirroring::GetOutputStreamInfo(DWORD aOutputStreamID,
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

			STDMETHODIMP HorizontMirroring::GetInputStreamAttributes(DWORD aInputStreamID,
				IMFAttributes** aPtrPtrAttributes)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP HorizontMirroring::GetOutputStreamAttributes(DWORD aOutputStreamID,
				IMFAttributes** aPtrPtrAttributes)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP HorizontMirroring::DeleteInputStream(DWORD aStreamID)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP HorizontMirroring::AddInputStreams(DWORD aStreams, DWORD* aPtrStreamIDs)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP HorizontMirroring::GetInputAvailableType(DWORD aInputStreamID, DWORD aTypeIndex,
				IMFMediaType** aPtrPtrType)
			{
				HRESULT lresult = S_OK;
				CComPtrCustom<IMFMediaType> lMediaType;

				do
				{
					std::lock_guard<std::mutex> lock(mMutex);

					LOG_CHECK_PTR_MEMORY(aPtrPtrType);

					LOG_CHECK_STATE_DESCR(aInputStreamID != 0, MF_E_INVALIDSTREAMNUMBER);

					*aPtrPtrType = NULL;
										
					if (!mInputMediaType)
					{
						LOG_INVOKE_FUNCTION(createMediaType,
							&lMediaType);
						
						*aPtrPtrType = lMediaType.Detach();
					}
					else if (aTypeIndex == 0)
					{
						*aPtrPtrType = mInputMediaType.get();

						(*aPtrPtrType)->AddRef();
					}
					else
					{
						lresult = MF_E_NO_MORE_TYPES;
					}

				} while (false);

				return lresult;
			}

			STDMETHODIMP HorizontMirroring::GetOutputAvailableType(DWORD aOutputStreamID, DWORD aTypeIndex,
				IMFMediaType** aPtrPtrType)
			{
				HRESULT lresult = S_OK;
				CComPtrCustom<IMFMediaType> lMediaType;

				do
				{
					std::lock_guard<std::mutex> lock(mMutex);

					LOG_CHECK_PTR_MEMORY(aPtrPtrType);

					LOG_CHECK_STATE_DESCR(aOutputStreamID != 0, MF_E_INVALIDSTREAMNUMBER);
					
					if (!mOutputMediaType)
					{
						LOG_INVOKE_FUNCTION(createMediaType,
							&lMediaType);

						*aPtrPtrType = lMediaType.get();
						(*aPtrPtrType)->AddRef();
					}
					else
					{
						*aPtrPtrType = mOutputMediaType.get();

						(*aPtrPtrType)->AddRef();
					}

				} while (false);

				return lresult;
			}

			STDMETHODIMP HorizontMirroring::SetInputType(DWORD aInputStreamID, IMFMediaType* aPtrType,
				DWORD aFlags)
			{
				HRESULT lresult = S_OK;
				CComPtrCustom<IMFAttributes> lTypeAttributes;

				do
				{
					lTypeAttributes = aPtrType;

					std::lock_guard<std::mutex> lock(mMutex);

					LOG_CHECK_STATE_DESCR(aInputStreamID != 0, MF_E_INVALIDSTREAMNUMBER);
					
					LOG_INVOKE_FUNCTION(checkMediaType,
						aPtrType);
					
					LOG_CHECK_STATE_DESCR(!(!mSample),
						MF_E_TRANSFORM_CANNOT_CHANGE_MEDIATYPE_WHILE_PROCESSING);
					
					if (aPtrType != nullptr && !(!mInputMediaType))
					{
						BOOL lBoolResult = FALSE;

						LOG_INVOKE_MF_METHOD(Compare,
							aPtrType,
							lTypeAttributes, 
							MF_ATTRIBUTES_MATCH_INTERSECTION, 
							&lBoolResult);

						if (lBoolResult == FALSE)
						{
							lresult = MF_E_INVALIDMEDIATYPE;

							break;
						}
					}


					if (aFlags != MFT_SET_TYPE_TEST_ONLY)
					{
						mInputMediaType = aPtrType;


						using namespace pugi;

						xml_document lxmlDoc;

						auto ldeclNode = lxmlDoc.append_child(node_declaration);

						ldeclNode.append_attribute(L"version") = L"1.0";

						xml_node lcommentNode = lxmlDoc.append_child(node_comment);

						lcommentNode.set_value(L"XML Document of sources");

						auto lRootXMLElement = lxmlDoc.append_child(L"Sources");
						DataParser::readMediaType(
							mInputMediaType,
							lRootXMLElement);

						std::wstringstream lwstringstream;

						lxmlDoc.print(lwstringstream);

						std::wstring lXMLDocumentString;

						lXMLDocumentString = lwstringstream.str();

						
						LOG_INVOKE_MF_METHOD(SetInputType, mSampleCopierMFT, 0, mInputMediaType, 0);

						LOG_INVOKE_MF_METHOD(SetOutputType, mSampleCopierMFT, 0, mInputMediaType, 0);

						
						UINT32 lWidthInPixels = 0;

						UINT32 lHeightInPixels = 0;

						LOG_INVOKE_FUNCTION(MFGetAttributeSize,
							mInputMediaType,
							MF_MT_FRAME_SIZE,
							&lWidthInPixels,
							&lHeightInPixels);

						LONG lStride = 0;

						do
						{
							LOG_INVOKE_MF_METHOD(GetUINT32,
								mInputMediaType,
								MF_MT_DEFAULT_STRIDE,
								(UINT32*)&lStride);

						} while (false);

						if (FAILED(lresult))
						{
							lStride = 0;

							lresult = S_OK;
						}	
						
						if (lStride < 0)
						{
							lStride = -lStride;

							class HorizontMirroringProcessor : public Processor
							{
							public:
								HorizontMirroringProcessor(LONG aStride, UINT32 aHeightInPixels) :
									mStride(aStride),
									mHeightInPixels(aHeightInPixels)
								{}

								virtual ~HorizontMirroringProcessor(){}

								virtual HRESULT process(IMFSample* aPtrSample)
								{
									HRESULT lresult = S_OK;

									do
									{
										CComPtrCustom<IMFSample> lSample;

										CComPtrCustom<IMFMediaBuffer> lMediaBuffer;

										lSample = aPtrSample;

										LOG_CHECK_PTR_MEMORY(lSample);

										LOG_INVOKE_MF_METHOD(ConvertToContiguousBuffer,
											lSample,
											&lMediaBuffer);

										LOG_CHECK_PTR_MEMORY(lMediaBuffer);

										BYTE* lScanline0;

										LOG_INVOKE_MF_METHOD(Lock,
											lMediaBuffer,
											&lScanline0,
											nullptr,
											nullptr);

										BYTE lTempValue;

										UINT32 lWidthTopShift;

										UINT32 lWidthBottomBorder = mStride * (mHeightInPixels - 1);

										UINT32 lWidthBottomShift;

										decltype(mHeightInPixels) lhalfHeightInPixels = mHeightInPixels >> 1;

										for (decltype(mHeightInPixels) lHeightIndex = 0;
											lHeightIndex < lhalfHeightInPixels;
											++lHeightIndex)
										{
											lWidthTopShift = mStride * lHeightIndex;

											lWidthBottomShift = lWidthBottomBorder - lWidthTopShift;

											for (decltype(mStride) lWidthIndex = 0;
												lWidthIndex < mStride;
												++lWidthIndex)
											{
												lTempValue = lScanline0[lWidthTopShift + lWidthIndex];

												lScanline0[lWidthTopShift + lWidthIndex] = lScanline0[lWidthBottomShift + lWidthIndex];

												lScanline0[lWidthBottomShift + lWidthIndex] = lTempValue;
											}
										}

										LOG_INVOKE_MF_METHOD(Unlock,
											lMediaBuffer);

									} while (false);

									return lresult;

								}

							private:

								LONG mStride;

								UINT32 mHeightInPixels;

							};

							mProcessor.reset(new HorizontMirroringProcessor(lStride, lHeightInPixels));

						}

					}

				} while (false);

				return lresult;
			}

			STDMETHODIMP HorizontMirroring::SetOutputType(DWORD aOutputStreamID, IMFMediaType* aPtrType,
				DWORD aFlags)
			{
				HRESULT lresult = S_OK;

				CComPtrCustom<IMFMediaType> lType;

				do
				{
					lType = aPtrType;

					std::lock_guard<std::mutex> lock(mMutex);

					LOG_CHECK_STATE_DESCR(aOutputStreamID != 0, MF_E_INVALIDSTREAMNUMBER);
					
					LOG_INVOKE_FUNCTION(checkMediaType,
						lType);
					
					LOG_CHECK_STATE_DESCR(!(!mSample), MF_E_TRANSFORM_CANNOT_CHANGE_MEDIATYPE_WHILE_PROCESSING);
					
					if (!(!lType) && !(!mInputMediaType))
					{
						DWORD flags = 0;

						LOG_INVOKE_MF_METHOD(IsEqual, 
							lType,
							mInputMediaType, 
							&flags);
					}

					if (aFlags != MFT_SET_TYPE_TEST_ONLY)
					{
						mOutputMediaType = lType.Detach();
					}

				} while (false);


				return lresult;
			}

			STDMETHODIMP HorizontMirroring::GetInputCurrentType(DWORD aInputStreamID, IMFMediaType** aPtrPtrType)
			{
				HRESULT lresult = S_OK;

				do
				{

					std::lock_guard<std::mutex> lock(mMutex);
					
					LOG_CHECK_PTR_MEMORY(aPtrPtrType);
					
					LOG_CHECK_STATE_DESCR(aInputStreamID != 0, MF_E_INVALIDSTREAMNUMBER);
					
					LOG_CHECK_STATE_DESCR(!mInputMediaType, MF_E_TRANSFORM_TYPE_NOT_SET);

					*aPtrPtrType = mInputMediaType;

					(*aPtrPtrType)->AddRef();

				} while (false);

				return lresult;
			}

			STDMETHODIMP HorizontMirroring::GetOutputCurrentType(DWORD aOutputStreamID, IMFMediaType** aPtrPtrType)
			{
				HRESULT lresult = S_OK;

				do
				{
					std::lock_guard<std::mutex> lock(mMutex);

					LOG_CHECK_PTR_MEMORY(aPtrPtrType);
					
					LOG_CHECK_STATE_DESCR(aOutputStreamID != 0, MF_E_INVALIDSTREAMNUMBER);
					
					LOG_CHECK_STATE_DESCR(!mOutputMediaType, MF_E_TRANSFORM_TYPE_NOT_SET);
					
					*aPtrPtrType = mOutputMediaType;
					(*aPtrPtrType)->AddRef();

				} while (false);

				return lresult;
			}

			STDMETHODIMP HorizontMirroring::GetInputStatus(DWORD aInputStreamID, DWORD* aPtrFlags)
			{
				HRESULT lresult = S_OK;

				do
				{
					std::lock_guard<std::mutex> lock(mMutex);

					LOG_CHECK_PTR_MEMORY(aPtrFlags);

					LOG_CHECK_STATE_DESCR(aInputStreamID != 0, MF_E_INVALIDSTREAMNUMBER);
										
					if (!mSample)
					{
						*aPtrFlags = MFT_INPUT_STATUS_ACCEPT_DATA;
					}
					else
					{
						*aPtrFlags = 0;
					}

				} while (false);

				return lresult;
			}

			STDMETHODIMP HorizontMirroring::GetOutputStatus(DWORD* aPtrFlags)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP HorizontMirroring::SetOutputBounds(LONGLONG aLowerBound, LONGLONG aUpperBound)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP HorizontMirroring::ProcessEvent(DWORD aInputStreamID, IMFMediaEvent* aPtrEvent)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP HorizontMirroring::GetAttributes(IMFAttributes** aPtrPtrAttributes)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP HorizontMirroring::ProcessMessage(MFT_MESSAGE_TYPE aMessage, ULONG_PTR aParam)
			{
				HRESULT lresult = S_OK;

				do
				{
					std::lock_guard<std::mutex> lock(mMutex);

					if (aMessage == MFT_MESSAGE_COMMAND_FLUSH)
					{
						mSample.Release();
					}
					else if (aMessage == MFT_MESSAGE_COMMAND_DRAIN)
					{
					}
					else if (aMessage == MFT_MESSAGE_NOTIFY_BEGIN_STREAMING)
					{
					}
					else if (aMessage == MFT_MESSAGE_NOTIFY_END_STREAMING)
					{
					}
					else if (aMessage == MFT_MESSAGE_NOTIFY_END_OF_STREAM)
					{
					}
					else if (aMessage == MFT_MESSAGE_NOTIFY_START_OF_STREAM)
					{
					}

				} while (false);

				return lresult;
			}

			STDMETHODIMP HorizontMirroring::ProcessInput(DWORD aInputStreamID, IMFSample* aPtrSample,
				DWORD aFlags)
			{
				HRESULT lresult = S_OK;
				DWORD dwBufferCount = 0;

				do
				{
					std::lock_guard<std::mutex> lock(mMutex);

					LOG_CHECK_PTR_MEMORY(aPtrSample);
					
					LOG_CHECK_STATE(aInputStreamID != 0 || aFlags != 0);
					
					LOG_CHECK_STATE_DESCR(!mInputMediaType, MF_E_NOTACCEPTING);

					LOG_CHECK_STATE_DESCR(!mOutputMediaType, MF_E_NOTACCEPTING);

					LOG_CHECK_STATE_DESCR(!(!mSample), MF_E_NOTACCEPTING);
										
					LOG_INVOKE_MF_METHOD(ProcessInput, mSampleCopierMFT,
						0, 
						aPtrSample, 
						0);
					
					CComPtrCustom<IMFMediaBuffer> lMediaBuffer;

					LOG_INVOKE_MF_METHOD(GetBufferByIndex, aPtrSample,
						0, 
						&lMediaBuffer);
					
					DWORD lprocessOutputStatus = 0;

					MFT_OUTPUT_DATA_BUFFER loutputDataBuffer;

					DWORD lCurrentLength;

					LOG_INVOKE_MF_METHOD(GetCurrentLength, 
						lMediaBuffer, 
						&lCurrentLength);

					LOG_INVOKE_FUNCTION(initOutputDataBuffer,
						mSampleCopierMFT,
						lCurrentLength,
						loutputDataBuffer);
					
					LOG_INVOKE_MF_METHOD(ProcessOutput,
						mSampleCopierMFT,
						0,
						1,
						&loutputDataBuffer,
						&lprocessOutputStatus);
					
					mSample = loutputDataBuffer.pSample;

					loutputDataBuffer.pSample->Release();

				} while (false);

				return lresult;
			}

			STDMETHODIMP HorizontMirroring::ProcessOutput(DWORD aFlags, DWORD aOutputBufferCount,
				MFT_OUTPUT_DATA_BUFFER* aPtrOutputSamples, DWORD* aPtrStatus)
			{
				HRESULT lresult = S_OK;

				do
				{
					std::lock_guard<std::mutex> lock(mMutex);

					LOG_CHECK_PTR_MEMORY(aPtrOutputSamples);
					
					LOG_CHECK_PTR_MEMORY(aPtrStatus);

					LOG_CHECK_STATE_DESCR(aOutputBufferCount != 1 || aFlags != 0, E_INVALIDARG);

					if (!mSample)
					{
						lresult = MF_E_TRANSFORM_NEED_MORE_INPUT;

						break;
					}
					
					LOG_CHECK_STATE_DESCR(!mSample, MF_E_TRANSFORM_NEED_MORE_INPUT);
															
					LOG_INVOKE_POINTER_METHOD(mProcessor, process, mSample);
															
					aPtrOutputSamples[0].pSample = mSample.Detach();

					aPtrOutputSamples[0].dwStatus = 0;

					*aPtrStatus = 0;

				} while (false);

				return lresult;
			}

			HRESULT HorizontMirroring::createMediaType(IMFMediaType** aPtrPtrMediaType)
			{
				HRESULT lresult = S_OK;

				CComPtrCustom<IMFMediaType> lMediaType;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrPtrMediaType);

					LOG_INVOKE_MF_FUNCTION(MFCreateMediaType,
						&lMediaType);
					
					LOG_INVOKE_MF_METHOD(SetGUID,
						lMediaType,
						MF_MT_MAJOR_TYPE, 
						MFMediaType_Video);
					
					LOG_INVOKE_MF_METHOD(SetGUID,
						lMediaType, 
						MF_MT_SUBTYPE, 
						mProcessedSubType);
					
					*aPtrPtrMediaType = lMediaType.Detach();

				} while (false);

				return lresult;
			}

			HRESULT HorizontMirroring::checkMediaType(IMFMediaType* aPtrMediaType)
			{
				GUID majorType = GUID_NULL;
				GUID subtype = GUID_NULL;
				MFVideoInterlaceMode interlacingMode = MFVideoInterlace_Unknown;
				HRESULT lresult = S_OK;

				CComPtrCustom<IMFMediaType> lType;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrMediaType);

					lType = aPtrMediaType;

					LOG_CHECK_PTR_MEMORY(lType);

					LOG_INVOKE_MF_METHOD(GetGUID, 
						lType,
						MF_MT_MAJOR_TYPE, 
						&majorType);

					LOG_CHECK_STATE_DESCR(majorType != MFMediaType_Video, MF_E_INVALIDMEDIATYPE);

					LOG_INVOKE_MF_METHOD(GetGUID,
						lType,
						MF_MT_SUBTYPE,
						&subtype);
					
					LOG_CHECK_STATE_DESCR(subtype != mProcessedSubType, MF_E_INVALIDMEDIATYPE);
					
					LOG_INVOKE_MF_METHOD(GetUINT32, 
						lType,
						MF_MT_INTERLACE_MODE, 
						(UINT32*)&interlacingMode);
					
					LOG_CHECK_STATE_DESCR(interlacingMode != MFVideoInterlace_Progressive, MF_E_INVALIDMEDIATYPE);
					
				} while (false);

				return lresult;
			}

		}
	}
}