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


#include "EncoderControl.h"
#include "../PugiXML/pugixml.hpp"
#include "../Common/ComPtrCustom.h"
#include "../CaptureManagerBroker/CaptureManagerBroker.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Common.h"
#include "EncoderNodeFactory.h"

namespace CaptureManager
{
	namespace COMServer
	{
		enum class MethodsEnum :DISPID
		{
			GetCollectionOfEncoders = 1,
			GetMediaTypeCollectionOfEncoder = GetCollectionOfEncoders + 1,
			CreateEncoderNodeFactory = GetMediaTypeCollectionOfEncoder + 1
		};

		// IEncoderControl interface implementation

		STDMETHODIMP EncoderControl::getCollectionOfEncoders(
			/* [out][in] */ BSTR *aPtrPtrXMLstring)
		{
			using namespace pugi;

			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrPtrXMLstring);

				if (*aPtrPtrXMLstring != nullptr)
				{
					SysFreeString(*aPtrPtrXMLstring);
				}							

				std::wstring lXMLDoc;

				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().getXMLDocumentStringOfEncoders,
					lXMLDoc);
				
				*aPtrPtrXMLstring = SysAllocString(lXMLDoc.c_str());								

			} while (false);
			
			return lresult;
		}

		STDMETHODIMP EncoderControl::getMediaTypeCollectionOfEncoder(
			IUnknown *aPtrUncompressedMediaType,
			REFCLSID aRefEncoderCLSID,
			BSTR *aPtrPtrXMLstring)
		{
			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrPtrXMLstring);

				if (*aPtrPtrXMLstring != nullptr)
				{
					SysFreeString(*aPtrPtrXMLstring);
				}

				std::wstring lXMLDoc;

				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().getXMLDocumentStringEncoderMediaTypes,
					aPtrUncompressedMediaType,
					aRefEncoderCLSID,
					lXMLDoc);

				*aPtrPtrXMLstring = SysAllocString(lXMLDoc.c_str());
				
			} while (false);
			
			return lresult;
		}

		STDMETHODIMP EncoderControl::createEncoderNodeFactory(
			REFCLSID aRefEncoderCLSID,
			REFIID aREFIID,
			IUnknown **aPtrPtrEncoderNodeFactory)
		{
			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrPtrEncoderNodeFactory);

				CComPtrCustom<IEncoderNodeFactory> lIUnknown = new (std::nothrow) EncoderNodeFactory();

				LOG_CHECK_PTR_MEMORY(lIUnknown);
				
				CComQIPtrCustom<IContainer> lIContainer = lIUnknown;

				LOG_CHECK_STATE_DESCR(!lIContainer, E_NOINTERFACE);
				
				LOG_INVOKE_POINTER_METHOD(lIContainer, setContainerFormat, aRefEncoderCLSID);
				
				LOG_INVOKE_POINTER_METHOD(lIContainer, QueryInterface,
					aREFIID,
					(void**)aPtrPtrEncoderNodeFactory);
				
			} while (false);
			
			return lresult;
		}
		

		// IDispatch interface stub

		STDMETHODIMP EncoderControl::GetIDsOfNames(
			__RPC__in REFIID riid,
			/* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
			/* [range][in] */ __RPC__in_range(0, 16384) UINT cNames,
			LCID lcid,
			/* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId) {

			HRESULT lresult(DISP_E_UNKNOWNNAME);

			do
			{
				LOG_CHECK_STATE(cNames != 1);
				
				if (_wcsicmp(*rgszNames, OLESTR("getCollectionOfEncoders")) == 0)
				{
					*rgDispId = (int)MethodsEnum::GetCollectionOfEncoders;

					lresult = S_OK;
				}
				else if (_wcsicmp(*rgszNames, OLESTR("getMediaTypeCollectionOfEncoder")) == 0)
				{
					*rgDispId = (int)MethodsEnum::GetMediaTypeCollectionOfEncoder;

					lresult = S_OK;
				}
				else if (_wcsicmp(*rgszNames, OLESTR("createEncoderNodeFactory")) == 0)
				{
					*rgDispId = (int)MethodsEnum::CreateEncoderNodeFactory;

					lresult = S_OK;
				}

			} while (false);

			return lresult;
		}

		HRESULT EncoderControl::invokeMethod(
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
				case (int)MethodsEnum::GetCollectionOfEncoders:
				{
					pVarResult->vt = VT_BSTR;

					LOG_INVOKE_FUNCTION(getCollectionOfEncoders, &pVarResult->bstrVal);
				}
				break;
				case (int)MethodsEnum::GetMediaTypeCollectionOfEncoder:
				{

					LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 2, DISP_E_BADPARAMCOUNT);
					
					CLSID lEncoderID;
					
					if (pDispParams->rgvarg[0].vt == VT_CLSID)
					{
					}
					else if (pDispParams->rgvarg[0].vt == VT_BSTR && pDispParams->rgvarg[0].bstrVal != nullptr)
					{
						LOG_INVOKE_FUNCTION(CLSIDFromString, pDispParams->rgvarg[0].bstrVal, &lEncoderID);
					}
					else
					{
						lresult = DISP_E_BADVARTYPE;

						break;
					}

					CComPtrCustom<IUnknown> lUncompressedMediaType;

					if (pDispParams->rgvarg[1].vt == VT_UNKNOWN && pDispParams->rgvarg[1].punkVal != nullptr)
					{
						pDispParams->rgvarg[1].punkVal->QueryInterface(IID_PPV_ARGS(&lUncompressedMediaType));
					}
					else
					{
						lresult = DISP_E_BADVARTYPE;

						break;
					}

					LOG_CHECK_STATE_DESCR(!lUncompressedMediaType, DISP_E_BADVARTYPE);
					
					LOG_INVOKE_FUNCTION(getMediaTypeCollectionOfEncoder,
						lUncompressedMediaType,
						lEncoderID,
						&pVarResult->bstrVal);
					
					pVarResult->vt = VT_BSTR;
				}
				break;
				case (int)MethodsEnum::CreateEncoderNodeFactory:
				{

					LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 2, DISP_E_BADPARAMCOUNT);
					
					CLSID lEncoderID;

					if (pDispParams->rgvarg[1].vt == VT_CLSID)
					{
					}
					else if (pDispParams->rgvarg[1].vt == VT_BSTR && pDispParams->rgvarg[1].bstrVal != nullptr)
					{
						LOG_INVOKE_FUNCTION(CLSIDFromString, pDispParams->rgvarg[1].bstrVal, &lEncoderID);
					}
					else
					{
						lresult = DISP_E_BADVARTYPE;

						break;
					}

					CLSID lInterfaceID;

					if (pDispParams->rgvarg[0].vt == VT_CLSID)
					{
					}
					else if (pDispParams->rgvarg[0].vt == VT_BSTR && pDispParams->rgvarg[0].bstrVal != nullptr)
					{
						LOG_INVOKE_FUNCTION(CLSIDFromString, pDispParams->rgvarg[0].bstrVal, &lInterfaceID);
					}
					else
					{
						lresult = DISP_E_BADVARTYPE;

						break;
					}
					
					LOG_INVOKE_FUNCTION(createEncoderNodeFactory,
						lEncoderID,
						lInterfaceID,
						&pVarResult->punkVal);
					
					pVarResult->vt = VT_UNKNOWN;

				}
				break;

				default:
					break;
				}


			} while (false);


			return lresult;
		}
	}
}