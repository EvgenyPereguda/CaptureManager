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

#include "DirectShowCaptureProcessorFactory.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/ICheck.h"
#include "../Common/Common.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../Common/MFHeaders.h"
#include "DirectShowVideoInputCaptureProcessor.h"
#include "DirectShowCommon.h"
#include <Strmif.h>
//#include <dshow.h>

namespace CaptureManager
{
	namespace Sources
	{
		using namespace DirectShow;

		using namespace Core;

		using namespace CaptureManager::Core;

		class DECLSPEC_UUID("62BE5D10-60EB-11d0-BD3B-00A0C911CE86")
			CLSID_SystemDeviceEnumProxy;

		class DECLSPEC_UUID("860BB310-5D01-11d0-BD3B-00A0C911CE86")
			CLSID_VideoInputDeviceCategoryProxy;
		
		DirectShowCaptureProcessorFactory::DirectShowCaptureProcessorFactory()
		{
		}

		DirectShowCaptureProcessorFactory::~DirectShowCaptureProcessorFactory()
		{
		}


		HRESULT DirectShowCaptureProcessorFactory::createDirectShowVideoInputCaptureProcessors(
			std::vector<std::wstring>& aUsedSymbolicLinks,
			std::vector<CComPtrCustom<IInnerCaptureProcessor>>& aVectorAudioEndpointCaptureProcessors)
		{
			HRESULT lresult;

			do
			{

				CComPtrCustom<ICreateDevEnum> lDevEnum;

				CComPtrCustom<IEnumMoniker> lEnum;

				LOG_INVOKE_FUNCTION(lDevEnum.CoCreateInstance, __uuidof(CLSID_SystemDeviceEnumProxy));

				LOG_CHECK_PTR_MEMORY(lDevEnum);

				LOG_INVOKE_POINTER_METHOD(lDevEnum, CreateClassEnumerator,
					__uuidof(CLSID_VideoInputDeviceCategoryProxy),
					&lEnum, 0);

				LOG_CHECK_PTR_MEMORY(lEnum);
				
				do
				{
					CComPtrCustom<IMoniker> lMoniker;

					lresult = lEnum->Next(
						1, 
						&lMoniker, 
						nullptr);

					LOG_CHECK_PTR_MEMORY(lMoniker);

					CComPtrCustom<IPropertyBag> lPropBag;

					CComPtrCustom<IBindCtx> pbc;
					
					LOG_INVOKE_FUNCTION(CreateBindCtx, NULL, &pbc);

					LOG_INVOKE_POINTER_METHOD(lMoniker, BindToStorage,
						pbc,
						nullptr,
						IID_PPV_ARGS(&lPropBag));

					VARIANT lVariant;

					VariantInit(&lVariant);

					do
					{
						LPOLESTR lDisplayName = nullptr;

						LOG_INVOKE_POINTER_METHOD(lMoniker, GetDisplayName,
							0, 0, &lDisplayName);

						if (lDisplayName == nullptr)
						{
							lresult = E_FAIL;
							break;
						}

						BSTR bstrString = SysAllocString(lDisplayName);

						lVariant.vt = VT_BSTR;

						lVariant.bstrVal = bstrString;

						CoTaskMemFree(lDisplayName);

					} while (false);

					if (FAILED(lresult))
					{
						do
						{
							LOG_INVOKE_POINTER_METHOD(lPropBag, Read,
								L"DevicePath",
								&lVariant,
								nullptr);

						} while (false);
					}
					
					if (FAILED(lresult))
					{
						do
						{
							LOG_INVOKE_POINTER_METHOD(lPropBag, Read,
								L"Description", // L"DevicePath",
								&lVariant,
								nullptr);
														
						} while (false);
					}

					if (FAILED(lresult))
					{
						do
						{
							LOG_INVOKE_POINTER_METHOD(lPropBag, Read,
								L"FriendlyName",
								&lVariant,
								nullptr);

						} while (false);
					}

					std::wstring lsymbolicLinkString(lVariant.bstrVal);
					
					VariantClear(&lVariant);

					CComPtrCustom<IBaseFilter> lVideoCaptureFilter;
					
					pbc.Release();

					lPropBag.Release();

					LOG_INVOKE_FUNCTION(CreateBindCtx, NULL, &pbc);

					LOG_INVOKE_POINTER_METHOD(lMoniker, BindToObject,
						pbc,
						nullptr,
						IID_PPV_ARGS(&lVideoCaptureFilter));
					
					LOG_CHECK_PTR_MEMORY(lVideoCaptureFilter);


					do
					{
						LOG_INVOKE_FUNCTION(checkStillImage,
							lVideoCaptureFilter);

					} while (false);


					bool lbSkip = false;

					for (auto& lItem : aUsedSymbolicLinks)
					{
						if (lsymbolicLinkString.find(lItem) != std::wstring::npos)
						{
							if (FAILED(lresult))
								lbSkip = true;

							break;
						}
					}

					if (lbSkip)
						continue;


					OUTPUT_INPUT_PIN_MAP lOutputInputPinMap;

					do
					{
						lOutputInputPinMap.clear();

						LOG_INVOKE_FUNCTION(checkCrossbar,
							lVideoCaptureFilter,
							lOutputInputPinMap);

					} while (false);

					if (FAILED(lresult))
					{
						lOutputInputPinMap.clear();

						do
						{
							LOG_INVOKE_FUNCTION(checkVideoCapture,
								lVideoCaptureFilter,
								lOutputInputPinMap);

						} while (false);
					}

					if (FAILED(lresult))
						continue;

					LOG_CHECK_STATE(lOutputInputPinMap.empty());

					CComPtrCustom<IInnerCaptureProcessor> lCaptureProcessor(new (std::nothrow) DirectShowVideoInputCaptureProcessor(
						lMoniker,
						lOutputInputPinMap));

					if (lCaptureProcessor)
						aVectorAudioEndpointCaptureProcessors.push_back(lCaptureProcessor);
					
				} while (true);
				
				lresult = S_OK;

			} while (false);

			return lresult;
		}

