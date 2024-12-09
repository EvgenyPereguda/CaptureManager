#pragma once


#ifdef _WIN64

#pragma comment(linker, "/INCLUDE:??1?$InstanceMaker@VScreenCaptureProcessorDirectX9@ScreenCapture@Sources@CaptureManager@@VScreenCaptureProcessorFactory@34@@Core@CaptureManager@@UEAA@XZ")
#pragma comment(linker, "/INCLUDE:??1?$InstanceMaker@VScreenCaptureProcessorGDI@ScreenCapture@Sources@CaptureManager@@VScreenCaptureProcessorFactory@34@@Core@CaptureManager@@UEAA@XZ")
#pragma comment(linker, "/INCLUDE:??1?$InstanceMaker@VScreenCaptureProcessorIDXGIOutputDuplication@ScreenCapture@Sources@CaptureManager@@VScreenCaptureProcessorFactory@34@@Core@CaptureManager@@UEAA@XZ")
#pragma comment(linker, "/INCLUDE:??1?$InstanceMaker@VEVRSinkFactory@COMServer@CaptureManager@@V?$Singleton@VSinkCollection@COMServer@CaptureManager@@@3@@Core@CaptureManager@@UEAA@XZ")
#pragma comment(linker, "/INCLUDE:??1?$InstanceMaker@VEVRMultiSinkFactory@COMServer@CaptureManager@@V?$Singleton@VSinkCollection@COMServer@CaptureManager@@@3@@Core@CaptureManager@@UEAA@XZ")
#pragma comment(linker, "/INCLUDE:??1?$InstanceMaker@VMFEVRMultiSinkFactory@COMServer@CaptureManager@@V?$Singleton@VSinkCollection@COMServer@CaptureManager@@@3@@Core@CaptureManager@@UEAA@XZ")
#pragma comment(linker, "/INCLUDE:??1?$InstanceMaker@VSARSinkFactory@COMServer@CaptureManager@@V?$Singleton@VSinkCollection@COMServer@CaptureManager@@@3@@Core@CaptureManager@@UEAA@XZ")
#pragma comment(linker, "/INCLUDE:??1?$InstanceMaker@VSampleGrabberCallSinkFactory@COMServer@CaptureManager@@V?$Singleton@VSinkCollection@COMServer@CaptureManager@@@3@@Core@CaptureManager@@UEAA@XZ")
#pragma comment(linker, "/INCLUDE:??1?$InstanceMaker@VSampleGrabberCallbackSinkFactory@COMServer@CaptureManager@@V?$Singleton@VSinkCollection@COMServer@CaptureManager@@@3@@Core@CaptureManager@@UEAA@XZ")
#pragma comment(linker, "/INCLUDE:??1?$InstanceMaker@VASFOutputNodeFactory@OutputNodeFactory@Sinks@CaptureManager@@V?$Singleton@VOutputNodeFactoryCollection@Sinks@CaptureManager@@@4@@Core@CaptureManager@@UEAA@XZ")
#pragma comment(linker, "/INCLUDE:??1?$InstanceMaker@VFileSinkFactory@COMServer@CaptureManager@@V?$Singleton@VSinkCollection@COMServer@CaptureManager@@@3@@Core@CaptureManager@@UEAA@XZ")
#pragma comment(linker, "/INCLUDE:??1?$InstanceMaker@VByteStreamSinkFactory@COMServer@CaptureManager@@V?$Singleton@VSinkCollection@COMServer@CaptureManager@@@3@@Core@CaptureManager@@UEAA@XZ")
#pragma comment(linker, "/INCLUDE:??1?$InstanceMaker@VWMVideo9ScreenEncoderManager@Encoder@Transform@CaptureManager@@V?$Singleton@VEncoderManagerFactory@Encoder@Transform@CaptureManager@@@4@@Core@CaptureManager@@UEAA@XZ")
#pragma comment(linker, "/INCLUDE:??1?$InstanceMaker@VWMVideo9EncoderManager@Encoder@Transform@CaptureManager@@V?$Singleton@VEncoderManagerFactory@Encoder@Transform@CaptureManager@@@4@@Core@CaptureManager@@UEAA@XZ")
#pragma comment(linker, "/INCLUDE:??1?$InstanceMaker@VWMVideo8EncoderManager@Encoder@Transform@CaptureManager@@V?$Singleton@VEncoderManagerFactory@Encoder@Transform@CaptureManager@@@4@@Core@CaptureManager@@UEAA@XZ")
#pragma comment(linker, "/INCLUDE:??1?$InstanceMaker@VWMAudioVoiceEncoderManager@Encoder@Transform@CaptureManager@@V?$Singleton@VEncoderManagerFactory@Encoder@Transform@CaptureManager@@@4@@Core@CaptureManager@@UEAA@XZ")
#pragma comment(linker, "/INCLUDE:??1?$InstanceMaker@VWMAudioEncoderManager@Encoder@Transform@CaptureManager@@V?$Singleton@VEncoderManagerFactory@Encoder@Transform@CaptureManager@@@4@@Core@CaptureManager@@UEAA@XZ")

