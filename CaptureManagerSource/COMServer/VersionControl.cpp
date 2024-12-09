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
#include <sstream>

#include "VersionControl.h"
#include "../PugiXML/pugixml.hpp"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Common.h"
#include "../Common/ComPtrCustom.h"
#include "../Common/BaseUnknown.h"
#include "../Common/Macros.h"
#include "../CaptureManagerBroker/CaptureManagerBroker.h"

#ifndef VERSION_BUILD
#define VERSION_BUILD            2057
#endif // !VERSION_BUILD


namespace CaptureManager
{
	namespace COMServer
	{
		enum class MethodsEnum :DISPID
		{
			GetVersion = 1,
			GetXMLStringVersion = GetVersion + 1,
			CheckVersion = GetXMLStringVersion + 1
		};

		// IVersionControl interface implementation		

		STDMETHODIMP VersionControl::getVersion(
			DWORD *aPtrMAJOR,
			DWORD *aPtrMINOR,
			DWORD *aPtrPATCH,
			BSTR *aPtrPtrAdditionalLabel)
		{
			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrMAJOR);

				LOG_CHECK_PTR_MEMORY(aPtrMINOR);

				LOG_CHECK_PTR_MEMORY(aPtrPATCH);

				LOG_CHECK_PTR_MEMORY(aPtrPtrAdditionalLabel);
				
				if (*aPtrPtrAdditionalLabel != nullptr)
				{
					SysFreeString(*aPtrPtrAdditionalLabel);
				}

				*aPtrMAJOR = VERSION_MAJOR;
				*aPtrMINOR = VERSION_MINOR;
				*aPtrPATCH = VERSION_PATCH;
				
				*aPtrPtrAdditionalLabel = SysAllocString(WSTRINGIZE(ADDITIONAL_LABEL));

				lresult = S_OK;

			} while (false);

			return lresult;
		}
		
		STDMETHODIMP VersionControl::getXMLStringVersion(
			BSTR *aPtrPtrXMLstring)
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

				xml_document lxmlDoc;

				auto ldeclNode = lxmlDoc.append_child(node_declaration);

				ldeclNode.append_attribute(L"version") = L"1.0";

				xml_node lcommentNode = lxmlDoc.append_child(node_comment);

				lcommentNode.set_value(L"XML Document of Capture Manager version");

				auto lRootXMLElement = lxmlDoc.append_child(L"CaptureManagerVersion");
				
				lRootXMLElement.append_attribute(L"MAJOR") = VERSION_MAJOR;

				lRootXMLElement.append_attribute(L"MINOR") = VERSION_MINOR;

				lRootXMLElement.append_attribute(L"PATCH") = VERSION_PATCH;

				lRootXMLElement.append_attribute(L"BUILD") = VERSION_BUILD;

				
				std::wstring lLabel(WSTRINGIZE(ADDITIONAL_LABEL));

				if (lLabel.size() > 0)
					lRootXMLElement.append_child(xml_node_type::node_pcdata).set_value(WSTRINGIZE(ADDITIONAL_LABEL));
				
				std::wstringstream lwstringstream;

				lxmlDoc.print(lwstringstream);

				*aPtrPtrXMLstring = SysAllocString(lwstringstream.str().c_str());

				lresult = S_OK;

			} while (false);
			
			return lresult;
		}

		STDMETHODIMP VersionControl::checkVersion(
			DWORD aMAJOR,
			DWORD aMINOR,
			DWORD aPATCH,
			boolean *aPtrResult)
		{			
			HRESULT lresult = S_FALSE;
						
			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrResult);
				
				lresult = S_OK;

				if (aMAJOR != VERSION_MAJOR)
				{
					*aPtrResult = false;

					break;
				}

				if (aMINOR > VERSION_MINOR)
				{
					*aPtrResult = false;

					break;
				}

				*aPtrResult = true;

			} while (false);

			return lresult;
		}



		// IDispatch interface stub

		STDMETHODIMP VersionControl::GetIDsOfNames(
			__RPC__in REFIID riid,
			/* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
			/* [range][in] */ __RPC__in_range(0, 16384) UINT cNames,
			LCID lcid,
			/* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId) {

			HRESULT lresult(DISP_E_UNKNOWNNAME);

			do
			{
				LOG_CHECK_STATE(cNames != 1);
				
				if (_wcsicmp(*rgszNames, OLESTR("getVersion")) == 0)
				{
					*rgDispId = (int)MethodsEnum::GetVersion;

					lresult = S_OK;
				}
				else if (_wcsicmp(*rgszNames, OLESTR("getXMLStringVersion")) == 0)
				{
					*rgDispId = (int)MethodsEnum::GetXMLStringVersion;

					lresult = S_OK;
				}
				else if (_wcsicmp(*rgszNames, OLESTR("checkVersion")) == 0)
				{
					*rgDispId = (int)MethodsEnum::CheckVersion;

					lresult = S_OK;
				}

			} while (false);

			return lresult;
		}

		HRESULT VersionControl::invokeMethod(
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

				LOG_CHECK_PTR_MEMORY (pVarResult);

				switch (dispIdMember)
				{
				case (int)MethodsEnum::GetVersion:
				{
					LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 0, DISP_E_BADPARAMCOUNT);

					lresult = DISP_E_BADCALLEE;

				}
				break;
				case (int)MethodsEnum::GetXMLStringVersion:
				{
					LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 0, DISP_E_BADPARAMCOUNT);

					LOG_INVOKE_FUNCTION(getXMLStringVersion, &pVarResult->bstrVal);
					
					pVarResult->vt = VT_BSTR;
				}
				break;
				case (int)MethodsEnum::CheckVersion:
				{
					LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 3, DISP_E_BADPARAMCOUNT);
										
					DWORD lMAJOR;
					DWORD lMINOR;
					DWORD lPATCH;
					boolean lResult;



					if (pDispParams->rgvarg[0].vt == VT_UI4 ||
						pDispParams->rgvarg[0].vt == VT_I4)
					{
						lPATCH = pDispParams->rgvarg[0].ulVal;
					}
					else
					{
						lresult = DISP_E_BADVARTYPE;

						break;
					}

					if (pDispParams->rgvarg[1].vt == VT_UI4 ||
						pDispParams->rgvarg[1].vt == VT_I4)
					{
						lMINOR = pDispParams->rgvarg[1].ulVal;
					}
					else
					{
						lresult = DISP_E_BADVARTYPE;

						break;
					}

					if (pDispParams->rgvarg[2].vt == VT_UI4 ||
						pDispParams->rgvarg[2].vt == VT_I4)
					{
						lMAJOR = pDispParams->rgvarg[2].ulVal;
					}
					else
					{
						lresult = DISP_E_BADVARTYPE;

						break;
					}

					LOG_INVOKE_FUNCTION(checkVersion,
						lMAJOR,
						lMINOR,
						lPATCH,
						&lResult);
					
					pVarResult->vt = VT_BOOL;

					pVarResult->boolVal = lResult == TRUE ? VARIANT_TRUE: VARIANT_FALSE;
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