		HRESULT DirectShowCaptureProcessorFactory::checkCrossbar(
			IBaseFilter* aPtrVideoInputFilter,
			std::unordered_map<int, std::vector<long>>& aRefOutputInputPinMap)
		{
			HRESULT lresult;

			do
			{
				aRefOutputInputPinMap.clear();

				LOG_CHECK_PTR_MEMORY(aPtrVideoInputFilter);

				CComPtrCustom<ICaptureGraphBuilder2> lCaptureGraph;

				LOG_INVOKE_FUNCTION(lCaptureGraph.CoCreateInstance, __uuidof(CLSID_CaptureGraphBuilder2Proxy));

				LOG_CHECK_PTR_MEMORY(lCaptureGraph);

				CComPtrCustom<IGraphBuilder> lGraphBuilder;

				LOG_INVOKE_FUNCTION(lGraphBuilder.CoCreateInstance, __uuidof(CLSID_FilterGraphProxy));

				LOG_CHECK_PTR_MEMORY(lGraphBuilder);

				LOG_INVOKE_POINTER_METHOD(lCaptureGraph, SetFiltergraph,
					lGraphBuilder);
				
				LOG_INVOKE_POINTER_METHOD(lGraphBuilder, AddFilter,
					aPtrVideoInputFilter,
					L"Video Capture");

				do
				{

					CComPtrCustom<IAMCrossbar> lCrossbar;

					LOG_INVOKE_POINTER_METHOD(lCaptureGraph, FindInterface,
						&LOOK_UPSTREAM_ONLY,
						nullptr, 
						aPtrVideoInputFilter,
						IID_PPV_ARGS(&lCrossbar)
						);

					LOG_CHECK_PTR_MEMORY(lCrossbar);

					LONG lInputPins = 0;

					LONG lOutputPins = 0;

					LOG_INVOKE_POINTER_METHOD(lCrossbar, get_PinCounts,
						&lOutputPins,
						&lInputPins);

					LONG lstreamCount = 0;

					do
					{
						--lOutputPins;


						long lPinIndexRelated = -1;

						long lPhysicalType = 0;

						lresult = lCrossbar->get_CrossbarPinInfo(
							FALSE, 
							lOutputPins, 
							&lPinIndexRelated, 
							&lPhysicalType);

						if (FAILED(lresult) || lPhysicalType != PhysConn_Video_VideoDecoder)
							continue;


						std::vector<long> lInputPinVector;
						
						for (LONG lInputPinIndex = 0; lInputPinIndex < lInputPins; lInputPinIndex++)
						{
							lPinIndexRelated = -1;

							lPhysicalType = -1;

							HRESULT lCheck = lCrossbar->get_CrossbarPinInfo(
								TRUE, 
								lInputPinIndex, 
								&lPinIndexRelated, 
								&lPhysicalType);

							if (FAILED(lCheck))
								continue;

							if (lPhysicalType != PhysConn_Video_Composite &&
								lPhysicalType != PhysConn_Video_SVideo &&
								lPhysicalType != PhysConn_Video_USB &&
								lPhysicalType != PhysConn_Video_1394)
								continue;

							lCheck = lCrossbar->CanRoute(
								lOutputPins, 
								lInputPinIndex);
							
							if (lCheck == S_OK)
								lInputPinVector.push_back(lPhysicalType);
						}

						if (!lInputPinVector.empty())
							aRefOutputInputPinMap[
								lOutputPins//++lstreamCount
							] = lInputPinVector;

					} while (lOutputPins > 0);
					
				} while (false);

				CComPtrCustom<IEnumFilters> lEnumFilters;

				lGraphBuilder->EnumFilters(&lEnumFilters);

				if (lEnumFilters)
				{
					ULONG lFilterIndex = 0;

					CComPtrCustom<IBaseFilter> lBaseFilter;

					while (lEnumFilters->Next(lFilterIndex++, &lBaseFilter, nullptr) == S_OK)
					{
						lGraphBuilder->RemoveFilter(lBaseFilter);

						lBaseFilter.Release();
					}					
				}

				

				//if (FAILED(lresult))
				//{
				//	lresult = S_OK;

				//	std::vector<long> lInputPins;

				//	lInputPins.push_back(0);

				//	aRefOutputInputPinMap[0] = lInputPins;

				//	break;
				//}

				//if (SUCCEEDED(hr))
				//{
				//	

				//	bool foundDevice = false;

				//	if (verbose)printf("SETUP: You are not a webcam! Setting Crossbar\n");
				//	lCrossbar->Release();

				//	IAMCrossbar *Crossbar;
				//	hr = lCaptureGraph->FindInterface(&__uuidof(PIN_CATEGORY_CAPTUREProxy), &MEDIATYPE_Interleaved, pVidFilter, IID_IAMCrossbar, (void **)&Crossbar);

				//	if (hr != NOERROR){
				//		hr = lCaptureGraph->FindInterface(&captureMode, &MEDIATYPE_Video, pVidFilter, IID_IAMCrossbar, (void **)&Crossbar);
				//	}



				//	if (foundDevice){
				//		BOOL OPin = FALSE; LONG pOIndex = 0, pORIndex = 0, pOType = 0;
				//		while (pOIndex < lOutpin)
				//		{
				//			hr = Crossbar->get_CrossbarPinInfo(OPin, pOIndex, &pORIndex, &pOType);
				//			if (pOType == PhysConn_Video_VideoDecoder)
				//				break;
				//		}
				//		Crossbar->Route(pOIndex, pIndex);
				//	}
				//	else{
				//		if (verbose)printf("SETUP: Didn't find specified Physical Connection type. Using Defualt. \n");
				//	}

				//	//we only free the crossbar when we close or restart the device
				//	//we were getting a crash otherwise
				//	//if(Crossbar)Crossbar->Release();
				//	//if(Crossbar)Crossbar = NULL;

				//	if (lCrossbar)lCrossbar->Release();
				//	if (lCrossbar)lCrossbar = NULL;

				//}
				//else{
				//	if (verbose)printf("SETUP: You are a webcam or snazzy firewire cam! No Crossbar needed\n");
				//	return hr;
				//}

			} while (false);

			return lresult;
		}

