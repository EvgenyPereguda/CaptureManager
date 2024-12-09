#pragma once


struct IMFTransform;

struct IMFMediaSink;

#pragma warning(suppress: 28251)
typedef long HRESULT;

typedef unsigned long DWORD;

namespace EVRMultiSink
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
					IMFMediaSink** aPtrPtrMediaSink);

			protected:
				EVRMediaSinkFactory();
				~EVRMediaSinkFactory();
			};
		}
	}
}