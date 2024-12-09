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

#include "Session.h"
#include "../CaptureManagerBroker/CaptureManagerBroker.h"
#include "../Common/Singleton.h"
#include "../Common/Common.h"
#include "../LogPrintOut/LogPrintOut.h"


namespace CaptureManager
{
	namespace COMServer
	{
		enum class MethodsEnum :DISPID
		{
			StartSession = 1,
			StopSession = StartSession + 1,
			PauseSession = StopSession + 1,
			CloseSession = PauseSession + 1,
			GetSessionDescriptor = CloseSession + 1,
			GetIConnectionPointContainer = GetSessionDescriptor + 1
		};

		HRESULT Session::init(VARIANT aArrayPtrSourceNodesOfTopology)
		{
			mSessionDescriptor = -1;

			HRESULT lresult(E_NOTIMPL);

			do
			{

				LOG_CHECK_STATE(!(aArrayPtrSourceNodesOfTopology.vt & VT_UNKNOWN) ||
					!(aArrayPtrSourceNodesOfTopology.vt & VT_SAFEARRAY));
				
				mSessionCallback = CComPtrCustom<SessionCallbackInner>(new (std::nothrow) SessionCallbackInner);

				LOG_CHECK_PTR_MEMORY(mSessionCallback);
				
				SAFEARRAY* lPtrSASourcesNodes;

				lPtrSASourcesNodes = aArrayPtrSourceNodesOfTopology.parray;

				LONG lBoundSourcesNodes;

				LONG uBoundSourcesNodes;

				LOG_INVOKE_FUNCTION(SafeArrayGetUBound, lPtrSASourcesNodes, 1, &uBoundSourcesNodes);

				LOG_INVOKE_FUNCTION(SafeArrayGetLBound, lPtrSASourcesNodes, 1, &lBoundSourcesNodes);

				std::vector<IUnknown*> lSourceNodes;

				for (LONG lIndex = lBoundSourcesNodes; lIndex <= uBoundSourcesNodes; lIndex++)
				{
					VARIANT lVar;

					LOG_INVOKE_FUNCTION(SafeArrayGetElement, lPtrSASourcesNodes, &lIndex, &lVar);

					if (lVar.vt == VT_UNKNOWN)
					{
						lSourceNodes.push_back(lVar.punkVal);
					}
					else if (lVar.vt == VT_DISPATCH)
					{
						lSourceNodes.push_back(lVar.pdispVal);
					}

					VariantClear(&lVar);
				}

				LOG_CHECK_STATE_DESCR(FAILED(lresult), lresult);

				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().createSessionDescriptor,
					lSourceNodes,
					mSessionCallback,
					mSessionDescriptor);
								
				mConnectionPointContainer = new (std::nothrow) ConnectionPointContainer();

				LOG_CHECK_PTR_MEMORY(mConnectionPointContainer);
				
				mConnectionPointContainer->Release();

				LOG_INVOKE_POINTER_METHOD(mConnectionPointContainer, addConnectionPoint, 
					new (std::nothrow) ConnectionPoint<SessionCallbackInner, ISessionCallback>(
					__uuidof(ISessionCallback),
					mSessionCallback));
								
				lresult = S_OK;

			} while (false);

			return lresult;
		}
		
		STDMETHODIMP Session::startSession(
			LONGLONG aStartPositionInHundredNanosecondUnits,
			REFGUID aGUIDTimeFormat)
		{

			HRESULT lresult(E_NOTIMPL);

			do
			{

				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().startSession,
					mSessionDescriptor,
					aStartPositionInHundredNanosecondUnits,
					aGUIDTimeFormat);
				
			} while (false);

