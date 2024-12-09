#pragma once

#include <guiddef.h>

struct IUnknown;

typedef long HRESULT;

typedef unsigned long DWORD;

namespace EVRMultiSink
{
	class VideoRenderingClassFactory
	{
	public:

		HRESULT createVideoRenderingClass(
			IUnknown* aPtrUnkPresenter,
			IUnknown* aPtrUnkMixer,
			DWORD aMixerStreamID,
			REFIID aRefIID,
			IUnknown** aPtrPtrVideoRenderingClass);
		
	protected:
		VideoRenderingClassFactory();
		~VideoRenderingClassFactory();
	};
}