		HRESULT DirectShowCaptureProcessorFactory::checkVideoCapture(
			IBaseFilter* aPtrVideoInputFilter,
			std::unordered_map<int, std::vector<long>>& aRefOutputInputPinMap)
		{
			HRESULT lresult;

			do
			{
				aRefOutputInputPinMap.clear();

				LOG_CHECK_PTR_MEMORY(aPtrVideoInputFilter);

				CComPtrCustom<ICaptureGraphBuilder2> lCaptureGraph;

				LOG_INVOKE_FUNCTION(lCaptureGraph.CoCreateInstance, __uuidof(CLSID_CaptureGraphBuilder2Proxy));

				LOG_CHECK_PTR_MEMORY(lCaptureGraph);

				CComPtrCustom<IGraphBuilder> lGraphBuilder;

				LOG_INVOKE_FUNCTION(lGraphBuilder.CoCreateInstance, __uuidof(CLSID_FilterGraphProxy));

				LOG_CHECK_PTR_MEMORY(lGraphBuilder);

				LOG_INVOKE_POINTER_METHOD(lCaptureGraph, SetFiltergraph,
					lGraphBuilder);

				LOG_INVOKE_POINTER_METHOD(lGraphBuilder, AddFilter,
					aPtrVideoInputFilter,
					L"Video Capture");

				do
				{

					CComPtrCustom<IAMStreamConfig> lStreamConfig;
					
					LOG_INVOKE_POINTER_METHOD(lCaptureGraph, FindInterface,
						&PIN_CATEGORY_CAPTURE,
						&MEDIATYPE_Video,
						aPtrVideoInputFilter,
						IID_PPV_ARGS(&lStreamConfig)
						);

					LOG_CHECK_PTR_MEMORY(lStreamConfig);

					LONG lInputPins = 0;

					LONG lOutputPins = 0;

					int lCount = 0;

					int lSize = 0;

					LOG_INVOKE_POINTER_METHOD(lStreamConfig, GetNumberOfCapabilities,
						&lCount,
						&lSize);


					//LogPrintOut::getInstance().printOutln(
					//	LogPrintOut::ERROR_LEVEL,
					//	__FUNCTIONW__,
					//	L"lCount",
					//	lCount);

					std::vector<long> lInputPinVector;

					int lMediaTypeCount = 0;

					for (int lindex = 0; lindex < lCount; lindex++)
					{
						AM_MEDIA_TYPE* lPtrMediaType = nullptr;

						std::unique_ptr<BYTE, void(*)(BYTE*)> lbyteBuffer(new BYTE[lSize],
							[](BYTE* aPtrData){delete[] aPtrData; });

						LOG_INVOKE_POINTER_METHOD(lStreamConfig, GetStreamCaps,
							lindex,
							&lPtrMediaType,
							lbyteBuffer.get());

						if (lPtrMediaType != nullptr)
						{
							if (lPtrMediaType->majortype == __uuidof(MEDIATYPE_VideoProxy))
							{

								CComPtrCustom<IMFMediaType> lMediaType;

								LOG_INVOKE_MF_FUNCTION(MFCreateMediaType,
									&lMediaType);

								LOG_INVOKE_MF_FUNCTION(MFInitMediaTypeFromAMMediaType,
									lMediaType,
									lPtrMediaType);

								//using namespace pugi;

								//xml_document lxmlDoc;

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
								//
								//std::wstring lXMLDocumentString;

								//lXMLDocumentString = lwstringstream.str();


								//LogPrintOut::getInstance().printOutln(
								//	LogPrintOut::ERROR_LEVEL,
								//	__FUNCTIONW__,
								//	L"lCount",
								//	lXMLDocumentString.c_str());



								if (lMediaType)
								{
									GUID lAM_FORMAT_TYPE;

									lresult = lMediaType->GetGUID(
										MF_MT_AM_FORMAT_TYPE,
										&lAM_FORMAT_TYPE);

									if (SUCCEEDED(lresult))
									{
										if (lAM_FORMAT_TYPE == FORMAT_VideoInfo)
										{
											lInputPinVector.push_back(lMediaTypeCount++);
										}
									}
									
									lresult = S_OK;
								}								
							}
							DirectShowCommon::_DeleteMediaType(lPtrMediaType);
						}

					}
					
					aRefOutputInputPinMap[0] = lInputPinVector;
					
				} while (false);



				CComPtrCustom<IEnumFilters> lEnumFilters;

				lGraphBuilder->EnumFilters(&lEnumFilters);

				if (lEnumFilters)
				{
					ULONG lFilterIndex = 0;

					CComPtrCustom<IBaseFilter> lBaseFilter;

					while (lEnumFilters->Next(lFilterIndex++, &lBaseFilter, nullptr) == S_OK)
					{
						lGraphBuilder->RemoveFilter(lBaseFilter);

						lBaseFilter.Release();
					}
				}
				
			} while (false);

			return lresult;
		}

