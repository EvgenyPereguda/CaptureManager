#pragma once

#include <Unknwnbase.h>
#include <map>
#include <vector>

#include "../PugiXML/pugixml.hpp"
#include "../Common/Common.h"

struct IMFAttributes;
struct IMFActivate;
struct IMFMediaSource;
struct IMFPresentationDescriptor;
struct IMFStreamDescriptor;
struct IMFMediaTypeHandler;
struct IMFMediaType;
struct IMFVideoMediaType;
struct IMFAudioMediaType;
struct IMFSampleGrabberSinkCallback;
struct IMFTopologyNode;
struct IMFTopology;
struct IMFMediaEventGenerator;
struct IMFMediaEvent;
struct IMFAsyncResult;
struct IMFAsyncCallback;
struct IMFMediaSession;
struct IMFASFProfile;
struct IMFASFContentInfo;
struct IMFASFStreamConfig;
struct IPropertyStore;
struct IMFMediaEventQueue;
struct IMFCollection;
struct IMFMediaSink;
struct IMFStreamSink;
struct IMFPresentationClock;
struct IMFPresentationTimeSource;
struct IMFTransform;
struct IMFSample;
struct IMFMediaBuffer;
struct IMFByteStream;
struct IMF2DBuffer;
struct IMFFinalizableMediaSink;
struct IMFClockStateSink;
struct IMFGetService;
struct IMFVideoDisplayControl;
struct IMFMediaSink;
struct IMFDXGIDeviceManager;
struct IMFVideoSampleAllocatorEx;
struct IMFMediaStream;
typedef struct __MIDL___MIDL_itf_mfobjects_0000_0008_0003 MFT_REGISTER_TYPE_INFO;
enum MF_TOPOLOGY_TYPE;
typedef enum _MF_ATTRIBUTES_MATCH_TYPE MF_ATTRIBUTES_MATCH_TYPE;
typedef DWORD MediaEventType; 
typedef unsigned __int64 TOPOID;
typedef struct _AMMediaType	AM_MEDIA_TYPE;
typedef struct tagBITMAPINFOHEADER BITMAPINFOHEADER;
typedef struct tWAVEFORMATEX WAVEFORMATEX;
typedef enum _MFVideoInterlaceMode 	MFVideoInterlaceMode;
typedef ULONGLONG QWORD;
typedef struct _MFVIDEOFORMAT MFVIDEOFORMAT;
typedef enum
{
	// MF_STANDARD_WORKQUEUE: Work queue in a thread without Window 
	// message loop.
	_MF_STANDARD_WORKQUEUE = 0,

	// MF_WINDOW_WORKQUEUE: Work queue in a thread running Window 
	// Message loop that calls PeekMessage() / DispatchMessage()..
	_MF_WINDOW_WORKQUEUE = 1,

	//
	//
	_MF_MULTITHREADED_WORKQUEUE = 2, // common MT threadpool
}   _MFASYNC_WORKQUEUE_TYPE;

typedef /* [public] */
enum __MIDL___MIDL_itf_mfobjects_0000_0017_0001 MF_FILE_ACCESSMODE;

typedef /* [public] */
enum __MIDL___MIDL_itf_mfobjects_0000_0017_0002 MF_FILE_OPENMODE;

typedef /* [public] */
enum __MIDL___MIDL_itf_mfobjects_0000_0017_0003 MF_FILE_FLAGS;

typedef 
enum _MFT_MESSAGE_TYPE MFT_MESSAGE_TYPE;

typedef struct _MFT_OUTPUT_DATA_BUFFER MFT_OUTPUT_DATA_BUFFER;

typedef struct _MFT_OUTPUT_STREAM_INFO MFT_OUTPUT_STREAM_INFO; 

typedef struct _MFT_INPUT_STREAM_INFO MFT_INPUT_STREAM_INFO;
typedef LONGLONG MFTIME;
typedef unsigned __int64 MFWORKITEM_KEY;

typedef enum _DXGI_FORMAT
{
	_DXGI_FORMAT_UNKNOWN = 0,
	_DXGI_FORMAT_R32G32B32A32_TYPELESS = 1,
	_DXGI_FORMAT_R32G32B32A32_FLOAT = 2,
	_DXGI_FORMAT_R32G32B32A32_UINT = 3,
	_DXGI_FORMAT_R32G32B32A32_SINT = 4,
	_DXGI_FORMAT_R32G32B32_TYPELESS = 5,
	_DXGI_FORMAT_R32G32B32_FLOAT = 6,
	_DXGI_FORMAT_R32G32B32_UINT = 7,
	_DXGI_FORMAT_R32G32B32_SINT = 8,
	_DXGI_FORMAT_R16G16B16A16_TYPELESS = 9,
	_DXGI_FORMAT_R16G16B16A16_FLOAT = 10,
	_DXGI_FORMAT_R16G16B16A16_UNORM = 11,
	_DXGI_FORMAT_R16G16B16A16_UINT = 12,
	_DXGI_FORMAT_R16G16B16A16_SNORM = 13,
	_DXGI_FORMAT_R16G16B16A16_SINT = 14,
	_DXGI_FORMAT_R32G32_TYPELESS = 15,
	_DXGI_FORMAT_R32G32_FLOAT = 16,
	_DXGI_FORMAT_R32G32_UINT = 17,
	_DXGI_FORMAT_R32G32_SINT = 18,
	_DXGI_FORMAT_R32G8X24_TYPELESS = 19,
	_DXGI_FORMAT_D32_FLOAT_S8X24_UINT = 20,
	_DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS = 21,
	_DXGI_FORMAT_X32_TYPELESS_G8X24_UINT = 22,
	_DXGI_FORMAT_R10G10B10A2_TYPELESS = 23,
	_DXGI_FORMAT_R10G10B10A2_UNORM = 24,
	_DXGI_FORMAT_R10G10B10A2_UINT = 25,
	_DXGI_FORMAT_R11G11B10_FLOAT = 26,
	_DXGI_FORMAT_R8G8B8A8_TYPELESS = 27,
	_DXGI_FORMAT_R8G8B8A8_UNORM = 28,
	_DXGI_FORMAT_R8G8B8A8_UNORM_SRGB = 29,
	_DXGI_FORMAT_R8G8B8A8_UINT = 30,
	_DXGI_FORMAT_R8G8B8A8_SNORM = 31,
	_DXGI_FORMAT_R8G8B8A8_SINT = 32,
	_DXGI_FORMAT_R16G16_TYPELESS = 33,
	_DXGI_FORMAT_R16G16_FLOAT = 34,
	_DXGI_FORMAT_R16G16_UNORM = 35,
	_DXGI_FORMAT_R16G16_UINT = 36,
	_DXGI_FORMAT_R16G16_SNORM = 37,
	_DXGI_FORMAT_R16G16_SINT = 38,
	_DXGI_FORMAT_R32_TYPELESS = 39,
	_DXGI_FORMAT_D32_FLOAT = 40,
	_DXGI_FORMAT_R32_FLOAT = 41,
	_DXGI_FORMAT_R32_UINT = 42,
	_DXGI_FORMAT_R32_SINT = 43,
	_DXGI_FORMAT_R24G8_TYPELESS = 44,
	_DXGI_FORMAT_D24_UNORM_S8_UINT = 45,
	_DXGI_FORMAT_R24_UNORM_X8_TYPELESS = 46,
	_DXGI_FORMAT_X24_TYPELESS_G8_UINT = 47,
	_DXGI_FORMAT_R8G8_TYPELESS = 48,
	_DXGI_FORMAT_R8G8_UNORM = 49,
	_DXGI_FORMAT_R8G8_UINT = 50,
	_DXGI_FORMAT_R8G8_SNORM = 51,
	_DXGI_FORMAT_R8G8_SINT = 52,
	_DXGI_FORMAT_R16_TYPELESS = 53,
	_DXGI_FORMAT_R16_FLOAT = 54,
	_DXGI_FORMAT_D16_UNORM = 55,
	_DXGI_FORMAT_R16_UNORM = 56,
	_DXGI_FORMAT_R16_UINT = 57,
	_DXGI_FORMAT_R16_SNORM = 58,
	_DXGI_FORMAT_R16_SINT = 59,
	_DXGI_FORMAT_R8_TYPELESS = 60,
	_DXGI_FORMAT_R8_UNORM = 61,
	_DXGI_FORMAT_R8_UINT = 62,
	_DXGI_FORMAT_R8_SNORM = 63,
	_DXGI_FORMAT_R8_SINT = 64,
	_DXGI_FORMAT_A8_UNORM = 65,
	_DXGI_FORMAT_R1_UNORM = 66,
	_DXGI_FORMAT_R9G9B9E5_SHAREDEXP = 67,
	_DXGI_FORMAT_R8G8_B8G8_UNORM = 68,
	_DXGI_FORMAT_G8R8_G8B8_UNORM = 69,
	_DXGI_FORMAT_BC1_TYPELESS = 70,
	_DXGI_FORMAT_BC1_UNORM = 71,
	_DXGI_FORMAT_BC1_UNORM_SRGB = 72,
	_DXGI_FORMAT_BC2_TYPELESS = 73,
	_DXGI_FORMAT_BC2_UNORM = 74,
	_DXGI_FORMAT_BC2_UNORM_SRGB = 75,
	_DXGI_FORMAT_BC3_TYPELESS = 76,
	_DXGI_FORMAT_BC3_UNORM = 77,
	_DXGI_FORMAT_BC3_UNORM_SRGB = 78,
	_DXGI_FORMAT_BC4_TYPELESS = 79,
	_DXGI_FORMAT_BC4_UNORM = 80,
	_DXGI_FORMAT_BC4_SNORM = 81,
	_DXGI_FORMAT_BC5_TYPELESS = 82,
	_DXGI_FORMAT_BC5_UNORM = 83,
	_DXGI_FORMAT_BC5_SNORM = 84,
	_DXGI_FORMAT_B5G6R5_UNORM = 85,
	_DXGI_FORMAT_B5G5R5A1_UNORM = 86,
	_DXGI_FORMAT_B8G8R8A8_UNORM = 87,
	_DXGI_FORMAT_B8G8R8X8_UNORM = 88,
	_DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM = 89,
	_DXGI_FORMAT_B8G8R8A8_TYPELESS = 90,
	_DXGI_FORMAT_B8G8R8A8_UNORM_SRGB = 91,
	_DXGI_FORMAT_B8G8R8X8_TYPELESS = 92,
	_DXGI_FORMAT_B8G8R8X8_UNORM_SRGB = 93,
	_DXGI_FORMAT_BC6H_TYPELESS = 94,
	_DXGI_FORMAT_BC6H_UF16 = 95,
	_DXGI_FORMAT_BC6H_SF16 = 96,
	_DXGI_FORMAT_BC7_TYPELESS = 97,
	_DXGI_FORMAT_BC7_UNORM = 98,
	_DXGI_FORMAT_BC7_UNORM_SRGB = 99,
	_DXGI_FORMAT_AYUV = 100,
	_DXGI_FORMAT_Y410 = 101,
	_DXGI_FORMAT_Y416 = 102,
	_DXGI_FORMAT_NV12 = 103,
	_DXGI_FORMAT_P010 = 104,
	_DXGI_FORMAT_P016 = 105,
	_DXGI_FORMAT_420_OPAQUE = 106,
	_DXGI_FORMAT_YUY2 = 107,
	_DXGI_FORMAT_Y210 = 108,
	_DXGI_FORMAT_Y216 = 109,
	_DXGI_FORMAT_NV11 = 110,
	_DXGI_FORMAT_AI44 = 111,
	_DXGI_FORMAT_IA44 = 112,
	_DXGI_FORMAT_P8 = 113,
	_DXGI_FORMAT_A8P8 = 114,
	_DXGI_FORMAT_B4G4R4A4_UNORM = 115,
	_DXGI_FORMAT_FORCE_UINT = 0xffffffff
} _DXGI_FORMAT;

