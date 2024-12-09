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

#include "MediaTypeParser.h"
#include "../Common/Singleton.h"
#include "../CaptureManagerBroker/CaptureManagerBroker.h"
#include "../Common/Common.h"
#include "../LogPrintOut/LogPrintOut.h"

namespace CaptureManager
{
	namespace COMServer
	{

		enum class MethodsEnum :DISPID
		{
			Parse = 1
		};

		STDMETHODIMP MediaTypeParser::parse(
			IUnknown *aPtrMediaType,
			BSTR *aPtrPtrXMLstring)
		{
			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrMediaType);
				
				LOG_CHECK_PTR_MEMORY(aPtrPtrXMLstring)
				
				if (*aPtrPtrXMLstring != nullptr)
				{
					SysFreeString(*aPtrPtrXMLstring);
				}

				std::wstring lwstring;
								
				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().getXMLDocumentStringOfMediaType,
					aPtrMediaType,
					lwstring);
				
				*aPtrPtrXMLstring = SysAllocString(lwstring.c_str());

			} while (false);

			return lresult;
		}



		// IDispatch interface stub

		STDMETHODIMP MediaTypeParser::GetIDsOfNames(
			__RPC__in REFIID riid,
			/* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
			/* [range][in] */ __RPC__in_range(0, 16384) UINT cNames,
			LCID lcid,
			/* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId) {

			HRESULT lresult(DISP_E_UNKNOWNNAME);

			do
			{
				LOG_CHECK_STATE(cNames != 1);
				
				if (_wcsicmp(*rgszNames, OLESTR("parse")) == 0)
				{
					*rgDispId = (int)MethodsEnum::Parse;

					lresult = S_OK;
				}

			} while (false);

			return lresult;
		}

		HRESULT MediaTypeParser::invokeMethod(
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
				case (int)MethodsEnum::Parse:
				{

					LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 1, DISP_E_BADPARAMCOUNT);
															
					if (pDispParams->rgvarg[0].vt == VT_UNKNOWN &&
						pDispParams->rgvarg[0].punkVal != nullptr)
					{
						LOG_INVOKE_FUNCTION(parse,
							pDispParams->rgvarg[0].punkVal,
							&pVarResult->bstrVal);
						
						pVarResult->vt = VT_BSTR;
					}
					else
					{
						lresult = DISP_E_BADVARTYPE;

						break;
					}

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