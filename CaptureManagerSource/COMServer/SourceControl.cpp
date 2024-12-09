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

#include <string>
#include <unordered_map>
#include <d3dcommon.h>

#include "SourceControl.h"
#include "../Common/ComPtrCustom.h"
#include "../Common/Singleton.h"
#include "../Common/IInnerCaptureProcessor.h"
#include "../Common/Common.h"
#include "../Common/BaseUnknown.h"
#include "../Common/MFHeaders.h"
#include "../Common/GUIDs.h"
#include "../ConfigManager/ConfigManager.h"
#include "../CaptureInvoker/CaptureInvoker.h"
#include "../CaptureManagerBroker/CaptureManagerBroker.h"
#include "../CaptureManagerBroker/IWebCamKernelStreamingControl.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../DirectXManager/Direct3D11Manager.h"
#include "../DirectXManager/DXGIManager.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../PugiXML/pugixml.hpp"
#include "../VideoRendererManager/Direct3D11VideoProcessor.h"
#include "../Common/IInnerGetService.h"
#include "AudioCaptureProcessorProxy.h"
#include "InitilaizeCaptureSource.h"
#include "CurrentMediaType.h"
#include "VideoCaptureProcessorProxy.h"


namespace CaptureManager
{
	namespace COMServer
	{

		// Driver types supported
		static D3D_DRIVER_TYPE gDriverTypes[] =
		{
			D3D_DRIVER_TYPE_HARDWARE
		};
		static UINT gNumDriverTypes = ARRAYSIZE(gDriverTypes);

		// Feature levels supported
		static D3D_FEATURE_LEVEL gFeatureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0
			//D3D_FEATURE_LEVEL_10_1,
			//D3D_FEATURE_LEVEL_10_0,
			//D3D_FEATURE_LEVEL_9_1
		};

		static UINT gNumFeatureLevels = ARRAYSIZE(gFeatureLevels);


		static UINT mUseDebugLayer(D3D11_CREATE_DEVICE_VIDEO_SUPPORT);


		enum class MethodsEnum :DISPID
		{
			GetCollectionOfSources = 1,
			GetSourceOutputMediaType = GetCollectionOfSources + 1,
			CreateSourceNode = GetSourceOutputMediaType + 1,
			CreateSourceNodeWithDownStreamConnection = CreateSourceNode + 1,
			CreateSource = CreateSourceNodeWithDownStreamConnection + 1,
			CreateSourceFromCaptureProcessor = CreateSource + 1,
			CreateSourceNodeFromExternalSource = CreateSourceFromCaptureProcessor + 1,
			CreateSourceNodeFromExternalSourceWithDownStreamConnection = CreateSourceNodeFromExternalSource + 1,
			CreateSourceControl = CreateSourceNodeFromExternalSourceWithDownStreamConnection + 1,
			GetSourceOutputMediaTypeFromMediaSource = CreateSourceControl + 1
		};

		// ISourceControl interface implementation

		STDMETHODIMP SourceControl::getCollectionOfSources(
			/* [out][in] */ BSTR *aPtrPtrXMLstring)
		{
			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrPtrXMLstring);

				if (*aPtrPtrXMLstring != nullptr)
				{
					SysFreeString(*aPtrPtrXMLstring);
				}

