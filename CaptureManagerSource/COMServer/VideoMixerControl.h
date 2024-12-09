#pragma once

#include "CaptureManagerTypeInfo.h"
#include "BaseDispatch.h"

namespace CaptureManager
{
	namespace COMServer
	{
		class VideoMixerControl :
			public BaseDispatch<IVideoMixerControl>
		{
		public:
			VideoMixerControl();
			virtual ~VideoMixerControl();




			// IVideoMixerControl methods

		public:
			virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE setPosition(
				/* [in] */ IUnknown *aPtrVideoMixerNode,
				/* [in] */ FLOAT aLeft,
				/* [in] */ FLOAT aRight,
				/* [in] */ FLOAT aTop,
				/* [in] */ FLOAT aBottom)override;

			virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE setSrcPosition(
				/* [in] */ IUnknown *aPtrVideoMixerNode,
				/* [in] */ FLOAT aLeft,
				/* [in] */ FLOAT aRight,
				/* [in] */ FLOAT aTop,
				/* [in] */ FLOAT aBottom)override;

			virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE setZOrder(
				/* [in] */ IUnknown *aPtrVideoMixerNode,
				/* [in] */ DWORD aZOrder)override;

			virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE setOpacity(
				/* [in] */ IUnknown *aPtrVideoMixerNode,
				/* [in] */ FLOAT aOpacity)override;

			virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE flush(
				/* [in] */ IUnknown *aPtrVideoMixerNode)override;
		};
	}
}