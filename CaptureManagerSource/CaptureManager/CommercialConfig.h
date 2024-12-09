#pragma once



#ifdef _WIN64
#define PLATFORM "x64"

#pragma comment(linker, "/INCLUDE:??1?$InstanceMaker@VW_AACAudioEncoderManager@Encoder@Transform@CaptureManager@@V?$Singleton@VEncoderManagerFactory@Encoder@Transform@CaptureManager@@@4@@Core@CaptureManager@@UEAA@XZ")
#pragma comment(linker, "/INCLUDE:??1?$InstanceMaker@VW_H264VideoEncoderManager@Encoder@Transform@CaptureManager@@V?$Singleton@VEncoderManagerFactory@Encoder@Transform@CaptureManager@@@4@@Core@CaptureManager@@UEAA@XZ")
#pragma comment(linker, "/INCLUDE:??1?$InstanceMaker@VW_H265VideoEncoderManager@Encoder@Transform@CaptureManager@@V?$Singleton@VEncoderManagerFactory@Encoder@Transform@CaptureManager@@@4@@Core@CaptureManager@@UEAA@XZ")


#else
#define PLATFORM ""

// Encoders 
#pragma comment(linker, "/INCLUDE:??1?$InstanceMaker@VW_AACAudioEncoderManager@Encoder@Transform@CaptureManager@@V?$Singleton@VEncoderManagerFactory@Encoder@Transform@CaptureManager@@@4@@Core@CaptureManager@@UAE@XZ")
#pragma comment(linker, "/INCLUDE:??1?$InstanceMaker@VW_H264VideoEncoderManager@Encoder@Transform@CaptureManager@@V?$Singleton@VEncoderManagerFactory@Encoder@Transform@CaptureManager@@@4@@Core@CaptureManager@@UAE@XZ")
#pragma comment(linker, "/INCLUDE:??1?$InstanceMaker@VW_H265VideoEncoderManager@Encoder@Transform@CaptureManager@@V?$Singleton@VEncoderManagerFactory@Encoder@Transform@CaptureManager@@@4@@Core@CaptureManager@@UAE@XZ")

#endif