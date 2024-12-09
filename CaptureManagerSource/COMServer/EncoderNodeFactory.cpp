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
#include <algorithm>


#include "EncoderNodeFactory.h"
#include "../Common/ComPtrCustom.h"
#include "../CaptureManagerBroker/CaptureManagerBroker.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Common.h"

namespace CaptureManager
{
	namespace COMServer
	{
		enum class MethodsEnum :DISPID
		{
			CreateCompressedMediaType = 1,
			CreateEncoderNode = CreateCompressedMediaType + 1
		};
				
		STDMETHODIMP EncoderNodeFactory::createCompressedMediaType(
			IUnknown *aPtrUncompressedMediaType,
			REFGUID aRefEncodingModeGUID,
			DWORD aEncodingModeValue,
			DWORD aIndexCompressedMediaType,
			IUnknown **aPtrPtrCompressedMediaType)
		{
			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrUncompressedMediaType);

				LOG_CHECK_PTR_MEMORY(aPtrPtrCompressedMediaType);
				
				EncodingSettings lEncodingSettings;

				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().getEncodingSettingsMode,
					aRefEncodingModeGUID,
					lEncodingSettings.mEncodingMode);
				
				lEncodingSettings.mEncodingModeValue = aEncodingModeValue;

				lEncodingSettings.mAverageBitrateValue = 20000000;
				
				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().createCompressedMediaType,
					aPtrUncompressedMediaType,
					lEncodingSettings,
					this->mEncoderCLSID,
					aIndexCompressedMediaType,
					aPtrPtrCompressedMediaType);

			} while (false);

			return lresult;
		}

		STDMETHODIMP EncoderNodeFactory::createEncoderNode(
			IUnknown *aPtrUncompressedMediaType,
			REFGUID aRefEncodingModeGUID,
			DWORD aEncodingModeValue,
			DWORD aIndexCompressedMediaType,
			IUnknown *aPtrDownStreamNode,
			IUnknown **aPtrPtrEncoderNode)
		{
			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrUncompressedMediaType);

				LOG_CHECK_PTR_MEMORY(aPtrDownStreamNode);

				LOG_CHECK_PTR_MEMORY(aPtrPtrEncoderNode);
				
				EncodingSettings lEncodingSettings;

				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().getEncodingSettingsMode,
					aRefEncodingModeGUID,
					lEncodingSettings.mEncodingMode);
				
				lEncodingSettings.mEncodingModeValue = aEncodingModeValue;

				lEncodingSettings.mAverageBitrateValue = 1000000;

				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().createEncoderNode,
					aPtrUncompressedMediaType,
					lEncodingSettings,
					this->mEncoderCLSID,
					aIndexCompressedMediaType,
					aPtrDownStreamNode,
					aPtrPtrEncoderNode);

			} while (false);

			return lresult;
		}



		// IDispatch interface stub

		STDMETHODIMP EncoderNodeFactory::GetIDsOfNames(
			__RPC__in REFIID riid,
			/* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
			/* [range][in] */ __RPC__in_range(0, 16384) UINT cNames,
			LCID lcid,
			/* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId) {

			HRESULT lresult(DISP_E_UNKNOWNNAME);

			do
			{
				LOG_CHECK_STATE(cNames != 1);
				
				if (_wcsicmp(*rgszNames, OLESTR("createCompressedMediaType")) == 0)
				{
					*rgDispId = (int)MethodsEnum::CreateCompressedMediaType;

					lresult = S_OK;
				}
				else if (_wcsicmp(*rgszNames, OLESTR("createEncoderNode")) == 0)
				{
					*rgDispId = (int)MethodsEnum::CreateEncoderNode;

					lresult = S_OK;
				}

			} while (false);

			return lresult;
		}

		HRESULT EncoderNodeFactory::invokeMethod(
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
				case (int)MethodsEnum::CreateCompressedMediaType:
				{

					LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 4, DISP_E_BADPARAMCOUNT);
					
					CComPtrCustom<IUnknown> lUncompressedMediaType;
					
					GUID lEncodingModeGUID;
					
					DWORD lEncodingModeValue;
					
					DWORD lIndexCompressedMediaType;


					if (pDispParams->rgvarg[3].vt == VT_UNKNOWN &&
						pDispParams->rgvarg[3].punkVal != nullptr)
					{
						LOG_INVOKE_QUERY_INTERFACE_METHOD(pDispParams->rgvarg[3].punkVal, &lUncompressedMediaType);
					}
					else
					{
						lresult = DISP_E_BADVARTYPE;

						break;
					}

					if (pDispParams->rgvarg[2].vt == VT_CLSID)
					{
					}
					else if (pDispParams->rgvarg[2].vt == VT_BSTR && pDispParams->rgvarg[2].bstrVal != nullptr)
					{
						LOG_INVOKE_FUNCTION(CLSIDFromString, pDispParams->rgvarg[2].bstrVal, &lEncodingModeGUID);
					}
					else
					{
						lresult = DISP_E_BADVARTYPE;

						break;
					}

					if (pDispParams->rgvarg[1].vt == VT_UI4 ||
						pDispParams->rgvarg[1].vt == VT_I4)
					{
						lEncodingModeValue = pDispParams->rgvarg[1].ulVal;
					}
					else
					{
						lresult = DISP_E_BADVARTYPE;

						break;
					}

					if (pDispParams->rgvarg[0].vt == VT_UI4 ||
						pDispParams->rgvarg[0].vt == VT_I4)
					{
						lIndexCompressedMediaType = pDispParams->rgvarg[0].ulVal;
					}
					else
					{
						lresult = DISP_E_BADVARTYPE;

						break;
					}
					
					CComPtrCustom<IUnknown> lCompressedMediaType;

					LOG_INVOKE_FUNCTION(createCompressedMediaType,
						lUncompressedMediaType,
						lEncodingModeGUID,
						lEncodingModeValue,
						lIndexCompressedMediaType,
						&lCompressedMediaType);
					
					pVarResult->vt = VT_UNKNOWN;

					pVarResult->punkVal = lCompressedMediaType.detach();
				}
				break;
				case (int)MethodsEnum::CreateEncoderNode:
				{
					LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 5, DISP_E_BADPARAMCOUNT);
					
					CComPtrCustom<IUnknown> lUncompressedMediaType;

					GUID lEncodingModeGUID;

					DWORD lEncodingModeValue;

					DWORD lIndexCompressedMediaType;

					CComPtrCustom<IUnknown> lDownStreamNode;


					if (pDispParams->rgvarg[4].vt == VT_UNKNOWN &&
						pDispParams->rgvarg[4].punkVal != nullptr)
					{
						LOG_INVOKE_QUERY_INTERFACE_METHOD(pDispParams->rgvarg[4].punkVal, &lUncompressedMediaType);
					}
					else
					{
						lresult = DISP_E_BADVARTYPE;

						break;
					}

					if (pDispParams->rgvarg[3].vt == VT_CLSID)
					{
					}
					else if (pDispParams->rgvarg[3].vt == VT_BSTR && pDispParams->rgvarg[3].bstrVal != nullptr)
					{
						LOG_INVOKE_FUNCTION(CLSIDFromString, pDispParams->rgvarg[3].bstrVal, &lEncodingModeGUID);
					}
					else
					{
						lresult = DISP_E_BADVARTYPE;

						break;
					}

					if (pDispParams->rgvarg[2].vt == VT_UI4 ||
						pDispParams->rgvarg[2].vt == VT_I4)
					{
						lEncodingModeValue = pDispParams->rgvarg[2].ulVal;
					}
					else
					{
						lresult = DISP_E_BADVARTYPE;

						break;
					}

					if (pDispParams->rgvarg[1].vt == VT_UI4 ||
						pDispParams->rgvarg[1].vt == VT_I4)
					{
						lIndexCompressedMediaType = pDispParams->rgvarg[1].ulVal;
					}
					else
					{
						lresult = DISP_E_BADVARTYPE;

						break;
					}


					if (pDispParams->rgvarg[0].vt == VT_UNKNOWN &&
						pDispParams->rgvarg[0].punkVal != nullptr)
					{
						LOG_INVOKE_QUERY_INTERFACE_METHOD(pDispParams->rgvarg[0].punkVal, &lDownStreamNode);
					}
					else
					{
						lresult = DISP_E_BADVARTYPE;

						break;
					}

					CComPtrCustom<IUnknown> lEncoderNode;

					LOG_INVOKE_FUNCTION(createEncoderNode,
						lUncompressedMediaType,
						lEncodingModeGUID,
						lEncodingModeValue,
						lIndexCompressedMediaType,
						lDownStreamNode,
						&lEncoderNode);
					
					pVarResult->vt = VT_UNKNOWN;

					pVarResult->punkVal = lEncoderNode.detach();
				}
				break;
				default:
					break;
				}


			} while (false);


			return lresult;
		}


		// IContainer interface implementation

		STDMETHODIMP EncoderNodeFactory::setContainerFormat(
			REFGUID aRefContainerTypeGUID)
		{
			HRESULT lresult;

			do
			{
				mEncoderCLSID = aRefContainerTypeGUID;

				lresult = S_OK;

			} while (false);

			return lresult;
		}
	}
}