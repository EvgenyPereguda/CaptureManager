#pragma once
#include <Unknwnbase.h>

struct IMFMediaType;
struct IMFSample;
struct IMFMediaEventGenerator;
struct IMFTransform;

namespace EVRMultiSink
{
	MIDL_INTERFACE("85C9E9F3-7CBA-45D2-821E-5D6724CA2A1D")
	IPresenter : public IUnknown
	{
		virtual HRESULT initialize(
		UINT32 aImageWidth,
		UINT32 aImageHeight,
		DWORD aNumerator,
		DWORD aDenominator,
		IMFTransform* aPtrMixer) = 0;
		virtual HRESULT ProcessFrame(BOOL aImmediate) = 0;
	};
}