#define CMEventType MEReservedMax + 5678
#define CMSnapTriggerEvent CMEventType + 1

#ifdef _DEBUG_CAPTUREMANAGER
#define LOG_INVOKE_MF_FUNCTION(Function, ...) lresult = MediaFoundation::MediaFoundationManager::Function(__VA_ARGS__);\
	if (FAILED(lresult))\
			{\
	LogPrintOut::getInstance().printOutln(\
		LogPrintOut::ERROR_LEVEL,\
		#Function,\
		L" Error code: ",\
		lresult);\
	LogPrintOut::getInstance().printOutln(\
		LogPrintOut::ERROR_LEVEL,\
		L" Line: ", (DWORD)__LINE__, L", Func: ", __FUNCTIONW__,\
		L" Error code: ",\
		lresult);\
		break;\
		}\

#else
#define LOG_INVOKE_MF_FUNCTION(Function, ...)lresult = MediaFoundation::MediaFoundationManager::Function(__VA_ARGS__);\
	if (FAILED(lresult))\
			{\
		break;\
		}\

#endif


#ifdef _DEBUG_CAPTUREMANAGER
#define LOG_INVOKE_MF_METHOD(Function, ...) lresult = MediaFoundation::MediaFoundationManager::Function(__VA_ARGS__);\
	if (FAILED(lresult))\
				{\
	LogPrintOut::getInstance().printOutln(\
		LogPrintOut::ERROR_LEVEL,\
		L" Line: ", (DWORD)__LINE__, L", Func: ", __FUNCTIONW__,\
		L" Error code: ",\
		lresult);\
		break;\
			}\

#else
#define LOG_INVOKE_MF_METHOD(Function, ...) lresult = MediaFoundation::MediaFoundationManager::Function(__VA_ARGS__);\
	if (FAILED(lresult))break;\

#endif


namespace CaptureManager
{
	namespace Core
	{
		namespace MediaFoundation
		{

			typedef HRESULT(STDAPICALLTYPE *MFStartup)(
				ULONG,
				DWORD);

			typedef HRESULT(STDAPICALLTYPE *MFCreateMediaType)(
				IMFMediaType**);


			typedef HRESULT(STDAPICALLTYPE *MFGetStrideForBitmapInfoHeader)(
				DWORD,
				DWORD,
				LONG*);

			typedef HRESULT(STDAPICALLTYPE *MFCalculateImageSize)(
				REFGUID,
				UINT32,
				UINT32,
				UINT32*);

			typedef HRESULT(STDAPICALLTYPE *MFCreateVideoMediaTypeFromBitMapInfoHeaderEx)(
				const BITMAPINFOHEADER*,
				UINT32,
				DWORD,
				DWORD,
				MFVideoInterlaceMode,
				QWORD,
				DWORD,
				DWORD,
				DWORD,
				IMFVideoMediaType**);

			typedef HRESULT(STDAPICALLTYPE *MFInitAMMediaTypeFromMFMediaType)(
				IMFMediaType*,
				GUID,
				AM_MEDIA_TYPE*);

			typedef HRESULT(STDAPICALLTYPE *MFPutWorkItem)(
				DWORD,
				IMFAsyncCallback *,
				IUnknown *);

			typedef HRESULT(STDAPICALLTYPE *MFInitMediaTypeFromWaveFormatEx)(
				IMFMediaType*,
				const WAVEFORMATEX*,
				UINT32);

			typedef HRESULT(STDAPICALLTYPE *MFCreateTopologyNode)(
				MF_TOPOLOGY_TYPE,
				IMFTopologyNode **);

			typedef HRESULT(STDAPICALLTYPE *MFTEnumEx)(
				GUID,
				UINT32,
				const MFT_REGISTER_TYPE_INFO*,
				const MFT_REGISTER_TYPE_INFO*,
				IMFActivate***,
				UINT32*);

			typedef HRESULT(STDAPICALLTYPE *MFTGetInfo)(
				CLSID,
				LPWSTR*,
				MFT_REGISTER_TYPE_INFO**,
				UINT32*,
				MFT_REGISTER_TYPE_INFO**,
				UINT32*,
				IMFAttributes**);

