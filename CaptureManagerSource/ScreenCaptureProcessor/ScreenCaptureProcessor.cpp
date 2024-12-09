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

#include "ScreenCaptureProcessor.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/MFHeaders.h"
#include "../Common/Common.h"
#include "../MemoryManager/MemoryBufferManager.h"

namespace CaptureManager
{
	namespace Sources
	{
		namespace ScreenCapture
		{
			using namespace Core;

			using namespace Core::MediaFoundation;

			ScreenCaptureProcessor::ScreenCaptureProcessor() :
				CaptureInvoker(AVRT_PRIORITY_AvrtManager::AVRT_PRIORITY_CRITICAL_AvrtManager),
				mSampleTime(0),
				mPrevTime(0),
				mDiscontinuity(FALSE),
				mVisibility(TRUE),
				mState(SourceState::SourceStateUninitialized),
				mCycleOfCapture(10),
				mSleepDuration(0),
				mDeltaTimeDuration(0),
				mCurrentGrabResult(S_OK),
				mFirstInvoke(true),
				mIsBlocked(true),
				mIsDirectX11(false),
				mIsFlipStride(FALSE)
			{
				fillVectorScreenCaptureConfigs(mVectorScreenCaptureConfigs);

				mBackImageResource.mIsEnabled = FALSE;

				mFrontImageResource.mIsEnabled = FALSE;
			}
			
			ScreenCaptureProcessor::~ScreenCaptureProcessor()
			{
				if (mBackImageResource.mIsEnabled == TRUE)
				{
					if (mBackImageResource.mHDC)
						DeleteDC(mBackImageResource.mHDC);

					if (mBackImageResource.mBitmap)
						DeleteObject(mBackImageResource.mBitmap);					
				}


				if (mFrontImageResource.mIsEnabled == TRUE)
				{
					if (mFrontImageResource.mHDC)
						DeleteDC(mFrontImageResource.mHDC);

					if (mFrontImageResource.mBitmap)
						DeleteObject(mFrontImageResource.mBitmap);
				}
			}
			
			//IInnerCaptureProcessor implementation
					   
			HRESULT ScreenCaptureProcessor::getFrendlyName(
				BSTR* aPtrString)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrString);

					*aPtrString = SysAllocString(getFrendlyName().c_str());

					lresult = S_OK;

				} while (false);

