#pragma once

#include "CaptureManagerTypeInfo.h"
#include "BaseDispatch.h"

namespace CaptureManager
{
	namespace COMServer
	{
		class SARVolumeControl :
			public BaseDispatch<ISARVolumeControl>
		{
		public:
			SARVolumeControl();
			virtual ~SARVolumeControl();

			//	ISARVolumeControl implements

			virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE getChannelCount(
				/* [in] */ IUnknown *aPtrSARNode,
				/* [out] */ UINT32 *aPtrCount) override;

			virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE setChannelVolume(
				/* [in] */ IUnknown *aPtrSARNode,
				/* [in] */ UINT32 aIndex,
				/* [in] */ const float aLevel) override;

			virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE getChannelVolume(
				/* [in] */ IUnknown *aPtrSARNode,
				/* [in] */ UINT32 aIndex,
				/* [out] */ float *aPtrLevel) override;
		};
	}
}
