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

#include "ScreenCaptureProcessorFactory.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/ICheck.h"
#include "../Common/IOptions.h"
#include "../Common/Common.h"
#include "IScreenCaptureProcessorEnum.h"
#include <regex>


namespace CaptureManager
{
	namespace Sources
	{
		ScreenCaptureProcessorFactory& ScreenCaptureProcessorFactory::getInstance()
		{
			static ScreenCaptureProcessorFactory lInstance;

			return lInstance;
		}

		ScreenCaptureProcessorFactory::ScreenCaptureProcessorFactory():
			mMinPriorety(-1),mMaxPriorety(-1)
		{}

		ScreenCaptureProcessorFactory::~ScreenCaptureProcessorFactory()
		{}

		HRESULT ScreenCaptureProcessorFactory::registerInstanceMaker(
			const int aPriorety,
			const CaptureManager::Core::IMaker* aPtrMaker)
		{
			HRESULT lresult;

			do
			{
				auto lendIter = mScreenCaptureProcessorMakers.end();

				auto lfindIter = mScreenCaptureProcessorMakers.find(aPriorety);

				LOG_CHECK_STATE(lfindIter != lendIter);

				mScreenCaptureProcessorMakers[aPriorety] = aPtrMaker;

				if (mMinPriorety == -1 || mMinPriorety > aPriorety)
					mMinPriorety = aPriorety;
				
				if (mMaxPriorety < aPriorety)
					mMaxPriorety = aPriorety;

				lresult = S_OK;

			} while (false);

			return lresult;
		}

		HRESULT ScreenCaptureProcessorFactory::createScreenCaptureProcessors(
			std::vector<CComPtrCustom<IInnerCaptureProcessor>>& aVectorAudioEndpointCaptureProcessors)
		{
			HRESULT lresult;
			
			do
			{
				LOG_CHECK_STATE_DESCR(mMaxPriorety == -1, E_BOUNDS);
								
				auto lendIter = mScreenCaptureProcessorMakers.end();

				for (decltype(mMaxPriorety) i = mMaxPriorety; i >= mMinPriorety; --i)
				{
					auto lfindIter = mScreenCaptureProcessorMakers.find(i);

					if (lfindIter != lendIter)
					{
						CComQIPtrCustom<Core::ICheck> lIScreenCaptureCheck = (*lfindIter).second->make();

						if (lIScreenCaptureCheck)
						{
							if(SUCCEEDED(lIScreenCaptureCheck->check()))
							{								
								CComPtrCustom<ScreenCapture::IScreenCaptureProcessorEnum> lIScreenCaptureProcessorEnum;
								
								lIScreenCaptureCheck->QueryInterface(IID_PPV_ARGS(&lIScreenCaptureProcessorEnum));

								if (lIScreenCaptureProcessorEnum)
								{

									CComQIPtrCustom<IInnerCaptureProcessor> lIInnerCaptureProcessor;

									UINT lIndex = 0;

									while (SUCCEEDED(lIScreenCaptureProcessorEnum->enumIInnerCaptureProcessor(
										lIndex++,
										0,
										&lIInnerCaptureProcessor)))
									{
										if (lIInnerCaptureProcessor)
										{
											aVectorAudioEndpointCaptureProcessors.push_back(lIInnerCaptureProcessor);

											lIInnerCaptureProcessor.Release();
										}
									}

									if (!aVectorAudioEndpointCaptureProcessors.empty())
										break;
								}
							}
						}
					}
				}
								
				lresult = S_OK;

			} while (false);
			
			return lresult;
		}

		HRESULT ScreenCaptureProcessorFactory::createScreenCaptureProcessor(
			std::wstring aSymbolicLink,
			IInnerCaptureProcessor** aPtrPtrIInnerCaptureProcessor)
		{
			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrPtrIInnerCaptureProcessor);

				LOG_CHECK_STATE_DESCR(mMaxPriorety == -1, E_BOUNDS);

				lresult = E_OUTOFMEMORY;

				BOOL lFind = FALSE;

				decltype(mMaxPriorety) lMaxPriorety = mMaxPriorety;

				decltype(mMinPriorety) lMinPriorety = mMinPriorety;
				
				UINT lOrientation = 0;				

