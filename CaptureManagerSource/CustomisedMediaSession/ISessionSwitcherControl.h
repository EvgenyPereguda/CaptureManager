#pragma once
#include "../Common/MFHeaders.h"


namespace CaptureManager
{
	namespace MediaSession
	{
		namespace CustomisedMediaSession
		{
			MIDL_INTERFACE("DADB2F8F-48A5-4A33-AFC3-FEF96CEE317D")
			ISessionSwitcherControl : public IUnknown
			{
				virtual HRESULT STDMETHODCALLTYPE pauseSwitchers() = 0;
				virtual HRESULT STDMETHODCALLTYPE resumeSwitchers() = 0;
				virtual HRESULT STDMETHODCALLTYPE detachSwitchers() = 0;
				virtual HRESULT STDMETHODCALLTYPE attachSwitcher(
					/* [in] */ IMFTopologyNode *aPtrSwitcherNode,
					/* [in] */ IMFTopologyNode *aPtrDownStreamNode) = 0;
			};
		}
	}
}