		HRESULT DirectShowCaptureProcessorFactory::checkStillImage(IBaseFilter* aPtrVideoInputFilter)
		{
			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrVideoInputFilter);

				CComPtrCustom<ICaptureGraphBuilder2> lCaptureGraph;

				LOG_INVOKE_FUNCTION(lCaptureGraph.CoCreateInstance, __uuidof(CLSID_CaptureGraphBuilder2Proxy));

				LOG_CHECK_PTR_MEMORY(lCaptureGraph);

				CComPtrCustom<IGraphBuilder> lGraphBuilder;

				LOG_INVOKE_FUNCTION(lGraphBuilder.CoCreateInstance, __uuidof(CLSID_FilterGraphProxy));

				LOG_CHECK_PTR_MEMORY(lGraphBuilder);

				LOG_INVOKE_POINTER_METHOD(lCaptureGraph, SetFiltergraph,
					lGraphBuilder);

				LOG_INVOKE_POINTER_METHOD(lGraphBuilder, AddFilter,
					aPtrVideoInputFilter,
					L"Video Capture");

				do
				{

					CComPtrCustom<IAMStreamConfig> lStreamConfig;

					LOG_INVOKE_POINTER_METHOD(lCaptureGraph, FindInterface,
						&PIN_CATEGORY_STILL,
						&MEDIATYPE_Video,
						aPtrVideoInputFilter,
						IID_PPV_ARGS(&lStreamConfig)
					);

					LOG_CHECK_PTR_MEMORY(lStreamConfig);

					CComPtrCustom<IPin> lPin;

					LOG_INVOKE_POINTER_METHOD(lCaptureGraph, FindPin,
						aPtrVideoInputFilter,
						PINDIR_OUTPUT,
						&PIN_CATEGORY_STILL,
						NULL,
						FALSE,
						0,
						&lPin
					);

					LOG_CHECK_PTR_MEMORY(lPin);

				} while (false);