#else

#pragma comment(linker, "/INCLUDE:??$?0H@?$InstanceMaker@VScreenCaptureProcessorDirectX9@ScreenCapture@Sources@CaptureManager@@VScreenCaptureProcessorFactory@34@@Core@CaptureManager@@QAE@H@Z")
#pragma comment(linker, "/INCLUDE:??$?0H@?$InstanceMaker@VScreenCaptureProcessorGDI@ScreenCapture@Sources@CaptureManager@@VScreenCaptureProcessorFactory@34@@Core@CaptureManager@@QAE@H@Z")
#pragma comment(linker, "/INCLUDE:??$?0H@?$InstanceMaker@VScreenCaptureProcessorIDXGIOutputDuplication@ScreenCapture@Sources@CaptureManager@@VScreenCaptureProcessorFactory@34@@Core@CaptureManager@@QAE@H@Z")
#pragma comment(linker, "/INCLUDE:??1?$InstanceMaker@VEVRSinkFactory@COMServer@CaptureManager@@V?$Singleton@VSinkCollection@COMServer@CaptureManager@@@3@@Core@CaptureManager@@UAE@XZ")
#pragma comment(linker, "/INCLUDE:??1?$InstanceMaker@VEVRMultiSinkFactory@COMServer@CaptureManager@@V?$Singleton@VSinkCollection@COMServer@CaptureManager@@@3@@Core@CaptureManager@@UAE@XZ")
#pragma comment(linker, "/INCLUDE:??1?$InstanceMaker@VMFEVRMultiSinkFactory@COMServer@CaptureManager@@V?$Singleton@VSinkCollection@COMServer@CaptureManager@@@3@@Core@CaptureManager@@UAE@XZ")
#pragma comment(linker, "/INCLUDE:??1?$InstanceMaker@VSARSinkFactory@COMServer@CaptureManager@@V?$Singleton@VSinkCollection@COMServer@CaptureManager@@@3@@Core@CaptureManager@@UAE@XZ")
#pragma comment(linker, "/INCLUDE:??1?$InstanceMaker@VSampleGrabberCallSinkFactory@COMServer@CaptureManager@@V?$Singleton@VSinkCollection@COMServer@CaptureManager@@@3@@Core@CaptureManager@@UAE@XZ")
#pragma comment(linker, "/INCLUDE:??1?$InstanceMaker@VSampleGrabberCallbackSinkFactory@COMServer@CaptureManager@@V?$Singleton@VSinkCollection@COMServer@CaptureManager@@@3@@Core@CaptureManager@@UAE@XZ")
#pragma comment(linker, "/INCLUDE:??1?$InstanceMaker@VASFOutputNodeFactory@OutputNodeFactory@Sinks@CaptureManager@@V?$Singleton@VOutputNodeFactoryCollection@Sinks@CaptureManager@@@4@@Core@CaptureManager@@UAE@XZ")
#pragma comment(linker, "/INCLUDE:??1?$InstanceMaker@VFileSinkFactory@COMServer@CaptureManager@@V?$Singleton@VSinkCollection@COMServer@CaptureManager@@@3@@Core@CaptureManager@@UAE@XZ")
#pragma comment(linker, "/INCLUDE:??1?$InstanceMaker@VByteStreamSinkFactory@COMServer@CaptureManager@@V?$Singleton@VSinkCollection@COMServer@CaptureManager@@@3@@Core@CaptureManager@@UAE@XZ")
#pragma comment(linker, "/INCLUDE:??1?$InstanceMaker@VWMVideo9ScreenEncoderManager@Encoder@Transform@CaptureManager@@V?$Singleton@VEncoderManagerFactory@Encoder@Transform@CaptureManager@@@4@@Core@CaptureManager@@UAE@XZ")
#pragma comment(linker, "/INCLUDE:??1?$InstanceMaker@VWMVideo9EncoderManager@Encoder@Transform@CaptureManager@@V?$Singleton@VEncoderManagerFactory@Encoder@Transform@CaptureManager@@@4@@Core@CaptureManager@@UAE@XZ")
#pragma comment(linker, "/INCLUDE:??1?$InstanceMaker@VWMVideo8EncoderManager@Encoder@Transform@CaptureManager@@V?$Singleton@VEncoderManagerFactory@Encoder@Transform@CaptureManager@@@4@@Core@CaptureManager@@UAE@XZ")
#pragma comment(linker, "/INCLUDE:??1?$InstanceMaker@VWMAudioVoiceEncoderManager@Encoder@Transform@CaptureManager@@V?$Singleton@VEncoderManagerFactory@Encoder@Transform@CaptureManager@@@4@@Core@CaptureManager@@UAE@XZ")
#pragma comment(linker, "/INCLUDE:??1?$InstanceMaker@VWMAudioEncoderManager@Encoder@Transform@CaptureManager@@V?$Singleton@VEncoderManagerFactory@Encoder@Transform@CaptureManager@@@4@@Core@CaptureManager@@UAE@XZ")

#endif
