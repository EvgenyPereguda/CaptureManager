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

#include "SwitcherNodeFactory.h"
#include "../CaptureManagerBroker/CaptureManagerBroker.h"
#include "../Common/ComPtrCustom.h"
#include "../LogPrintOut/LogPrintOut.h"


namespace CaptureManager
{
	namespace COMServer
	{

		enum class MethodsEnum :DISPID
		{
			CreateSwitcherNode = 1
		};

		SwitcherNodeFactory::SwitcherNodeFactory()
		{
		}
		
		SwitcherNodeFactory::~SwitcherNodeFactory()
		{
		}

		// ISwitcherNodeFactory interface implementation

		STDMETHODIMP SwitcherNodeFactory::createSwitcherNode(
			IUnknown *aPtrDownStreamTopologyNode,
			IUnknown **aPtrPtrTopologySwitcherNode)
		{
			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrPtrTopologySwitcherNode);
				
				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().createSwitcherNode,
					aPtrDownStreamTopologyNode,
					aPtrPtrTopologySwitcherNode);

			} while (false);

			return lresult;
		}




		// IDispatch interface stub

		STDMETHODIMP SwitcherNodeFactory::GetIDsOfNames(
			__RPC__in REFIID riid,
			/* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
			/* [range][in] */ __RPC__in_range(0, 16384) UINT cNames,
			LCID lcid,
			/* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId) {

			HRESULT lresult(DISP_E_UNKNOWNNAME);

			do
			{
				LOG_CHECK_STATE(cNames != 1);

				if (_wcsicmp(*rgszNames, OLESTR("createSwitcherNode")) == 0)
				{
					*rgDispId = (int)MethodsEnum::CreateSwitcherNode;

					lresult = S_OK;
				}

			} while (false);

			return lresult;
		}

		HRESULT SwitcherNodeFactory::invokeMethod(
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
				case (int)MethodsEnum::CreateSwitcherNode:
				{

					LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 1, DISP_E_BADPARAMCOUNT);

					CComPtrCustom<IUnknown> lDownStreamTopologyNode;

					if (pDispParams->rgvarg[0].vt == VT_UNKNOWN && pDispParams->rgvarg[0].punkVal != nullptr)
					{
						LOG_INVOKE_QUERY_INTERFACE_METHOD(pDispParams->rgvarg[0].punkVal, &lDownStreamTopologyNode);
					}
					else
					{
						lresult = DISP_E_BADVARTYPE;

						break;
					}

					LOG_CHECK_PTR_MEMORY(lDownStreamTopologyNode);

					CComPtrCustom<IUnknown> lSwitcherNode;

					LOG_INVOKE_FUNCTION(createSwitcherNode,
						lDownStreamTopologyNode,
						&lSwitcherNode);

					pVarResult->vt = VT_UNKNOWN;

					pVarResult->punkVal = lSwitcherNode.detach();

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