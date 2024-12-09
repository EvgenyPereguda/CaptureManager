#pragma once
#include "BaseClasses.h"

#define var(interafce) const IID IID_##interafce
#define decl(x) var(x)
#define ADD_IID(inteface) decl(inteface)  = __uuidof(inteface)

#define DEFINE_GUID_FULL(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
        EXTERN_C const GUID DECLSPEC_SELECTANY name \
                = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }

ADD_IID(IPin);
ADD_IID(IEnumPins);
ADD_IID(IEnumMediaTypes);
ADD_IID(IMediaFilter);
ADD_IID(IBaseFilter);
ADD_IID(IMediaSample);
ADD_IID(IMediaSample2);
ADD_IID(IMemAllocator);
ADD_IID(IMemAllocatorCallbackTemp);
ADD_IID(IMemInputPin);
ADD_IID(IAMovieSetup);
ADD_IID(IFilterMapper);
ADD_IID(IMediaEventSink);
ADD_IID(IFilterGraph2);
ADD_IID(ISeekingPassThru);
ADD_IID(IPinConnection);
ADD_IID(IPinFlowControl);
ADD_IID(IDeferredCommand);
ADD_IID(IBasicVideo2);
ADD_IID(IBasicVideo);
ADD_IID(IVideoWindow);
ADD_IID(IBasicAudio);
ADD_IID(IMediaEventEx);
ADD_IID(IMediaEvent);
ADD_IID(IMediaControl);
ADD_IID(IMediaPosition);
ADD_IID(IMediaSeeking);
ADD_IID(IAsyncReader);
ADD_IID(IReferenceClock);
ADD_IID(IReferenceClockTimerControl);
ADD_IID(IAMClockAdjust);

DEFINE_GUID_FULL(LIBID_QuartzTypeLib, 0x56A868B0L, 0x0AD4, 0x11CE, 0xB0, 0x3A, 0x00, 0x20, 0xAF, 0x0B, 0xA7, 0x70);



