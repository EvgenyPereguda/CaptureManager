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

#include "SwitcherControl.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Common.h"
#include "../Common/Macros.h"
#include "../CaptureManagerBroker/CaptureManagerBroker.h"
#include "../Switcher/ISwitcher.h"
#include "../Common/Singleton.h"
#include "../Common/ComPtrCustom.h"


namespace CaptureManager
{
	namespace COMServer
	{
		enum class MethodsEnum :DISPID
		{
			PauseSwitchers = 1,
			ResumeSwitchers = PauseSwitchers + 1,
			PauseSwitcher = ResumeSwitchers + 1,
			ResumeSwitcher = PauseSwitcher + 1,
			DetachSwitchers = ResumeSwitcher + 1,
			AttachSwitcher = DetachSwitchers + 1
		};
		

		// ISwitcherControl interface

		HRESULT STDMETHODCALLTYPE SwitcherControl::pauseSwitchers(
			/* [in] */ DWORD aSessionDescriptor)
		{
			HRESULT lresult(E_NOTIMPL);

			do
			{				
				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().pauseSwitchers,
					aSessionDescriptor);

			} while (false);

			return lresult;
		}

		HRESULT STDMETHODCALLTYPE SwitcherControl::resumeSwitchers(
			/* [in] */ DWORD aSessionDescriptor)
		{
			HRESULT lresult(E_NOTIMPL);

			do
			{
				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().resumeSwitchers,
					aSessionDescriptor);

			} while (false);