			typedef HRESULT(STDAPICALLTYPE *MFCreateAttributes)(
				IMFAttributes**,
				UINT32);

			typedef HRESULT(STDAPICALLTYPE *MFEnumDeviceSources)(
				IMFAttributes*,
				IMFActivate***,
				UINT32*);

			typedef HRESULT(STDAPICALLTYPE *MFCreateDeviceSourceActivate)(
				IMFAttributes*,
				IMFActivate**);

			typedef HRESULT(STDAPICALLTYPE *MFCreateTopology)(
				IMFTopology **);

			typedef HRESULT(STDAPICALLTYPE *MFCreateVideoSampleFromSurface)(
				IUnknown*,
				IMFSample**);

			typedef HRESULT(STDAPICALLTYPE *MFCreateDXSurfaceBuffer)(
				REFIID,
				IUnknown *,
				BOOL,
				IMFMediaBuffer **);

			typedef HRESULT(STDAPICALLTYPE *MFCreateEventQueue)(
				IMFMediaEventQueue **);

			typedef HRESULT(STDAPICALLTYPE *MFCreatePresentationClock)(
				IMFPresentationClock**);

			typedef HRESULT(STDAPICALLTYPE *MFCreateSystemTimeSource)(
				IMFPresentationTimeSource**);

			typedef HRESULT(STDAPICALLTYPE *MFCreateMediaEvent)(
				MediaEventType,
				REFGUID,
				HRESULT,
				const PROPVARIANT *,
				IMFMediaEvent **);

			typedef HRESULT(STDAPICALLTYPE *MFAllocateWorkQueueEx)(
				_MFASYNC_WORKQUEUE_TYPE,
				DWORD *);

			typedef HRESULT(STDAPICALLTYPE *MFUnlockWorkQueue)(
				DWORD);

			typedef HRESULT(STDAPICALLTYPE *MFCreateStreamDescriptor)(
				DWORD,
				DWORD,
				IMFMediaType**,
				IMFStreamDescriptor**);

			typedef HRESULT(STDAPICALLTYPE *MFCreatePresentationDescriptor)(
				DWORD,
				IMFStreamDescriptor**,
				IMFPresentationDescriptor**);

			typedef HRESULT(STDAPICALLTYPE *MFCreateSampleGrabberSinkActivate)(
				IMFMediaType *,
				IMFSampleGrabberSinkCallback*,
				IMFActivate**);

			typedef HRESULT(STDAPICALLTYPE *MFCreateVideoRendererActivate)(
				HWND hwndVideo,
				IMFActivate**);

			typedef HRESULT(STDAPICALLTYPE *MFCreateAudioRendererActivate)(
				IMFActivate**);
			
			typedef HRESULT(STDAPICALLTYPE *MFCreateASFContentInfo)(
				IMFASFContentInfo**);

			typedef HRESULT(STDAPICALLTYPE *MFCreateASFProfile)(
				IMFASFProfile**);

			typedef HRESULT(STDAPICALLTYPE *MFCreateASFMediaSinkActivate)(
				LPCWSTR,
				IMFASFContentInfo*,
				IMFActivate**);

			typedef HRESULT(STDAPICALLTYPE *MFCreateASFStreamingMediaSinkActivate)(
				IMFActivate*,
				IMFASFContentInfo*,
				IMFActivate**);

			typedef HRESULT(STDAPICALLTYPE *MFCreateWaveFormatExFromMFMediaType)(
				IMFMediaType*,
				WAVEFORMATEX**,
				UINT32*,
				UINT32);

			typedef HRESULT(STDAPICALLTYPE *MFCreateSample)(
				IMFSample**);

			typedef HRESULT(STDAPICALLTYPE *MFCreateMemoryBuffer)(
				DWORD,
				IMFMediaBuffer**);

			typedef HRESULT(STDAPICALLTYPE *MFShutdown)();

			typedef HRESULT(STDAPICALLTYPE *MFCreateAudioMediaType)(
				const WAVEFORMATEX*,
				IMFAudioMediaType**);

			typedef HRESULT(STDAPICALLTYPE *MFCopyImage)(
				BYTE*,
				LONG,
				const BYTE*,
				LONG,
				DWORD,
				DWORD);

			typedef HRESULT(STDAPICALLTYPE *MFCreateMediaSession)(
				IMFAttributes* pConfiguration,
				_Outptr_ IMFMediaSession** ppMediaSession
				);

			typedef HRESULT(STDAPICALLTYPE *MFCreateMPEG4MediaSink)(
				IMFByteStream* pIByteStream,
				IMFMediaType* pVideoMediaType,
				IMFMediaType* pAudioMediaType,
				IMFMediaSink** ppIMediaSink
				);

			typedef HRESULT(STDAPICALLTYPE *MFCreateFMPEG4MediaSink)(
				IMFByteStream* pIByteStream,
				IMFMediaType* pVideoMediaType,
				IMFMediaType* pAudioMediaType,
				IMFMediaSink** ppIMediaSink
				);

			typedef HRESULT(STDAPICALLTYPE *MFCreateFile)(
				MF_FILE_ACCESSMODE  AccessMode,
				MF_FILE_OPENMODE    OpenMode,
				MF_FILE_FLAGS       fFlags,
				LPCWSTR             pwszFileURL,
				IMFByteStream       **ppIByteStream
				);

			typedef HRESULT(STDAPICALLTYPE *MFLockWorkQueue)(
				DWORD dwWorkQueue
				);

			typedef HRESULT(STDAPICALLTYPE *MFLockSharedWorkQueue)(
				PCWSTR wszClass,
				LONG   BasePriority,
				DWORD  *pdwTaskId,
				DWORD  *pID
				);

			typedef HRESULT(STDAPICALLTYPE *MFCreateSampleCopierMFT)(
				IMFTransform **ppCopierMFT);

			typedef HRESULT(STDAPICALLTYPE *MFAllocateSerialWorkQueue)(
				DWORD,
				DWORD *);

			typedef MFTIME(STDAPICALLTYPE *MFGetSystemTime)();

			typedef HRESULT(STDAPICALLTYPE *MFInitMediaTypeFromAMMediaType)(
				IMFMediaType*,
				const AM_MEDIA_TYPE*);

			typedef HRESULT(STDAPICALLTYPE *MFFrameRateToAverageTimePerFrame)(
				UINT32,
				UINT32,
				UINT64*);
			
			typedef HRESULT(STDAPICALLTYPE *MFCreateAsyncResult)(
				IUnknown*,
				IMFAsyncCallback*,
				IUnknown*,
				IMFAsyncResult**);

			typedef HRESULT(STDAPICALLTYPE *MFPutWaitingWorkItem)(
				HANDLE,
				LONG,
				IMFAsyncResult*,
				MFWORKITEM_KEY*);

			typedef HRESULT(STDAPICALLTYPE *MFCancelWorkItem)(
				MFWORKITEM_KEY);

			typedef HRESULT(STDAPICALLTYPE *MFCreateVideoSampleAllocatorEx)(
				REFIID,
				void**);

			typedef HRESULT(STDAPICALLTYPE *MFCreateVideoSampleAllocator)(
				REFIID,
				void**);

			typedef HRESULT(STDAPICALLTYPE *MFCreateDXGIDeviceManager)(
				UINT*,
				IMFDXGIDeviceManager**);

			typedef HRESULT(STDAPICALLTYPE *MFInitVideoFormat_RGB)(
				MFVIDEOFORMAT*,
				DWORD,
				DWORD,
				DWORD);

			typedef HRESULT(STDAPICALLTYPE *MFCreateVideoMediaType)(
				const MFVIDEOFORMAT*,
				IMFVideoMediaType**);

			typedef HRESULT(STDAPICALLTYPE *MFCreateDXGISurfaceBuffer)(
				REFIID,
				IUnknown*,
				UINT,
				BOOL,
				IMFMediaBuffer**);

			typedef _DXGI_FORMAT(STDAPICALLTYPE *MFMapDX9FormatToDXGIFormat)(
				DWORD);