				auto lFindDepricated = aSymbolicLink.find(L" --normalize=");

				if (lFindDepricated != std::wstring::npos)
				{

					do
					{
						lFindDepricated = aSymbolicLink.find(L" --normalize=Landscape");

						if (lFindDepricated != std::wstring::npos)
						{
							lOrientation = 1;

							break;
						}

					} while (false);

				}

				bool l_captureHWND = false;

				std::wstring l_HWNDoption = L" ";

				//ULONG l_HWNDNumber = 0;

				lFindDepricated = aSymbolicLink.find(L" --HWND=");

				if (lFindDepricated != std::wstring::npos)
				{

					do
					{
						auto lSubString = aSymbolicLink.substr(lFindDepricated + 1, aSymbolicLink.length());

						if (lSubString.empty())
							break;

						lFindDepricated = lSubString.find(L" ");

						if (lFindDepricated != std::wstring::npos)
						{
							lSubString = lSubString.substr(0, lFindDepricated);

							break;
						}

						if (lSubString.empty())
							break;

						l_HWNDoption += lSubString;

						lMaxPriorety = 1;

						l_captureHWND = true;

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

				//if (l_captureHWND)
				//{
				//	lMaxPriorety = 0;

				//	lMinPriorety = 0;
				//}

				auto lendIter = mScreenCaptureProcessorMakers.end();

				for (decltype(lMaxPriorety) i = lMaxPriorety; i >= lMinPriorety; --i)
				{
					auto lfindIter = mScreenCaptureProcessorMakers.find(i);

					if (lfindIter != lendIter)
					{
						CComQIPtrCustom<Core::ICheck> lIScreenCaptureCheck = (*lfindIter).second->make();

						if (lIScreenCaptureCheck)
						{
							if (SUCCEEDED(lIScreenCaptureCheck->check()))
							{
								CComPtrCustom<ScreenCapture::IScreenCaptureProcessorEnum> lIScreenCaptureProcessorEnum;

								lIScreenCaptureCheck->QueryInterface(IID_PPV_ARGS(&lIScreenCaptureProcessorEnum));

								if (lIScreenCaptureProcessorEnum)
								{

									CComQIPtrCustom<IInnerCaptureProcessor> lIInnerCaptureProcessor;

									UINT lIndex = 0;

									while (SUCCEEDED(lIScreenCaptureProcessorEnum->enumIInnerCaptureProcessor(
										lIndex++,
										lOrientation,
										&lIInnerCaptureProcessor)))
									{
										if (lIInnerCaptureProcessor)
										{
											BSTR lBSTRString = nullptr;

											LOG_INVOKE_POINTER_METHOD(lIInnerCaptureProcessor, getSymbolicLink,
												&lBSTRString);
																						
											std::wstring lSymbolicLink(lBSTRString);

											SysFreeString(lBSTRString);

											if (lSymbolicLink.size() != aSymbolicLink.size())
												lSymbolicLink += L" ";

											auto lFindIter = aSymbolicLink.find(lSymbolicLink);

											if (lFindIter != std::wstring::npos)
											{

												CComPtrCustom<Core::IOptions> lIOptions;

												do
												{
													LOG_INVOKE_QUERY_INTERFACE_METHOD(lIInnerCaptureProcessor, &lIOptions);

												} while (false);

												lFindIter = aSymbolicLink.find(L" --options=");

												if (lFindIter != std::wstring::npos)
												{
													do
													{
														LOG_CHECK_PTR_MEMORY(lIOptions);

														auto loptions = aSymbolicLink.substr(lFindIter + 1);

														lIOptions->setOptions(loptions.c_str());

													} while (false);
												}
												else
												{
													do
													{
														LOG_CHECK_PTR_MEMORY(lIOptions);
														
														lIOptions->setOptions(l_HWNDoption.c_str());

													} while (false);
												}

												*aPtrPtrIInnerCaptureProcessor = lIInnerCaptureProcessor.detach();

												lresult = S_OK;

												lFind = TRUE;

												break;
											}

											lIInnerCaptureProcessor.Release();
										}
									}
								}
							}
						}
					}

					if (lFind == TRUE)
						break;
				}

			} while (false);
			
			return lresult;
		}

	}
}