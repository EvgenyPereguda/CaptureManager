#include "pch.h"
#include "SessionControl.h"
#include "Session.h"
#include "TopologyNode.h"

namespace CaptureManagerToWinRTProxy
{
	SessionControl::SessionControl(IntPtr aIntPtr)
	{
		do
		{
			auto lPtrUnk = reinterpret_cast<IUnknown*>((void*)aIntPtr);

			if (lPtrUnk == nullptr)
				break;

			lPtrUnk->QueryInterface(IID_PPV_ARGS(&mISessionControl));

		} while (false);
	}

	ISession^ SessionControl::createSession(
		IVector<ITopologyNode^>^ aArrayPtrSourceNodesOfTopology)
	{
		ISession^ lresult = nullptr;

		do
		{
			if (!mISessionControl)
				break;

			if (aArrayPtrSourceNodesOfTopology == nullptr)
				break;
			

			SAFEARRAY *lPtrSAFEARRAY = nullptr;

			SAFEARRAYBOUND lbound[1];

			lbound[0].lLbound = 0;

			lbound[0].cElements = aArrayPtrSourceNodesOfTopology->Size;

			lPtrSAFEARRAY = SafeArrayCreate(
				VT_VARIANT,
				1,
				lbound);


			LONG lElementIndex = 0;

			for (decltype(aArrayPtrSourceNodesOfTopology->Size) lIndex = 0;
				lIndex < aArrayPtrSourceNodesOfTopology->Size;
				lIndex++)
			{
				auto lObjectSourceNode = aArrayPtrSourceNodesOfTopology->GetAt(lIndex);

				if (lObjectSourceNode == nullptr)
					break;
				
				TopologyNode^ lTopologyNode = (TopologyNode^)lObjectSourceNode;

				if (lTopologyNode == nullptr)
					break;
				
				auto lISourceTopologyNode = reinterpret_cast<IUnknown*>((void*)lTopologyNode->getTopologyNode());

				if (lISourceTopologyNode == nullptr)
					break;
				
				VARIANT lVar;

				VariantInit(&lVar);

				lVar.vt = VT_UNKNOWN;

				lVar.punkVal = lISourceTopologyNode;

				SafeArrayPutElement(lPtrSAFEARRAY, &lElementIndex, &lVar);

				++lElementIndex;
			}


			VARIANT lSourceNodesSAFEARRAY;

			VariantInit(&lSourceNodesSAFEARRAY);

			lSourceNodesSAFEARRAY.vt = VT_UNKNOWN | VT_SAFEARRAY;

			lSourceNodesSAFEARRAY.parray = lPtrSAFEARRAY;


			CComPtrCustom<IUnknown> lSession;

			auto lhresult = mISessionControl->createSession(
				lSourceNodesSAFEARRAY,
				__uuidof(::ISession),
				&lSession);

			SafeArrayDestroy(lPtrSAFEARRAY);

			if (FAILED(lhresult))
				break;

			if (!lSession)
				break;
						
			lresult = ref new Session(IntPtr(lSession.get()));
						
		} while (false);

		return lresult;
	}
}