			typedef DWORD(STDAPICALLTYPE *MFMapDXGIFormatToDX9Format)(
				_DXGI_FORMAT);

			typedef DWORD(STDAPICALLTYPE *MFGetService)(
				IUnknown*,
				REFGUID,
				REFIID,
				LPVOID*);

			typedef HRESULT(STDAPICALLTYPE *MFScheduleWorkItemEx)(
				IMFAsyncResult*,
				INT64,
				MFWORKITEM_KEY*);

			typedef HRESULT(STDAPICALLTYPE *MFRegisterPlatformWithMMCSS)(
				PCWSTR,
				DWORD*,
				LONG);

			typedef HRESULT(STDAPICALLTYPE *MFUnregisterPlatformFromMMCSS)();


#define REFPROPERTYKEY const PROPERTYKEY &

			class MediaFoundationManager
			{
			public:

				static MFCreateMediaType MFCreateMediaType;

				static MFGetStrideForBitmapInfoHeader MFGetStrideForBitmapInfoHeader;

				static MFCalculateImageSize MFCalculateImageSize;

				static MFCreateVideoMediaTypeFromBitMapInfoHeaderEx MFCreateVideoMediaTypeFromBitMapInfoHeaderEx;

				static MFInitAMMediaTypeFromMFMediaType MFInitAMMediaTypeFromMFMediaType;

				static MFPutWorkItem MFPutWorkItem;

				static MFInitMediaTypeFromWaveFormatEx MFInitMediaTypeFromWaveFormatEx;

				static MFCreateTopologyNode MFCreateTopologyNode;

				static MFTEnumEx MFTEnumEx;

				static MFTGetInfo MFTGetInfo;

				static MFCreateAttributes MFCreateAttributes;

				static MFEnumDeviceSources MFEnumDeviceSources;

				static MFCreateDeviceSourceActivate MFCreateDeviceSourceActivate;

				static MFCreateTopology MFCreateTopology;

				static MFCreateVideoSampleFromSurface MFCreateVideoSampleFromSurface;

				static MFCreateDXSurfaceBuffer MFCreateDXSurfaceBuffer;

				static MFCreateEventQueue MFCreateEventQueue;

				static MFCreatePresentationClock MFCreatePresentationClock;

				static MFCreateSystemTimeSource MFCreateSystemTimeSource;

				static MFCreateMediaEvent MFCreateMediaEvent;

				static MFAllocateWorkQueueEx MFAllocateWorkQueueEx;

				static MFUnlockWorkQueue MFUnlockWorkQueue;

				static MFCreateStreamDescriptor MFCreateStreamDescriptor;

				static MFCreatePresentationDescriptor MFCreatePresentationDescriptor;

				static MFCreateSampleGrabberSinkActivate MFCreateSampleGrabberSinkActivate;

				static MFCreateVideoRendererActivate MFCreateVideoRendererActivate;

				static MFCreateAudioRendererActivate MFCreateAudioRendererActivate;

				static MFCreateASFContentInfo MFCreateASFContentInfo;

				static MFCreateASFProfile MFCreateASFProfile;

				static MFCreateASFMediaSinkActivate MFCreateASFMediaSinkActivate;

				static MFCreateASFStreamingMediaSinkActivate MFCreateASFStreamingMediaSinkActivate;

				static MFCreateWaveFormatExFromMFMediaType MFCreateWaveFormatExFromMFMediaType;

				static MFCreateSample MFCreateSample;

				static MFCreateMemoryBuffer MFCreateMemoryBuffer;

				static MFShutdown MFShutdown;

				static MFCreateAudioMediaType MFCreateAudioMediaType;

				static MFCopyImage MFCopyImage;

				static MFCreateMediaSession MFCreateMediaSession;

				static MFCreateFMPEG4MediaSink MFCreateFMPEG4MediaSink;

				static MFCreateMPEG4MediaSink MFCreateMPEG4MediaSink;

				static MFCreateFile MFCreateFile;

				static MFLockWorkQueue MFLockWorkQueue;

				static MFLockSharedWorkQueue MFLockSharedWorkQueue;

				static MFCreateSampleCopierMFT MFCreateSampleCopierMFT;

				static MFAllocateSerialWorkQueue MFAllocateSerialWorkQueue;

				static MFGetSystemTime MFGetSystemTime;

				static MFInitMediaTypeFromAMMediaType MFInitMediaTypeFromAMMediaType;

				static MFFrameRateToAverageTimePerFrame MFFrameRateToAverageTimePerFrame;

				static MFCreateAsyncResult MFCreateAsyncResult;

				static MFPutWaitingWorkItem MFPutWaitingWorkItem;

				static MFCancelWorkItem MFCancelWorkItem;

				static MFCreateVideoSampleAllocatorEx MFCreateVideoSampleAllocatorEx;

				static MFCreateVideoSampleAllocator MFCreateVideoSampleAllocator;

				static MFCreateDXGIDeviceManager MFCreateDXGIDeviceManager;

				static MFInitVideoFormat_RGB MFInitVideoFormat_RGB;

				static MFCreateVideoMediaType MFCreateVideoMediaType;

				static MFCreateDXGISurfaceBuffer MFCreateDXGISurfaceBuffer;

				static MFMapDX9FormatToDXGIFormat MFMapDX9FormatToDXGIFormat;

				static MFMapDXGIFormatToDX9Format MFMapDXGIFormatToDX9Format;

				static MFGetService MFGetService;

				static MFScheduleWorkItemEx MFScheduleWorkItemEx;

				static MFRegisterPlatformWithMMCSS MFRegisterPlatformWithMMCSS;

				static MFUnregisterPlatformFromMMCSS MFUnregisterPlatformFromMMCSS;

			public:

				MediaFoundationManager();

				~MediaFoundationManager();

				HRESULT initialize();

				HRESULT unInitialize();

				static HRESULT GetStatus();

				static HRESULT DetachObject(
					IMFActivate* aPtrActivate);

				static HRESULT Shutdown(
					IMFMediaSource* aPtrMediaSource);

				static HRESULT Shutdown(
					IMFMediaSink* aPtrMediaSink);

				static HRESULT AddBuffer(
					IMFSample* aPtrSample,
					IMFMediaBuffer* aPtrlMediaBuffer);

				static HRESULT SetSampleDuration(
					IMFSample* aPtrSample,
					LONGLONG aSampleDuration);

				static HRESULT SetSampleTime(
					IMFSample* aPtrSample,
					LONGLONG aSampleTime);

				static HRESULT SetUINT64(
					IMFSample* aPtrSample,
					REFGUID aRefGUIDKey,
					UINT64 aValue);

				static HRESULT Clone(
					IMFPresentationDescriptor* aPtrOriginalPresentationDescriptor,
					IMFPresentationDescriptor** aPtrPtrPresentationDescriptor);

				static HRESULT GetLockWorkQueue(
					DWORD* aPtrID);

				static HRESULT SetGUID(
					IMFAttributes* aPtrAttributes,
					REFGUID aGUIDKey,
					REFGUID aGUIDValue);

				static HRESULT SetCurrentLength(
					IMFMediaBuffer* aPtrMediaBuffer,
					DWORD aValue);

				static HRESULT Lock(
					IMFMediaBuffer* aPtrMediaBuffer,
					BYTE** aPtrPtrBuffer,
					DWORD* aPtrMaxLength,
					DWORD* aPtrCurrentLength);

				static HRESULT Unlock(
					IMFMediaBuffer* aPtrMediaBuffer);

				static HRESULT GetGUID(
					IMFAttributes *aPtrAttributes,
					REFGUID aGUIDKey,
					GUID* aPtrGUIDValue);

				static HRESULT CopyAllItems(
					IMFAttributes *aPtrSourceAttributes,
					IMFAttributes *aPtrDestinationAttributes);

				static HRESULT DeleteItem(
					IMFAttributes *aPtrSourceAttributes,
					REFGUID aGUIDKey);



