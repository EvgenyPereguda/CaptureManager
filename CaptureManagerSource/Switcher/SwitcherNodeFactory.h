#pragma once
#include <Unknwnbase.h>


namespace CaptureManager
{
	namespace Transform
	{
		class SwitcherNodeFactory
		{
		public:
			SwitcherNodeFactory();
			virtual ~SwitcherNodeFactory();

			HRESULT STDMETHODCALLTYPE createSwitcherNode(
				IUnknown *aPtrDownStreamTopologyNode,
				IUnknown **aPtrPtrTopologySwitcherNode);
		};
	}
}