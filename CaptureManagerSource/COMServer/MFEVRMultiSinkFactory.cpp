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

#include "MFEVRMultiSinkFactory.h"

#include "../Common/ComPtrCustom.h"
#include "../Common/MFHeaders.h"
#include "../Common/InstanceMaker.h"
#include "SinkCollection.h"
#include "../CaptureManagerBroker/CaptureManagerBroker.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Common.h"
#include "../Common/Singleton.h"

namespace CaptureManager
{
	namespace COMServer
	{

		enum class MethodsEnum :DISPID
		{
			CreateOutputNodes = 1
		};

		static const Core::InstanceMaker<MFEVRMultiSinkFactory, Singleton<SinkCollection>> staticInstanceMaker(
			MFEVRMultiSinkFactory::getGUIDToNamePair);

		GUIDToNamePair MFEVRMultiSinkFactory::getGUIDToNamePair()
		{
			GUIDToNamePair lGUIDToNamePair;

			lGUIDToNamePair.mGUID = __uuidof(CMVRMultiSinkFactoryProxy);

			lGUIDToNamePair.mName = L"CMVRMultiSinkFactory";

			lGUIDToNamePair.mTitle = L"CaptureManager Video Renderer multi sink factory";

			getReadModes(lGUIDToNamePair.mContainers);

			return lGUIDToNamePair;
		}

		void MFEVRMultiSinkFactory::getReadModes(
			std::vector<GUIDToNamePair>& aRefReadModes)
		{
			Singleton<CaptureManagerBroker>::getInstance().getEVRMultiSinkModes(
				aRefReadModes);
		}

		// IEVRMultiSinkFactory interface

		STDMETHODIMP MFEVRMultiSinkFactory::createOutputNodes(
			/* [in] */ LPVOID aHandle,
			/* [in] */ IUnknown *aPtrUnkTarget,
			/* [in] */ DWORD aOutputNodeAmount,
			/* [out] */ VARIANT *aPtrArrayPtrTopologyOutputNodes)
		{
			using namespace Core;

			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrArrayPtrTopologyOutputNodes);

				LOG_CHECK_STATE_DESCR(aOutputNodeAmount == 0, E_INVALIDARG);

				std::vector<CComPtrCustom<IUnknown>> lOutputNodes;

				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().createEVRMultiSinkOutputNodes,
					aHandle,
					aPtrUnkTarget,
					aOutputNodeAmount,
					lOutputNodes);

				LOG_CHECK_STATE_DESCR(lOutputNodes.empty(), E_FAIL);

				SAFEARRAY* lPtrSAOutputNodes;

				SAFEARRAYBOUND bound[1];

				bound[0].lLbound = 0;

				bound[0].cElements = lOutputNodes.size();

				lPtrSAOutputNodes = SafeArrayCreate(VT_VARIANT, 1, bound);

				LONG lCount = lOutputNodes.size();

				for (LONG lindex = 0;
					lindex < lCount;
					lindex++)
				{
					IUnknown *lPtrIUnknown;

					lOutputNodes[lindex]->QueryInterface(IID_PPV_ARGS(&lPtrIUnknown));


					VARIANT *lVar = new VARIANT;

					VariantInit(lVar);

					lVar->vt = VT_UNKNOWN;

					lVar->punkVal = lPtrIUnknown;

					do
					{
						LOG_INVOKE_FUNCTION(SafeArrayPutElement, lPtrSAOutputNodes, &lindex, lVar);

					} while (false);

					VariantClear(lVar);

					delete lVar;
				}

				aPtrArrayPtrTopologyOutputNodes->vt = VT_ARRAY | VT_VARIANT;

				aPtrArrayPtrTopologyOutputNodes->parray = lPtrSAOutputNodes;

