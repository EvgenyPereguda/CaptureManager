#pragma once

#include "../Common/BaseMFAttributes.h"
#include "../Common/MFHeaders.h"
#include "../Common/ComPtrCustom.h"
#include <strmif.h>

namespace EVRMultiSink
{
	using namespace CaptureManager;

	MIDL_INTERFACE("D4BB697C-E0C9-4ABE-B02F-F24557AF1DA8")
		ICustomisedFilterTopologyNode{};

	class CustomisedFilterTopologyNode :
		public BaseUnknown<IUnknown>
	{
	public:
		CustomisedFilterTopologyNode(
			IMFTopologyNode* aPtrIMFTopologyNode,
			IMFActivate* aPtrIMFActivate);
		virtual ~CustomisedFilterTopologyNode();
				
	protected:

		virtual bool findIncapsulatedInterface(
				REFIID aRefIID,
				void** aPtrPtrVoidObject);

	private:

		CComPtrCustom<IMFTopologyNode> mTopologyNode;

		CComPtrCustom<IBaseFilter> mBaseFilter;

		CComPtrCustom<IMFActivate> mActivate;


		HRESULT createBaseFilter();
	};
}
