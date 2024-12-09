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


#include "StrideForBitmap.h"
#include "../Common/Singleton.h"
#include "../CaptureManagerBroker/CaptureManagerBroker.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Common.h"

namespace CaptureManager
{
	namespace COMServer
	{
		enum class MethodsEnum :DISPID
		{
			GetStrideForBitmap = 1,
		};


		// IStrideForBitmap interface implementation

		STDMETHODIMP StrideForBitmap::getStrideForBitmap(
			REFGUID aMFVideoFormat,
			DWORD aWidthInPixels,
			LONG *aPtrStride)
		{
			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrStride);
				
				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().getStrideForBitmapInfoHeader,
					aMFVideoFormat,
					aWidthInPixels,
					*aPtrStride);

			} while (false);
			
			return lresult;
		}



		// IDispatch interface stub

		STDMETHODIMP StrideForBitmap::GetIDsOfNames(
			__RPC__in REFIID riid,
			/* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
			/* [range][in] */ __RPC__in_range(0, 16384) UINT cNames,
			LCID lcid,
			/* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId) {

			HRESULT lresult(DISP_E_UNKNOWNNAME);

			do
			{
				LOG_CHECK_STATE(cNames != 1);
				
				if (_wcsicmp(*rgszNames, OLESTR("getStrideForBitmap")) == 0)
				{
					*rgDispId = (int)MethodsEnum::GetStrideForBitmap;

					lresult = S_OK;
				}

			} while (false);

			return lresult;
		}

		HRESULT StrideForBitmap::invokeMethod(
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
				case (int)MethodsEnum::GetStrideForBitmap:
				{
					LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 2, DISP_E_BADPARAMCOUNT);
										
					GUID lMFVideoFormatGUID;

					DWORD lWidthInPixels;
					
					if (pDispParams->rgvarg[1].vt == VT_CLSID)
					{
					}
					else if (pDispParams->rgvarg[1].vt == VT_BSTR && pDispParams->rgvarg[1].bstrVal != nullptr)
					{
						LOG_INVOKE_FUNCTION(CLSIDFromString, pDispParams->rgvarg[1].bstrVal, &lMFVideoFormatGUID);
					}
					else
					{
						lresult = DISP_E_BADVARTYPE;

						break;
					}

					if (pDispParams->rgvarg[0].vt == VT_UI4 ||
						pDispParams->rgvarg[0].vt == VT_I4)
					{
						lWidthInPixels = pDispParams->rgvarg[0].ulVal;
					}
					else
					{
						lresult = DISP_E_BADVARTYPE;

						break;
					}

					LONG lStride;

					LOG_INVOKE_FUNCTION(getStrideForBitmap,
						lMFVideoFormatGUID,
						lWidthInPixels,
						&lStride);
					
					pVarResult->vt = VT_I4;

					pVarResult->lVal = lStride;

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