				return lresult;
			}

			std::wstring ScreenCaptureProcessor::getFrendlyName()
			{
				return L"Screen Capture";
			}

			HRESULT ScreenCaptureProcessor::getSymbolicLink(
				BSTR* aPtrString)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrString);

					*aPtrString = SysAllocString(getSymbolicLink().c_str());

					lresult = S_OK;

				} while (false);

				return lresult;
			}

			HRESULT ScreenCaptureProcessor::getStreamAmount(
				UINT32* aPtrStreamAmount)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrStreamAmount);

					*aPtrStreamAmount = 1;

					lresult = S_OK;

				} while (false);

				return lresult;
			}

			HRESULT ScreenCaptureProcessor::getMediaTypes(
				UINT32 aStreamIndex,
				BSTR* aPtrStreamNameString,
				IUnknown*** aPtrPtrPtrMediaType,
				UINT32* aPtrMediaTypeCount)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrMediaTypeCount);

					LOG_CHECK_PTR_MEMORY(aPtrPtrPtrMediaType);

					LOG_CHECK_STATE(aStreamIndex > 0);

					auto lMediaTypeSize = mVectorScreenCaptureConfigs.size();

					LOG_CHECK_STATE_DESCR(lMediaTypeSize == 0, E_FAIL);

					decltype(lMediaTypeSize) lMediaTypeCount = 0;

					auto lPtrVoid = CoTaskMemAlloc(sizeof(IUnknown*)* lMediaTypeSize);

					LOG_CHECK_PTR_MEMORY(lPtrVoid);

					IUnknown** lPtrPtrMediaTypes = (IUnknown **)(lPtrVoid);

					LOG_CHECK_PTR_MEMORY(lPtrPtrMediaTypes);

					for (decltype(lMediaTypeSize) lMediaTypeIndex = 0; lMediaTypeIndex < lMediaTypeSize; lMediaTypeIndex++)
					{
						CComPtrCustom<IMFMediaType> lMediaType;

						LOG_INVOKE_FUNCTION(createVideoMediaType,
							mVectorScreenCaptureConfigs[lMediaTypeIndex],
							&lMediaType);

						lPtrPtrMediaTypes[lMediaTypeIndex] = lMediaType.detach();

						lMediaTypeCount++;
					}

					if (FAILED(lresult))
					{
						for (decltype(lMediaTypeCount) lMediaTypeIndex = 0; lMediaTypeIndex < lMediaTypeCount; lMediaTypeIndex++)
						{
							lPtrPtrMediaTypes[lMediaTypeIndex]->Release();
						}

						CoTaskMemFree(lPtrPtrMediaTypes);

						break;
					}

					*aPtrPtrPtrMediaType = lPtrPtrMediaTypes;

					*aPtrMediaTypeCount = lMediaTypeCount;

					if (aPtrStreamNameString != nullptr)
						*(aPtrStreamNameString) = SysAllocString(L"Video stream");

				} while (false);

				return lresult;
			}

			HRESULT ScreenCaptureProcessor::setCurrentMediaType(
				UINT32 aStreamIndex,
				IUnknown* aPtrMediaType)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrMediaType);

					LOG_CHECK_STATE(aStreamIndex > 0);

					CComPtrCustom<IMFMediaType> lCurrentMediaType;

					LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrMediaType, &lCurrentMediaType);

					LOG_CHECK_PTR_MEMORY(lCurrentMediaType);

					auto lMediaTypeSize = mVectorScreenCaptureConfigs.size();

					LOG_CHECK_STATE_DESCR(lMediaTypeSize == 0, E_FAIL);

					BOOL lCompareResult = FALSE;

					decltype(lMediaTypeSize) lSelectedMediaTypeIndex = 0;

					for (decltype(lMediaTypeSize) lMediaTypeIndex = 0; lMediaTypeIndex < lMediaTypeSize; lMediaTypeIndex++)
					{
						CComPtrCustom<IMFMediaType> lMediaType;

						LOG_INVOKE_FUNCTION(createVideoMediaType,
							mVectorScreenCaptureConfigs[lMediaTypeIndex],
							&lMediaType);

						LOG_INVOKE_MF_METHOD(Compare,
							lCurrentMediaType,
							lMediaType,
							MF_ATTRIBUTES_MATCH_THEIR_ITEMS,
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

					if (lCompareResult == FALSE)
					{
						lresult = E_FAIL;

						break;
					}

					auto lSelectedScreenCaptureConfig = mVectorScreenCaptureConfigs[lSelectedMediaTypeIndex];

					LOG_INVOKE_FUNCTION(setCurrentScreenCaptureConfig, lSelectedScreenCaptureConfig, lCurrentMediaType);

				} while (false);

				return lresult;
			}


			HRESULT ScreenCaptureProcessor::getNewSample(
				DWORD aStreamIdentifier,
				IUnknown** aPtrPtrSample)
			{
				HRESULT lresult = E_FAIL;

				do
				{

					LOG_CHECK_PTR_MEMORY(aPtrPtrSample);

					LOG_CHECK_STATE(aStreamIdentifier > 0);

					CComPtrCustom<IMFSample> lSample;

					if (mIsDirectX11)
					{
						std::unique_lock<std::mutex> lLock(mAccessBufferQueueMutex);

						auto lconditionResult = mNewFrameCondition.wait_for(
							lLock,
							std::chrono::seconds(5));

						LOG_CHECK_STATE_DESCR(lconditionResult == std::cv_status::timeout, CONTEXT_E_SYNCH_TIMEOUT);
						
						LOG_INVOKE_QUERY_INTERFACE_METHOD(mUnkSample, &lSample);

						mUnkSample.Release();
					}
					else
					{
						LONGLONG lSampleTime;

						LONGLONG lSampleDuration;

						LOG_INVOKE_MF_FUNCTION(MFCreateSample, &lSample);

						CComPtrCustom<IMFMediaBuffer> lMediaBuffer;

						if (!mBufferContainer.getBuffer(
							&lMediaBuffer,
							&lSampleTime,
							&lSampleDuration))
						{
							std::unique_lock<std::mutex> lLock(mAccessBufferQueueMutex);

							auto lconditionResult = mNewFrameCondition.wait_for(
								lLock,
								std::chrono::seconds(5));

							LOG_CHECK_STATE_DESCR(lconditionResult == std::cv_status::timeout, CONTEXT_E_SYNCH_TIMEOUT);

							LOG_CHECK_STATE_DESCR(!mBufferContainer.getBuffer(&lMediaBuffer, &lSampleTime, &lSampleDuration), E_FAIL);

							LOG_CHECK_PTR_MEMORY(lMediaBuffer);
						}					
						
						LOG_INVOKE_MF_METHOD(AddBuffer, lSample, lMediaBuffer);

						LOG_INVOKE_MF_METHOD(SetSampleDuration, lSample, lSampleDuration);

						LOG_INVOKE_MF_METHOD(SetSampleTime, lSample, lSampleTime);

						LOG_INVOKE_MF_METHOD(SetUINT64, lSample, MFSampleExtension_DeviceTimestamp, lSampleTime);
					}

					LOG_CHECK_PTR_MEMORY(lSample);

					LOG_INVOKE_MF_METHOD(SetUINT32, lSample, MFSampleExtension_BottomFieldFirst, TRUE);

					LOG_INVOKE_MF_METHOD(SetUINT32, lSample, MFSampleExtension_CleanPoint, TRUE);

					LOG_INVOKE_MF_METHOD(SetUINT32, lSample, MFSampleExtension_Interlaced, TRUE);

					LOG_INVOKE_MF_METHOD(SetUINT32, lSample, MFSampleExtension_Discontinuity, mDiscontinuity);

					mDiscontinuity = FALSE;

					std::lock_guard<std::mutex> lLock(mAccessMutex);

					if (mState == SourceState::SourceStateStopped)
					{
						lresult = E_FAIL;

						break;
					}

					*aPtrPtrSample = lSample.Detach();

				} while (false);

				if (FAILED(mCurrentGrabResult))
					lresult = mCurrentGrabResult;

				return lresult;
			}

			HRESULT ScreenCaptureProcessor::start()
			{
				HRESULT lresult;

				do
				{
					std::lock_guard<std::mutex> lLock(mAccessMutex);

					lresult = checkInitialisation();

					if (FAILED(lresult))
						break;
										
					if (mState == SourceState::SourceStateStarted)
					{
						lresult = S_OK;

						break;
					}

					if (mState == SourceState::SourceStatePaused)
					{
						mState = SourceState::SourceStateStarted;

						LOG_INVOKE_FUNCTION(CaptureInvoker::start);

						lresult = S_OK;

						break;
					}

					mFirstInvoke = true;

					mDeltaTimeDuration = 0;

					mSleepDuration = (mVideoFrameDuration / mCycleOfCapture) / 10000;
					
					LOG_INVOKE_FUNCTION(CaptureInvoker::start);

					mSampleTime = 0;

					mState = SourceState::SourceStateStarted;
					
				} while (false);

				return S_OK;
			}

			void ScreenCaptureProcessor::update()
			{
				auto lCurrentTime = MediaFoundation::MediaFoundationManager::MFGetSystemTime();

				if (mIsDirectX11)
					createSample(
					lCurrentTime,
					mVideoFrameDuration);
				else
					createMediaBuffer(
						lCurrentTime,
						mVideoFrameDuration);
			}
			
			HRESULT STDMETHODCALLTYPE ScreenCaptureProcessor::invoke()
			{
				HRESULT lresult(E_NOTIMPL);

				do
				{

					if (mState != SourceState::SourceStateStarted)
					{
						lresult = S_OK;

						break;
					}
					
					if (mFirstInvoke)
					{
						mPrevTime = MediaFoundation::MediaFoundationManager::MFGetSystemTime();

						mFirstInvoke = false;

						lresult = S_OK;

						break;
					}

					execute();

					if (mIsBlocked)
					{
						auto lCurrentTime = MediaFoundation::MediaFoundationManager::MFGetSystemTime();

						auto ldif = lCurrentTime - mPrevTime;

						if ((ldif + mDeltaTimeDuration) >= mVideoFrameDuration)
						{
							mPrevTime = lCurrentTime;

							mDeltaTimeDuration = (ldif + mDeltaTimeDuration) - mVideoFrameDuration;

							lresult = S_OK;

							update();
						}
						else
							std::this_thread::sleep_for(std::chrono::milliseconds(mSleepDuration));
					}
					else
					{
						auto lCurrentTime = MediaFoundation::MediaFoundationManager::MFGetSystemTime();

						auto ldif = lCurrentTime - mPrevTime;

						if ((ldif + mDeltaTimeDuration) >= mVideoFrameDuration)
						{
							mPrevTime = lCurrentTime;

							mDeltaTimeDuration = (ldif + mDeltaTimeDuration) - mVideoFrameDuration;

							lresult = S_OK;

							update();

#ifdef _DEBUG_CAPTUREMANAGER
							static MFTIME k = MediaFoundation::MediaFoundationManager::MFGetSystemTime();

							auto gh = MediaFoundation::MediaFoundationManager::MFGetSystemTime();

							float l_time = (float)(gh - k) / 10000.0f;

							static float kl = 0;

							static int g = 0;

							if (l_time > 12.0f)
							{
								kl += l_time;
								
								kl = kl / g;

								g = 2;
								
							}
							else
							{
								g = 1;
							}


							LogPrintOut::getInstance().printOutln(
								LogPrintOut::INFO_LEVEL,
								L" SystemTime: ",
								l_time,
								L", averTime:",
								kl,
								L", mDeltaTimeDuration: ",
								(float)mDeltaTimeDuration / 10000.0f);

							k = gh;

#endif

						}
						else
							std::this_thread::sleep_for(std::chrono::milliseconds(mSleepDuration));
					}
					
					lresult = S_OK;

#ifdef _DEBUG_CAPTUREMANAGER

					//static MFTIME k = MediaFoundation::MediaFoundationManager::MFGetSystemTime();

					//auto gh = MediaFoundation::MediaFoundationManager::MFGetSystemTime();

					//float l_time = (float)(gh - k) / 10000.0f;

					//LogPrintOut::getInstance().printOutln(
					//	LogPrintOut::INFO_LEVEL,
					//	L" SystemTime: ",
					//	l_time);

					//k = gh;

#endif

				} while (false);
				
				return lresult;
			}

			HRESULT ScreenCaptureProcessor::stop()
			{
				HRESULT lresult = S_OK;

				do
				{
					std::lock_guard<std::mutex> lLock(mAccessMutex);
					
					LOG_INVOKE_FUNCTION(CaptureInvoker::stop);

					mState = SourceState::SourceStateStopped;

					clearMediaBuffer();
					
					//lresult = releaseResources();
					
				} while (false);

				return lresult;
			}

			HRESULT ScreenCaptureProcessor::pause()
			{
				HRESULT lresult;

				do
				{
					std::lock_guard<std::mutex> lLock(mAccessMutex);

					if (mState == SourceState::SourceStateStarted)
					{

						LOG_INVOKE_FUNCTION(CaptureInvoker::stop);
						
						std::unique_lock<std::mutex> lReadyPauseMutexLock(mReadyPauseMutex);

						mState = SourceState::SourceStatePaused;						
					}

					mNewFrameCondition.notify_all();

					lresult = S_OK;

				} while (false);

				return lresult;
			}

			HRESULT ScreenCaptureProcessor::restart()
			{
				return S_OK;
			}

			HRESULT ScreenCaptureProcessor::shutdown()
			{
				std::lock_guard<std::mutex> lLock(mAccessMutex);

				HRESULT lresult = releaseResources();

				clearMediaBuffer();

				if(mMemoryBufferManager)
					mMemoryBufferManager->clearMemory();

				mState = SourceState::SourceStateUninitialized;

				return lresult;
			}

			HRESULT ScreenCaptureProcessor::registerMediaEvent(IUnknown* pUnkIMediaEvent)
			{
				return E_NOTIMPL;
			}


			// IOptions interface

			HRESULT STDMETHODCALLTYPE ScreenCaptureProcessor::setOptions(const OLECHAR* aOptionsString)
			{
				HRESULT lresult(E_FAIL);

				do
				{

					std::wstring lOptions(aOptionsString);

					auto lFindIter = lOptions.find(L"<?xml");

					if (lFindIter != std::wstring::npos)
					{
						auto lFinishIter = lOptions.find(L" --");

						auto lXMLString = lOptions.substr(lFindIter, lFinishIter);

						if (!lXMLString.empty())
						{

							using namespace pugi;

							xml_document lxmlDoc;

							auto lXMLRes = lxmlDoc.load_string(lXMLString.c_str());

							if (lXMLRes.status == xml_parse_status::status_ok)
							{
								auto ldocument = lxmlDoc.document_element();

								if (!ldocument.empty() && std::wstring(ldocument.name()) == L"Options")
								{
									auto lChildNode = ldocument.first_child();

									bool lUpdateScreenCaptureConfigs = false;

									while (!lChildNode.empty() && std::wstring(lChildNode.name()) == L"Option")
									{
										OptionType lOptionType = OptionType::None;

										auto lAttribute = lChildNode.attribute(L"Type");

										if (!lAttribute.empty())
										{
											if (std::wstring(lAttribute.value()) == L"Cursor")
											{

												lAttribute = lChildNode.attribute(L"Visiblity");

												if (!lAttribute.empty())
												{
													std::wstring lValue(lAttribute.value());

													if (lValue == L"True" || lValue == L"true")
													{
														mVisibility = TRUE;
													}
													else if (lValue == L"False" || lValue == L"false")
													{
														mVisibility = FALSE;
													}
												}

												lOptionType = OptionType::Cursor;
											}
											else if (std::wstring(lAttribute.value()) == L"Clip")
											{												
												lOptionType = OptionType::Clip;
											}
											else if (std::wstring(lAttribute.value()) == L"Resize")
											{
												lOptionType = OptionType::Resize;
											}

										}

										switch (lOptionType)
										{
										case CaptureManager::Sources::ScreenCapture::ScreenCaptureProcessor::None:
											break;
										case CaptureManager::Sources::ScreenCapture::ScreenCaptureProcessor::Cursor:
										{

											auto lExtensionsChildNode = lChildNode.first_child();

											if (!lExtensionsChildNode.empty() && std::wstring(lExtensionsChildNode.name()) == L"Option.Extensions")
											{

												auto lExtensionChildNode = lExtensionsChildNode.first_child();

												while (!lExtensionChildNode.empty() && std::wstring(lExtensionChildNode.name()) == L"Extension")
												{

													auto lAttribute = lExtensionChildNode.attribute(L"Type");

													if (!lAttribute.empty())
													{
														if (std::wstring(lAttribute.value()) == L"BackImage")
														{
															ImageResource lImageResource;

															lImageResource.mImageShape = lImageResource.Rectangle;

															lImageResource.mIsEnabled = FALSE;

															lAttribute = lExtensionChildNode.attribute(L"Height");

															if (!lAttribute.empty())
															{
																lImageResource.mBitmapHeight = _wtoi(lAttribute.value());
															}

															lAttribute = lExtensionChildNode.attribute(L"Width");

															if (!lAttribute.empty())
															{
																lImageResource.mBitmapWidth = _wtoi(lAttribute.value());
															}

															lAttribute = lExtensionChildNode.attribute(L"Fill");

															int lwidth = 0;

															if (!lAttribute.empty())
															{
																std::wstring s = lAttribute.value();

																lImageResource.mFill = std::stoul(s, nullptr, 16);

																lImageResource.mIsFill = TRUE;
															}
															else
															{
																auto ltextNode = lExtensionChildNode.first_child();

																if (!ltextNode.empty() && ltextNode.type() == xml_node_type::node_pcdata)
																{
																	std::wstring s = ltextNode.value();
																	unsigned int x = std::stoul(s, nullptr, 16);
																}
															}

															lAttribute = lExtensionChildNode.attribute(L"Shape");

															if (!lAttribute.empty())
															{
																std::wstring s = lAttribute.value();

																if (s == L"Rectangle")
																	lImageResource.mImageShape = lImageResource.Rectangle;

																if (s == L"Ellipse")
																	lImageResource.mImageShape = lImageResource.Ellipse;
															}

															lImageResource.mIsEnabled = (lImageResource.mBitmapHeight > 0
																&& lImageResource.mBitmapWidth > 0) ? TRUE : FALSE;

															mBackImageResource = lImageResource;

														}

													}

													lExtensionChildNode = lExtensionChildNode.next_sibling();
												}
											}

										}
											break;
										case CaptureManager::Sources::ScreenCapture::ScreenCaptureProcessor::Clip:
										{

											auto lExtensionsChildNode = lChildNode.first_child();

											if (!lExtensionsChildNode.empty() && std::wstring(lExtensionsChildNode.name()) == L"Option.Extensions")
											{

												auto lExtensionChildNode = lExtensionsChildNode.first_child();

												while (!lExtensionChildNode.empty() && std::wstring(lExtensionChildNode.name()) == L"Extension")
												{
													int lLeft = 0;

													int lTop = 0;

													int lHeight = 0;

													int lWidth = 0;

													lAttribute = lExtensionChildNode.attribute(L"Left");

													if (!lAttribute.empty())
													{
														lLeft = _wtoi(lAttribute.value());
													}

													lAttribute = lExtensionChildNode.attribute(L"Top");

													if (!lAttribute.empty())
													{
														lTop = _wtoi(lAttribute.value());
													}

													lAttribute = lExtensionChildNode.attribute(L"Height");

													if (!lAttribute.empty())
													{
														lHeight = _wtoi(lAttribute.value());
													}

													lAttribute = lExtensionChildNode.attribute(L"Width");

													if (!lAttribute.empty())
													{
														lWidth = _wtoi(lAttribute.value());
													}

													if (lHeight > 0 && lWidth > 0)
													{
														mClipResource.mLeft = lLeft;
														mClipResource.mTop = lTop;
														mClipResource.mHeight = lHeight;
														mClipResource.mWidth = lWidth;
														mClipResource.misEnable = true;

														lUpdateScreenCaptureConfigs = true;
													}
																																																					
													lExtensionChildNode = lExtensionChildNode.next_sibling();
												}
											}
										}
											break;
										case Resize:
										{

											auto lExtensionsChildNode = lChildNode.first_child();

											if (!lExtensionsChildNode.empty() && std::wstring(lExtensionsChildNode.name()) == L"Option.Extensions")
											{

												auto lExtensionChildNode = lExtensionsChildNode.first_child();

												while (!lExtensionChildNode.empty() && std::wstring(lExtensionChildNode.name()) == L"Extension")
												{
													int lHeight = 0;

													int lWidth = 0;
													
													lAttribute = lExtensionChildNode.attribute(L"Height");

													if (!lAttribute.empty())
													{
														lHeight = _wtoi(lAttribute.value());
													}

													lAttribute = lExtensionChildNode.attribute(L"Width");

													if (!lAttribute.empty())
													{
														lWidth = _wtoi(lAttribute.value());
													}

													if (lHeight > 0 && lWidth > 0 &&
														lHeight < 100 && lWidth < 100)
													{
														mResizeResource.mLeft = 0;
														mResizeResource.mTop = 0;
														mResizeResource.mHeight = lHeight;
														mResizeResource.mWidth = lWidth;
														mResizeResource.misEnable = true;

														lUpdateScreenCaptureConfigs = true;
													}

													lExtensionChildNode = lExtensionChildNode.next_sibling();
												}
											}
										}
											break;
										default:
											break;
										}
										
										lChildNode = lChildNode.next_sibling();
									}

									if (lUpdateScreenCaptureConfigs)
										fillVectorScreenCaptureConfigs(mVectorScreenCaptureConfigs);

								}
							}

							//auto ldeclNode = lxmlDoc.append_child(node_declaration);

							//ldeclNode.append_attribute(L"version") = L"1.0";

							//xml_node lcommentNode = lxmlDoc.append_child(node_comment);

							//lcommentNode.set_value(L"XML Document of sources");

							//auto lRootXMLElement = lxmlDoc.append_child(L"Sources");
							//DataParser::readMediaType(
							//	lMediaType,
							//	lRootXMLElement);

							//std::wstringstream lwstringstream;

							//lxmlDoc.print(lwstringstream);

							//lxmlDoc.save_file("doc.xml");

							//std::wstring lXMLDocumentString;

							//lXMLDocumentString = lwstringstream.str();

						}
					}
					
					lFindIter = lOptions.find(L" --HWND=");

					if (lFindIter != std::wstring::npos)
					{

						do
						{
							auto lSubString = lOptions.substr(lFindIter + 8, lOptions.length());

							if (lSubString.empty())
								break;

							lFindIter = lSubString.find(L" ");

							if (lFindIter != std::wstring::npos)
							{
								lSubString = lSubString.substr(0, lFindIter);

								break;
							}

							if (lSubString.empty())
								break;
													
							auto l_HWNDNumber = std::stoul(lSubString, nullptr, 10);

							if (l_HWNDNumber == 0)
								break;

							mHWNDHandler.mHandler = (HWND)l_HWNDNumber;

							mHWNDHandler.misEnable = true;

							lFindIter = lSubString.find(L"|Window");

							if (lFindIter != std::wstring::npos)
							{
								mHWNDHandler.mIsWindow = true;
							}


							fillVectorScreenCaptureConfigs(mVectorScreenCaptureConfigs);

							//const std::wregex l_separator(L"([\\|]+)");
							//const std::wsregex_token_iterator l_endOfSequence;

							//std::wsregex_token_iterator l_token(lSubString.begin(), lSubString.end(), l_separator, -1);

							//while (l_token != l_endOfSequence)
							//{
							//	l_HWNDNumber = std::stoul((*l_token).str(), nullptr, 16);

							//	l_token++;

							//	//lInitSection.LAS = std::stoul((*l_token).str(), nullptr, 16);

							//	l_token++;

							//	//lInitSection.LAE = std::stoul((*l_token).str(), nullptr, 16);

							//	//l_token++;
							//}

						} while (false);

					}

				} while (false);

				return lresult;
			}

			std::wstring ScreenCaptureProcessor::getSymbolicLink()
			{
				return L"CaptureManager///Software///Sources///ScreenCapture///ScreenCapture";
			}

			HRESULT ScreenCaptureProcessor::GetService(
				REFGUID aRefGUIDService,
				REFIID aRefIID,
				LPVOID* aPtrPtrObject)
			{
				HRESULT lresult(E_NOTIMPL);
				do
				{

				} while (false);

				return lresult;
			}


			// ScreenCaptureProcessor implementations

			HRESULT ScreenCaptureProcessor::createVideoMediaType(
				ScreenCaptureConfig aScreenCaptureConfig,
				IMFMediaType** aPtrPtrMediaType)
			{
				HRESULT lresult;

				do
				{
					auto lBitmapHeader = aScreenCaptureConfig.mBitMapInfo.bmiHeader;

					CComPtrCustom<IMFVideoMediaType> lVideoMediaType;

					DWORD lBitRate = lBitmapHeader.biSizeImage * 8 * aScreenCaptureConfig.mVideoFPS;


					LOG_INVOKE_MF_FUNCTION(MFCreateVideoMediaTypeFromBitMapInfoHeaderEx,
						&lBitmapHeader,                     // video info header to convert
						lBitmapHeader.biSize,               // size of the header structure
						1,                                  // pixel aspect ratio X
						1,                                  // pixel aspect ratio Y
						MFVideoInterlace_Progressive,       // interlace mode 
						0,                                  // video flags
						aScreenCaptureConfig.mVideoFPS,           // FPS numerator
						1,          // FPS denominator
						lBitRate,          // max bitrate
						&lVideoMediaType);                           // result - out

					LOG_INVOKE_FUNCTION(MFSetAttributeRatio,
						lVideoMediaType,
						MF_MT_FRAME_RATE_RANGE_MAX,
						aScreenCaptureConfig.mVideoFPS,
						1);

					LOG_INVOKE_FUNCTION(MFSetAttributeRatio,
						lVideoMediaType,
						MF_MT_FRAME_RATE_RANGE_MIN,
						aScreenCaptureConfig.mVideoFPS,
						1);

					if (mIsFlipStride != FALSE)
					{
						INT32 lStride = 0;

						LOG_INVOKE_MF_METHOD(GetUINT32, lVideoMediaType, MF_MT_DEFAULT_STRIDE, (UINT32*)&lStride);

						lStride = -lStride;

						LOG_INVOKE_MF_METHOD(SetUINT32, lVideoMediaType, MF_MT_DEFAULT_STRIDE, UINT32(lStride));
					}

					*aPtrPtrMediaType = lVideoMediaType.detach();

				} while (false);

				return lresult;
			}

			HRESULT ScreenCaptureProcessor::fillVectorScreenCaptureConfigs(
				std::vector<ScreenCaptureConfig>& aRefVectorScreenCaptureConfigs)
			{
				HRESULT lresult;

				int lWidth = GetSystemMetrics(SM_CXSCREEN);

				int lHeight = GetSystemMetrics(SM_CYSCREEN);

				UINT32 lSizeImage = 0;

				do
				{
					aRefVectorScreenCaptureConfigs.clear();

					BITMAPINFO	lBmpInfo;

					// BMP 32 bpp

					ZeroMemory(&lBmpInfo, sizeof(BITMAPINFO));

					lBmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

					lBmpInfo.bmiHeader.biBitCount = 32;

					lBmpInfo.bmiHeader.biCompression = BI_RGB;

					lBmpInfo.bmiHeader.biWidth = lWidth;

					lBmpInfo.bmiHeader.biHeight = lHeight;

					lBmpInfo.bmiHeader.biPlanes = 1;


					LOG_INVOKE_MF_FUNCTION(MFCalculateImageSize,
						MFVideoFormat_RGB32,
						lBmpInfo.bmiHeader.biWidth,
						lBmpInfo.bmiHeader.biHeight,
						&lSizeImage);

					lBmpInfo.bmiHeader.biSizeImage = lSizeImage;



					ScreenCaptureConfig lScreenCaptureConfig;

					lScreenCaptureConfig.mBitMapInfo = lBmpInfo;


					// 1 fps

					lScreenCaptureConfig.mVideoFPS = 1;

					aRefVectorScreenCaptureConfigs.push_back(lScreenCaptureConfig);


					// 5 fps

					lScreenCaptureConfig.mVideoFPS = 5;

					aRefVectorScreenCaptureConfigs.push_back(lScreenCaptureConfig);


					// 10 fps

					lScreenCaptureConfig.mVideoFPS = 10;

					aRefVectorScreenCaptureConfigs.push_back(lScreenCaptureConfig);


					// 15 fps

					lScreenCaptureConfig.mVideoFPS = 15;

					aRefVectorScreenCaptureConfigs.push_back(lScreenCaptureConfig);


					// 20 fps 

					lScreenCaptureConfig.mVideoFPS = 20;

					aRefVectorScreenCaptureConfigs.push_back(lScreenCaptureConfig);


					// 25 fps 

					lScreenCaptureConfig.mVideoFPS = 25;

					aRefVectorScreenCaptureConfigs.push_back(lScreenCaptureConfig);


					// 30 fps 

					lScreenCaptureConfig.mVideoFPS = 30;

					aRefVectorScreenCaptureConfigs.push_back(lScreenCaptureConfig);


				} while (false);

				return lresult;
			}

			HRESULT ScreenCaptureProcessor::setCurrentScreenCaptureConfig(
				ScreenCaptureConfig& aScreenCaptureConfig,
				IMFMediaType* aPtrMediaType)
			{
				HRESULT lresult;

				do
				{
					//lresult = releaseResources();

					//if (FAILED(lresult))
					//{
					//	break;
					//}

					mScreenRect.top = 0;

					mScreenRect.left = 0;

					mHeight = aScreenCaptureConfig.mBitMapInfo.bmiHeader.biHeight;

					mScreenRect.bottom = aScreenCaptureConfig.mBitMapInfo.bmiHeader.biHeight;

					mWidth = aScreenCaptureConfig.mBitMapInfo.bmiHeader.biWidth;

					mScreenRect.right = aScreenCaptureConfig.mBitMapInfo.bmiHeader.biWidth;

					mVideoFPS = aScreenCaptureConfig.mVideoFPS;

					mVideoFrameDuration = 10000000 / aScreenCaptureConfig.mVideoFPS;

					aPtrMediaType->GetUINT32(MF_MT_DEFAULT_STRIDE, (UINT32*)&mStride);

					mStride = abs(mStride);

					GUID lSubType;

					LOG_INVOKE_MF_METHOD(GetGUID,
						aPtrMediaType,
						MF_MT_SUBTYPE,
						&lSubType);

					LOG_INVOKE_MF_FUNCTION(MFCalculateImageSize,
						lSubType,
						mWidth,
						mHeight,
						&mSizeImage);

					mMemoryBufferManager.Release();

					MemoryBufferManager::createSystemMemoryBufferManager(mSizeImage, &mMemoryBufferManager);

					LOG_CHECK_PTR_MEMORY(mMemoryBufferManager);
					
					lresult = initResources(aScreenCaptureConfig, aPtrMediaType);

					if (FAILED(lresult))
					{
						mState = SourceState::SourceStateUninitialized;

						break;
					}

					mState = SourceState::SourceStateInitialized;

					LOG_INVOKE_FUNCTION(createMediaBuffer,
						0,
						mVideoFrameDuration);
					
				} while (false);

				return lresult;
			}
						
			void ScreenCaptureProcessor::drawOn(RECT aDesktopCoordinates, HDC aHDC, DXGI_MODE_ROTATION aRotation)
			{
				do
				{

					CURSORINFO lCursorInfo = { 0 };

					lCursorInfo.cbSize = sizeof(lCursorInfo);

					auto lBoolres = GetCursorInfo(&lCursorInfo);

					if (lBoolres == TRUE)
					{
						if (lCursorInfo.flags == CURSOR_SHOWING)
						{
							auto lCursorPosition = lCursorInfo.ptScreenPos;

							if (aRotation == DXGI_MODE_ROTATION::DXGI_MODE_ROTATION_IDENTITY)
							{
								lCursorPosition.x -= aDesktopCoordinates.left;

								lCursorPosition.y -= aDesktopCoordinates.top;
							}
							else if (aRotation == DXGI_MODE_ROTATION::DXGI_MODE_ROTATION_ROTATE90)
							{
								auto ldiffX = lCursorPosition.x - aDesktopCoordinates.left;

								auto ldiffY = lCursorPosition.y - aDesktopCoordinates.top;

								long lhalfWidth = mBackImageResource.mBitmapWidth / 2;

								long lhalfHeight = mBackImageResource.mBitmapHeight / 2;

								if (ldiffX >= -lhalfWidth)
								{
									lCursorPosition.y = mScreenRect.bottom - ldiffX;
								}
								else
									break;

								if (ldiffY >= -lhalfHeight)
								{
									lCursorPosition.x = ldiffY;
								}
							}
							else if (aRotation == DXGI_MODE_ROTATION::DXGI_MODE_ROTATION_ROTATE180)
							{
								auto ldiffX = lCursorPosition.x - aDesktopCoordinates.left;

								auto ldiffY = lCursorPosition.y - aDesktopCoordinates.top;

								long lhalfWidth = mBackImageResource.mBitmapWidth / 2;

								long lhalfHeight = mBackImageResource.mBitmapHeight / 2;

								if (ldiffX >= -lhalfWidth)
								{
									lCursorPosition.x = aDesktopCoordinates.right - aDesktopCoordinates.left - ldiffX;
								}
								else
									break;

								if (ldiffY >= -lhalfHeight)
								{
									lCursorPosition.y = aDesktopCoordinates.bottom - aDesktopCoordinates.top - ldiffY;
								}
								else
									break;
							}
							else if (aRotation == DXGI_MODE_ROTATION::DXGI_MODE_ROTATION_ROTATE270)
							{
								auto ldiffX = lCursorPosition.x - aDesktopCoordinates.left;

								auto ldiffY = lCursorPosition.y - aDesktopCoordinates.top;

								long lhalfWidth = mBackImageResource.mBitmapWidth / 2;

								long lhalfHeight = mBackImageResource.mBitmapHeight / 2;

								if (ldiffX >= -lhalfWidth)
								{
									lCursorPosition.y = ldiffX;
								}
								else
									break;

								if (ldiffY >= -lhalfHeight)
								{
									lCursorPosition.x = mScreenRect.right - ldiffY;
								}
								else
									break;
							}

							if (mBackImageResource.mIsEnabled == TRUE)
							{
								GdiAlphaBlend(aHDC, lCursorPosition.x - mBackImageResource.mBitmapWidth / 2,
									lCursorPosition.y - mBackImageResource.mBitmapHeight / 2,
									mBackImageResource.mBitmapWidth, mBackImageResource.mBitmapHeight,
									mBackImageResource.mHDC, 0, 0, mBackImageResource.mBitmapWidth, mBackImageResource.mBitmapHeight,
									mBlendFunction);
							}

							if (mVisibility == TRUE)
							{
								DrawIconEx(
									aHDC,
									lCursorPosition.x,
									lCursorPosition.y,
									lCursorInfo.hCursor,
									0,
									0,
									0,
									0,
									DI_NORMAL | DI_DEFAULTSIZE);
							}

							if (mFrontImageResource.mIsEnabled == TRUE)
							{
								GdiAlphaBlend(aHDC, lCursorPosition.x - mFrontImageResource.mBitmapWidth / 2,
									lCursorPosition.y - mFrontImageResource.mBitmapHeight / 2,
									mFrontImageResource.mBitmapWidth, mFrontImageResource.mBitmapHeight,
									mFrontImageResource.mHDC, 0, 0, mFrontImageResource.mBitmapWidth, mFrontImageResource.mBitmapHeight,
									mBlendFunction);
							}
						}
					}

				} while (false);
			}

			HRESULT ScreenCaptureProcessor::initResources(HDC aHDC)
			{
				HRESULT lresult(E_FAIL);

				do
				{

					//ImageResource lImageResource;

					//HBITMAP hbitmap;       // bitmap handle 
					BITMAPINFO bmi;        // bitmap header

					UINT32   x, y;          // stepping variables 

					// zero the memory for the bitmap info 
					ZeroMemory(&bmi, sizeof(BITMAPINFO));
					VOID *pvBits = nullptr;          // pointer to DIB section 

					mBackImageResource.mHDC = CreateCompatibleDC(aHDC);

					



					// setup bitmap info  
					// set the bitmap width and height to 60% of the width and height of each of the three horizontal areas. Later on, the blending will occur in the center of each of the three areas. 
					bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
					bmi.bmiHeader.biWidth = mBackImageResource.mBitmapWidth;
					bmi.bmiHeader.biHeight = mBackImageResource.mBitmapHeight;
					bmi.bmiHeader.biPlanes = 1;
					bmi.bmiHeader.biBitCount = 32;         // four 8-bit components 
					bmi.bmiHeader.biCompression = BI_RGB;
					bmi.bmiHeader.biSizeImage = mBackImageResource.mBitmapWidth * mBackImageResource.mBitmapHeight * 4;

					// create our DIB section and select the bitmap into the dc 
					auto lBitmap = CreateDIBSection(mBackImageResource.mHDC, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0x0);

					if (lBitmap == nullptr || pvBits == nullptr)
						break;

					mBackImageResource.mBitmap = lBitmap;
					
					SelectObject(mBackImageResource.mHDC, lBitmap);

					

					mBackImageResource.mIsEnabled = TRUE;

					mFrontImageResource.mIsEnabled = FALSE;

					if (mBackImageResource.mIsFill == TRUE)
					{

						switch (mBackImageResource.mImageShape)
						{
						case ImageResource::Ellipse:
						{

							auto lFilleColour = mBackImageResource.mFill;

							BYTE lAlpha = (lFilleColour & 0xFF000000) >> 24;

							BYTE lRed = (lFilleColour & 0x00FF0000) >> 16;

							BYTE lGreen = (lFilleColour & 0x0000FF00) >> 8;

							BYTE lBlue = (lFilleColour & 0x000000FF);

							lFilleColour = RGB(lRed * lAlpha / 0xFF, lGreen * lAlpha / 0xFF, lBlue * lAlpha / 0xFF);

							//Brush and Pen colours
							SelectObject(mBackImageResource.mHDC, GetStockObject(DC_BRUSH));
							SetDCBrushColor(mBackImageResource.mHDC, lFilleColour);
							SelectObject(mBackImageResource.mHDC, GetStockObject(DC_PEN));
							SetDCPenColor(mBackImageResource.mHDC, lFilleColour);


							lFilleColour = RGB2BGR(lFilleColour);

							Ellipse(mBackImageResource.mHDC, 0, 0, mBackImageResource.mBitmapWidth, mBackImageResource.mBitmapHeight);


							for (y = 0; y < mBackImageResource.mBitmapHeight; y++)
								for (x = 0; x < mBackImageResource.mBitmapWidth; x++)
								{
								auto lpixel = ((UINT32 *)pvBits)[x + y * mBackImageResource.mBitmapWidth];

								if (lpixel == lFilleColour)
								{
									((UINT32 *)pvBits)[x + y * mBackImageResource.mBitmapWidth] = lFilleColour + (mBackImageResource.mFill & 0xFF000000);
								}
								}

							mBlendFunction.BlendOp = AC_SRC_OVER;
							mBlendFunction.BlendFlags = 0;
							mBlendFunction.SourceConstantAlpha = 0xFF;// (mBackImageResource.mFill & 0xFF000000) >> 24;  // half of 0xff = 50% transparency 
							mBlendFunction.AlphaFormat = AC_SRC_ALPHA;             // ignore source alpha channel 
						}
							break;
						case ImageResource::Rectangle:
						default:
						{
							for (y = 0; y < mBackImageResource.mBitmapHeight; y++)
								for (x = 0; x < mBackImageResource.mBitmapWidth; x++)
									((UINT32 *)pvBits)[x + y * mBackImageResource.mBitmapWidth] = mBackImageResource.mFill;

							mBlendFunction.BlendOp = AC_SRC_OVER;
							mBlendFunction.BlendFlags = 0;
							mBlendFunction.SourceConstantAlpha = (mBackImageResource.mFill & 0xFF000000) >> 24;  // half of 0xff = 50% transparency 
							mBlendFunction.AlphaFormat = 0;             // ignore source alpha channel 
						}
							break;
						}
					}


				} while (false);

				return lresult;
			}

			void ScreenCaptureProcessor::BufferContainer::setBuffer(IMFMediaBuffer* aPtrMediaBuffer,
				INT64 aCurrentTime,
				INT64 aCurrentDuration)
			{
				IMFMediaBuffer* lTempMediaBuffer = nullptr;

				do
				{
					std::lock_guard<std::mutex> lLock(mAccessMutex);

					lTempMediaBuffer = mPtrMediaBuffer;
					
					mPtrMediaBuffer = aPtrMediaBuffer;

					mCurrentTime = aCurrentTime;

					mCurrentDuration = aCurrentDuration;

					IsBufferReady = true;

					if (lTempMediaBuffer != nullptr)
						lTempMediaBuffer->Release();

				} while (false);
			}

			bool ScreenCaptureProcessor::BufferContainer::getBuffer(IMFMediaBuffer** aPtrPtrMediaBuffer,
				INT64* aPtrCurrentTime,
				INT64* aPtrCurrentDuration)
			{
				if (!IsBufferReady)
					return false;

				std::lock_guard<std::mutex> lLock(mAccessMutex);

				*aPtrPtrMediaBuffer = mPtrMediaBuffer;

				mPtrMediaBuffer = nullptr;

				*aPtrCurrentTime = mCurrentTime;

				*aPtrCurrentDuration = mCurrentDuration;

				IsBufferReady = false;

				return true;
			}

			void ScreenCaptureProcessor::BufferContainer::clearBuffer()
			{

				std::lock_guard<std::mutex> lLock(mAccessMutex);

				if (mPtrMediaBuffer != nullptr)
					mPtrMediaBuffer->Release();

				mPtrMediaBuffer = nullptr;

				IsBufferReady = false;
			}
			
			void ScreenCaptureProcessor::clearMediaBuffer()
			{
				std::lock_guard<std::mutex> lLock(mAccessBufferQueueMutex);

				mBufferContainer.clearBuffer();
			}

			HRESULT ScreenCaptureProcessor::checkInitialisation()
			{
				HRESULT lresult(S_OK);

				if (mState == SourceState::SourceStateUninitialized)
				{
					lresult = MF_E_MEDIA_SOURCE_WRONGSTATE;

				}

				return lresult;
			}

			HRESULT ScreenCaptureProcessor::grabImage(IUnknown** aPtrPtrUnkSample)
			{
				HRESULT lresult(E_NOTIMPL);
				
				do
				{

				} while (false);

				return lresult;
			}
			
			HRESULT ScreenCaptureProcessor::execute()
			{
				HRESULT lresult(E_NOTIMPL);

				do
				{

				} while (false);

				return lresult;
			}

			HRESULT ScreenCaptureProcessor::createSample(
				INT64 aCurrentTime,
				INT64 aCurrentDuration)
			{
				HRESULT lresult;

				do
				{
					CComPtrCustom<IUnknown> lUnkSample;

					{
						auto l = grabImage(&lUnkSample);

						if (FAILED(l))
						{
							mCurrentGrabResult = l;
						}

						LOG_CHECK_PTR_MEMORY(lUnkSample);

						CComPtrCustom<IMFSample> lSample;

						LOG_INVOKE_QUERY_INTERFACE_METHOD(lUnkSample, &lSample);

						LOG_CHECK_PTR_MEMORY(lSample);

						LOG_INVOKE_MF_METHOD(SetSampleDuration, lSample, aCurrentDuration);

						LOG_INVOKE_MF_METHOD(SetSampleTime, lSample, aCurrentTime);

						LOG_INVOKE_MF_METHOD(SetUINT64, lSample, MFSampleExtension_DeviceTimestamp, aCurrentTime);
					}

					std::lock_guard<std::mutex> lLock(mAccessBufferQueueMutex);

					mUnkSample.Release();

					LOG_INVOKE_QUERY_INTERFACE_METHOD(lUnkSample, &mUnkSample);

					mNewFrameCondition.notify_all();					

				} while (false);

				return lresult;
			}
			
			HRESULT ScreenCaptureProcessor::createMediaBuffer(
				INT64 aCurrentTime,
				INT64 aCurrentDuration)
			{
				HRESULT lresult;

				do
				{
					CComPtrCustom<IMFMediaBuffer> lMediaBuffer;

					LOG_INVOKE_POINTER_METHOD(mMemoryBufferManager, 
						getMediaBuffer,
						&lMediaBuffer);
					
					BYTE* lPtrBuffer = NULL;

					DWORD lMaxLength;

					DWORD lCurrentLength;

					LOG_INVOKE_MF_METHOD(Lock, lMediaBuffer,
						&lPtrBuffer,
						&lMaxLength,
						&lCurrentLength);

					auto l = grabImage(lPtrBuffer);

					if (FAILED(l))
					{
						mCurrentGrabResult = l;
					}

					LOG_INVOKE_MF_METHOD(Unlock, lMediaBuffer);

					LOG_INVOKE_MF_METHOD(SetCurrentLength, lMediaBuffer, mSizeImage);

					mBufferContainer.setBuffer(
						lMediaBuffer.detach(),
						aCurrentTime,
						aCurrentDuration);

					mNewFrameCondition.notify_all();
						
				} while (false);

				return lresult;
			}

		}
	}
}