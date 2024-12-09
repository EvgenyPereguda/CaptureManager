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


#include "ByteStreamSinkFactory.h"
#include "../Common/InstanceMaker.h"
#include "SinkCollection.h"
#include "../Common/MFHeaders.h"
#include "../Common/Common.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../CaptureManagerBroker/CaptureManagerBroker.h"
#include "../Common/CustomisedActivate.h"

namespace CaptureManager
{
	namespace COMServer
	{
		enum class MethodsEnum :DISPID
		{
			CreateOutputNodes = 1
		};

		static const Core::InstanceMaker<ByteStreamSinkFactory, Singleton<SinkCollection>> staticInstanceMaker(
			ByteStreamSinkFactory::getGUIDToNamePair);
		
		GUIDToNamePair ByteStreamSinkFactory::getGUIDToNamePair()
		{
			GUIDToNamePair lGUIDToNamePair;

			lGUIDToNamePair.mGUID = __uuidof(IByteStreamSinkFactory);

			lGUIDToNamePair.mName = L"ByteStreamSinkFactory";

			lGUIDToNamePair.mTitle = L"Byte stream sink factory";

			Singleton<CaptureManagerBroker>::getInstance().GetOutputFactoryCollection(
				SinkType::ByteStream,
				lGUIDToNamePair.mContainers);
			
			return lGUIDToNamePair;
		}

		// IByteStreamSinkFactory interface implementation

		STDMETHODIMP ByteStreamSinkFactory::createOutputNodes(
			VARIANT aArrayPtrCompressedMediaTypes,
			IUnknown *aPtrByteStreamActivate,
			VARIANT *aPtrArrayPtrTopologyOutputNodes)
		{

			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(mIOutputNodeFactory);

				LOG_CHECK_PTR_MEMORY(aPtrByteStreamActivate);

				LOG_CHECK_PTR_MEMORY(aPtrArrayPtrTopologyOutputNodes);
				
				CComQIPtrCustom<IMFActivate> lActivate = aPtrByteStreamActivate;

				if (!lActivate)
				{
					CComQIPtrCustom<IMFByteStream> lByteStream = aPtrByteStreamActivate;

					if (lByteStream)
					{
						lActivate = new (std::nothrow) CaptureManager::Activates::CustomisedActivate::CustomisedActivate(lByteStream);
					}
				}

				LOG_CHECK_PTR_MEMORY(lActivate);
				
				LOG_INVOKE_POINTER_METHOD(mIOutputNodeFactory, createOutputNodes,
					aArrayPtrCompressedMediaTypes,
					lActivate,
					aPtrArrayPtrTopologyOutputNodes);

			} while (false);

			return lresult;
		}



		// IDispatch interface stub

		STDMETHODIMP ByteStreamSinkFactory::GetIDsOfNames(
			__RPC__in REFIID riid,
			/* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
			/* [range][in] */ __RPC__in_range(0, 16384) UINT cNames,
			LCID lcid,
			/* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId) {

			HRESULT lresult(DISP_E_UNKNOWNNAME);

			do
			{
				LOG_CHECK_STATE_DESCR(cNames != 1, DISP_E_BADPARAMCOUNT);
				
				if (_wcsicmp(*rgszNames, OLESTR("createOutputNodes")) == 0)
				{
					*rgDispId = (int)MethodsEnum::CreateOutputNodes;

					lresult = S_OK;
				}

			} while (false);

			return lresult;
		}

		HRESULT ByteStreamSinkFactory::invokeMethod(
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
				
				switch (dispIdMember)
				{
				case (int)MethodsEnum::CreateOutputNodes:
				{

					LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 2, DISP_E_BADPARAMCOUNT);
					
					VARIANT lArrayPtrCompressedMediaTypes;

					CComPtrCustom<IUnknown> lByteStreamActivate;

					if (pDispParams->rgvarg[0].vt == VT_UNKNOWN && pDispParams->rgvarg[0].punkVal != nullptr)
					{
						LOG_INVOKE_QUERY_INTERFACE_METHOD(pDispParams->rgvarg[0].punkVal, &lByteStreamActivate);
					}
					else
					{
						lresult = DISP_E_BADVARTYPE;

						break;
					}

					LOG_CHECK_PTR_MEMORY(lByteStreamActivate);
										
					if (pDispParams->rgvarg[1].vt == VT_VARIANT && pDispParams->rgvarg[1].pvarVal != nullptr)
					{
						lArrayPtrCompressedMediaTypes = *(pDispParams->rgvarg[1].pvarVal);
					}
					else if (pDispParams->rgvarg[1].vt == (VT_ARRAY | VT_VARIANT))
					{

						SAFEARRAY* lPtrSAMediaTypes = nullptr;

						lPtrSAMediaTypes = pDispParams->rgvarg[1].parray;

						LOG_CHECK_STATE_DESCR(lPtrSAMediaTypes == nullptr, DISP_E_BADVARTYPE);
						
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
						lByteStreamActivate,
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

		STDMETHODIMP ByteStreamSinkFactory::setContainerFormat(
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