				static HRESULT SetUINT32(
					IMFAttributes* aPtrAttributes,
					REFGUID aGUIDKey,
					UINT32 aValue);

				static HRESULT GetUINT32(
					IMFAttributes* aPtrAttributes,
					REFGUID aRefGUIDKey,
					UINT32* aPtrValue);

				static HRESULT SetUINT64(
					IMFAttributes* aPtrAttributes,
					REFGUID aGUIDKey,
					UINT64 aValue);

				static HRESULT SetBlob(
					IMFAttributes* aPtrAttributes,
					REFGUID aGUIDKey,
					UINT8* aPtrBlob,
					UINT32 aBlobSize);

				static HRESULT GetUINT64(
					IMFAttributes* aPtrAttributes,
					REFGUID aGUIDKey,
					UINT64* aPtrValue);

				static HRESULT SetUnknown(
					IMFAttributes* aPtrAttributes,
					REFGUID aRefGUIDKey,
					IUnknown* aPtrUnknown);

				static HRESULT GetUnknown(
					IMFAttributes* aPtrAttributes,
					REFGUID aRefGUIDKey,
					REFIID aRefIID,
					void** aPtrPtrUnknown);

				static HRESULT SetString(
					IMFAttributes* aPtrAttributes,
					REFGUID aGUIDKey,
					LPCWSTR aPtrStringValue);

				static HRESULT GetAllocatedString(
					IMFAttributes* aPtrAttributes,
					REFGUID aGUIDKey,
					std::wstring& aStringValue);

				static HRESULT SetAttributeRatio(
					IMFAttributes* aPtrAttributes,
					REFGUID aGUIDKey,
					UINT32 aNumerator,
					UINT32 aDenominator);

				static HRESULT SetAttributeSize(
					IMFAttributes* aPtrAttributes,
					REFGUID aGUIDKey,
					UINT32 aWidth,
					UINT32 aHeight);

				static HRESULT GetAttributeSize(
					IMFAttributes* aPtrAttributes,
					REFGUID aGUIDKey,
					UINT32& aRefWidth,
					UINT32& aRefHeight);

				static HRESULT GetItem(
					IMFAttributes* aPtrAttributes,
					REFGUID aGUIDKey,
					PROPVARIANT* aPtrVarItem);

				static HRESULT SetItem(
					IMFAttributes* aPtrAttributes,
					REFGUID aGUIDKey,
					PROPVARIANT& lVarItem);

				static HRESULT CompareItem(
					IMFAttributes* aPtrAttributes,
					REFGUID aGUIDKey,
					REFPROPVARIANT aRefValue,
					BOOL* aPtrResult);

				static HRESULT isCompressedFormat(
					IMFMediaType* aPtrAttributes,
					BOOL& aRefResult);

				static HRESULT GetSource(
					IMFActivate* aPtrSourceActivate,
					IMFMediaSource** aPtrPtrMediaSource);

				static HRESULT ActivateObject(
					IMFActivate* aPtrActivate,
					REFIID aRefIID,
					void** aPtrPtrObject);

				static HRESULT SetInputType(
					IMFTransform* aPtrTransform,
					DWORD aInputStreamID,
					IMFMediaType* aPtrType,
					DWORD aFlags);

				static HRESULT GetStreamSinkById(
					IMFMediaSink* aPtrPtrMediaSink,
					DWORD aStreamSinkIdentifier,
					IMFStreamSink** aPtrPtrStreamSink);

				static HRESULT GetStreamSinkByIndex(
					IMFMediaSink* aPtrPtrMediaSink,
					DWORD aIndex,
					IMFStreamSink** aPtrPtrStreamSink);

				static HRESULT AddStreamSink(
					IMFMediaSink* aPtrPtrMediaSink,
					DWORD aStreamSinkIdentifier,
					IMFMediaType* aPtrMediaType,
					IMFStreamSink** aPtrPtrStreamSink);

				static HRESULT CreatePresentationDescriptor(
					IMFMediaSource* aPtrMediaSource,
					IMFPresentationDescriptor** aPtrPtrPresentationDescriptor);

				static HRESULT GetStreamDescriptorCount(
					IMFPresentationDescriptor* aPtrPresentationDescriptor,
					DWORD* aPtrStreamCount);

				static HRESULT GetStreamDescriptorByIndex(
					IMFPresentationDescriptor* aPtrPD,
					DWORD aIndex,
					BOOL* aPtrIsSelected,
					IMFStreamDescriptor** aPtrPtrSD);

				static HRESULT GetStreamDescriptor(
					IMFMediaStream* aPtrIMFMediaStream,
					IMFStreamDescriptor** aPtrPtrSD);
				
				static HRESULT GetStreamIdentifier(
					IMFStreamDescriptor* aPtrIMFStreamDescriptor,
					DWORD* aPtrStreamIdentifier);

				static HRESULT SelectStream(
					IMFPresentationDescriptor* aPtrPD,
					int aIndexStream);

				static HRESULT SetCurrentMediaType(
					IMFStreamDescriptor* aPtrSD,
					int aIndexMediaType);

				static HRESULT IsEqual(
					IMFMediaType* aPtrThisMediaType,
					IMFMediaType* aPtrIMediaType,
					DWORD* aPtrFlags);

				static HRESULT GetCurrentLength(
					IMFMediaBuffer* aPtrMediaBuffer,
					DWORD* aPtrCurrentLength);

				static HRESULT SetCurrentMediaType(
					IMFMediaTypeHandler* aMediaTypeHandler,
					IMFMediaType* aPtrMediaType);

				static HRESULT GetMediaTypeHandler(
					IMFStreamDescriptor* aPtrSD,
					IMFMediaTypeHandler** aPtrPtrMediaTypeHandler);

				static HRESULT GetMediaTypeHandler(
					IMFStreamSink* aPtrStreamSink,
					IMFMediaTypeHandler** aPtrPtrMediaTypeHandler);

				static HRESULT GetMajorType(
					IMFMediaTypeHandler* aPtrMediaTypeHandler,
					GUID* aPtrGUIDMajorType);

				static HRESULT GetMajorType(
					IMFMediaType* aPtrMediaType,
					GUID* aPtrGUIDMajorType);

				static HRESULT GetMediaTypeCount(
					IMFMediaTypeHandler* aPtrMediaTypeHandler,
					DWORD* aPtrMediaTypeCount);

				static HRESULT GetMediaTypeByIndex(
					IMFMediaTypeHandler* aPtrMediaTypeHandler,
					DWORD aMediaTypeIndex,
					IMFMediaType** aPtrPtrMediaType);

				static HRESULT LockStore(
					IMFAttributes* aPtrAttributes);

				static HRESULT UnlockStore(
					IMFAttributes* aPtrAttributes);

				static HRESULT GetCount(
					IMFAttributes* aPtrAttributes,
					UINT32* aPtrItemCount);

				static HRESULT GetItemByIndex(
					IMFAttributes* aPtrAttributes,
					UINT32 aIndex,
					GUID* aPtrGUIDKey,
					PROPVARIANT* aPtrValue);

				static HRESULT GetAttributes(
					IMFTransform* aPtrTransform,
					IMFAttributes** aPtrPtrAttributes);

				static HRESULT enumMediaTypes(
					IMFStreamDescriptor* aPtrSD,
					pugi::xml_node& aRefStreamDescriptorNode);

				static HRESULT SetObject(
					IMFTopologyNode* aPtrTopologyNode,
					IUnknown* aPtrObject);

				static HRESULT ProcessMessage(
					IMFTransform* aPtrIMFTransform,
					MFT_MESSAGE_TYPE aMessage, 
					ULONG_PTR aParam);

				static HRESULT GetObject(
					IMFTopologyNode* aPtrTopologyNode,
					IUnknown** aPtrPtrObject);

				static HRESULT GetEvent(
					IMFMediaEventQueue* aPtrIMFMediaEventQueue,
					DWORD aFlags,
					IMFMediaEvent ** aPtrPtrEvent);

