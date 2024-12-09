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


#include "FileSinkFactory.h"
#include "../Common/InstanceMaker.h"
#include "SinkCollection.h"
#include "../Common/MFHeaders.h"
#include "../CaptureManagerBroker/CaptureManagerBroker.h"
#include "../Common/CustomisedActivate.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Common.h"

namespace CaptureManager
{
	namespace COMServer
	{

		enum class MethodsEnum :DISPID
		{
			CreateOutputNodes = 1
		};

		static const Core::InstanceMaker<FileSinkFactory, Singleton<SinkCollection>> staticInstanceMaker(
			FileSinkFactory::getGUIDToNamePair);

		GUIDToNamePair FileSinkFactory::getGUIDToNamePair()
		{
			GUIDToNamePair lGUIDToNamePair;

			lGUIDToNamePair.mGUID = __uuidof(IFileSinkFactory);

			lGUIDToNamePair.mName = L"FileSinkFactory";

			lGUIDToNamePair.mTitle = L"File sink factory";

			Singleton<CaptureManagerBroker>::getInstance().GetOutputFactoryCollection(
				SinkType::File,
				lGUIDToNamePair.mContainers);

			return lGUIDToNamePair;
		}
		
		// IFileSinkFactory interface implementation

		STDMETHODIMP FileSinkFactory::createOutputNodes(
			VARIANT aArrayPtrCompressedMediaTypes,
			BSTR aPtrFileName,
			VARIANT *aPtrArrayPtrTopologyOutputNodes)
		{
			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(mIOutputNodeFactory);
				
				LOG_CHECK_PTR_MEMORY(aPtrFileName);
				
				LOG_INVOKE_POINTER_METHOD(mIOutputNodeFactory, createOutputNodes,
					aArrayPtrCompressedMediaTypes,
					aPtrFileName,
					aPtrArrayPtrTopologyOutputNodes);

			} while (false);

			return lresult;
		}



		// IDispatch interface stub

		STDMETHODIMP FileSinkFactory::GetIDsOfNames(
			__RPC__in REFIID riid,
			/* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
			/* [range][in] */ __RPC__in_range(0, 16384) UINT cNames,
			LCID lcid,
			/* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId) {

			HRESULT lresult(DISP_E_UNKNOWNNAME);

			do
			{
				LOG_CHECK_STATE(cNames != 1);
				
				if (_wcsicmp(*rgszNames, OLESTR("createOutputNodes")) == 0)
				{
					*rgDispId = (int)MethodsEnum::CreateOutputNodes;

					lresult = S_OK;
				}

			} while (false);

			return lresult;
		}

		HRESULT FileSinkFactory::invokeMethod(
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
				case (int)MethodsEnum::CreateOutputNodes:
				{

					LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 2, DISP_E_BADPARAMCOUNT);
					
					VARIANT lArrayPtrCompressedMediaTypes;

					BSTR lFileName;

					if (pDispParams->rgvarg[0].vt == VT_BSTR && pDispParams->rgvarg[0].bstrVal != nullptr)
					{
						lFileName = pDispParams->rgvarg[0].bstrVal;
					}
					else
					{
						lresult = DISP_E_BADVARTYPE;

						break;
					}

					if (pDispParams->rgvarg[1].vt == VT_VARIANT && pDispParams->rgvarg[1].pvarVal != nullptr)
					{
						lArrayPtrCompressedMediaTypes = *(pDispParams->rgvarg[1].pvarVal);
					}
					else if (pDispParams->rgvarg[1].vt == (VT_ARRAY | VT_VARIANT))
					{

						SAFEARRAY* lPtrSAMediaTypes = nullptr;

						lPtrSAMediaTypes = pDispParams->rgvarg[1].parray;
						
						if (lPtrSAMediaTypes == nullptr)
						{
							lresult = DISP_E_BADVARTYPE;

							break;
						}

						lArrayPtrCompressedMediaTypes.vt = VT_UNKNOWN | VT_SAFEARRAY;

						lArrayPtrCompressedMediaTypes.parray = lPtrSAMediaTypes;

					}
					else
					{						
						lresult = DISP_E_BADVARTYPE;

						break;
					}
					
					LOG_INVOKE_FUNCTION(createOutputNodes,
						lArrayPtrCompressedMediaTypes,
						lFileName, 
						pVarResult);
					
					pVarResult->vt = VT_ARRAY | VT_VARIANT;

				}
				break;
				default:
					break;
				}


			} while (false);


			return lresult;
		}

		// IContainer interface implementation

		STDMETHODIMP FileSinkFactory::setContainerFormat(
			REFGUID aRefContainerTypeGUID)
		{
			HRESULT lresult;

			do
			{
				CComPtrCustom<IUnknown> lIUnknown;
				
				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().createOutputNodeFactory,
					aRefContainerTypeGUID,
					&lIUnknown);

				LOG_CHECK_PTR_MEMORY(lIUnknown);

				mIOutputNodeFactory.Release();

				LOG_INVOKE_QUERY_INTERFACE_METHOD(lIUnknown,
					&mIOutputNodeFactory);

				LOG_CHECK_PTR_MEMORY(mIOutputNodeFactory);

			} while (false);

			return lresult;
		}
	}
}

