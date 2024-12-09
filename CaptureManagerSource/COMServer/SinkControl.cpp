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

#include <sstream>
#include <Unknwnbase.h>

#include "../Common/Common.h"
#include "../Common/Singleton.h"
#include "../Common/ComPtrCustom.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "SinkControl.h"
#include "SinkCollection.h"
#include "IContainer.h"

namespace CaptureManager
{
	namespace COMServer
	{
		enum class MethodsEnum :DISPID
		{
			GetCollectionOfSinks = 1,
			CreateSinkFactory = GetCollectionOfSinks + 1
		};
		// ISinkControl interface implementation

		STDMETHODIMP SinkControl::getCollectionOfSinks(
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

				std::wstringstream lwstringstream;

				LOG_INVOKE_FUNCTION(Singleton<SinkCollection>::getInstance().getXMLDocumentStringListOfSinkFactories,
					lwstringstream);
				
				*aPtrPtrXMLstring = SysAllocString(lwstringstream.str().c_str());
				
			} while (false);
			
			return lresult;
		}

		STDMETHODIMP SinkControl::createSinkFactory(
			REFGUID aRefContainerTypeGUID,
			REFIID aREFIID,
			IUnknown **aPtrPtrSink)
		{
			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrPtrSink);

				CComPtrCustom<IUnknown> lIUnknown;

				LOG_INVOKE_FUNCTION(Singleton<SinkCollection>::getInstance().createSink,
					aREFIID, 
					&lIUnknown);

				LOG_CHECK_PTR_MEMORY(lIUnknown);
				
				CComQIPtrCustom<IContainer> lIContainer = lIUnknown;

				LOG_CHECK_PTR_MEMORY(lIContainer);
				
				LOG_INVOKE_POINTER_METHOD(lIContainer, setContainerFormat,
					aRefContainerTypeGUID);
								
				LOG_INVOKE_POINTER_METHOD(lIContainer, QueryInterface,
					aREFIID,
					(void**)aPtrPtrSink);

				LOG_CHECK_STATE_DESCR(FAILED(lresult), lresult);

			} while (false);
			
			return lresult;
		}



		// IDispatch interface stub

		STDMETHODIMP SinkControl::GetIDsOfNames(
			__RPC__in REFIID riid,
			/* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
			/* [range][in] */ __RPC__in_range(0, 16384) UINT cNames,
			LCID lcid,
			/* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId) {

			HRESULT lresult(DISP_E_UNKNOWNNAME);

			do
			{
				LOG_CHECK_STATE(cNames != 1);
				
				if (_wcsicmp(*rgszNames, OLESTR("getCollectionOfSinks")) == 0)
				{
					*rgDispId = (int)MethodsEnum::GetCollectionOfSinks;

					lresult = S_OK;
				}
				else if (_wcsicmp(*rgszNames, OLESTR("createSinkFactory")) == 0)
				{
					*rgDispId = (int)MethodsEnum::CreateSinkFactory;

					lresult = S_OK;
				}

			} while (false);

			return lresult;
		}

		HRESULT SinkControl::invokeMethod(
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
				case (int)MethodsEnum::GetCollectionOfSinks:
				{

					LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 0, DISP_E_BADPARAMCOUNT);
										
					LOG_INVOKE_FUNCTION(getCollectionOfSinks,
						&pVarResult->bstrVal);
					
					pVarResult->vt = VT_BSTR;

				}
				break;
				case (int)MethodsEnum::CreateSinkFactory:
				{
					LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 2, DISP_E_BADPARAMCOUNT);
					
					GUID lContainerTypeGUID;
					
					IID lIID;					
					
					if (pDispParams->rgvarg[1].vt == VT_CLSID)
					{
					}
					else if (pDispParams->rgvarg[1].vt == VT_BSTR && pDispParams->rgvarg[1].bstrVal != nullptr)
					{
						LOG_INVOKE_FUNCTION(CLSIDFromString, pDispParams->rgvarg[1].bstrVal, &lContainerTypeGUID);
					}
					else
					{
						lresult = DISP_E_BADVARTYPE;

						break;
					}



					if (pDispParams->rgvarg[0].vt == VT_CLSID)
					{
					}
					else if (pDispParams->rgvarg[0].vt == VT_BSTR && pDispParams->rgvarg[0].bstrVal != nullptr)
					{
						LOG_INVOKE_FUNCTION(CLSIDFromString, pDispParams->rgvarg[0].bstrVal, &lIID);
					}
					else
					{
						lresult = DISP_E_BADVARTYPE;

						break;
					}

					CComPtrCustom<IUnknown> lSinkNode;

					LOG_INVOKE_FUNCTION(createSinkFactory,
						lContainerTypeGUID,
						lIID,
						&lSinkNode);
					
					pVarResult->vt = VT_UNKNOWN;

					pVarResult->punkVal = lSinkNode.detach();
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