				static HRESULT BeginGetEvent(
					IMFMediaEventQueue* aPtrIMFMediaEventQueue,
					IMFAsyncCallback *aPtrCallback,
					IUnknown *aPtrUnkState);

				static HRESULT EndGetEvent(
					IMFMediaEventQueue* aPtrIMFMediaEventQueue,
					IMFAsyncResult *aPtrResult,
					IMFMediaEvent ** aPtrPtrEvent);

				static HRESULT QueueEventParamVar(
					IMFMediaEventQueue* aPtrIMFMediaEventQueue, 
					MediaEventType aMediaEventType,
					REFGUID aGUIDExtendedType,
					HRESULT aHRStatus,
					const PROPVARIANT * aPtrValue);
				
				static HRESULT QueueEventParamUnk(
					IMFMediaEventQueue* aPtrIMFMediaEventQueue,
					MediaEventType aMediaEventType,
					REFGUID aRefGUIDExtendedType,
					HRESULT aHRStatus,
					IUnknown* aPtrUnk);

				static HRESULT GetState(
					IMFAsyncResult* aPtrIMFAsyncResult,
					IUnknown ** aPtrPtrUnkState);

				static HRESULT EndGetEvent(
					IMFMediaEventGenerator* aPtrMediaEventGenerator,
					IMFAsyncResult* aPtrAsyncResult,
					IMFMediaEvent** aPtrPtrMediaEvent);

				static HRESULT BeginGetEvent(
					IMFMediaEventGenerator* aPtrMediaEventGenerator,
					IMFAsyncCallback* aPtrAsyncCallback,
					IUnknown* aPtrUnkState);

				static HRESULT GetType(
					IMFMediaEvent* aPtrMediaEvent,
					MediaEventType* aPtrMediaEventType);

				static HRESULT GetExtendedType(
					IMFMediaEvent* aPtrMediaEvent,
					GUID& aRefGUIDExtendedType);

				static HRESULT GetStatus(
					IMFMediaEvent* aPtrMediaEvent,
					HRESULT * aPtrStatus);

				static HRESULT GetStatus(
					IMFAsyncResult* aPtrAsyncResult);

				static HRESULT GetValue(
					IMFMediaEvent* aPtrMediaEvent,
					PROPVARIANT* aPtrValue);

				static HRESULT AddNode(
					IMFTopology* aPtrTopology,
					IMFTopologyNode* aPtrTopologyNode);

				static HRESULT ConnectOutput(
					IMFTopologyNode* aPtrSourceTopologyNode,
					DWORD aOutputIndex,
					IMFTopologyNode* aPtrOutputTopologyNode,
					DWORD aIntputIndex);

				static HRESULT SetTopology(
					IMFMediaSession* aPtrMediaSession,
					DWORD aSetTopologyFlags,
					IMFTopology* aPtrTopology);

				static HRESULT Start(
					IMFMediaSession* aPtrMediaSession,
					const GUID* aPtrGUIDTimeFormat,
					const PROPVARIANT *aPtrVarStartPosition);

				static HRESULT Start(
					IMFMediaSource* aPtrMediaSource,
					IMFPresentationDescriptor* aPtrPresentationDescriptor,
					const GUID* aPtrguidTimeFormat,
					const PROPVARIANT* aPtrvarStartPosition);

				static HRESULT Pause(
					IMFMediaSession* aPtrMediaSession);

				static HRESULT Pause(
					IMFMediaSource* aPtrMediaSource);

				static HRESULT Stop(
					IMFMediaSession* aPtrMediaSession);

				static HRESULT Stop(
					IMFMediaSource* aPtrMediaSource);

				static HRESULT GetOutputCount(
					IMFTopologyNode* aPtrTopologyNode,
					DWORD* aPtrOutputNodeCount);

				static HRESULT GetOutput(
					IMFTopologyNode* aPtrUpstreamNode,
					DWORD aOutputNodeIndex,
					IMFTopologyNode** aPtrPtrDownstreamNode,
					DWORD* aPtrInputIndexOnDownstreamNode);

				static HRESULT SetOutputPrefType(
					IMFTopologyNode* aPtrUpstreamNode,
					DWORD dwOutputIndex,
					IMFMediaType *pType);

				static HRESULT GetOutputPrefType(
					IMFTopologyNode* aPtrUpstreamNode,
					DWORD dwOutputIndex,
					IMFMediaType **ppType);

				static HRESULT SetInputPrefType(
					IMFTopologyNode* aPtrUpstreamNode,
					DWORD dwInputIndex,
					IMFMediaType *pType);

				static HRESULT GetInputPrefType(
					IMFTopologyNode* aPtrUpstreamNode,
					DWORD dwInputIndex,
					IMFMediaType **ppType);

				static HRESULT CloneFrom(
					IMFTopologyNode* aPtrUpstreamNode,
					IMFTopologyNode *pNode);

				static HRESULT CreateStream(
					IMFASFProfile* aPtrASFProfile,
					IMFMediaType* aPtrMediaType,
					IMFASFStreamConfig** aPtrPtrASFContentInfo);

				static HRESULT SetStreamNumber(
					IMFASFStreamConfig* aPtrASFContentInfo,
					WORD aStreamNumber);

				static HRESULT SetStream(
					IMFASFProfile* aPtrASFProfile,
					IMFASFStreamConfig* aPtrASFContentInfo);

				static HRESULT GetEncodingConfigurationPropertyStore(
					IMFASFContentInfo* aPtrASFContentInfo,
					WORD aStreamNumber,
					IPropertyStore** aPtrPtrASFPropertyStore);

				static HRESULT SetValue(
					IPropertyStore* aPtrPropertyStore,
					REFPROPERTYKEY aKey,
					REFPROPVARIANT aValue);

				static HRESULT SetProfile(
					IMFASFContentInfo* aPtrASFContentInfo,
					IMFASFProfile* aPtrASFProfile);

				static HRESULT GetSourceNodeCollection(
					IMFTopology* aPtrTopology,
					IMFCollection** aPtrPtrNodeCollection);

				static HRESULT GetOutputNodeCollection(
					IMFTopology* aPtrTopology,
					IMFCollection** aPtrPtrNodeCollection);

				static HRESULT GetElementCount(
					IMFCollection* aPtrElementCollection,
					DWORD* aPtrCount);

				static HRESULT GetElement(
					IMFCollection* aPtrElementCollection,
					DWORD aElementIndex,
					IUnknown** aPtrPtrElement);

				static HRESULT GetCurrentMediaType(
					IMFMediaTypeHandler* aPtrMediaTypeHandler,
					IMFMediaType** aPtrPtrMediaType);

				static HRESULT GetNodeType(
					IMFTopologyNode* aPtrTopologyNode,
					MF_TOPOLOGY_TYPE* aPtrTopologyType);

				static HRESULT GetInputAvailableType(
					IMFTransform* aPtrTransform,
					DWORD aInputStreamID,
					DWORD aTypeIndex,
					IMFMediaType** aPtrPtrType);

				static HRESULT GetOutputAvailableType(
					IMFTransform* aPtrTransform,
					DWORD aOutputStreamID,
					DWORD aTypeIndex,
					IMFMediaType** aPtrPtrType);

				static HRESULT GetInputStreamInfo(
					IMFTransform* aPtrTransform,
					DWORD aInputStreamID,
					MFT_INPUT_STREAM_INFO* aPtrStreamInfo);

				static HRESULT GetOutputCurrentType(
					IMFTransform* aPtrTransform,
					DWORD aOutputStreamID,
					IMFMediaType** aPtrPtrType);

				static HRESULT SetOutputType(
					IMFTransform* aPtrTransform,
					DWORD aOutputStreamID,
					IMFMediaType* aPtrType,
					DWORD aFlag);

				static HRESULT ProcessInput(
					IMFTransform* aPtrTransform,
					DWORD aInputStreamID,
					IMFSample* aPtrSample,
					DWORD aFlags);

				static HRESULT ProcessOutput(
					IMFTransform* aPtrTransform,
					DWORD aFlags,
					DWORD aOutputBufferCount,
					MFT_OUTPUT_DATA_BUFFER* aPtrOutputSamples,
					DWORD* aPtrStatus);

