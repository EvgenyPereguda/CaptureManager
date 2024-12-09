#include "pch.h"
#include "SpreaderNodeFactory.h"
#include "TopologyNode.h"


namespace CaptureManagerToWinRTProxy
{
	SpreaderNodeFactory::SpreaderNodeFactory(IntPtr aIntPtr)
	{
		do
		{
			auto lPtrUnk = reinterpret_cast<IUnknown*>((void*)aIntPtr);

			if (lPtrUnk == nullptr)
				break;

			lPtrUnk->QueryInterface(IID_PPV_ARGS(&mISpreaderNodeFactory));

		} while (false);
	}

	ITopologyNode^ SpreaderNodeFactory::createSpreaderNode(
		IVector<ITopologyNode^>^ aArrayPtrDownStreamTopologyNodes)
	{
		ITopologyNode^ lresult = nullptr;

		do
		{
			if (!mISpreaderNodeFactory)
				break;

			if (aArrayPtrDownStreamTopologyNodes == nullptr)
				break;
			
			CComPtrCustom<IUnknown> lSpreaderNode;
			
			SAFEARRAY *lPtrSAFEARRAY = nullptr;

			SAFEARRAYBOUND lbound[1];

			lbound[0].lLbound = 0;

			lbound[0].cElements = aArrayPtrDownStreamTopologyNodes->Size;

			lPtrSAFEARRAY = SafeArrayCreate(
				VT_VARIANT,
				1,
				lbound);



			LONG lElementIndex = 0;

			for (decltype(aArrayPtrDownStreamTopologyNodes->Size) lIndex = 0;
				lIndex < aArrayPtrDownStreamTopologyNodes->Size;
				lIndex++)
			{
				auto lSourceNode = aArrayPtrDownStreamTopologyNodes->GetAt(lIndex);

				if (lSourceNode == nullptr)
					break;

				TopologyNode^ lTopologyNode = (TopologyNode^)lSourceNode;

				auto lPtrUnk = reinterpret_cast<IUnknown*>((void*)lTopologyNode->getTopologyNode());

				if (lPtrUnk == nullptr)
					break;

				auto lISourceTopologyNode = lPtrUnk;

				VARIANT lVar;

				VariantInit(&lVar);

				lVar.vt = VT_UNKNOWN;

				lVar.punkVal = lISourceTopologyNode;

				SafeArrayPutElement(lPtrSAFEARRAY, &lElementIndex, &lVar);

				++lElementIndex;
			}


			VARIANT lCompressedMediaTypesSAFEARRAY;

			VariantInit(&lCompressedMediaTypesSAFEARRAY);

			lCompressedMediaTypesSAFEARRAY.vt = VT_UNKNOWN | VT_SAFEARRAY;

			lCompressedMediaTypesSAFEARRAY.parray = lPtrSAFEARRAY;

			
			auto lhresult = mISpreaderNodeFactory->createSpreaderNode(
				lCompressedMediaTypesSAFEARRAY,
				&lSpreaderNode);

			SafeArrayDestroy(lPtrSAFEARRAY);

			if (lhresult != 0)
				break;

			if (!lSpreaderNode)
				break;
			
			lresult = ref new TopologyNode(IntPtr(lSpreaderNode.get()));

		} while (false);

		return lresult;
	}
}