#pragma once
#include "IMixerWrapper.h"
#include "../Common/BaseUnknown.h"
#include "../Common/ComPtrCustom.h"


namespace CaptureManager
{
	namespace MediaSession
	{
		namespace CustomisedMediaSession
		{

			class MixerWrapper :
				public BaseUnknown<IMixerWrapper>
			{
			public:
				MixerWrapper(DWORD aInputNodeAmount);
				virtual ~MixerWrapper();
							   
				static HRESULT getStreamCount(
					REFGUID aRefMajorType,
					DWORD& aRefInputNodeAmount);

			//	IMixerWrapper implementation

				virtual HRESULT initialize(
					IMFMediaType* aPtrMediaTypeType)override;

				virtual HRESULT getMixer(
					IMFTransform** aPtrMixer)override;
		

			private:

				CComPtrCustom<IUnknown> mMixerUnkTransform;

				const DWORD mInputNodeAmount;
			};
		}
	}
}