				static HRESULT ConvertToContiguousBuffer(
					IMFSample* aPtrSample,
					IMFMediaBuffer** aPtrPtrBuffer);

				static HRESULT CopyToBuffer(
					IMFSample* aPtrSample,
					IMFMediaBuffer* aPtrBuffer);

				static HRESULT GetTotalLength(
					IMFSample* aPtrSample,
					DWORD* aPtrTotalLength);

				static HRESULT GetSampleTime(
					IMFSample* aPtrSample,
					LONGLONG* aPtrPtrSampleTime);

				static HRESULT GetSampleDuration(
					IMFSample* aPtrSample,
					LONGLONG* aPtrPtrSampleDuration);

				static HRESULT GetBufferByIndex(
					IMFSample* aPtrSample,
					DWORD aIndex,
					IMFMediaBuffer** aPtrPtrBuffer);

				static HRESULT GetContiguousLength(
					IMF2DBuffer* aPtr2DBuffer,
					DWORD* aPtrLength);

				static HRESULT ContiguousCopyFrom(
					IMF2DBuffer* aPtr2DBuffer,
					const BYTE * aPtrSrcBuffer,
					DWORD aSrcBufferLength);

				static HRESULT ContiguousCopyTo(
					IMF2DBuffer* aPtr2DBuffer,
					BYTE * aPtrDestBuffer,
					DWORD aDestBuffer);

				static HRESULT GetInputCurrentType(
					IMFTransform* aPtrTransform,
					DWORD aOutputStreamID,
					IMFMediaType** aPtrPtrType);

				static HRESULT Compare(
					IMFAttributes* aPtrAttributes,
					IMFAttributes* aPtrTheirs,
					MF_ATTRIBUTES_MATCH_TYPE aMatchType,
					BOOL* aPtrResult);

				static HRESULT SetTimeSource(
					IMFPresentationClock* aPtrPresentationClock,
					IMFPresentationTimeSource* aPtrTimeSource);

				static HRESULT SetPresentationClock(
					IMFMediaSink* aPtrMediaSink,
					IMFPresentationClock* aPtrPresentationClock);

				static HRESULT Start(
					IMFPresentationClock* aPtrPresentationClock,
					LONGLONG aClockStartOffset);

				static HRESULT GetInput(
					IMFTopologyNode* aPtrDownStreamTopologyNode,
					DWORD aInputIndex,
					IMFTopologyNode** aPtrPtrUpStreamTopologyNode,
					DWORD* aPtrOutputIndexOnUpStreamNode);
				
				static HRESULT DisconnectOutput(
					IMFTopologyNode* aPtrUpStreamTopologyNode,
					DWORD aOutputIndex);


				static HRESULT Shutdown(
					IMFMediaSession* aPtrMediaSession);

				static HRESULT Shutdown(
					IMFMediaEventQueue*  aPtrEventQueue);

				static HRESULT QueueEvent(
					IMFMediaEventGenerator* aPtrMediaEventGenerator,
					MediaEventType aMediaEventType,
					REFGUID aRefGUID,
					HRESULT aHRESULT,
					const PROPVARIANT* aPtrPropVariantValue);

				static HRESULT QueueEvent(
					IMFMediaEventQueue* aPtrMediaEventQueue,
					IMFMediaEvent* aPtrEvent);

				static HRESULT GetTopoNodeID(
					IMFTopologyNode* aPtrTopologyNode,
					TOPOID* aPtrTOPOI);

				static HRESULT SetTopoNodeID(
					IMFTopologyNode* aPtrTopologyNode,
					TOPOID ullTopoID);

				static HRESULT GetNodeByID(
					IMFTopology* aPtrTopology,
					TOPOID aTopoNodeID,
					IMFTopologyNode** aPtrPtrTopologyNode);

				static HRESULT GetNodeCount(
					IMFTopology* aPtrTopology,
					WORD* aPtrTopoNodeCount);

				static HRESULT GetNode(
					IMFTopology* aPtrTopology,
					WORD aTopoNodeIndex,
					IMFTopologyNode** aPtrPtrTopologyNode);

				static HRESULT pause(
					IMFMediaSource* aPtrMediaSource);

				static HRESULT BeginFinalize(
					IMFFinalizableMediaSink* aPtrFinalizableMediaSink,
					IMFAsyncCallback* aPtrCallback,
					IUnknown* aPtrUnkState);

				static HRESULT EndFinalize(
					IMFFinalizableMediaSink* aPtrFinalizableMediaSink,
					IMFAsyncResult* aPtrResult);

				static HRESULT GetOutputStreamInfo(
					IMFTransform* aPtrIMFTransform,
					DWORD aOutputStreamID,
					MFT_OUTPUT_STREAM_INFO* aPtrStreamInfo);

				static HRESULT GetIdentifier(
					IMFStreamSink* aPtrStreamSink,
					DWORD* aPtrIdentifier);

				static HRESULT ProcessSample(
					IMFStreamSink* aPtrStreamSink,
					IMFSample* aPtrSample);

				static HRESULT AddClockStateSink(
					IMFPresentationClock* aPtrPresentationClock,
					IMFClockStateSink* aPtrPtrStateSink);							
				
				static HRESULT RemoveClockStateSink(
					IMFPresentationClock* aPtrPresentationClock,
					IMFClockStateSink* aPtrPtrStateSink);

				static HRESULT GetService(
					IMFGetService* aPtrGetService,
					REFGUID aRefGUIDService,
					REFIID aRefIID,
					LPVOID* aPtrPtrObject);
				
				static HRESULT SetVideoWindow(
					IMFVideoDisplayControl* aPtrVideoDisplayControl,
					HWND aHWNDVideo);

				static HRESULT GetCharacteristics(
					IMFMediaSource* aPtrMediaSource,
					DWORD* aPtrCharacteristics);

				static HRESULT InitializeSampleAllocatorEx(
					IMFVideoSampleAllocatorEx*  aPtrIMFVideoSampleAllocatorEx,
					DWORD aInitialSamples,
					DWORD aMaximumSamples,
					IMFAttributes* aPtrAttributes,
					IMFMediaType* aPtrMediaType);

				static HRESULT GetInputCount(
					IMFTopologyNode* aPtrDownStreamTopologyNode,
					DWORD* aPtrInputs);

				static HRESULT GetStreamSinkCount(
					IMFMediaSink* aPtrMediaSink,
					DWORD* aPtrStreamSinkCount);

				static HRESULT GetBufferCount(
					IMFSample* aPtrSample,
					DWORD *pdwBufferCount);

				static HRESULT GetPresentationClock(
					IMFMediaSink* aPtrIMFMediaSink,
					IMFPresentationClock ** aPtrPtrPresentationClock);

				static HRESULT GetCorrelatedTime(
					IMFPresentationClock* aPtrPresentationClock,
					DWORD aReserved,
					LONGLONG* aPtrClockTime,
					MFTIME* aPtrSystemTime);
				
				static HRESULT GetBlobSize(
					IMFAttributes* aPtrIMFAttributes,
					REFGUID aGUIDKey,
					UINT32* aPtrBlobSize);

				static HRESULT GetBlob(
					IMFAttributes* aPtrIMFAttributes,
					REFGUID aGUIDKey,
					UINT8* aPtrBuffer,
					UINT32 aBufferSize,
					UINT32* aPtrBlobSize);
				
			private:

				static HRESULT mResult;

				static std::vector<HMODULE> mModules;

				static MFStartup MFStartup;

				HRESULT loadLibraries();

				HRESULT initFunctions(HMODULE aModule);

				HRESULT fillPtrFuncCollection();