			return lresult;
		}
		
		HRESULT STDMETHODCALLTYPE SwitcherControl::pauseSwitcher(
			/* [in] */ IUnknown *aPtrSwitcherNode)
		{
			using namespace Transform;

			HRESULT lresult(E_NOTIMPL);

			do
			{
				CComPtrCustom<ISwitcher> lSwitcher;

				LOG_CHECK_PTR_MEMORY(aPtrSwitcherNode);

				LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrSwitcherNode, &lSwitcher);

				LOG_CHECK_PTR_MEMORY(lSwitcher);

				lresult = lSwitcher->pause();

			} while (false);

			return lresult;
		}

		HRESULT STDMETHODCALLTYPE SwitcherControl::resumeSwitcher(
			/* [in] */ IUnknown *aPtrSwitcherNode)
		{
			using namespace Transform;

			HRESULT lresult(E_NOTIMPL);

			do
			{
				CComPtrCustom<ISwitcher> lSwitcher;

				LOG_CHECK_PTR_MEMORY(aPtrSwitcherNode);

				LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrSwitcherNode, &lSwitcher);

				LOG_CHECK_PTR_MEMORY(lSwitcher);

				lresult = lSwitcher->resume();

			} while (false);

			return lresult;
		}

		HRESULT STDMETHODCALLTYPE SwitcherControl::detachSwitchers(
			/* [in] */ DWORD aSessionDescriptor)
		{
			HRESULT lresult(E_NOTIMPL);

			do
			{

				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().pauseSwitchers,
					aSessionDescriptor);

				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().detachSwitchers,
					aSessionDescriptor);

			} while (false);

			return lresult;
		}
				
		HRESULT STDMETHODCALLTYPE SwitcherControl::attachSwitcher(
			/* [in] */ IUnknown *aPtrSwitcherNode,
			/* [in] */ IUnknown *aPtrDownStreamNode)
		{
			HRESULT lresult(E_NOTIMPL);

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrSwitcherNode);

				LOG_CHECK_PTR_MEMORY(aPtrDownStreamNode);

				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().attachSwitcher,
					aPtrSwitcherNode,
					aPtrDownStreamNode);

			} while (false);

			return lresult;
		}


		// IDispatch interface stub

		STDMETHODIMP SwitcherControl::GetIDsOfNames(
			__RPC__in REFIID riid,
			/* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
			/* [range][in] */ __RPC__in_range(0, 16384) UINT cNames,
			LCID lcid,
			/* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId) {

			HRESULT lresult(DISP_E_UNKNOWNNAME);

			do
			{
				LOG_CHECK_STATE(cNames != 1);

				if (_wcsicmp(*rgszNames, OLESTR("pauseSwitchers")) == 0)
				{
					*rgDispId = (int)MethodsEnum::PauseSwitchers;

					lresult = S_OK;
				}
				else if (_wcsicmp(*rgszNames, OLESTR("resumeSwitchers")) == 0)
				{
					*rgDispId = (int)MethodsEnum::ResumeSwitchers;

					lresult = S_OK;
				}
				else if (_wcsicmp(*rgszNames, OLESTR("pauseSwitcher")) == 0)
				{
					*rgDispId = (int)MethodsEnum::PauseSwitcher;

					lresult = S_OK;
				}
				else if (_wcsicmp(*rgszNames, OLESTR("resumeSwitcher")) == 0)
				{
					*rgDispId = (int)MethodsEnum::ResumeSwitcher;

					lresult = S_OK;
				}
				else if (_wcsicmp(*rgszNames, OLESTR("detachSwitchers")) == 0)
				{
					*rgDispId = (int)MethodsEnum::DetachSwitchers;

					lresult = S_OK;
				}
				else if (_wcsicmp(*rgszNames, OLESTR("attachSwitcher")) == 0)
				{
					*rgDispId = (int)MethodsEnum::AttachSwitcher;

					lresult = S_OK;
				}

				

			} while (false);

			return lresult;
		}

		HRESULT SwitcherControl::invokeMethod(
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
				case (int)MethodsEnum::PauseSwitchers:
				{
					LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 1, DISP_E_BADPARAMCOUNT);

					DWORD lSessionDescriptor = 0;

					if (pDispParams->rgvarg[0].vt == VT_I8)
					{
						lSessionDescriptor = (DWORD)(pDispParams->rgvarg[0].llVal);
					}
					else if (pDispParams->rgvarg[0].vt == VT_I4)
					{
						lSessionDescriptor = (DWORD)(pDispParams->rgvarg[0].intVal);
					}
					else if (pDispParams->rgvarg[0].vt == VT_UI4)
					{
						lSessionDescriptor = pDispParams->rgvarg[0].uintVal;
					}
					else
					{
						lresult = DISP_E_BADVARTYPE;

						break;
					}

					LOG_INVOKE_FUNCTION(pauseSwitchers, lSessionDescriptor);
				}
				break;
				case (int)MethodsEnum::ResumeSwitchers:
				{
					LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 1, DISP_E_BADPARAMCOUNT);

					DWORD lSessionDescriptor = 0;

					if (pDispParams->rgvarg[0].vt == VT_I8)
					{
						lSessionDescriptor = (DWORD)(pDispParams->rgvarg[0].llVal);
					}
					else if (pDispParams->rgvarg[0].vt == VT_I4)
					{
						lSessionDescriptor = (DWORD)(pDispParams->rgvarg[0].intVal);
					}
					else if (pDispParams->rgvarg[0].vt == VT_UI4)
					{
						lSessionDescriptor = pDispParams->rgvarg[0].uintVal;
					}
					else
					{
						lresult = DISP_E_BADVARTYPE;

						break;
					}

					LOG_INVOKE_FUNCTION(resumeSwitchers, lSessionDescriptor);
				}
				break;
				case (int)MethodsEnum::PauseSwitcher:
				{
					LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 1, DISP_E_BADPARAMCOUNT);

					CComPtrCustom<IUnknown> lSwitcherNode;

					if (pDispParams->rgvarg[0].vt == VT_UNKNOWN && pDispParams->rgvarg[0].punkVal != nullptr)
					{
						LOG_INVOKE_QUERY_INTERFACE_METHOD(pDispParams->rgvarg[0].punkVal, &lSwitcherNode);
					}
					else
					{
						lresult = DISP_E_BADVARTYPE;

						break;
					}

					LOG_INVOKE_FUNCTION(pauseSwitcher,
						lSwitcherNode);
				}
				break;
				case (int)MethodsEnum::ResumeSwitcher:
				{
					LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 1, DISP_E_BADPARAMCOUNT);

					CComPtrCustom<IUnknown> lSwitcherNode;

					if (pDispParams->rgvarg[0].vt == VT_UNKNOWN && pDispParams->rgvarg[0].punkVal != nullptr)
					{
						LOG_INVOKE_QUERY_INTERFACE_METHOD(pDispParams->rgvarg[0].punkVal, &lSwitcherNode);
					}
					else
					{
						lresult = DISP_E_BADVARTYPE;

						break;
					}

					LOG_INVOKE_FUNCTION(resumeSwitcher,
						lSwitcherNode);
				}
				break;
				case (int)MethodsEnum::DetachSwitchers:
				{
					LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 1, DISP_E_BADPARAMCOUNT);

					DWORD lSessionDescriptor = 0;

					if (pDispParams->rgvarg[0].vt == VT_I8)
					{
						lSessionDescriptor = (DWORD)(pDispParams->rgvarg[0].llVal);
					}
					else if (pDispParams->rgvarg[0].vt == VT_I4)
					{
						lSessionDescriptor = (DWORD)(pDispParams->rgvarg[0].intVal);
					}
					else if (pDispParams->rgvarg[0].vt == VT_UI4)
					{
						lSessionDescriptor = pDispParams->rgvarg[0].uintVal;
					}
					else
					{
						lresult = DISP_E_BADVARTYPE;

						break;
					}

					LOG_INVOKE_FUNCTION(detachSwitchers, lSessionDescriptor);
				}
				break;
				case (int)MethodsEnum::AttachSwitcher:
				{
					LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 2, DISP_E_BADPARAMCOUNT);
					
					CComPtrCustom<IUnknown> lSwitcherNode;

					if (pDispParams->rgvarg[1].vt == VT_UNKNOWN && pDispParams->rgvarg[1].punkVal != nullptr)
					{
						LOG_INVOKE_QUERY_INTERFACE_METHOD(pDispParams->rgvarg[1].punkVal, &lSwitcherNode);
					}
					else
					{
						lresult = DISP_E_BADVARTYPE;

						break;
					}


					CComPtrCustom<IUnknown> lDownStreamNode;

					if (pDispParams->rgvarg[0].vt == VT_UNKNOWN && pDispParams->rgvarg[0].punkVal != nullptr)
					{
						LOG_INVOKE_QUERY_INTERFACE_METHOD(pDispParams->rgvarg[0].punkVal, &lDownStreamNode);
					}
					else
					{
						lresult = DISP_E_BADVARTYPE;

						break;
					}
					
					LOG_INVOKE_FUNCTION(attachSwitcher,
						lSwitcherNode,
						lDownStreamNode);
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