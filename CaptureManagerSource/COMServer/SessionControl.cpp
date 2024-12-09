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

#include "SessionControl.h"
#include "Session.h"
#include "../Common/ComPtrCustom.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Common.h"


namespace CaptureManager
{
	namespace COMServer
	{ 

		enum class MethodsEnum :DISPID
		{
			CreateSession = 1
		};

		STDMETHODIMP SessionControl::createSession(
			VARIANT aArrayPtrSourceNodesOfTopology,
			REFIID aREFIID,
			IUnknown **aPtrPtrSession)
		{

			HRESULT lresult;

			do
			{
				LOG_CHECK_STATE(aPtrPtrSession == nullptr);
				
				LOG_CHECK_STATE(!(aArrayPtrSourceNodesOfTopology.vt & VT_UNKNOWN) ||
					!(aArrayPtrSourceNodesOfTopology.vt & VT_SAFEARRAY));

				CComPtrCustom<IUnknown> ltempInstance;				

				if (aREFIID == __uuidof(ISession))
				{
					auto lSession = CComPtrCustom<Session>(new (std::nothrow)Session);

					LOG_INVOKE_POINTER_METHOD(lSession, init, aArrayPtrSourceNodesOfTopology);
					
					LOG_INVOKE_QUERY_INTERFACE_METHOD(lSession, &ltempInstance);
				}				
				else
				{
					lresult = E_NOINTERFACE;

					break;
				}
				
				LOG_CHECK_PTR_MEMORY(ltempInstance);
								
				LOG_INVOKE_FUNCTION(ltempInstance->QueryInterface,
					aREFIID,
					(void**)aPtrPtrSession);

			} while (false);

			return lresult;
		}




		// IDispatch interface stub

		STDMETHODIMP SessionControl::GetIDsOfNames(
			__RPC__in REFIID riid,
			/* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
			/* [range][in] */ __RPC__in_range(0, 16384) UINT cNames,
			LCID lcid,
			/* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId) {

			HRESULT lresult(DISP_E_UNKNOWNNAME);

			do
			{
				LOG_CHECK_STATE(cNames != 1);
				
				if (_wcsicmp(*rgszNames, OLESTR("createSession")) == 0)
				{
					*rgDispId = (int)MethodsEnum::CreateSession;

					lresult = S_OK;
				}

			} while (false);

			return lresult;
		}

		HRESULT SessionControl::invokeMethod(
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
				case (int)MethodsEnum::CreateSession:
				{

					LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 2, DISP_E_BADPARAMCOUNT)
					
					VARIANT lArrayPtrSourceNodesOfTopology;

					GUID lIID;
					
					if (pDispParams->rgvarg[1].vt == VT_VARIANT && pDispParams->rgvarg[1].pvarVal != nullptr)
					{
						lArrayPtrSourceNodesOfTopology = *(pDispParams->rgvarg[1].pvarVal);
					}
					else if (pDispParams->rgvarg[1].vt == (VT_ARRAY | VT_VARIANT))
					{

						SAFEARRAY* lPtrSAMediaTypes = nullptr;

						lPtrSAMediaTypes = pDispParams->rgvarg[1].parray;

						LOG_CHECK_STATE_DESCR(lPtrSAMediaTypes == nullptr, DISP_E_BADVARTYPE);
						
						lArrayPtrSourceNodesOfTopology.vt = VT_UNKNOWN | VT_SAFEARRAY;

						lArrayPtrSourceNodesOfTopology.parray = lPtrSAMediaTypes;

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
					
					CComPtrCustom<IUnknown> lSession;

					LOG_INVOKE_FUNCTION(createSession,
						lArrayPtrSourceNodesOfTopology,
						lIID,
						&lSession);
					
					pVarResult->vt = VT_UNKNOWN;

					pVarResult->punkVal = lSession.detach();

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