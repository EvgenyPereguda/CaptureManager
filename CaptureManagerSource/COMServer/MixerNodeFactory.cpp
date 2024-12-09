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

#include "MixerNodeFactory.h"
#include "../Common/ComPtrCustom.h"
#include "../Common/Singleton.h"
#include "../Common/IInnerCaptureProcessor.h"
#include "../Common/Common.h"
#include "../Common/BaseUnknown.h"
#include "../Common/MFHeaders.h"
#include "../Common/GUIDs.h"
#include "../ConfigManager/ConfigManager.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../CaptureManagerBroker/CaptureManagerBroker.h"


namespace CaptureManager
{
	namespace COMServer
	{
		enum class MethodsEnum :DISPID
		{
			CreateMixerNode = 1
		};

		MixerNodeFactory::MixerNodeFactory()
		{
		}
		
		MixerNodeFactory::~MixerNodeFactory()
		{
		}

		StreamControlInfo MixerNodeFactory::getStreamControlInfo(
			pugi::xml_node& aRefRootXMLElement,
			std::vector<StreamControlInfo>& aCollection)
		{
			StreamControlInfo lStreamControlInfo;

			lStreamControlInfo.mName = L"";

			lStreamControlInfo.mTitle = L"";

			lStreamControlInfo.mGUID = GUID_NULL;

			lStreamControlInfo.mInputPortCount = 1;

			lStreamControlInfo.mIsInputPortCountConstant = true;

			lStreamControlInfo.mOutputPortCount = 1;

			lStreamControlInfo.mIsOutputPortCountConstant = true;


			Singleton<CaptureManagerBroker>::getInstance().getMixerNodeFactoryCollection(
				aRefRootXMLElement,
				aCollection);

			return lStreamControlInfo;
		}
		
		// IMixerNodeFactory DWORDerface

		STDMETHODIMP MixerNodeFactory::createMixerNodes(
			/* [in] */ IUnknown *aPtrDownStreamTopologyNode,
			/* [in] */ DWORD aInputNodeAmount,
			/* [out] */ VARIANT *aPtrArrayPtrTopologyInputNodes)
		{
			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrDownStreamTopologyNode);

				LOG_CHECK_PTR_MEMORY(aPtrArrayPtrTopologyInputNodes);

				LOG_CHECK_STATE_DESCR(aInputNodeAmount == 0, E_INVALIDARG);
							   
				std::vector<CComPtrCustom<IUnknown>> lOutputNodes;

				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().createMixerNodes,
					aPtrDownStreamTopologyNode,
					aInputNodeAmount,
					lOutputNodes);

				LOG_CHECK_STATE_DESCR(lOutputNodes.empty(), E_FAIL);

				SAFEARRAY* lPtrSAInputNodes;

				SAFEARRAYBOUND bound[1];

				bound[0].lLbound = 0;

				bound[0].cElements = lOutputNodes.size();

				lPtrSAInputNodes = SafeArrayCreate(VT_VARIANT, 1, bound);

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
						LOG_INVOKE_FUNCTION(SafeArrayPutElement, lPtrSAInputNodes, &lindex, lVar);

					} while (false);

					VariantClear(lVar);

					delete lVar;
				}

				aPtrArrayPtrTopologyInputNodes->vt = VT_ARRAY | VT_VARIANT;

				aPtrArrayPtrTopologyInputNodes->parray = lPtrSAInputNodes;

				lresult = S_OK;

			} while (false);

			return lresult;
		}





		// IDispatch interface stub

		STDMETHODIMP MixerNodeFactory::GetIDsOfNames(
			__RPC__in REFIID riid,
			/* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
			/* [range][in] */ __RPC__in_range(0, 16384) UINT cNames,
			LCID lcid,
			/* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId) {

			HRESULT lresult(DISP_E_UNKNOWNNAME);

			do
			{
				LOG_CHECK_STATE(cNames != 1);

				if (_wcsicmp(*rgszNames, OLESTR("createMixerNode")) == 0)
				{
					*rgDispId = (int)MethodsEnum::CreateMixerNode;

					lresult = S_OK;
				}

			} while (false);

			return lresult;
		}

		HRESULT MixerNodeFactory::invokeMethod(
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
				case (int)MethodsEnum::CreateMixerNode:
				{
					pVarResult->vt = VT_BSTR;

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