				lresult = S_OK;


			} while (false);

			return lresult;

		}


		// IDispatch interface stub

		STDMETHODIMP MFEVRMultiSinkFactory::GetIDsOfNames(
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

		HRESULT MFEVRMultiSinkFactory::invokeMethod(
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
					LPVOID lHandle = nullptr;

					CComPtrCustom<IUnknown> lUnkTarget;

					DWORD lOutputNodeAmount = 0;

					if (pDispParams->cArgs == 3)
					{
						if (pDispParams->rgvarg[2].vt == VT_BYREF)
						{
							lHandle = pDispParams->rgvarg[2].byref;
						}
						else if (pDispParams->rgvarg[2].vt == VT_UI4)
						{
							lHandle = (LPVOID)pDispParams->rgvarg[2].uintVal;
						}
						else if (pDispParams->rgvarg[2].vt == VT_I4)
						{
							lHandle = (LPVOID)pDispParams->rgvarg[2].intVal;
						}
						else if (pDispParams->rgvarg[2].vt == VT_UI8)
						{
							lHandle = (LPVOID)pDispParams->rgvarg[2].ullVal;
						}
						else if (pDispParams->rgvarg[2].vt == VT_I8)
						{
							lHandle = (LPVOID)pDispParams->rgvarg[2].llVal;
						}

						if ((pDispParams->rgvarg[1].vt == VT_UNKNOWN || pDispParams->rgvarg[1].vt == VT_I8) && pDispParams->rgvarg[1].punkVal != nullptr)
						{
							pDispParams->rgvarg[1].punkVal->QueryInterface(IID_PPV_ARGS(&lUnkTarget));
						}

						if (pDispParams->rgvarg[0].vt == VT_UI4)
						{
							lOutputNodeAmount = pDispParams->rgvarg[0].ulVal;
						}
						else
						{
							lresult = DISP_E_BADVARTYPE;

							break;
						}
					}
					else if (pDispParams->cArgs == 5)
					{
						LPVOID ltempHandle = nullptr;
						
						if (pDispParams->rgvarg[2].vt == VT_BYREF)
						{
							ltempHandle = pDispParams->rgvarg[2].byref;
						}
						else if (pDispParams->rgvarg[2].vt == VT_UI4)
						{
							ltempHandle = (LPVOID)pDispParams->rgvarg[2].uintVal;
						}
						else if (pDispParams->rgvarg[2].vt == VT_I4)
						{
							ltempHandle = (LPVOID)pDispParams->rgvarg[2].intVal;
						}
						else if (pDispParams->rgvarg[2].vt == VT_UI8)
						{
							ltempHandle = (LPVOID)pDispParams->rgvarg[2].ullVal;
						}
						else if (pDispParams->rgvarg[2].vt == VT_I8)
						{
							ltempHandle = (LPVOID)pDispParams->rgvarg[2].llVal;
						}

						if (ltempHandle != nullptr)
							((IUnknown*)ltempHandle)->QueryInterface(IID_PPV_ARGS(&lUnkTarget));

						if (pDispParams->rgvarg[0].vt == VT_UI4)
						{
							lOutputNodeAmount = pDispParams->rgvarg[0].ulVal;
						}
						else
						{
							lresult = DISP_E_BADVARTYPE;

							break;
						}
					}
					else
						LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 3, DISP_E_BADPARAMCOUNT);


					CComPtrCustom<IUnknown> lOutputNode;

					LOG_INVOKE_FUNCTION(createOutputNodes,
						lHandle,
						lUnkTarget,
						lOutputNodeAmount,
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

		STDMETHODIMP MFEVRMultiSinkFactory::setContainerFormat(
			REFGUID aRefContainerTypeGUID)
		{
			HRESULT lresult;

			do
			{
				lresult = S_OK;

			} while (false);

			return lresult;
		}



		MFEVRMultiSinkFactory::MFEVRMultiSinkFactory()
		{
		}


		MFEVRMultiSinkFactory::~MFEVRMultiSinkFactory()
		{
		}
	}
}
