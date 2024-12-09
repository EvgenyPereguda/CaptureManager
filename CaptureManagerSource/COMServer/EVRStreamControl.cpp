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

#include "EVRStreamControl.h"
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
			SetPosition = 1,
			SetZOrder = SetPosition + 1,
			GetPosition = SetZOrder + 1,
			GetZOrder = GetPosition + 1,
			Flush = GetZOrder + 1,
			GetCollectionOfFilters = Flush + 1,
			SetFilterParametr = GetCollectionOfFilters + 1,
			GetCollectionOfOutputFeatures = SetFilterParametr + 1,
			SetOutputFeatureParametr = GetCollectionOfOutputFeatures + 1
		};

		EVRStreamControl::EVRStreamControl()
		{
		}
		
		EVRStreamControl::~EVRStreamControl()
		{
		}


		// IEVRStreamControl methods


		HRESULT STDMETHODCALLTYPE EVRStreamControl::setPosition(
			/* [in] */ IUnknown *aPtrEVROutputNode,
			/* [in] */ FLOAT aLeft,
			/* [in] */ FLOAT aRight,
			/* [in] */ FLOAT aTop,
			/* [in] */ FLOAT aBottom)
		{

			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrEVROutputNode);

				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().setEVRStreamPosition,
					aPtrEVROutputNode,
					aLeft,
					aRight,
					aTop,
					aBottom);

			} while (false);

			return lresult;
		}

		HRESULT STDMETHODCALLTYPE EVRStreamControl::setZOrder(
			/* [in] */ IUnknown *aPtrEVROutputNode,
			/* [out] */ DWORD aZOrder)
		{
			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrEVROutputNode);

				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().setEVRStreamZOrder,
					aPtrEVROutputNode,
					aZOrder);

			} while (false);

			return lresult;
		}

		HRESULT STDMETHODCALLTYPE EVRStreamControl::getPosition(
			/* [in] */ IUnknown *aPtrEVROutputNode,
			/* [out] */ FLOAT *aPtrLeft,
			/* [out] */ FLOAT *aPtrRight,
			/* [out] */ FLOAT *aPtrTop,
			/* [out] */ FLOAT *aPtrBottom)
		{
			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrEVROutputNode);

				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().getEVRStreamPosition,
					aPtrEVROutputNode,
					aPtrLeft,
					aPtrRight,
					aPtrTop,
					aPtrBottom);

			} while (false);

			return lresult;
		}


		HRESULT STDMETHODCALLTYPE EVRStreamControl::getZOrder(
			/* [in] */ IUnknown *aPtrEVROutputNode,
			/* [out] */ DWORD *aPtrZOrder)
		{
			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrEVROutputNode);

				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().getEVRStreamZOrder,
					aPtrEVROutputNode,
					aPtrZOrder);

			} while (false);

			return lresult;
		}

		HRESULT STDMETHODCALLTYPE EVRStreamControl::flush(
			/* [in] */ IUnknown *aPtrEVROutputNode)
		{
			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrEVROutputNode);

				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().setEVRStreamFlush,
					aPtrEVROutputNode);

			} while (false);

			return lresult;
		}
		
		HRESULT STDMETHODCALLTYPE EVRStreamControl::setSrcPosition(
			/* [in] */ IUnknown *aPtrEVROutputNode,
			/* [in] */ FLOAT aLeft,
			/* [in] */ FLOAT aRight,
			/* [in] */ FLOAT aTop,
			/* [in] */ FLOAT aBottom)
		{

			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrEVROutputNode);

				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().setEVRStreamSrcPosition,
					aPtrEVROutputNode,
					aLeft,
					aRight,
					aTop,
					aBottom);

			} while (false);

			return lresult;
		}

		HRESULT STDMETHODCALLTYPE EVRStreamControl::getSrcPosition(
			/* [in] */ IUnknown *aPtrEVROutputNode,
			/* [in] */ FLOAT *aPtrLeft,
			/* [in] */ FLOAT *aPtrRight,
			/* [in] */ FLOAT *aPtrTop,
			/* [in] */ FLOAT *aPtrBottom)
		{

			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrEVROutputNode);

				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().getEVRStreamSrcPosition,
					aPtrEVROutputNode,
					aPtrLeft,
					aPtrRight,
					aPtrTop,
					aPtrBottom);

			} while (false);

			return lresult;
		}
		
		HRESULT STDMETHODCALLTYPE EVRStreamControl::getCollectionOfFilters(
			/* [in] */ IUnknown *aPtrEVROutputNode,
			/* [out] */ BSTR *aPtrPtrXMLstring)
		{

			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrEVROutputNode);

				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().getCollectionOfFilters,
					aPtrEVROutputNode,
					aPtrPtrXMLstring);

			} while (false);

			return lresult;
		}

		HRESULT STDMETHODCALLTYPE EVRStreamControl::setFilterParametr(
			/* [in] */ IUnknown *aPtrEVROutputNode,
			/* [in] */ DWORD aParametrIndex,
			/* [in] */ LONG aNewValue,
			/* [in] */ BOOL aIsEnabled)
		{

			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrEVROutputNode);

				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().setFilterParametr,
					aPtrEVROutputNode,
					aParametrIndex,
					aNewValue,
					aIsEnabled);

			} while (false);

			return lresult;
		}

		HRESULT STDMETHODCALLTYPE EVRStreamControl::getCollectionOfOutputFeatures(
			/* [in] */ IUnknown *aPtrEVROutputNode,
			/* [out] */ BSTR *aPtrPtrXMLstring)
		{

			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrEVROutputNode);

				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().getCollectionOfOutputFeatures,
					aPtrEVROutputNode,
					aPtrPtrXMLstring);

			} while (false);

			return lresult;
		}

		HRESULT STDMETHODCALLTYPE EVRStreamControl::setOutputFeatureParametr(
			/* [in] */ IUnknown *aPtrEVROutputNode,
			/* [in] */ DWORD aParametrIndex,
			/* [in] */ LONG aNewValue)
		{

			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrEVROutputNode);

				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().setOutputFeatureParametr,
					aPtrEVROutputNode,
					aParametrIndex,
					aNewValue);

			} while (false);

			return lresult;
		}


		// IDispatch interface stub

		STDMETHODIMP EVRStreamControl::GetIDsOfNames(
			__RPC__in REFIID riid,
			/* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
			/* [range][in] */ __RPC__in_range(0, 16384) UINT cNames,
			LCID lcid,
			/* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId) {

			HRESULT lresult(DISP_E_UNKNOWNNAME);

			do
			{
				LOG_CHECK_STATE(cNames != 1);

				if (_wcsicmp(*rgszNames, OLESTR("setPosition")) == 0)
				{
					*rgDispId = (int)MethodsEnum::SetPosition;

					lresult = S_OK;
				}
				else if (_wcsicmp(*rgszNames, OLESTR("setZOrder")) == 0)
				{
					*rgDispId = (int)MethodsEnum::SetZOrder;

					lresult = S_OK;
				}
				else if (_wcsicmp(*rgszNames, OLESTR("getPosition")) == 0)
				{
					*rgDispId = (int)MethodsEnum::GetPosition;

					lresult = S_OK;
				}
				else if (_wcsicmp(*rgszNames, OLESTR("getZOrder")) == 0)
				{
					*rgDispId = (int)MethodsEnum::GetZOrder;

					lresult = S_OK;
				}
				else if (_wcsicmp(*rgszNames, OLESTR("flush")) == 0)
				{
					*rgDispId = (int)MethodsEnum::Flush;

					lresult = S_OK;
				}
				else if (_wcsicmp(*rgszNames, OLESTR("getCollectionOfFilters")) == 0)
				{
					*rgDispId = (int)MethodsEnum::GetCollectionOfFilters;

					lresult = S_OK;
				}
				else if (_wcsicmp(*rgszNames, OLESTR("setFilterParametr")) == 0)
				{
					*rgDispId = (int)MethodsEnum::SetFilterParametr;

					lresult = S_OK;
				}
				else if (_wcsicmp(*rgszNames, OLESTR("getCollectionOfOutputFeatures")) == 0)
				{
					*rgDispId = (int)MethodsEnum::GetCollectionOfOutputFeatures;

					lresult = S_OK;
				}
				else if (_wcsicmp(*rgszNames, OLESTR("setOutputFeatureParametr")) == 0)
				{
					*rgDispId = (int)MethodsEnum::SetOutputFeatureParametr;

					lresult = S_OK;
				}

			} while (false);

			return lresult;
		}

		HRESULT EVRStreamControl::invokeMethod(
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
				case (int)MethodsEnum::SetPosition:
				{
					lresult = invokeSetPosition(
						pDispParams,
						pVarResult);
				}
					break;
				case (int)MethodsEnum::SetZOrder:
				{
					lresult = invokeSetZOrder(
						pDispParams,
						pVarResult);
				}
					break;
				case (int)MethodsEnum::GetPosition:
				{
					lresult = invokeGetPosition(
						pDispParams,
						pVarResult);
				}
					break;
				case (int)MethodsEnum::GetZOrder:
				{
					lresult = invokeGetZOrder(
						pDispParams,
						pVarResult);
				}
					break;
				case (int)MethodsEnum::Flush:
				{
					lresult = invokeFlush(
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



		HRESULT STDMETHODCALLTYPE EVRStreamControl::invokeSetPosition(
			_In_  DISPPARAMS *pDispParams,
			/* [annotation][out] */
			_Out_opt_  VARIANT *pVarResult)
		{

			HRESULT lresult(DISP_E_UNKNOWNINTERFACE);

			do
			{

				LOG_CHECK_STATE_DESCR(pDispParams == nullptr, DISP_E_PARAMNOTFOUND);

				LOG_CHECK_PTR_MEMORY(pVarResult);

				switch (pDispParams->cArgs)
				{
				case 5:
				{

					CComPtrCustom<IUnknown> lUnkEVROutputNode;


					FLOAT lLeft = 0.0f;
					FLOAT lRight = 1.0f;
					FLOAT lTop = 0.0f;
					FLOAT lBottom = 1.0f;

					VARIANT lFifthArg = pDispParams->rgvarg[0];

					VARIANT lFouthArg = pDispParams->rgvarg[1];

					VARIANT lThirdArg = pDispParams->rgvarg[2];

					VARIANT lSecondArg = pDispParams->rgvarg[3];

					VARIANT lFirstArg = pDispParams->rgvarg[4];


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


					if (lSecondArg.vt == VT_R4)
					{
						lLeft = lSecondArg.fltVal;
					}

					if (lThirdArg.vt == VT_R4)
					{
						lRight = lThirdArg.fltVal;
					}

					if (lFouthArg.vt == VT_R4)
					{
						lTop = lFouthArg.fltVal;
					}

					if (lFifthArg.vt == VT_R4)
					{
						lBottom = lFifthArg.fltVal;
					}

					LOG_INVOKE_FUNCTION(setPosition,
						lUnkEVROutputNode,
						lLeft,
						lRight,
						lTop,
						lBottom);
				}
					break;
				case 6:
				{

					CComPtrCustom<IUnknown> lUnkEVROutputNode;


					FLOAT lLeft = 0.0f;
					FLOAT lRight = 1.0f;
					FLOAT lTop = 0.0f;
					FLOAT lBottom = 1.0f;

					VARIANT lFifthArg = pDispParams->rgvarg[0];

					VARIANT lFouthArg = pDispParams->rgvarg[1];

					VARIANT lThirdArg = pDispParams->rgvarg[2];

					VARIANT lSecondArg = pDispParams->rgvarg[3];

					VARIANT lFirstArg = pDispParams->rgvarg[4];

					VARIANT lZeroArg = pDispParams->rgvarg[5];


					if (lFirstArg.vt == VT_I8 && lFirstArg.punkVal != nullptr)
					{
						lFirstArg.punkVal->QueryInterface(IID_PPV_ARGS(&lUnkEVROutputNode));
					}
					else if (lFirstArg.vt == VT_UNKNOWN && lFirstArg.punkVal != nullptr)
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


					if (lSecondArg.vt == VT_R4)
					{
						lLeft = lSecondArg.fltVal;
					}

					if (lThirdArg.vt == VT_R4)
					{
						lRight = lThirdArg.fltVal;
					}

					if (lFouthArg.vt == VT_R4)
					{
						lTop = lFouthArg.fltVal;
					}

					if (lFifthArg.vt == VT_R4)
					{
						lBottom = lFifthArg.fltVal;
					}

					LOG_INVOKE_FUNCTION(setPosition,
						lUnkEVROutputNode,
						lLeft,
						lRight,
						lTop,
						lBottom);
				}
				break;
				default:
					LOG_CHECK_STATE_DESCR(true, DISP_E_BADPARAMCOUNT);
					break;
				}
				
			} while (false);

			return lresult;
		}
		
		HRESULT STDMETHODCALLTYPE EVRStreamControl::invokeSetZOrder(
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


				DWORD lZOrder = 0;
				
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


				if (lSecondArg.vt == VT_I4)
				{
					lZOrder = lSecondArg.intVal;
				}
				else if (lSecondArg.vt == VT_UI4)
				{
					lZOrder = lSecondArg.uintVal;
				}

				LOG_INVOKE_FUNCTION(setZOrder,
					lUnkEVROutputNode,
					lZOrder);

			} while (false);

			return lresult;
		}

		HRESULT STDMETHODCALLTYPE EVRStreamControl::invokeGetPosition(
			_In_  DISPPARAMS *pDispParams,
			/* [annotation][out] */
			_Out_opt_  VARIANT *pVarResult)
		{
			HRESULT lresult(E_NOTIMPL);

			do
			{

			} while (false);

			return lresult;
		}
		
		HRESULT STDMETHODCALLTYPE EVRStreamControl::invokeGetZOrder(
			_In_  DISPPARAMS *pDispParams,
			/* [annotation][out] */
			_Out_opt_  VARIANT *pVarResult)
		{
			HRESULT lresult(DISP_E_UNKNOWNINTERFACE);

			do
			{

				LOG_CHECK_STATE_DESCR(pDispParams == nullptr, DISP_E_PARAMNOTFOUND);

				LOG_CHECK_PTR_MEMORY(pVarResult);

				LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 1, DISP_E_BADPARAMCOUNT);


				CComPtrCustom<IUnknown> lUnkEVROutputNode;

				VARIANT lFirstArg = pDispParams->rgvarg[0];


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

				DWORD lZOrder = 0;

				LOG_INVOKE_FUNCTION(getZOrder,
					lUnkEVROutputNode,
					&lZOrder);

				pVarResult->vt = VT_UI4;

				pVarResult->uintVal = lZOrder;

			} while (false);

			return lresult;
		}

		HRESULT STDMETHODCALLTYPE EVRStreamControl::invokeFlush(
			_In_  DISPPARAMS *pDispParams,
			/* [annotation][out] */
			_Out_opt_  VARIANT *pVarResult)
		{
			HRESULT lresult(DISP_E_UNKNOWNINTERFACE);

			do
			{

				LOG_CHECK_STATE_DESCR(pDispParams == nullptr, DISP_E_PARAMNOTFOUND);

				LOG_CHECK_PTR_MEMORY(pVarResult);

				LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 1, DISP_E_BADPARAMCOUNT);


				CComPtrCustom<IUnknown> lUnkEVROutputNode;
								
				VARIANT lFirstArg = pDispParams->rgvarg[0];


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
				
				LOG_INVOKE_FUNCTION(flush,
					lUnkEVROutputNode);

			} while (false);

			return lresult;
		}

	}
}