				CComPtrCustom<IEnumFilters> lEnumFilters;

				lGraphBuilder->EnumFilters(&lEnumFilters);

				if (lEnumFilters)
				{
					ULONG lFilterIndex = 0;

					CComPtrCustom<IBaseFilter> lBaseFilter;

					while (lEnumFilters->Next(lFilterIndex++, &lBaseFilter, nullptr) == S_OK)
					{
						lGraphBuilder->RemoveFilter(lBaseFilter);

						lBaseFilter.Release();
					}
				}

			} while (false);

			return lresult;
		}

		HRESULT DirectShowCaptureProcessorFactory::createDirectShowVideoInputCaptureProcessor(
			std::wstring aSymbolicLink,
			IInnerCaptureProcessor** aPtrPtrIInnerCaptureProcessor)
		{
			HRESULT lresult;

			do
			{

				CComPtrCustom<ICreateDevEnum> lDevEnum;

				CComPtrCustom<IEnumMoniker> lEnum;

				LOG_INVOKE_FUNCTION(lDevEnum.CoCreateInstance, __uuidof(CLSID_SystemDeviceEnumProxy));

				LOG_CHECK_PTR_MEMORY(lDevEnum);

				LOG_INVOKE_POINTER_METHOD(lDevEnum, CreateClassEnumerator,
					__uuidof(CLSID_VideoInputDeviceCategoryProxy),
					&lEnum, 0);

				LOG_CHECK_PTR_MEMORY(lEnum);

				do
				{
					CComPtrCustom<IMoniker> lMoniker;

					LOG_INVOKE_POINTER_METHOD(lEnum, Next,
						1,
						&lMoniker,
						nullptr);

					LOG_CHECK_PTR_MEMORY(lMoniker);

					
					CComPtrCustom<IPropertyBag> lPropBag;

					CComPtrCustom<IBindCtx> pbc;

					LOG_INVOKE_FUNCTION(CreateBindCtx, NULL, &pbc);

					LOG_INVOKE_POINTER_METHOD(lMoniker, BindToStorage,
						pbc,
						nullptr,
						IID_PPV_ARGS(&lPropBag));

					VARIANT lVariant;

					VariantInit(&lVariant);

					do
					{
						LPOLESTR lDisplayName = nullptr;

						LOG_INVOKE_POINTER_METHOD(lMoniker, GetDisplayName,
							0, 0, &lDisplayName);

						if (lDisplayName == nullptr)
						{
							lresult = E_FAIL;
							break;
						}

						BSTR bstrString = SysAllocString(lDisplayName);

						lVariant.vt = VT_BSTR;

						lVariant.bstrVal = bstrString;

						CoTaskMemFree(lDisplayName);

					} while (false);

					if (FAILED(lresult))
					{
						do
						{
							LOG_INVOKE_POINTER_METHOD(lPropBag, Read,
								L"DevicePath",
								&lVariant,
								nullptr);

						} while (false);
					}

					if (FAILED(lresult))
					{
						do
						{
							LOG_INVOKE_POINTER_METHOD(lPropBag, Read,
								L"Description", // L"DevicePath",
								&lVariant,
								nullptr);

						} while (false);
					}

					if (FAILED(lresult))
					{
						do
						{
							LOG_INVOKE_POINTER_METHOD(lPropBag, Read,
								L"FriendlyName",
								&lVariant,
								nullptr);

						} while (false);
					}
					
					std::wstring lsymbolicLink(lVariant.bstrVal);

					VariantClear(&lVariant);

					if (aSymbolicLink == (L"DirectShowCapture///" + lsymbolicLink))
					{


						CComPtrCustom<IBaseFilter> lVideoCaptureFilter;

						CComPtrCustom<IBindCtx> pbc;

						LOG_INVOKE_FUNCTION(CreateBindCtx, NULL, &pbc);

						LOG_INVOKE_POINTER_METHOD(lMoniker, BindToObject,
							pbc,
							nullptr,
							IID_PPV_ARGS(&lVideoCaptureFilter));

						LOG_CHECK_PTR_MEMORY(lVideoCaptureFilter);

						OUTPUT_INPUT_PIN_MAP lOutputInputPinMap;
						
						do
						{

							LOG_INVOKE_FUNCTION(checkCrossbar,
								lVideoCaptureFilter,
								lOutputInputPinMap);

						} while (false);

						if (FAILED(lresult))
						{
							lOutputInputPinMap.clear();

							LOG_INVOKE_FUNCTION(checkVideoCapture,
								lVideoCaptureFilter,
								lOutputInputPinMap);							
						}

						LOG_CHECK_STATE(lOutputInputPinMap.empty());

						CComPtrCustom<IInnerCaptureProcessor> lCaptureProcessor(new DirectShowVideoInputCaptureProcessor(
							lMoniker,
							lOutputInputPinMap));

						LOG_INVOKE_QUERY_INTERFACE_METHOD(lCaptureProcessor, aPtrPtrIInnerCaptureProcessor);

						break;
					}


				} while (true);

			} while (false);

			return lresult;
		}
	}
}