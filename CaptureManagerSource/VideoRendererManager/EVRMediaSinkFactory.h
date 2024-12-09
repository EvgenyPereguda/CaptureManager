#pragma once


struct IMFTransform;

struct IMFMediaSink;

#pragma warning(suppress: 28251)
typedef long HRESULT;

typedef unsigned long DWORD;

namespace CaptureManager
{
	namespace Sinks
	{
		namespace EVR
		{
			struct IPresenter;

			class EVRMediaSinkFactory
			{
			public:

				HRESULT createMediaSink(
					IPresenter* aPtrPresenter,
					IMFTransform* aPtrMixer,
					DWORD aMixerStreamID,
					bool aIsSingleStream,
					IMFMediaSink** aPtrPtrMediaSink);

			protected:
				EVRMediaSinkFactory();
				~EVRMediaSinkFactory();
			};
		}
	}
}