			return lresult;
		}

		STDMETHODIMP Session::stopSession(void)
		{

			HRESULT lresult(E_NOTIMPL);

			do
			{

				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().stopSession,
					mSessionDescriptor);
				
			} while (false);

			return lresult;
		}

		STDMETHODIMP Session::pauseSession(void)
		{

			HRESULT lresult(E_NOTIMPL);

			do
			{

				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().pauseSession,
					mSessionDescriptor);

			} while (false);

			return lresult;
		}

		STDMETHODIMP Session::closeSession(void)
		{

			HRESULT lresult(E_NOTIMPL);

			do
			{

				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().closeSession,
					mSessionDescriptor);
				
			} while (false);

			return lresult;
		}

		STDMETHODIMP Session::getSessionDescriptor(
			DWORD *aPtrSessionDescriptor)
		{
			HRESULT lresult(E_NOTIMPL);

			do
			{
				*aPtrSessionDescriptor = mSessionDescriptor;

				lresult = S_OK;

			} while (false);

			return lresult;
		}

		STDMETHODIMP Session::getIConnectionPointContainer(
			REFIID aREFIID,
			IUnknown **aPtrPtrControl)
		{

			HRESULT lresult(E_NOTIMPL);

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrPtrControl);
				
				lresult = E_NOINTERFACE;

				if (aREFIID == __uuidof(IConnectionPointContainer))
				{

					LOG_CHECK_PTR_MEMORY(mConnectionPointContainer);
					
					LOG_INVOKE_POINTER_METHOD(mConnectionPointContainer, QueryInterface,
						aREFIID,
						(void**)aPtrPtrControl);
				}
								
			} while (false);

			return lresult;
		}



		// IDispatch interface stub

		STDMETHODIMP Session::GetIDsOfNames(
			__RPC__in REFIID riid,
			/* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
			/* [range][in] */ __RPC__in_range(0, 16384) UINT cNames,
			LCID lcid,
			/* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId) {

			HRESULT lresult(DISP_E_UNKNOWNNAME);

			do
			{
				LOG_CHECK_STATE(cNames != 1);
				
				if (_wcsicmp(*rgszNames, OLESTR("startSession")) == 0)
				{
					*rgDispId = (int)MethodsEnum::StartSession;

					lresult = S_OK;
				}
				else if (_wcsicmp(*rgszNames, OLESTR("stopSession")) == 0)
				{
					*rgDispId = (int)MethodsEnum::StopSession;

					lresult = S_OK;
				}
				else if (_wcsicmp(*rgszNames, OLESTR("pauseSession")) == 0)
				{
					*rgDispId = (int)MethodsEnum::PauseSession;

					lresult = S_OK;
				}
				else if (_wcsicmp(*rgszNames, OLESTR("closeSession")) == 0)
				{
					*rgDispId = (int)MethodsEnum::CloseSession;

					lresult = S_OK;
				}
				else if (_wcsicmp(*rgszNames, OLESTR("getSessionDescriptor")) == 0)
				{
					*rgDispId = (int)MethodsEnum::GetSessionDescriptor;

					lresult = S_OK;
				}
				else if (_wcsicmp(*rgszNames, OLESTR("getIConnectionPointContainer")) == 0)
				{
					*rgDispId = (int)MethodsEnum::GetIConnectionPointContainer;

					lresult = S_OK;
				}

			} while (false);

			return lresult;
		}

		HRESULT Session::invokeMethod(
			/* [annotation][in] */
			DISPID dispIdMember,
			/* [annotation][out][in] */
			DISPPARAMS *pDispParams,
			/* [annotation][out] */
			VARIANT *pVarResult) {

			HRESULT lresult(DISP_E_UNKNOWNINTERFACE);

			do
			{
				LOG_CHECK_STATE_DESCR(pDispParams == nullptr, DISP_E_PARAMNOTFOUND);

				LOG_CHECK_PTR_MEMORY(pVarResult);

				switch (dispIdMember)
				{
				case (int)MethodsEnum::StartSession:
				{

					LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 2, DISP_E_BADPARAMCOUNT);
					
					LONGLONG lStartPositionInHundredNanosecondUnits;

					GUID lGUIDTimeFormat;
					
					if (pDispParams->rgvarg[0].vt == VT_BSTR && pDispParams->rgvarg[0].bstrVal != nullptr)
					{
						LOG_INVOKE_FUNCTION(CLSIDFromString, pDispParams->rgvarg[0].bstrVal, &lGUIDTimeFormat);
					}
					else
					{
						lresult = DISP_E_BADVARTYPE;

						break;
					}

					if (pDispParams->rgvarg[1].vt == VT_I8)
					{
						lStartPositionInHundredNanosecondUnits = pDispParams->rgvarg[1].llVal;
					}
					else if (pDispParams->rgvarg[1].vt == VT_I4)
					{
						lStartPositionInHundredNanosecondUnits = pDispParams->rgvarg[1].intVal;
					}
					else if (pDispParams->rgvarg[1].vt == VT_UI4)
					{
						lStartPositionInHundredNanosecondUnits = pDispParams->rgvarg[1].uintVal;
					}
					else
					{
						lresult = DISP_E_BADVARTYPE;

						break;
					}
										
					LOG_INVOKE_FUNCTION(startSession,
						lStartPositionInHundredNanosecondUnits,
						lGUIDTimeFormat);
				}
				break;
				case (int)MethodsEnum::StopSession:
				{
					LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 0, DISP_E_BADPARAMCOUNT);
					
					LOG_INVOKE_FUNCTION(stopSession);

				}
				break;
				case (int)MethodsEnum::PauseSession:
				{
					LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 0, DISP_E_BADPARAMCOUNT);

					LOG_INVOKE_FUNCTION(pauseSession);

				}
				break;
				case (int)MethodsEnum::CloseSession:
				{
					LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 0, DISP_E_BADPARAMCOUNT);

					LOG_INVOKE_FUNCTION(closeSession);

				}
				break;
				case (int)MethodsEnum::GetSessionDescriptor:
				{
					LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 0, DISP_E_BADPARAMCOUNT);

					DWORD ldesr;

					LOG_INVOKE_FUNCTION(getSessionDescriptor, &ldesr);
					
					pVarResult->vt = VT_UI4;

					pVarResult->ulVal = ldesr;
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