				std::wstring aRefXMLDocumentString;

				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().getXMLDocumentStringListOfSources, 
					aRefXMLDocumentString);

				*aPtrPtrXMLstring = SysAllocString(aRefXMLDocumentString.c_str());

			} while (false);

			return lresult;
		}
		
		STDMETHODIMP SourceControl::getSourceOutputMediaType(
			BSTR aSymbolicLink,
			DWORD aIndexStream,
			DWORD aIndexMediaType,
			IUnknown **aPtrPtrOutputMediaType)
		{

			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(aSymbolicLink);

				LOG_CHECK_PTR_MEMORY(aPtrPtrOutputMediaType);

				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().getSourceOutputMediaType,
					std::wstring(aSymbolicLink),
					aIndexStream,
					aIndexMediaType,
					aPtrPtrOutputMediaType);
				
			} while (false);

			return lresult;
		}

		STDMETHODIMP SourceControl::createSourceNode(
			BSTR aSymbolicLink,
			DWORD aIndexStream,
			DWORD aIndexMediaType,
			IUnknown **aPtrPtrTopologyNode)
		{
			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(aSymbolicLink);

				LOG_CHECK_PTR_MEMORY(aPtrPtrTopologyNode);

				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().createSourceNode,
					std::wstring(aSymbolicLink),
					aIndexStream,
					aIndexMediaType,
					aPtrPtrTopologyNode);

			} while (false);

			return lresult;
		}

		STDMETHODIMP SourceControl::createSourceNodeWithDownStreamConnection(
			BSTR aSymbolicLink,
			DWORD aIndexStream,
			DWORD aIndexMediaType,
			IUnknown *aPtrDownStreamTopologyNode,
			IUnknown **aPtrPtrTopologyNode)
		{
			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(aSymbolicLink);

				LOG_CHECK_PTR_MEMORY(aPtrDownStreamTopologyNode);

				LOG_CHECK_PTR_MEMORY(aPtrPtrTopologyNode);

				CComPtrCustom<IUnknown> lSourceNode;

				LOG_INVOKE_FUNCTION(createSourceNode,
					aSymbolicLink,
					aIndexStream,
					aIndexMediaType,
					&lSourceNode);
				
				LOG_CHECK_PTR_MEMORY(lSourceNode);
				
				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().connectOutputNode,
					lSourceNode,
					0,
					aPtrDownStreamTopologyNode,
					0);

				LOG_INVOKE_QUERY_INTERFACE_METHOD(lSourceNode, aPtrPtrTopologyNode);


			} while (false);

			return lresult;
		}

		STDMETHODIMP SourceControl::createSource(
			BSTR aSymbolicLink,
			IUnknown **aPtrPtrMediaSource)
		{
			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(aSymbolicLink);

				LOG_CHECK_PTR_MEMORY(aPtrPtrMediaSource);

				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().createSource,
					std::wstring(aSymbolicLink),
					aPtrPtrMediaSource);

			} while (false);

			return lresult;
		}

		STDMETHODIMP SourceControl::createSourceFromCaptureProcessor(
			IUnknown *aPtrCaptureProcessor,
			IUnknown **aPtrPtrMediaSource)
		{			
			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrCaptureProcessor);

				LOG_CHECK_PTR_MEMORY(aPtrPtrMediaSource);
				
				do
				{


					CComPtrCustom<ICaptureProcessor> lCaptureProcessor;

					LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrCaptureProcessor, &lCaptureProcessor);

					LOG_CHECK_PTR_MEMORY(lCaptureProcessor);


					CComPtrCustom<InitilaizeCaptureSource> lInitilaizeCaptureSource(new (std::nothrow) InitilaizeCaptureSource);
					
					LOG_INVOKE_POINTER_METHOD(lCaptureProcessor,
						initilaize,
						lInitilaizeCaptureSource);	

					auto lDeviceType = lInitilaizeCaptureSource->getDeviceType();

					CComPtrCustom<IInnerCaptureProcessor> lIInnerCaptureProcessor;

					if (lDeviceType == DeviceType_Video)
					{
						CComPtrCustom<VideoCaptureProcessorProxy> lCaptureProcessorProxy(
							new (std::nothrow) VideoCaptureProcessorProxy);

						LOG_CHECK_PTR_MEMORY(lCaptureProcessorProxy);

						LOG_INVOKE_POINTER_METHOD(lCaptureProcessorProxy, init, lCaptureProcessor, lInitilaizeCaptureSource);


						LOG_INVOKE_QUERY_INTERFACE_METHOD(lCaptureProcessorProxy, &lIInnerCaptureProcessor);
					}
					else if (lDeviceType == DeviceType_Audio)
						{
							CComPtrCustom<AudioCaptureProcessorProxy> lCaptureProcessorProxy(
								new (std::nothrow) AudioCaptureProcessorProxy);

							LOG_CHECK_PTR_MEMORY(lCaptureProcessorProxy);

							LOG_INVOKE_POINTER_METHOD(lCaptureProcessorProxy, init, lCaptureProcessor, lInitilaizeCaptureSource);


							LOG_INVOKE_QUERY_INTERFACE_METHOD(lCaptureProcessorProxy, &lIInnerCaptureProcessor);
						}


					LOG_CHECK_PTR_MEMORY(lIInnerCaptureProcessor);

					LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().createSource,
						lIInnerCaptureProcessor,
						aPtrPtrMediaSource);

				} while (false);							

			} while (false);

			return lresult;
		}
		
		STDMETHODIMP SourceControl::createSourceNodeFromExternalSource(
			IUnknown *aPtrMediaSource,
			DWORD aIndexStream,
			DWORD aIndexMediaType,
			IUnknown **aPtrPtrTopologyNode)
		{
			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrMediaSource);

				LOG_CHECK_PTR_MEMORY(aPtrPtrTopologyNode);
				
				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().createSourceNode,
					aPtrMediaSource,
					aIndexStream,
					aIndexMediaType,
					aPtrPtrTopologyNode);

			} while (false);

			return lresult;
		}

		STDMETHODIMP SourceControl::createSourceNodeFromExternalSourceWithDownStreamConnection(
			IUnknown *aPtrMediaSource,
			DWORD aIndexStream,
			DWORD aIndexMediaType,
			IUnknown *aPtrDownStreamTopologyNode,
			IUnknown **aPtrPtrTopologyNode)
		{
			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrMediaSource);

				LOG_CHECK_PTR_MEMORY(aPtrDownStreamTopologyNode);

				LOG_CHECK_PTR_MEMORY(aPtrPtrTopologyNode);
				
				CComPtrCustom<IUnknown> lSourceNode;
		
				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().createSourceNode,
					aPtrMediaSource,
					aIndexStream,
					aIndexMediaType,
					&lSourceNode);

				LOG_CHECK_PTR_MEMORY(lSourceNode);

				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().connectOutputNode,
					lSourceNode,
					0,
					aPtrDownStreamTopologyNode,
					0);
				
				LOG_INVOKE_QUERY_INTERFACE_METHOD(lSourceNode, aPtrPtrTopologyNode);
				
			} while (false);

			return lresult;
		}

		STDMETHODIMP SourceControl::createSourceControl(
			BSTR aSymbolicLink,
			REFIID aREFIID,
			IUnknown** aPtrPtrSourceControl)
		{
			class Proxy :
				public BaseDispatch<IWebCamControl>
			{

				// Enum of Parametr Flag of one parametr of current video device
				enum ParametrFlag
				{
					Auto = 0x1,
					Manual = 0x2
				};

				// Structure for collecting info about one parametr of current video device
				struct Parametr
				{
					long mCurrentValue;

					long mMin;

					long mMax;

					long mStep;

					long mDefault;

					long mFlag;

					Parametr() :
						mCurrentValue(0),
						mMin(0),
						mMax(0),
						mStep(0),
						mDefault(0),
						mFlag(0)
					{

					}
				};


				enum class MethodsEnum :DISPID
				{
					GetCamParametrs = 1,
					GetCamParametr = GetCamParametrs + 1,
					SetCamParametr = GetCamParametr + 1
				};

			public:

				Proxy(Controls::WebCamControls::IWebCamKernelStreamingControl* aWebCamControl)
				{
					mWebCamControl = aWebCamControl;
				}

				virtual ~Proxy(){}

				virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE getCamParametrs(
					BSTR *aXMLstring)
				{
					using namespace pugi;

					HRESULT lresult;

					do
					{
						LOG_CHECK_STATE_DESCR(!mWebCamControl, E_NOT_VALID_STATE);
												
						LOG_INVOKE_POINTER_METHOD(mWebCamControl, getCamParametrs, aXMLstring);
												
					} while (false);

					lresult = S_OK;

					return lresult;
				}

				virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE getCamParametr(
					DWORD aParametrIndex,
					LONG *aCurrentValue,
					LONG *aMin,
					LONG *aMax,
					LONG *aStep,
					LONG *aDefault,
					LONG *aFlag)
				{

					HRESULT lresult;

					do
					{
						//LOG_CHECK_STATE(aParametrIndex >= 17 || aParametrIndex < 0);
						
						LOG_CHECK_STATE(aCurrentValue == nullptr ||
							aMin == nullptr ||
							aMax == nullptr ||
							aStep == nullptr ||
							aDefault == nullptr ||
							aFlag == nullptr);
																		
						LOG_INVOKE_POINTER_METHOD(mWebCamControl, getCamParametr,
							aParametrIndex,
							aCurrentValue,
							aMin,
							aMax,
							aStep,
							aDefault,
							aFlag);								
											
					} while (false);

					return lresult;
				}

				virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE getCamParametr(
					DWORD aParametrIndex,
					BSTR *aXMLstring)
				{
					using namespace pugi;

					HRESULT lresult;

					do
					{
						//LOG_CHECK_STATE(aParametrIndex >= 17 || aParametrIndex < 0);
						
						Parametr lparametr;

						LOG_INVOKE_FUNCTION(getCamParametr,
							 aParametrIndex,
							 &lparametr.mCurrentValue,
							 &lparametr.mMin,
							 &lparametr.mMax,
							 &lparametr.mStep,
							 &lparametr.mDefault,
							 &lparametr.mFlag);
						
						xml_document lxmlDoc;

						auto ldeclNode = lxmlDoc.append_child(node_declaration);

						ldeclNode.append_attribute(L"version") = L"1.0";

						xml_node lcommentNode = lxmlDoc.append_child(node_comment);

						lcommentNode.set_value(L"XML Document of web cam parametr");

						auto lRootXMLElement = lxmlDoc.append_child(L"Parametr");

						writeParametr(
							lRootXMLElement,
							lparametr);

						std::wstringstream lwstringstream;

						lxmlDoc.print(lwstringstream);

						std::wstring lXMLString = lwstringstream.str();

						*aXMLstring = SysAllocString(lXMLString.c_str());

						lresult = S_OK;

					} while (false);

					return lresult;
				}

				virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE setCamParametr(
					DWORD aParametrIndex,
					LONG aNewValue,
					LONG aFlag)
				{
					HRESULT lresult;

					do
					{						
						
						LOG_INVOKE_POINTER_METHOD(mWebCamControl, setCamParametr,
							aParametrIndex,
							aNewValue,
							aFlag);

					} while (false);

					return lresult;
				}


				// IDispatch interface stub

				virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetIDsOfNames(
					__RPC__in REFIID riid,
					/* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
					/* [range][in] */ __RPC__in_range(0, 16384) UINT cNames,
					LCID lcid,
					/* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId) {

					HRESULT lresult(DISP_E_UNKNOWNNAME);

					do
					{
						LOG_CHECK_STATE(cNames != 1);
						
						if (_wcsicmp(*rgszNames, OLESTR("getCamParametrs")) == 0)
						{
							*rgDispId = (int)MethodsEnum::GetCamParametrs;

							lresult = S_OK;
						}
						else if (_wcsicmp(*rgszNames, OLESTR("getCamParametr")) == 0)
						{
							*rgDispId = (int)MethodsEnum::GetCamParametr;

							lresult = S_OK;
						}
						else if (_wcsicmp(*rgszNames, OLESTR("setCamParametr")) == 0)
						{
							*rgDispId = (int)MethodsEnum::SetCamParametr;

							lresult = S_OK;
						}

					} while (false);

					return lresult;
				}

				virtual HRESULT invokeMethod(
					/* [annotation][in] */
					_In_  DISPID dispIdMember,
					/* [annotation][out][in] */
					_In_  DISPPARAMS *pDispParams,
					/* [annotation][out] */
					_Out_opt_  VARIANT *pVarResult) {

					HRESULT lresult(DISP_E_UNKNOWNINTERFACE);

					do
					{
						LOG_CHECK_STATE_DESCR(pDispParams == nullptr, DISP_E_PARAMNOTFOUND);

						LOG_CHECK_PTR_MEMORY(pVarResult);

						switch (dispIdMember)
						{
						case (int)MethodsEnum::GetCamParametrs:
						{
							pVarResult->vt = VT_BSTR;

							lresult = getCamParametrs(&pVarResult->bstrVal);
						}
						break;
						case (int)MethodsEnum::GetCamParametr:
						{

							LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 1, DISP_E_BADPARAMCOUNT);
							
							DWORD lParametrIndex = 0;

							VARIANT lFirstArg = pDispParams->rgvarg[0];

							if (lFirstArg.vt == VT_I4 )
							{
								lParametrIndex = lFirstArg.intVal;
							}
							else if (lFirstArg.vt == VT_UI4)
							{
								lParametrIndex = lFirstArg.uintVal;
							}
							else
							{
								lresult = DISP_E_BADVARTYPE;

								break;
							}

							LOG_INVOKE_FUNCTION(getCamParametr,
								lParametrIndex,
								&pVarResult->bstrVal);
							
							pVarResult->vt = VT_BSTR;
						}
						break;
						case (int)MethodsEnum::SetCamParametr:
						{
							LOG_INVOKE_FUNCTION(invokeSetCamParametr,
								pDispParams,
								pVarResult);
						}
						break;
						
						default:
							break;
						}


					} while (false);
					
					return lresult;
				}
				
			private:

				CComPtrCustom<Controls::WebCamControls::IWebCamKernelStreamingControl> mWebCamControl;
				
				void writeParametr(
					pugi::xml_node& aRefParametrNode,
					Parametr& aRefParametr)
				{
					aRefParametrNode.append_attribute(L"CurrentValue") = aRefParametr.mCurrentValue;

					aRefParametrNode.append_attribute(L"Default") = aRefParametr.mDefault;

					aRefParametrNode.append_attribute(L"Min") = aRefParametr.mMin;

					aRefParametrNode.append_attribute(L"Max") = aRefParametr.mMax;

					aRefParametrNode.append_attribute(L"Step") = aRefParametr.mStep;

					aRefParametrNode.append_attribute(L"Flag") = aRefParametr.mFlag;
				}
				
				HRESULT invokeSetCamParametr(
					_In_  DISPPARAMS *pDispParams,
					/* [annotation][out] */
					VARIANT *pVarResult) {

					HRESULT lresult(DISP_E_UNKNOWNINTERFACE);

					do
					{
						LOG_CHECK_STATE_DESCR(pDispParams == nullptr, DISP_E_PARAMNOTFOUND);

						LOG_CHECK_PTR_MEMORY(pVarResult);

						LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 3, DISP_E_BADPARAMCOUNT);
					
						DWORD lParametrIndex;
						
						LONG lNewValue;
						
						LONG lFlag;



						VARIANT lFirstArg = pDispParams->rgvarg[0];

						VARIANT lSecondArg = pDispParams->rgvarg[1];

						VARIANT lThirdArg = pDispParams->rgvarg[2];


						if (lFirstArg.vt == VT_I8)
						{
							lFlag = lFirstArg.lVal;
						}
						else if (lFirstArg.vt == VT_UI8)
						{
							lFlag = lFirstArg.ulVal;
						}
						if (lFirstArg.vt == VT_I4)
						{
							lFlag = lFirstArg.intVal;
						}
						else if (lFirstArg.vt == VT_UI4)
						{
							lFlag = lFirstArg.uintVal;
						}
						else
						{
							lresult = DISP_E_BADVARTYPE;

							break;
						}


						if (lSecondArg.vt == VT_I8)
						{
							lNewValue = lSecondArg.lVal;
						}
						else if (lSecondArg.vt == VT_UI8)
						{
							lNewValue = lSecondArg.ulVal;
						}
						if (lSecondArg.vt == VT_I4)
						{
							lNewValue = lSecondArg.intVal;
						}
						else if (lSecondArg.vt == VT_UI4)
						{
							lNewValue = lSecondArg.uintVal;
						}
						else
						{
							lresult = DISP_E_BADVARTYPE;

							break;
						}

						if (lThirdArg.vt == VT_I8)
						{
							lParametrIndex = lThirdArg.lVal;
						}
						else if (lThirdArg.vt == VT_UI8)
						{
							lParametrIndex = lThirdArg.ulVal;
						}
						if (lThirdArg.vt == VT_I4)
						{
							lParametrIndex = lThirdArg.intVal;
						}
						else if (lThirdArg.vt == VT_UI4)
						{
							lParametrIndex = lThirdArg.uintVal;
						}
						else
						{
							lresult = DISP_E_BADVARTYPE;

							break;
						}

						LOG_INVOKE_FUNCTION(setCamParametr, lParametrIndex, lNewValue, lFlag);															

					} while (false);

					return lresult;
				}
			};

			HRESULT lresult;

			do
			{

				LOG_CHECK_PTR_MEMORY(aSymbolicLink);

				LOG_CHECK_PTR_MEMORY(aPtrPtrSourceControl);
				
				if (aREFIID == __uuidof(IWebCamControl))
				{					
					CComPtrCustom<Controls::WebCamControls::IWebCamKernelStreamingControl> lWebCamControl;
					
					LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().createWebCamControl,
						std::wstring(aSymbolicLink),
						&lWebCamControl);

					LOG_CHECK_PTR_MEMORY(lWebCamControl);
					
					CComPtrCustom<Proxy> lProxy = new (std::nothrow)Proxy(lWebCamControl);

					LOG_CHECK_PTR_MEMORY(lProxy);

					LOG_INVOKE_QUERY_INTERFACE_METHOD(lProxy, aPtrPtrSourceControl);

				}
				else
				{
					lresult = E_NOINTERFACE;
				}

			} while (false);

			return lresult;
		}
		
		STDMETHODIMP SourceControl::getSourceOutputMediaTypeFromMediaSource(
			IUnknown *aPtrMediaSource,
			DWORD aIndexStream,
			DWORD aIndexMediaType,
			IUnknown **aPtrPtrOutputMediaType)
		{

			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrMediaSource);

				LOG_CHECK_PTR_MEMORY(aPtrPtrOutputMediaType);

				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().getSourceOutputMediaType,
					aPtrMediaSource,
					aIndexStream,
					aIndexMediaType,
					aPtrPtrOutputMediaType);

			} while (false);

			return lresult;
		}
	

		// IDispatch interface stub

		STDMETHODIMP SourceControl::GetIDsOfNames(
			__RPC__in REFIID riid,
			/* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
			/* [range][in] */ __RPC__in_range(0, 16384) UINT cNames,
			LCID lcid,
			/* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId) {

			HRESULT lresult(DISP_E_UNKNOWNNAME);

			do
			{
				LOG_CHECK_STATE(cNames != 1);
				
				if (_wcsicmp(*rgszNames, OLESTR("getCollectionOfSources")) == 0)
				{
					*rgDispId = (int)MethodsEnum::GetCollectionOfSources;

					lresult = S_OK;
				}
				else if (_wcsicmp(*rgszNames, OLESTR("getSourceOutputMediaType")) == 0)
				{
					*rgDispId = (int)MethodsEnum::GetSourceOutputMediaType;

					lresult = S_OK;
				}
				else if (_wcsicmp(*rgszNames, OLESTR("createSourceNode")) == 0)
				{
					*rgDispId = (int)MethodsEnum::CreateSourceNode;

					lresult = S_OK;
				}
				else if (_wcsicmp(*rgszNames, OLESTR("createSourceNodeWithDownStreamConnection")) == 0)
				{
					*rgDispId = (int)MethodsEnum::CreateSourceNodeWithDownStreamConnection;

					lresult = S_OK;
				}
				else if (_wcsicmp(*rgszNames, OLESTR("createSource")) == 0)
				{
					*rgDispId = (int)MethodsEnum::CreateSource;

					lresult = S_OK;
				}
				else if (_wcsicmp(*rgszNames, OLESTR("createSourceFromCaptureProcessor")) == 0)
				{
					*rgDispId = (int)MethodsEnum::CreateSourceFromCaptureProcessor;

					lresult = S_OK;
				}
				else if (_wcsicmp(*rgszNames, OLESTR("createSourceNodeFromExternalSource")) == 0)
				{
					*rgDispId = (int)MethodsEnum::CreateSourceNodeFromExternalSource;

					lresult = S_OK;
				}
				else if (_wcsicmp(*rgszNames, OLESTR("createSourceNodeFromExternalSourceWithDownStreamConnection")) == 0)
				{
					*rgDispId = (int)MethodsEnum::CreateSourceNodeFromExternalSourceWithDownStreamConnection;

					lresult = S_OK;
				}
				else if (_wcsicmp(*rgszNames, OLESTR("createSourceControl")) == 0)
				{
					*rgDispId = (int)MethodsEnum::CreateSourceControl;

					lresult = S_OK;
				}
				else if (_wcsicmp(*rgszNames, OLESTR("getSourceOutputMediaTypeFromMediaSource")) == 0)
				{
					*rgDispId = (int)MethodsEnum::GetSourceOutputMediaTypeFromMediaSource;

					lresult = S_OK;
				}


			} while (false);

			return lresult;
		}

		HRESULT SourceControl::invokeMethod(
			/* [annotation][in] */
			_In_  DISPID dispIdMember,
			/* [annotation][out][in] */
			_In_  DISPPARAMS *pDispParams,
			/* [annotation][out] */
			_Out_opt_  VARIANT *pVarResult) {

			HRESULT lresult(DISP_E_UNKNOWNINTERFACE);

			do
			{
				LOG_CHECK_STATE_DESCR(pDispParams == nullptr, DISP_E_PARAMNOTFOUND);

				LOG_CHECK_PTR_MEMORY(pVarResult);

				switch (dispIdMember)
				{
				case (int)MethodsEnum::GetCollectionOfSources:
				{
					pVarResult->vt = VT_BSTR;					

					lresult = getCollectionOfSources(&pVarResult->bstrVal);
				}
				break;
				case (int)MethodsEnum::GetSourceOutputMediaType:
				{
					lresult = invokeGetSourceOutputMediaType(
						pDispParams,
						pVarResult);
				}
				break;
				case (int)MethodsEnum::CreateSourceNode:
				{
					lresult = invokeCreateSourceNode(
						pDispParams,
						pVarResult);
				}
				break;
				case (int)MethodsEnum::CreateSource:
				{
					lresult = invokeCreateSource(
						pDispParams,
						pVarResult);
				}
				break;
				case (int)MethodsEnum::CreateSourceFromCaptureProcessor:
				{
					lresult = invokeCreateSourceFromCaptureProcessor(
						pDispParams,
						pVarResult);
				}
				break;
				case (int)MethodsEnum::CreateSourceNodeFromExternalSource:
				{
					lresult = invokeCreateSourceNodeFromExternalSource(
						pDispParams,
						pVarResult);
				}
				break;
				case (int)MethodsEnum::CreateSourceNodeFromExternalSourceWithDownStreamConnection:
				{
					lresult = invokeCreateSourceNodeFromExternalSourceWithDownStreamConnection(
						pDispParams,
						pVarResult);
				}
				break;
				case (int)MethodsEnum::CreateSourceControl:
				{
					lresult = invokeCreateSourceControl(
						pDispParams,
						pVarResult);
				}
				break;
				case (int)MethodsEnum::GetSourceOutputMediaTypeFromMediaSource:
				{
					lresult = invokeGetSourceOutputMediaTypeFromMediaSource(
						pDispParams,
						pVarResult);
				}
				break;
				case (int)MethodsEnum::CreateSourceNodeWithDownStreamConnection:
				{
					lresult = invokeCreateSourceNodeWithDownStreamConnection(
						pDispParams,
						pVarResult);
				}
				break;
								
				default:
					break;
				}


			} while (false);
			
			return lresult;
		}
		
		HRESULT SourceControl::invokeGetSourceOutputMediaType(
			_In_  DISPPARAMS *pDispParams,
			/* [annotation][out] */
			_Out_opt_  VARIANT *pVarResult) {

			HRESULT lresult(DISP_E_UNKNOWNINTERFACE);

			do
			{
				LOG_CHECK_STATE_DESCR(pDispParams == nullptr, DISP_E_PARAMNOTFOUND);

				LOG_CHECK_PTR_MEMORY(pVarResult);

				LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 3, DISP_E_BADPARAMCOUNT);
				
				BSTR lSymbolicLink;
				
				DWORD lIndexStream;
				
				DWORD lIndexMediaType;

				VARIANT lFirstArg = pDispParams->rgvarg[0];

				VARIANT lSecondArg = pDispParams->rgvarg[1];

				VARIANT lThirdArg = pDispParams->rgvarg[2];

				if (lThirdArg.vt == VT_BSTR && lThirdArg.bstrVal != nullptr)
				{
					lSymbolicLink = lThirdArg.bstrVal;
				}
				else
				{
					lresult = DISP_E_BADVARTYPE;

					break;
				}

				if (lSecondArg.vt == VT_UI4 || 
					lSecondArg.vt == VT_I4)
				{
					lIndexStream = lSecondArg.uintVal;
				}
				else
				{
					lresult = DISP_E_BADVARTYPE;

					break;
				}

				if (lFirstArg.vt == VT_UI4 ||
					lFirstArg.vt == VT_I4)
				{
					lIndexMediaType = lFirstArg.uintVal;
				}
				else
				{
					lresult = DISP_E_BADVARTYPE;

					break;
				}

				//std::wstringstream lwstringstream;

				//lwstringstream << lLevelType << lFilePath;

				//MessageBox(NULL, lwstringstream.str().c_str(), L"lFirstArg.vt", 0);

				CComPtrCustom<IUnknown> lOutputMediaType;

				LOG_INVOKE_FUNCTION(getSourceOutputMediaType,
								lSymbolicLink,
								lIndexStream,
								lIndexMediaType,
								&lOutputMediaType);
				
				pVarResult->vt = VT_UNKNOWN;
				
				pVarResult->punkVal = lOutputMediaType.detach();

			} while (false);


			return lresult;
		}

		HRESULT SourceControl::invokeCreateSourceNode(
			_In_  DISPPARAMS *pDispParams,
			/* [annotation][out] */
			_Out_opt_  VARIANT *pVarResult) {

			HRESULT lresult(DISP_E_UNKNOWNINTERFACE);

			do
			{
				LOG_CHECK_STATE_DESCR(pDispParams == nullptr, DISP_E_PARAMNOTFOUND);

				LOG_CHECK_PTR_MEMORY(pVarResult);

				LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 3, DISP_E_BADPARAMCOUNT);
				
				BSTR lSymbolicLink;

				DWORD lIndexStream;

				DWORD lIndexMediaType;

				VARIANT lFirstArg = pDispParams->rgvarg[0];

				VARIANT lSecondArg = pDispParams->rgvarg[1];

				VARIANT lThirdArg = pDispParams->rgvarg[2];

				if (lThirdArg.vt == VT_BSTR && lThirdArg.bstrVal != nullptr)
				{
					lSymbolicLink = lThirdArg.bstrVal;
				}
				else
				{
					lresult = DISP_E_BADVARTYPE;

					break;
				}

				if (lSecondArg.vt == VT_UI4 ||
					lSecondArg.vt == VT_I4)
				{
					lIndexStream = lSecondArg.uintVal;
				}
				else
				{
					lresult = DISP_E_BADVARTYPE;

					break;
				}

				if (lFirstArg.vt == VT_UI4 ||
					lFirstArg.vt == VT_I4)
				{
					lIndexMediaType = lFirstArg.uintVal;
				}
				else
				{
					lresult = DISP_E_BADVARTYPE;

					break;
				}

				//std::wstringstream lwstringstream;

				//lwstringstream << lLevelType << lFilePath;

				//MessageBox(NULL, lwstringstream.str().c_str(), L"lFirstArg.vt", 0);

				CComPtrCustom<IUnknown> lOutputMediaType;

				LOG_INVOKE_FUNCTION(createSourceNode,
					lSymbolicLink,
					lIndexStream,
					lIndexMediaType,
					&lOutputMediaType);
				
				pVarResult->vt = VT_UNKNOWN;

				pVarResult->punkVal = lOutputMediaType.detach();

			} while (false);


			return lresult;
		}

		HRESULT SourceControl::invokeCreateSourceNodeWithDownStreamConnection(
			_In_  DISPPARAMS *pDispParams,
			/* [annotation][out] */
			_Out_opt_  VARIANT *pVarResult) {

			HRESULT lresult(DISP_E_UNKNOWNINTERFACE);

			do
			{
				LOG_CHECK_STATE_DESCR(pDispParams == nullptr, DISP_E_PARAMNOTFOUND);
								
				LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 4, DISP_E_BADPARAMCOUNT);
				
				BSTR lSymbolicLink;

				DWORD lIndexStream;

				DWORD lIndexMediaType;

				CComPtrCustom<IUnknown> lDownStreamNode;

				VARIANT lFirstArg = pDispParams->rgvarg[0];

				VARIANT lSecondArg = pDispParams->rgvarg[1];

				VARIANT lThirdArg = pDispParams->rgvarg[2];

				VARIANT lFourthArg = pDispParams->rgvarg[3];

				if (lFourthArg.vt == VT_BSTR && lFourthArg.bstrVal != nullptr)
				{
					lSymbolicLink = lFourthArg.bstrVal;
				}
				else
				{
					lresult = DISP_E_BADVARTYPE;

					break;
				}

				if (lThirdArg.vt == VT_UI4 ||
					lThirdArg.vt == VT_I4)
				{
					lIndexStream = lThirdArg.uintVal;
				}
				else
				{
					lresult = DISP_E_BADVARTYPE;

					break;
				}

				if (lSecondArg.vt == VT_UI4 ||
					lSecondArg.vt == VT_I4)
				{
					lIndexMediaType = lSecondArg.uintVal;
				}
				else
				{
					lresult = DISP_E_BADVARTYPE;

					break;
				}

				if (lFirstArg.vt == VT_UNKNOWN && lFirstArg.punkVal !=  nullptr)
				{
					LOG_INVOKE_QUERY_INTERFACE_METHOD(lFirstArg.punkVal, &lDownStreamNode);					
				}
				else if (lFirstArg.vt == VT_DISPATCH && lFirstArg.pdispVal != nullptr)
				{
					LOG_INVOKE_QUERY_INTERFACE_METHOD(lFirstArg.pdispVal,&lDownStreamNode);
				}
				else
				{
					lresult = DISP_E_BADVARTYPE;

					break;
				}

				//std::wstringstream lwstringstream;

				//lwstringstream << lLevelType << lFilePath;

				//MessageBox(NULL, lwstringstream.str().c_str(), L"lFirstArg.vt", 0);

				CComPtrCustom<IUnknown> lOutputMediaType;

				LOG_INVOKE_FUNCTION(createSourceNodeWithDownStreamConnection,
					lSymbolicLink,
					lIndexStream,
					lIndexMediaType,
					lDownStreamNode,
					&lOutputMediaType);
				
				pVarResult->vt = VT_UNKNOWN;

				pVarResult->punkVal = lOutputMediaType.detach();

			} while (false);


			return lresult;
		}

		HRESULT SourceControl::invokeCreateSource(
			_In_  DISPPARAMS *pDispParams,
			/* [annotation][out] */
			_Out_opt_  VARIANT *pVarResult) {

			HRESULT lresult(DISP_E_UNKNOWNINTERFACE);

			do
			{
				LOG_CHECK_STATE_DESCR(pDispParams == nullptr, DISP_E_PARAMNOTFOUND);

				LOG_CHECK_PTR_MEMORY(pVarResult);

				LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 1, DISP_E_BADPARAMCOUNT);
				
				BSTR lSymbolicLink;

				VARIANT lFirstArg = pDispParams->rgvarg[0];

				if (lFirstArg.vt == VT_BSTR && lFirstArg.bstrVal != nullptr)
				{
					lSymbolicLink = lFirstArg.bstrVal;
				}
				else
				{
					lresult = DISP_E_BADVARTYPE;

					break;
				}

				
				//std::wstringstream lwstringstream;

				//lwstringstream << lLevelType << lFilePath;

				//MessageBox(NULL, lwstringstream.str().c_str(), L"lFirstArg.vt", 0);

				CComPtrCustom<IUnknown> lSource;

				LOG_INVOKE_FUNCTION(createSource,
					lSymbolicLink,
					&lSource);
				
				pVarResult->vt = VT_UNKNOWN;

				pVarResult->punkVal = lSource.detach();

			} while (false);


			return lresult;
		}

		HRESULT SourceControl::invokeCreateSourceFromCaptureProcessor(
			_In_  DISPPARAMS *pDispParams,
			/* [annotation][out] */
			_Out_opt_  VARIANT *pVarResult) {

			HRESULT lresult(DISP_E_UNKNOWNINTERFACE);

			do
			{

				LOG_CHECK_STATE_DESCR(pDispParams == nullptr, DISP_E_PARAMNOTFOUND);

				LOG_CHECK_PTR_MEMORY(pVarResult);

				LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 1, DISP_E_BADPARAMCOUNT);


				CComPtrCustom<IUnknown> lCaptureProcessor;

				VARIANT lFirstArg = pDispParams->rgvarg[0];

				
				if (lFirstArg.vt == VT_UNKNOWN && lFirstArg.punkVal != nullptr)
				{
					lFirstArg.punkVal->QueryInterface(IID_PPV_ARGS(&lCaptureProcessor));
				}
				else if (lFirstArg.vt == VT_DISPATCH && lFirstArg.pdispVal != nullptr)
				{
					lFirstArg.pdispVal->QueryInterface(IID_PPV_ARGS(&lCaptureProcessor));
				}
				else
				{
					lresult = DISP_E_BADVARTYPE;

					break;
				}


				//std::wstringstream lwstringstream;

				//lwstringstream << lLevelType << lFilePath;

				//MessageBox(NULL, lwstringstream.str().c_str(), L"lFirstArg.vt", 0);

				CComPtrCustom<IUnknown> lSource;

				LOG_INVOKE_FUNCTION(createSourceFromCaptureProcessor,
					lCaptureProcessor,
					&lSource);

				pVarResult->vt = VT_UNKNOWN;

				pVarResult->punkVal = lSource.detach();

			} while (false);
			
			return lresult;
		}

		HRESULT SourceControl::invokeCreateSourceNodeFromExternalSource(
			_In_  DISPPARAMS *pDispParams,
			/* [annotation][out] */
			_Out_opt_  VARIANT *pVarResult) {

			HRESULT lresult(DISP_E_UNKNOWNINTERFACE);

			do
			{
				LOG_CHECK_STATE_DESCR(pDispParams == nullptr, DISP_E_PARAMNOTFOUND);

				LOG_CHECK_PTR_MEMORY(pVarResult);
				
				LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 3, DISP_E_BADPARAMCOUNT);
				
				CComPtrCustom<IUnknown> lExternalSource;

				DWORD lIndexStream;

				DWORD lIndexMediaType;
				
				VARIANT lFirstArg = pDispParams->rgvarg[0];

				VARIANT lSecondArg = pDispParams->rgvarg[1];

				VARIANT lThirdArg = pDispParams->rgvarg[2];


				if (lThirdArg.vt == VT_UNKNOWN && lThirdArg.punkVal != nullptr)
				{
					lThirdArg.punkVal->QueryInterface(IID_PPV_ARGS(&lExternalSource));
				}
				else if (lThirdArg.vt == VT_DISPATCH && lThirdArg.pdispVal != nullptr)
				{
					lThirdArg.pdispVal->QueryInterface(IID_PPV_ARGS(&lExternalSource));
				}
				else
				{
					lresult = DISP_E_BADVARTYPE;

					break;
				}

				if (lSecondArg.vt == VT_UI4 ||
					lSecondArg.vt == VT_I4)
				{
					lIndexStream = lSecondArg.uintVal;
				}
				else
				{
					lresult = DISP_E_BADVARTYPE;

					break;
				}

				if (lFirstArg.vt == VT_UI4 ||
					lFirstArg.vt == VT_I4)
				{
					lIndexMediaType = lFirstArg.uintVal;
				}
				else
				{
					lresult = DISP_E_BADVARTYPE;

					break;
				}




				//std::wstringstream lwstringstream;

				//lwstringstream << lLevelType << lFilePath;

				//MessageBox(NULL, lwstringstream.str().c_str(), L"lFirstArg.vt", 0);

				CComPtrCustom<IUnknown> lSourceNode;

				LOG_INVOKE_FUNCTION(createSourceNodeFromExternalSource,
					lExternalSource,
					lIndexStream,
					lIndexMediaType,
					&lSourceNode);
				
				pVarResult->vt = VT_UNKNOWN;

				pVarResult->punkVal = lSourceNode.detach();

			} while (false);

			return lresult;
		}

		HRESULT SourceControl::invokeCreateSourceNodeFromExternalSourceWithDownStreamConnection(
			_In_  DISPPARAMS *pDispParams,
			/* [annotation][out] */
			_Out_opt_  VARIANT *pVarResult) {

			HRESULT lresult(DISP_E_UNKNOWNINTERFACE);

			do
			{
				LOG_CHECK_STATE_DESCR(pDispParams == nullptr, DISP_E_PARAMNOTFOUND);

				LOG_CHECK_PTR_MEMORY(pVarResult);

				LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 4, DISP_E_BADPARAMCOUNT);
				
				CComPtrCustom<IUnknown> lExternalSource;

				DWORD lIndexStream;

				DWORD lIndexMediaType;

				CComPtrCustom<IUnknown> lDownStreamNode;

				VARIANT lFirstArg = pDispParams->rgvarg[0];

				VARIANT lSecondArg = pDispParams->rgvarg[1];

				VARIANT lThirdArg = pDispParams->rgvarg[2];

				VARIANT lFourthArg = pDispParams->rgvarg[3];


				if (lFourthArg.vt == VT_UNKNOWN && lFourthArg.punkVal != nullptr)
				{
					lFourthArg.punkVal->QueryInterface(IID_PPV_ARGS(&lExternalSource));
				}
				else if (lFourthArg.vt == VT_DISPATCH && lFourthArg.pdispVal != nullptr)
				{
					lFourthArg.pdispVal->QueryInterface(IID_PPV_ARGS(&lExternalSource));
				}
				else
				{
					lresult = DISP_E_BADVARTYPE;

					break;
				}

				if (lThirdArg.vt == VT_UI4 ||
					lThirdArg.vt == VT_I4)
				{
					lIndexStream = lThirdArg.uintVal;
				}
				else
				{
					lresult = DISP_E_BADVARTYPE;

					break;
				}

				if (lSecondArg.vt == VT_UI4 ||
					lSecondArg.vt == VT_I4)
				{
					lIndexMediaType = lSecondArg.uintVal;
				}
				else
				{
					lresult = DISP_E_BADVARTYPE;

					break;
				}


				if (lFirstArg.vt == VT_UNKNOWN && lFirstArg.punkVal != nullptr)
				{
					lFirstArg.punkVal->QueryInterface(IID_PPV_ARGS(&lDownStreamNode));
				}
				else if (lFirstArg.vt == VT_DISPATCH && lFirstArg.pdispVal != nullptr)
				{
					lFirstArg.pdispVal->QueryInterface(IID_PPV_ARGS(&lDownStreamNode));
				}
				else
				{
					lresult = DISP_E_BADVARTYPE;

					break;
				}




				//std::wstringstream lwstringstream;

				//lwstringstream << lLevelType << lFilePath;

				//MessageBox(NULL, lwstringstream.str().c_str(), L"lFirstArg.vt", 0);

				CComPtrCustom<IUnknown> lSourceNode;

				LOG_INVOKE_FUNCTION(createSourceNodeFromExternalSourceWithDownStreamConnection,
					lExternalSource,
					lIndexStream,
					lIndexMediaType,
					lDownStreamNode,
					&lSourceNode);
				
				pVarResult->vt = VT_UNKNOWN;

				pVarResult->punkVal = lSourceNode.detach();

			} while (false);

			return lresult;
		}

		HRESULT SourceControl::invokeCreateSourceControl(
			_In_  DISPPARAMS *pDispParams,
			/* [annotation][out] */
			_Out_opt_  VARIANT *pVarResult) {

			HRESULT lresult(DISP_E_UNKNOWNINTERFACE);

			do
			{
				LOG_CHECK_STATE_DESCR(pDispParams == nullptr, DISP_E_PARAMNOTFOUND);

				LOG_CHECK_PTR_MEMORY(pVarResult);

				LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 2, DISP_E_BADPARAMCOUNT);
				
				BSTR lSymbolicLink;

				CLSID lIID;

				VARIANT lFirstArg = pDispParams->rgvarg[0];

				VARIANT lSecondArg = pDispParams->rgvarg[1];

				if (lFirstArg.vt == VT_CLSID)
				{
				}
				else if (lFirstArg.vt == VT_BSTR)
				{
					LOG_INVOKE_FUNCTION(CLSIDFromString, lFirstArg.bstrVal, &lIID);
				}
				else
				{
					lresult = DISP_E_BADVARTYPE;

					break;
				}

				if (lSecondArg.vt == VT_BSTR)
				{
					lSymbolicLink = lSecondArg.bstrVal;
				}
				else
				{
					lresult = DISP_E_BADVARTYPE;

					break;
				}

								
				//std::wstringstream lwstringstream;

				//lwstringstream << lLevelType << lFilePath;

				//MessageBox(NULL, lwstringstream.str().c_str(), L"lFirstArg.vt", 0);

				CComPtrCustom<IUnknown> lSourceControl;

				LOG_INVOKE_FUNCTION(createSourceControl,
					lSymbolicLink,
					lIID,
					&lSourceControl);
				
				LOG_CHECK_PTR_MEMORY(lSourceControl);
				
				CComPtrCustom<IDispatch> lDispSourceControl;

				LOG_INVOKE_QUERY_INTERFACE_METHOD(lSourceControl, &lDispSourceControl);
				
				LOG_CHECK_PTR_MEMORY(lDispSourceControl);
				
				pVarResult->vt = VT_UNKNOWN;

				pVarResult->punkVal = lDispSourceControl.detach();

			} while (false);

			return lresult;
		}

		HRESULT SourceControl::invokeGetSourceOutputMediaTypeFromMediaSource(
			_In_  DISPPARAMS *pDispParams,
			/* [annotation][out] */
			_Out_opt_  VARIANT *pVarResult) {

			HRESULT lresult(DISP_E_UNKNOWNINTERFACE);

			do
			{
				LOG_CHECK_STATE_DESCR(pDispParams == nullptr, DISP_E_PARAMNOTFOUND);

				LOG_CHECK_PTR_MEMORY(pVarResult);

				LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 3, DISP_E_BADPARAMCOUNT);
				
				CComPtrCustom<IUnknown> lExternalSource;

				DWORD lIndexStream;

				DWORD lIndexMediaType;

				VARIANT lFirstArg = pDispParams->rgvarg[0];

				VARIANT lSecondArg = pDispParams->rgvarg[1];

				VARIANT lThirdArg = pDispParams->rgvarg[2];


				if (lThirdArg.vt == VT_UNKNOWN && lThirdArg.punkVal != nullptr)
				{
					LOG_INVOKE_QUERY_INTERFACE_METHOD(lThirdArg.punkVal, &lExternalSource);
				}
				else if (lThirdArg.vt == VT_DISPATCH && lThirdArg.pdispVal != nullptr)
				{
					LOG_INVOKE_QUERY_INTERFACE_METHOD(lThirdArg.pdispVal, &lExternalSource);
				}
				else
				{
					lresult = DISP_E_BADVARTYPE;

					break;
				}

				if (lSecondArg.vt == VT_UI4 ||
					lSecondArg.vt == VT_I4)
				{
					lIndexStream = lSecondArg.uintVal;
				}
				else
				{
					lresult = DISP_E_BADVARTYPE;

					break;
				}

				if (lFirstArg.vt == VT_UI4 ||
					lFirstArg.vt == VT_I4)
				{
					lIndexMediaType = lFirstArg.uintVal;
				}
				else
				{
					lresult = DISP_E_BADVARTYPE;

					break;
				}
				

				//std::wstringstream lwstringstream;

				//lwstringstream << lLevelType << lFilePath;

				//MessageBox(NULL, lwstringstream.str().c_str(), L"lFirstArg.vt", 0);

				CComPtrCustom<IUnknown> lOutputMediaType;

				LOG_INVOKE_FUNCTION(getSourceOutputMediaTypeFromMediaSource,
					lExternalSource,
					lIndexStream,
					lIndexMediaType,
					&lOutputMediaType);
				
				pVarResult->vt = VT_UNKNOWN;

				pVarResult->punkVal = lOutputMediaType.detach();

			} while (false);

			return lresult;
		}
	}
}