				static HRESULT STDAPICALLTYPE stubMFStartup(
					ULONG,
					DWORD){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFCreateMediaType(
					IMFMediaType**){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFGetStrideForBitmapInfoHeader(
					DWORD,
					DWORD,
					LONG*){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFCalculateImageSize(
					REFGUID,
					UINT32,
					UINT32,
					UINT32*){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFCreateVideoMediaTypeFromBitMapInfoHeaderEx(
					const BITMAPINFOHEADER*,
					UINT32,
					DWORD,
					DWORD,
					MFVideoInterlaceMode,
					QWORD,
					DWORD,
					DWORD,
					DWORD,
					IMFVideoMediaType**){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFInitAMMediaTypeFromMFMediaType(
					IMFMediaType*,
					GUID,
					AM_MEDIA_TYPE*){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFPutWorkItem(
					DWORD,
					IMFAsyncCallback *,
					IUnknown *){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFInitMediaTypeFromWaveFormatEx(
					IMFMediaType*,
					const WAVEFORMATEX*,
					UINT32){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFCreateTopologyNode(
					MF_TOPOLOGY_TYPE,
					IMFTopologyNode **){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFTEnumEx(
					GUID,
					UINT32,
					const MFT_REGISTER_TYPE_INFO*,
					const MFT_REGISTER_TYPE_INFO*,
					IMFActivate***,
					UINT32*){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFTGetInfo(
					CLSID,
					LPWSTR*,
					MFT_REGISTER_TYPE_INFO**,
					UINT32*,
					MFT_REGISTER_TYPE_INFO**,
					UINT32*,
					IMFAttributes**){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFCreateAttributes(
					IMFAttributes**,
					UINT32){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFEnumDeviceSources(
					IMFAttributes*,
					IMFActivate***,
					UINT32*){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFCreateDeviceSourceActivate(
					IMFAttributes*,
					IMFActivate**){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFCreateTopology(
					IMFTopology **){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFCreateVideoSampleFromSurface(
					IUnknown*,
					IMFSample**){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFCreateDXSurfaceBuffer(
					REFIID,
					IUnknown *,
					BOOL,
					IMFMediaBuffer **){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFCreateEventQueue(
					IMFMediaEventQueue **){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFCreatePresentationClock(
					IMFPresentationClock**){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFCreateSystemTimeSource(
					IMFPresentationTimeSource**){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFCreateMediaEvent(
					MediaEventType,
					REFGUID,
					HRESULT,
					const PROPVARIANT *,
					IMFMediaEvent **){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFAllocateWorkQueueEx(
					_MFASYNC_WORKQUEUE_TYPE,
					DWORD *){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFUnlockWorkQueue(
					DWORD){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFCreateStreamDescriptor(
					DWORD,
					DWORD,
					IMFMediaType**,
					IMFStreamDescriptor**){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFCreatePresentationDescriptor(
					DWORD,
					IMFStreamDescriptor**,
					IMFPresentationDescriptor**){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFCreateSampleGrabberSinkActivate(
					IMFMediaType *,
					IMFSampleGrabberSinkCallback*,
					IMFActivate**){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFCreateVideoRendererActivate(
					HWND hwndVideo,
					IMFActivate**){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFCreateAudioRendererActivate(
					IMFActivate**) {
					return E_NOTIMPL;
				}				

				static HRESULT STDAPICALLTYPE stubMFCreateASFContentInfo(
					IMFASFContentInfo**){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFCreateASFProfile(
					IMFASFProfile**){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFCreateASFMediaSinkActivate(
					LPCWSTR,
					IMFASFContentInfo*,
					IMFActivate**){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFCreateASFStreamingMediaSinkActivate(
					IMFActivate*,
					IMFASFContentInfo*,
					IMFActivate**){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFCreateWaveFormatExFromMFMediaType(
					IMFMediaType*,
					WAVEFORMATEX**,
					UINT32*,
					UINT32){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFCreateSample(
					IMFSample**){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFCreateMemoryBuffer(
					DWORD,
					IMFMediaBuffer**){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFShutdown(){ return E_NOTIMPL; }

				static HRESULT STDAPICALLTYPE stubMFCreateAudioMediaType(
					const WAVEFORMATEX*,
					IMFAudioMediaType**){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFCopyImage(
					BYTE*,
					LONG,
					const BYTE*,
					LONG,
					DWORD,
					DWORD){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFCreateMediaSession(
					IMFAttributes* pConfiguration,
					_Outptr_ IMFMediaSession** ppMediaSession
					){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFCreateMPEG4MediaSink(
					IMFByteStream* pIByteStream,
					IMFMediaType* pVideoMediaType,
					IMFMediaType* pAudioMediaType,
					IMFMediaSink** ppIMediaSink
					){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFCreateFMPEG4MediaSink(
					IMFByteStream* pIByteStream,
					IMFMediaType* pVideoMediaType,
					IMFMediaType* pAudioMediaType,
					IMFMediaSink** ppIMediaSink
					){
					return E_NOTIMPL;
				}



				static HRESULT STDAPICALLTYPE stubMFCreateFile(
					MF_FILE_ACCESSMODE  AccessMode,
					MF_FILE_OPENMODE    OpenMode,
					MF_FILE_FLAGS       fFlags,
					LPCWSTR             pwszFileURL,
					IMFByteStream       **ppIByteStream
					){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFLockWorkQueue(
					DWORD dwWorkQueue
					){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFLockSharedWorkQueue(
					PCWSTR wszClass,
					LONG   BasePriority,
					DWORD  *pdwTaskId,
					DWORD  *pID
					){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFCreateSampleCopierMFT(
					IMFTransform **ppCopierMFT){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFAllocateSerialWorkQueue(
					DWORD dwWorkQueue,
					DWORD * pdwWorkQueue){
					return E_NOTIMPL;
				}


				static MFTIME STDAPICALLTYPE stubMFGetSystemTime()
				{
					return 0;
				}

				static HRESULT STDAPICALLTYPE stubMFInitMediaTypeFromAMMediaType(
					IMFMediaType*,
					const AM_MEDIA_TYPE*){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFFrameRateToAverageTimePerFrame(
					UINT32,
					UINT32,
					UINT64*){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFCreateAsyncResult(
					IUnknown*,
					IMFAsyncCallback*,
					IUnknown*,
					IMFAsyncResult**){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFPutWaitingWorkItem(
					HANDLE,
					LONG,
					IMFAsyncResult*,
					MFWORKITEM_KEY*){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFCancelWorkItem(
					MFWORKITEM_KEY){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFCreateVideoSampleAllocatorEx(
					REFIID,
					void**){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFCreateVideoSampleAllocator(
					REFIID,
					void**){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFCreateDXGIDeviceManager(
					UINT*,
					IMFDXGIDeviceManager**){
					return E_NOTIMPL;
				}			
				
				static HRESULT STDAPICALLTYPE stubMFInitVideoFormat_RGB(
					MFVIDEOFORMAT*,
					DWORD,
					DWORD,
					DWORD){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFCreateVideoMediaType(
					const MFVIDEOFORMAT*,
					IMFVideoMediaType**){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFCreateDXGISurfaceBuffer(
					REFIID,
					IUnknown*,
					UINT,
					BOOL,
					IMFMediaBuffer**){
					return E_NOTIMPL;
				}

				static _DXGI_FORMAT STDAPICALLTYPE stubMFMapDX9FormatToDXGIFormat(
					DWORD){
					return _DXGI_FORMAT::_DXGI_FORMAT_UNKNOWN;
				}

				static DWORD STDAPICALLTYPE stubMFMapDXGIFormatToDX9Format(
					_DXGI_FORMAT){
					return 0;
				}

				static DWORD STDAPICALLTYPE stubMFGetService(
					IUnknown*,
					REFGUID,
					REFIID,
					LPVOID*){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFScheduleWorkItemEx(
					IMFAsyncResult*,
					INT64,
					MFWORKITEM_KEY*){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFRegisterPlatformWithMMCSS(
					PCWSTR,
					DWORD*,
					LONG){
					return E_NOTIMPL;
				}

				static HRESULT STDAPICALLTYPE stubMFUnregisterPlatformFromMMCSS(){
					return E_NOTIMPL;
				}

				MediaFoundationManager(
					const MediaFoundationManager&) = delete;
				MediaFoundationManager& operator=(
					const MediaFoundationManager&) = delete;
			};

		}
	}
}


