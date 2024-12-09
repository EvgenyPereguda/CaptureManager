#include "pch.h"
#include "EVRMultiSinkFactory.h"
#include "TopologyNode.h"
#include "WrapperIntPtr.h"
#include <Windows.ui.xaml.media.dxinterop.h>
#include <d3d11_2.h>

namespace CaptureManagerToWinRTProxy
{
	EVRMultiSinkFactory::EVRMultiSinkFactory(IntPtr aIntPtr)
	{
		do
		{
			auto lPtrUnk = reinterpret_cast<IUnknown*>((void*)aIntPtr);

			if (lPtrUnk == nullptr)
				break;

			lPtrUnk->QueryInterface(IID_PPV_ARGS(&mIEVRMultiSinkFactory));

		} while (false);
	}

	void EVRMultiSinkFactory::createOutputNodes(
		SwapChainPanel^ aSwapChainPanel,
		IVector<ITopologyNode^>^ aPtrArrayPtrTopologyOutputNodes)
	{
		do
		{
			if (!mIEVRMultiSinkFactory)
				break;

			if (!aSwapChainPanel)
				break;

			if (!aPtrArrayPtrTopologyOutputNodes)
				break;

			Microsoft::WRL::ComPtr<ISwapChainPanelNative> lSwapChainNative;
			

			IInspectable* panelInspectable = (IInspectable*) reinterpret_cast<IInspectable*>(aSwapChainPanel);

			panelInspectable->QueryInterface(__uuidof(ISwapChainPanelNative), (void **)&lSwapChainNative);

			if (lSwapChainNative == nullptr)
				break;

			CComPtrCustom<IUnknown> lOutputNode;

			VARIANT lOutputNodesSAFEARRAY;

			auto lhr = mIEVRMultiSinkFactory->createOutputNodes(
				nullptr,
				lSwapChainNative.Get(),
				1,
				&lOutputNodesSAFEARRAY);

			if (lhr != 0)
				break;

			if (lOutputNodesSAFEARRAY.parray == nullptr)
				break;

			if (lOutputNodesSAFEARRAY.vt != (VT_ARRAY | VT_VARIANT))
				break;

			LONG lBoundMediaTypes(0);

			LONG uBoundMediaTypes(0);

			SafeArrayGetUBound(lOutputNodesSAFEARRAY.parray, 1, &uBoundMediaTypes);

			SafeArrayGetUBound(lOutputNodesSAFEARRAY.parray, 1, &lBoundMediaTypes);


			for (LONG lIndex = lBoundMediaTypes; lIndex <= uBoundMediaTypes; lIndex++)
			{
				VARIANT lVar;

				auto lr = SafeArrayGetElement(lOutputNodesSAFEARRAY.parray, &lIndex, &lVar);

				if (SUCCEEDED(lr) && lVar.vt == VT_UNKNOWN && lVar.punkVal != nullptr)
				{
					TopologyNode^ lTopologyNode = ref new TopologyNode(IntPtr(lVar.punkVal));

					aPtrArrayPtrTopologyOutputNodes->Append(lTopologyNode);
				}

				VariantClear(&lVar);
			}

			SafeArrayDestroy(lOutputNodesSAFEARRAY.parray);

		} while (false);
	}

	void EVRMultiSinkFactory::createOutputNodes(
		IIntPtr^ aIIntPtr,
		IVector<ITopologyNode^>^ aPtrArrayPtrTopologyOutputNodes)
	{
		do
		{
			if (!mIEVRMultiSinkFactory)
				break;

			if (aIIntPtr == nullptr)
				break;

			WrapperIntPtr^ lWrapperIntPtr = (WrapperIntPtr^)aIIntPtr;

			if (lWrapperIntPtr == nullptr)
				break;

			auto lIntPtr = lWrapperIntPtr->getIntPtr();

			if ((void*)lIntPtr == nullptr)
				break;

			IUnknown* lPtrIUnknown = (IUnknown*) reinterpret_cast<IUnknown*>((void*)lIntPtr);

			if (lPtrIUnknown == nullptr)
				break;

			CComPtrCustom<IUnknown> lOutputNode;

			VARIANT lOutputNodesSAFEARRAY;

			auto lhr = mIEVRMultiSinkFactory->createOutputNodes(
				nullptr,
				lPtrIUnknown,
				1,
				&lOutputNodesSAFEARRAY);

			if (lhr != 0)
				break;

			if (lOutputNodesSAFEARRAY.parray == nullptr)
				break;

			if (lOutputNodesSAFEARRAY.vt != (VT_ARRAY | VT_VARIANT))
				break;

			LONG lBoundMediaTypes(0);

			LONG uBoundMediaTypes(0);

			SafeArrayGetUBound(lOutputNodesSAFEARRAY.parray, 1, &uBoundMediaTypes);

			SafeArrayGetUBound(lOutputNodesSAFEARRAY.parray, 1, &lBoundMediaTypes);


			for (LONG lIndex = lBoundMediaTypes; lIndex <= uBoundMediaTypes; lIndex++)
			{
				VARIANT lVar;

				auto lr = SafeArrayGetElement(lOutputNodesSAFEARRAY.parray, &lIndex, &lVar);

				if (SUCCEEDED(lr) && lVar.vt == VT_UNKNOWN && lVar.punkVal != nullptr)
				{
					TopologyNode^ lTopologyNode = ref new TopologyNode(IntPtr(lVar.punkVal));

					aPtrArrayPtrTopologyOutputNodes->Append(lTopologyNode);
				}

				VariantClear(&lVar);
			}

			SafeArrayDestroy(lOutputNodesSAFEARRAY.parray);

		} while (false);
	}
}