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

#include "RenderingControl.h"
#include "../Common/ComPtrCustom.h"
#include "../Common/Singleton.h"
#include "../Common/Common.h"
#include "../CaptureManagerBroker/CaptureManagerBroker.h"
#include "../LogPrintOut/LogPrintOut.h"

namespace CaptureManager
{
	namespace COMServer
	{
		enum class MethodsEnum :DISPID
		{
			EnableInnerRendering = 1,
			RenderToTarget = EnableInnerRendering + 1
		};

		RenderingControl::RenderingControl()
		{
		}

		RenderingControl::~RenderingControl()
		{
		}


		// IRenderingControl implements

		HRESULT STDMETHODCALLTYPE RenderingControl::enableInnerRendering(
			/* [in] */ IUnknown *aPtrEVROutputNode,
			/* [in] */ BOOL aIsInnerRendering)
		{

			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrEVROutputNode);

				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().enableInnerRendering,
					aPtrEVROutputNode,
					aIsInnerRendering);

			} while (false);

			return lresult;
		}

		HRESULT STDMETHODCALLTYPE RenderingControl::renderToTarget(
			/* [in] */ IUnknown *aPtrEVROutputNode,
			/* [in] */ IUnknown *aPtrRenderTarget,
			/* [in] */ BOOL aCopyMode)
		{

			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrEVROutputNode);

				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().renderToTarget,
					aPtrEVROutputNode,
					aPtrRenderTarget,
					aCopyMode);

			} while (false);

			return lresult;
		}

		// IDispatch interface stub

		STDMETHODIMP RenderingControl::GetIDsOfNames(
			__RPC__in REFIID riid,
			/* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
			/* [range][in] */ __RPC__in_range(0, 16384) UINT cNames,
			LCID lcid,
			/* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId) {

			HRESULT lresult(DISP_E_UNKNOWNNAME);

			do
			{
				LOG_CHECK_STATE(cNames != 1);

				if (_wcsicmp(*rgszNames, OLESTR("enableInnerRendering")) == 0)
				{
					*rgDispId = (int)MethodsEnum::EnableInnerRendering;

					lresult = S_OK;
				}
				else if (_wcsicmp(*rgszNames, OLESTR("renderToTarget")) == 0)
				{
					*rgDispId = (int)MethodsEnum::RenderToTarget;

					lresult = S_OK;
				}

			} while (false);

			return lresult;
		}

		HRESULT RenderingControl::invokeMethod(
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
				case (int)MethodsEnum::EnableInnerRendering:
				{
					lresult = invokeEnableInnerRendering(
						pDispParams,
						pVarResult);
				}
					break;
				case (int)MethodsEnum::RenderToTarget:
				{
					lresult = invokeRenderToTarget(
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



		HRESULT STDMETHODCALLTYPE RenderingControl::invokeEnableInnerRendering(
			_In_  DISPPARAMS *pDispParams,
			/* [annotation][out] */
			_Out_opt_  VARIANT *pVarResult)
		{

			HRESULT lresult(DISP_E_UNKNOWNINTERFACE);

			do
			{

				LOG_CHECK_STATE_DESCR(pDispParams == nullptr, DISP_E_PARAMNOTFOUND);

				LOG_CHECK_PTR_MEMORY(pVarResult);

				LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 2, DISP_E_BADPARAMCOUNT);


				CComPtrCustom<IUnknown> lUnkEVROutputNode;


				BOOL lIsInnerRendering(TRUE);
				
				VARIANT lSecondArg = pDispParams->rgvarg[0];

				VARIANT lFirstArg = pDispParams->rgvarg[1];


				if (lFirstArg.vt == VT_UNKNOWN && lFirstArg.punkVal != nullptr)
				{
					lFirstArg.punkVal->QueryInterface(IID_PPV_ARGS(&lUnkEVROutputNode));
				}
				else if (lFirstArg.vt == VT_DISPATCH && lFirstArg.pdispVal != nullptr)
				{
					lFirstArg.pdispVal->QueryInterface(IID_PPV_ARGS(&lUnkEVROutputNode));
				}
				else
				{
					lresult = DISP_E_BADVARTYPE;

					break;
				}

				if (lSecondArg.vt == VT_BOOL)
				{
					lIsInnerRendering = lSecondArg.boolVal;
				}
				
				LOG_INVOKE_FUNCTION(enableInnerRendering,
					lUnkEVROutputNode,
					lIsInnerRendering);

			} while (false);

			return lresult;
		}

		HRESULT STDMETHODCALLTYPE RenderingControl::invokeRenderToTarget(
			_In_  DISPPARAMS *pDispParams,
			/* [annotation][out] */
			_Out_opt_  VARIANT *pVarResult)
		{
			HRESULT lresult(DISP_E_UNKNOWNINTERFACE);

			do
			{

				LOG_CHECK_STATE_DESCR(pDispParams == nullptr, DISP_E_PARAMNOTFOUND);

				LOG_CHECK_PTR_MEMORY(pVarResult);

				LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 3, DISP_E_BADPARAMCOUNT);


				CComPtrCustom<IUnknown> lUnkEVROutputNode;

				CComPtrCustom<IUnknown> lUnkTargetRender;

				BOOL lIsInnerRendering(TRUE);

				
				VARIANT lThirdArg = pDispParams->rgvarg[0];

				VARIANT lSecondArg = pDispParams->rgvarg[1];

				VARIANT lFirstArg = pDispParams->rgvarg[2];


				if (lFirstArg.vt == VT_UNKNOWN && lFirstArg.punkVal != nullptr)
				{
					lFirstArg.punkVal->QueryInterface(IID_PPV_ARGS(&lUnkEVROutputNode));
				}
				else if (lFirstArg.vt == VT_DISPATCH && lFirstArg.pdispVal != nullptr)
				{
					lFirstArg.pdispVal->QueryInterface(IID_PPV_ARGS(&lUnkEVROutputNode));
				}
				else
				{
					lresult = DISP_E_BADVARTYPE;

					break;
				}

				if (lSecondArg.vt == VT_UNKNOWN && lSecondArg.punkVal != nullptr)
				{
					lSecondArg.punkVal->QueryInterface(IID_PPV_ARGS(&lUnkTargetRender));
				}
				else
				{
					lresult = DISP_E_BADVARTYPE;

					break;
				}

				if (lThirdArg.vt == VT_BOOL)
				{
					lIsInnerRendering = lThirdArg.boolVal;
				}


				LOG_INVOKE_FUNCTION(renderToTarget,
					lUnkEVROutputNode,
					lUnkTargetRender,
					lIsInnerRendering);

			} while (false);

			return lresult;
		}
	}
}