/*
MIT License

Copyright(c) 2020 Evgeny Pereguda

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "DataParser.h"
#include "../Common/Config.h"
#include "../Common/GUIDs.h"
#include "../Common/MFHeaders.h"
#include "../Common/ComPtrCustom.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../LogPrintOut/LogPrintOut.h"
#include <Strsafe.h>

namespace CaptureManager
{
#ifndef IF_EQUAL_RETURN
#define IF_EQUAL_RETURN(param, val) if(val == param) return L#val
#endif

#ifndef BIND_DESCRIPTION
#define BIND_DESCRIPTION(param, val, descr) if((HRESULT)(val) == (param)) return L#descr
#endif

#ifndef BIND_DESCRIPTIONDOUBLE
#define BIND_DESCRIPTIONDOUBLE(param, val, descr) if((HRESULT)(val) == (param)) return descr
#endif

#ifndef BIND_DESCRIPTIONTRIPLE
#define BIND_DESCRIPTIONTRIPLE(param, val, descr) if((HRESULT)(val) == (param)) return descr
#endif

#ifndef ADD_IF_EQUAL
#define ADD_IF_EQUAL(param, val, valuePartsNode) if(val & param){ \
	auto lValuePart = valuePartsNode.append_child(L"ValuePart"); \
	lValuePart.append_attribute(L"Title") = L"Speaker position"; \
	lValuePart.append_attribute(L"Value") = L#val;}
#endif

	EXTERN_GUID(MEDIASUBTYPE_PHOTOMOTION,
		0x1d4a45f2, 0xe5f6, 0x4b44, 0x83, 0x88, 0xf0, 0xae, 0x5c, 0x0e, 0x0c, 0x37);

	EXTERN_GUID(MFVideoFormat_Intel_Hardware_MediaType_One,
		0x3231564E, 0x3961, 0x42AE, 0xBA, 0x67, 0xFF, 0x47, 0xCC, 0xC1, 0x3E, 0xED);

	EXTERN_GUID(MF_MT_FIRST_UNKNOWN,
		0xC496F370, 0x2F8B, 0x4F51, 0xAE, 0x46, 0x9C, 0xFC, 0x1B, 0xC8, 0x2A, 0x47);

	EXTERN_GUID(CLSID_WebcamInterfaceDeviceCategory,
		0xE5323777, 0xF976, 0x4F5B, 0x9B, 0x55, 0xB9, 0x46, 0x99, 0xC4, 0x6E, 0x44);
		
	///{5E574E59-E28D-41DB-9314-089BFED6957C}
	EXTERN_GUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_HW_SOURCE, 
		0x5e574e59, 0xe28d, 0x41db, 0x93, 0x14, 0x08, 0x9b, 0xfe, 0xd6, 0x95, 0x7c);

	///{F72A76A0-EB0A-11D0-ACE4-0000C0CC16BA}
	EXTERN_GUID(VIDEOINFOHEADER2,
		0xF72A76A0, 0xEB0A, 0x11D0, 0xAC, 0xE4, 0x00, 0x00, 0xC0, 0xCC, 0x16, 0xBA);

	///{5a54a0c9-c7ec-4bd9-8ede-f3c75dc4393b}
	EXTERN_GUID(VideoProcProgressiveDevice,
		0x5a54a0c9, 0xc7ec, 0x4bd9, 0x8e, 0xde, 0xf3, 0xc7, 0x5d, 0xc4, 0x39, 0x3b);

	///{335aa36e-7884-43a4-9c91-7f87faf3e37e}
	EXTERN_GUID(VideoProcBobDevice,
		0x335aa36e, 0x7884, 0x43a4, 0x9c, 0x91, 0x7f, 0x87, 0xfa, 0xf3, 0xe3, 0x7e);

	///{4553d47f-ee7e-4e3f-9475-dbf1376c4810}
	EXTERN_GUID(VideoProcSoftwareDevice,
		0x4553d47f, 0xee7e, 0x4e3f, 0x94, 0x75, 0xdb, 0xf1, 0x37, 0x6c, 0x48, 0x10);

	///{552C0DAD-CCBC-420B-83C8-74943CF9F1A6}
	EXTERN_GUID(MotionAdaptiveDevice_AMD,
		0x552C0DAD, 0xCCBC, 0x420B, 0x83, 0xC8, 0x74, 0x94, 0x3C, 0xF9, 0xF1, 0xA6);

#define STATIC_PINNAME_VIDEO_STILL					\
	0xfb6c428A,0x353,0x11d1,0x90,0x5f,0x0,0x0,0xc0,0xcc,0x16,0xba
	DEFINE_GUIDSTRUCT("FB6C428A-0353-11d1-905F-0000C0CC16BA", PINNAME_VIDEO_STILL);
#define PINNAME_VIDEO_STILL DEFINE_GUIDNAMED(PINNAME_VIDEO_STILL)


	DEFINE_MEDIATYPE_GUID(MFVideoFormat_Y16, FCC('Y16 '));

	DEFINE_MEDIATYPE_GUID(MFVideoFormat_NV21_Double, FCC('NV21'));

#define MFVideoFormat_NV21 MFVideoFormat_NV21_Double

	DEFINE_GUID(MF_DEVICESTREAM_ATTRIBUTE_FRAMESOURCE_TYPES_Double,
		0x17145fd1, 0x1b2b, 0x423c, 0x80, 0x1, 0x2b, 0x68, 0x33, 0xed, 0x35, 0x88);

#define MF_DEVICESTREAM_ATTRIBUTE_FRAMESOURCE_TYPES MF_DEVICESTREAM_ATTRIBUTE_FRAMESOURCE_TYPES_Double

	DEFINE_MEDIATYPE_GUID(MFVideoFormat_L8_Double, D3DFMT_L8);

#define MFVideoFormat_L8 MFVideoFormat_L8_Double

	DEFINE_MEDIATYPE_GUID(MFVideoFormat_L16_Double, D3DFMT_L16);

#define MFVideoFormat_L16 MFVideoFormat_L16_Double

	DEFINE_MEDIATYPE_GUID(MFVideoFormat_D16_Double, D3DFMT_D16);

#define MFVideoFormat_D16 MFVideoFormat_D16_Double




	namespace
	{
		EXTERN_GUID(MF_VIDEO_CAPTURE,
			0xFB6C4281, 0x0353, 0x11d1, 0x90, 0x5F, 0x00, 0x00, 0xC0, 0xCC, 0x16, 0xBA);

		static int DwordToFourCC(DWORD fcc, char* buffer)
		{
			//I'm not sure about the order of these, but try messing with them
			buffer[3] = HIBYTE(HIWORD(fcc));
			buffer[2] = LOBYTE(HIWORD(fcc));
			buffer[1] = HIBYTE(LOWORD(fcc));
			buffer[0] = LOBYTE(LOWORD(fcc));
			return 0;
		}
		
		using namespace Core;

		class AttributesLock
		{
		public:
			AttributesLock(IUnknown* aPtrIUnknown) :
				mAttributes(aPtrIUnknown)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					if (!mAttributes)
						break;
										
					LOG_INVOKE_MF_METHOD(LockStore, mAttributes);

				} while (false);

				mResult = lresult;
			}

			~AttributesLock()
			{
				HRESULT lresult;

				do
				{
					if (!mAttributes)
						break;

					LOG_INVOKE_MF_METHOD(UnlockStore, mAttributes);

				} while (false);
			}

			HRESULT getResult()
			{
				return this->mResult;
			}

		private:

			CComQIPtrCustom<IMFAttributes> mAttributes;

			HRESULT mResult;

		};
	}

	LPCWSTR DataParser::GetGUIDNameConst(
		const GUID& guid)
	{

		// MF Stream Attributes

		IF_EQUAL_RETURN(guid, MF_SD_LANGUAGE);
		IF_EQUAL_RETURN(guid, MF_SD_MUTUALLY_EXCLUSIVE);
		IF_EQUAL_RETURN(guid, MF_SD_PROTECTED);
		IF_EQUAL_RETURN(guid, MF_SD_STREAM_NAME);
		IF_EQUAL_RETURN(guid, MF_DEVICESTREAM_ATTRIBUTE_FRAMESOURCE_TYPES);
		IF_EQUAL_RETURN(guid, MF_DEVICESTREAM_STREAM_CATEGORY);

		
				
		// Capture Device Attributes

		IF_EQUAL_RETURN(guid, MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME);
		IF_EQUAL_RETURN(guid, MF_DEVSOURCE_ATTRIBUTE_MEDIA_TYPE);
		IF_EQUAL_RETURN(guid, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE);
		IF_EQUAL_RETURN(guid, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_ENDPOINT_ID);
		IF_EQUAL_RETURN(guid, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_ROLE);
		IF_EQUAL_RETURN(guid, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_CATEGORY);
		IF_EQUAL_RETURN(guid, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_HW_SOURCE);
		IF_EQUAL_RETURN(guid, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_HW_SOURCE);		
		IF_EQUAL_RETURN(guid, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_MAX_BUFFERS);
		IF_EQUAL_RETURN(guid, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK);
		IF_EQUAL_RETURN(guid, MFT_HW_TIMESTAMP_WITH_QPC_Attribute);
		IF_EQUAL_RETURN(guid, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_GUID);
		IF_EQUAL_RETURN(guid, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
		IF_EQUAL_RETURN(guid, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_SYMBOLIC_LINK);
		IF_EQUAL_RETURN(guid, CLSID_VideoInputDeviceCategory);
		IF_EQUAL_RETURN(guid, CLSID_WebcamInterfaceDeviceCategory);
		IF_EQUAL_RETURN(guid, CM_DeviceManager);
		IF_EQUAL_RETURN(guid, CM_DirectX11_Capture_Texture);
		IF_EQUAL_RETURN(guid, CM_SourceStride);
		 
		
		
		// Media Type GUID connstant

		IF_EQUAL_RETURN(guid, MF_MT_MAJOR_TYPE);
		IF_EQUAL_RETURN(guid, MF_MT_SUBTYPE);
		IF_EQUAL_RETURN(guid, MF_MT_ALL_SAMPLES_INDEPENDENT);
		IF_EQUAL_RETURN(guid, MF_MT_FIXED_SIZE_SAMPLES);
		IF_EQUAL_RETURN(guid, MF_MT_COMPRESSED);
		IF_EQUAL_RETURN(guid, MF_MT_SAMPLE_SIZE);
		IF_EQUAL_RETURN(guid, MF_MT_WRAPPED_TYPE);
		IF_EQUAL_RETURN(guid, MF_MT_AUDIO_NUM_CHANNELS);
		IF_EQUAL_RETURN(guid, MF_MT_AUDIO_SAMPLES_PER_SECOND);
		IF_EQUAL_RETURN(guid, MF_MT_AUDIO_FLOAT_SAMPLES_PER_SECOND);
		IF_EQUAL_RETURN(guid, MF_MT_AUDIO_AVG_BYTES_PER_SECOND);
		IF_EQUAL_RETURN(guid, MF_MT_AUDIO_BLOCK_ALIGNMENT);
		IF_EQUAL_RETURN(guid, MF_MT_AUDIO_BITS_PER_SAMPLE);
		IF_EQUAL_RETURN(guid, MF_MT_AUDIO_VALID_BITS_PER_SAMPLE);
		IF_EQUAL_RETURN(guid, MF_MT_AUDIO_SAMPLES_PER_BLOCK);
		IF_EQUAL_RETURN(guid, MF_MT_AUDIO_CHANNEL_MASK);
		IF_EQUAL_RETURN(guid, MF_MT_AUDIO_FOLDDOWN_MATRIX);
		IF_EQUAL_RETURN(guid, MF_MT_AUDIO_WMADRC_PEAKREF);
		IF_EQUAL_RETURN(guid, MF_MT_AUDIO_WMADRC_PEAKTARGET);
		IF_EQUAL_RETURN(guid, MF_MT_AUDIO_WMADRC_AVGREF);
		IF_EQUAL_RETURN(guid, MF_MT_AUDIO_WMADRC_AVGTARGET);
		IF_EQUAL_RETURN(guid, MF_MT_AUDIO_PREFER_WAVEFORMATEX);
		IF_EQUAL_RETURN(guid, MF_MT_AAC_PAYLOAD_TYPE);
		IF_EQUAL_RETURN(guid, MF_MT_AAC_AUDIO_PROFILE_LEVEL_INDICATION);
		IF_EQUAL_RETURN(guid, MF_MT_FRAME_SIZE);
		IF_EQUAL_RETURN(guid, MF_MT_FRAME_RATE);
		IF_EQUAL_RETURN(guid, MF_MT_FRAME_RATE_RANGE_MAX);
		IF_EQUAL_RETURN(guid, MF_MT_FRAME_RATE_RANGE_MIN);
		IF_EQUAL_RETURN(guid, MF_MT_PIXEL_ASPECT_RATIO);
		IF_EQUAL_RETURN(guid, MF_MT_DRM_FLAGS);
		IF_EQUAL_RETURN(guid, MF_MT_PAD_CONTROL_FLAGS);
		IF_EQUAL_RETURN(guid, MF_MT_SOURCE_CONTENT_HINT);
		IF_EQUAL_RETURN(guid, MF_MT_VIDEO_CHROMA_SITING);
		IF_EQUAL_RETURN(guid, MF_MT_INTERLACE_MODE);
		IF_EQUAL_RETURN(guid, MF_MT_TRANSFER_FUNCTION);
		IF_EQUAL_RETURN(guid, MF_MT_VIDEO_PRIMARIES);
		IF_EQUAL_RETURN(guid, MF_MT_CUSTOM_VIDEO_PRIMARIES);
		IF_EQUAL_RETURN(guid, MF_MT_YUV_MATRIX);
		IF_EQUAL_RETURN(guid, DirectShowPhysicalType);
		IF_EQUAL_RETURN(guid, MF_MT_VIDEO_LIGHTING);
		IF_EQUAL_RETURN(guid, MF_MT_VIDEO_NOMINAL_RANGE);
		IF_EQUAL_RETURN(guid, MF_MT_GEOMETRIC_APERTURE);
		IF_EQUAL_RETURN(guid, MF_MT_MINIMUM_DISPLAY_APERTURE);
		IF_EQUAL_RETURN(guid, MF_MT_PAN_SCAN_APERTURE);
		IF_EQUAL_RETURN(guid, MF_MT_PAN_SCAN_ENABLED);
		IF_EQUAL_RETURN(guid, MF_MT_AVG_BITRATE);
		IF_EQUAL_RETURN(guid, MF_MT_AVG_BIT_ERROR_RATE);
		IF_EQUAL_RETURN(guid, MF_MT_MAX_KEYFRAME_SPACING);
		IF_EQUAL_RETURN(guid, MF_MT_DEFAULT_STRIDE);
		IF_EQUAL_RETURN(guid, MF_MT_PALETTE);
		IF_EQUAL_RETURN(guid, MF_MT_USER_DATA);
		IF_EQUAL_RETURN(guid, MF_MT_AM_FORMAT_TYPE);
		IF_EQUAL_RETURN(guid, MF_MT_MPEG_START_TIME_CODE);
		IF_EQUAL_RETURN(guid, MF_MT_MPEG2_PROFILE);
		IF_EQUAL_RETURN(guid, MF_MT_MPEG2_LEVEL);
		IF_EQUAL_RETURN(guid, MF_MT_MPEG2_FLAGS);
		IF_EQUAL_RETURN(guid, MF_MT_MPEG_SEQUENCE_HEADER);
		IF_EQUAL_RETURN(guid, MF_MT_DV_AAUX_SRC_PACK_0);
		IF_EQUAL_RETURN(guid, MF_MT_DV_AAUX_CTRL_PACK_0);
		IF_EQUAL_RETURN(guid, MF_MT_DV_AAUX_SRC_PACK_1);
		IF_EQUAL_RETURN(guid, MF_MT_DV_AAUX_CTRL_PACK_1);
		IF_EQUAL_RETURN(guid, MF_MT_DV_VAUX_SRC_PACK);
		IF_EQUAL_RETURN(guid, MF_MT_DV_VAUX_CTRL_PACK);
		IF_EQUAL_RETURN(guid, MF_MT_ARBITRARY_HEADER);
		IF_EQUAL_RETURN(guid, MF_MT_ARBITRARY_FORMAT);
		IF_EQUAL_RETURN(guid, MF_MT_IMAGE_LOSS_TOLERANT);
		IF_EQUAL_RETURN(guid, MF_MT_MPEG4_SAMPLE_DESCRIPTION);
		IF_EQUAL_RETURN(guid, MF_MT_MPEG4_CURRENT_SAMPLE_ENTRY);
		IF_EQUAL_RETURN(guid, MF_MT_ORIGINAL_4CC);
		IF_EQUAL_RETURN(guid, MF_MT_ORIGINAL_WAVE_FORMAT_TAG);

		// Media types

		IF_EQUAL_RETURN(guid, MFMediaType_Audio);
		IF_EQUAL_RETURN(guid, MFMediaType_Video);
		IF_EQUAL_RETURN(guid, MFMediaType_Protected);
		IF_EQUAL_RETURN(guid, MFMediaType_SAMI);
		IF_EQUAL_RETURN(guid, MFMediaType_Script);
		IF_EQUAL_RETURN(guid, MFMediaType_Image);
		IF_EQUAL_RETURN(guid, MFMediaType_HTML);
		IF_EQUAL_RETURN(guid, MFMediaType_Binary);
		IF_EQUAL_RETURN(guid, MFMediaType_FileTransfer);

		IF_EQUAL_RETURN(guid, MFVideoFormat_AI44); //     FCC('AI44')
		IF_EQUAL_RETURN(guid, MFVideoFormat_ARGB32); //   D3DFMT_A8R8G8B8 
		IF_EQUAL_RETURN(guid, MFVideoFormat_AYUV); //     FCC('AYUV')
		IF_EQUAL_RETURN(guid, MFVideoFormat_DV25); //     FCC('dv25')
		IF_EQUAL_RETURN(guid, MFVideoFormat_DV50); //     FCC('dv50')
		IF_EQUAL_RETURN(guid, MFVideoFormat_DVH1); //     FCC('dvh1')
		IF_EQUAL_RETURN(guid, MFVideoFormat_DVSD); //     FCC('dvsd')
		IF_EQUAL_RETURN(guid, MFVideoFormat_DVSL); //     FCC('dvsl')
		IF_EQUAL_RETURN(guid, MFVideoFormat_H264); //     FCC('H264')
		IF_EQUAL_RETURN(guid, MFVideoFormat_HEVC); //     FCC('HEVC')
		IF_EQUAL_RETURN(guid, MFVideoFormat_I420); //     FCC('I420')
		IF_EQUAL_RETURN(guid, MFVideoFormat_IYUV); //     FCC('IYUV')
		IF_EQUAL_RETURN(guid, MFVideoFormat_M4S2); //     FCC('M4S2')
		IF_EQUAL_RETURN(guid, MFVideoFormat_MJPG);
		IF_EQUAL_RETURN(guid, MFVideoFormat_MP43); //     FCC('MP43')
		IF_EQUAL_RETURN(guid, MFVideoFormat_MP4S); //     FCC('MP4S')
		IF_EQUAL_RETURN(guid, MFVideoFormat_MP4V); //     FCC('MP4V')
		IF_EQUAL_RETURN(guid, MFVideoFormat_MPG1); //     FCC('MPG1')
		IF_EQUAL_RETURN(guid, MFVideoFormat_MSS1); //     FCC('MSS1')
		IF_EQUAL_RETURN(guid, MFVideoFormat_MSS2); //     FCC('MSS2')
		IF_EQUAL_RETURN(guid, MFVideoFormat_NV11); //     FCC('NV11')
		IF_EQUAL_RETURN(guid, MFVideoFormat_NV12); //     FCC('NV12')
		IF_EQUAL_RETURN(guid, MFVideoFormat_P010); //     FCC('P010')
		IF_EQUAL_RETURN(guid, MFVideoFormat_P016); //     FCC('P016')
		IF_EQUAL_RETURN(guid, MFVideoFormat_P210); //     FCC('P210')
		IF_EQUAL_RETURN(guid, MFVideoFormat_P216); //     FCC('P216')
		IF_EQUAL_RETURN(guid, MFVideoFormat_RGB24); //    D3DFMT_R8G8B8 
		IF_EQUAL_RETURN(guid, MFVideoFormat_RGB32); //    D3DFMT_X8R8G8B8 
		IF_EQUAL_RETURN(guid, MFVideoFormat_RGB555); //   D3DFMT_X1R5G5B5 
		IF_EQUAL_RETURN(guid, MFVideoFormat_RGB565); //   D3DFMT_R5G6B5 
		IF_EQUAL_RETURN(guid, MFVideoFormat_RGB8);
		IF_EQUAL_RETURN(guid, MFVideoFormat_UYVY); //     FCC('UYVY')
		IF_EQUAL_RETURN(guid, MFVideoFormat_v210); //     FCC('v210')
		IF_EQUAL_RETURN(guid, MFVideoFormat_v410); //     FCC('v410')
		IF_EQUAL_RETURN(guid, MFVideoFormat_WMV1); //     FCC('WMV1')
		IF_EQUAL_RETURN(guid, MFVideoFormat_WMV2); //     FCC('WMV2')
		IF_EQUAL_RETURN(guid, MFVideoFormat_WMV3); //     FCC('WMV3')
		IF_EQUAL_RETURN(guid, MFVideoFormat_WVC1); //     FCC('WVC1')
		IF_EQUAL_RETURN(guid, MFVideoFormat_Y210); //     FCC('Y210')
		IF_EQUAL_RETURN(guid, MFVideoFormat_Y216); //     FCC('Y216')
		IF_EQUAL_RETURN(guid, MFVideoFormat_Y410); //     FCC('Y410')
		IF_EQUAL_RETURN(guid, MFVideoFormat_Y416); //     FCC('Y416')
		IF_EQUAL_RETURN(guid, MFVideoFormat_Y41P);
		IF_EQUAL_RETURN(guid, MFVideoFormat_Y41T);
		IF_EQUAL_RETURN(guid, MFVideoFormat_YUY2); //     FCC('YUY2')
		IF_EQUAL_RETURN(guid, MFVideoFormat_YV12); //     FCC('YV12')
		IF_EQUAL_RETURN(guid, MFVideoFormat_YVYU);
		IF_EQUAL_RETURN(guid, MEDIASUBTYPE_PHOTOMOTION);
		IF_EQUAL_RETURN(guid, MFVideoFormat_Intel_Hardware_MediaType_One);
		IF_EQUAL_RETURN(guid, MFVideoFormat_Y16);
		IF_EQUAL_RETURN(guid, MFVideoFormat_NV21);
		IF_EQUAL_RETURN(guid, MFVideoFormat_D16);
		IF_EQUAL_RETURN(guid, MFVideoFormat_L16);
		IF_EQUAL_RETURN(guid, MFVideoFormat_L8);
		IF_EQUAL_RETURN(guid, MEDIASUBTYPE_V410);
		IF_EQUAL_RETURN(guid, MEDIASUBTYPE_V216);

		


		

		
		

		IF_EQUAL_RETURN(guid, MFAudioFormat_PCM); //              WAVE_FORMAT_PCM 
		IF_EQUAL_RETURN(guid, MFAudioFormat_Float); //            WAVE_FORMAT_IEEE_FLOAT 
		IF_EQUAL_RETURN(guid, MFAudioFormat_DTS); //              WAVE_FORMAT_DTS 
		IF_EQUAL_RETURN(guid, MFAudioFormat_Dolby_AC3); //		  WAVE_FORMAT_DOLBY_AC3
		IF_EQUAL_RETURN(guid, MFAudioFormat_Dolby_AC3_SPDIF); //  WAVE_FORMAT_DOLBY_AC3_SPDIF 
		IF_EQUAL_RETURN(guid, MFAudioFormat_DRM); //              WAVE_FORMAT_DRM 
		IF_EQUAL_RETURN(guid, MFAudioFormat_WMAudioV8); //        WAVE_FORMAT_WMAUDIO2 
		IF_EQUAL_RETURN(guid, MFAudioFormat_WMAudioV9); //        WAVE_FORMAT_WMAUDIO3 
		IF_EQUAL_RETURN(guid, MFAudioFormat_WMAudio_Lossless); // WAVE_FORMAT_WMAUDIO_LOSSLESS 
		IF_EQUAL_RETURN(guid, MFAudioFormat_WMASPDIF); //         WAVE_FORMAT_WMASPDIF 
		IF_EQUAL_RETURN(guid, MFAudioFormat_MSP1); //             WAVE_FORMAT_WMAVOICE9 
		IF_EQUAL_RETURN(guid, MFAudioFormat_MP3); //              WAVE_FORMAT_MPEGLAYER3 
		IF_EQUAL_RETURN(guid, MFAudioFormat_MPEG); //             WAVE_FORMAT_MPEG 
		IF_EQUAL_RETURN(guid, MFAudioFormat_AAC); //              WAVE_FORMAT_MPEG_HEAAC 
		IF_EQUAL_RETURN(guid, MFAudioFormat_ADTS); //             WAVE_FORMAT_MPEG_ADTS_AAC 



		IF_EQUAL_RETURN(guid, MF_VIDEO_CAPTURE); //             PINNAME_VIDEO_CAPTURE 



		IF_EQUAL_RETURN(guid, MFSampleExtension_CleanPoint);
		IF_EQUAL_RETURN(guid, MFSampleExtension_Discontinuity);

		// DirectShow Format type
		IF_EQUAL_RETURN(guid, VIDEOINFOHEADER2);		


		// Processor devices
		IF_EQUAL_RETURN(guid, VideoProcProgressiveDevice);
		IF_EQUAL_RETURN(guid, VideoProcBobDevice);
		IF_EQUAL_RETURN(guid, VideoProcSoftwareDevice);
		IF_EQUAL_RETURN(guid, MotionAdaptiveDevice_AMD);



		IF_EQUAL_RETURN(guid, PINNAME_VIDEO_STILL);

		return nullptr;
	}
	
	LPCWSTR DataParser::getHRESULTDescription(
		const HRESULT& aValue)
	{
		BIND_DESCRIPTION(aValue, S_OK, Success with the result.);
		BIND_DESCRIPTION(aValue, S_FALSE, Success without the result.);
		BIND_DESCRIPTION(aValue, E_FAIL, Unspecified error.);
		BIND_DESCRIPTION(aValue, E_ACCESSDENIED, Access denied.);
		BIND_DESCRIPTION(aValue, E_INVALIDARG, Invalid parameter value.);
		BIND_DESCRIPTION(aValue, E_OUTOFMEMORY, Out of memory.);
		BIND_DESCRIPTION(aValue, E_POINTER, nullptr was passed incorrectly for a pointer value.);
		BIND_DESCRIPTION(aValue, E_UNEXPECTED, Unexpected condition.);
		BIND_DESCRIPTION(aValue, E_NOTIMPL, Method is not supported.);
		BIND_DESCRIPTION(aValue, CONTEXT_E_SYNCH_TIMEOUT, The component is configured to use synchronization and a thread has timed out waiting to enter the context.);
		BIND_DESCRIPTION(aValue, E_ASYNC_OPERATION_NOT_STARTED, An async operation was not properly started.);
		BIND_DESCRIPTION(aValue, MF_E_OUT_OF_RANGE, The operation attempted to access data outside the valid range.);
		BIND_DESCRIPTION(aValue, E_NOINTERFACE, No such interface supported.);
		BIND_DESCRIPTION(aValue, HRESULT_FROM_WIN32(ERROR_NOT_FOUND), Element not found.);
		BIND_DESCRIPTION(aValue, HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND), The system cannot find the file specified.);
		BIND_DESCRIPTION(aValue, 0x887a0027, The time - out interval elapsed before the next desktop frame was available.);
		
		

		BIND_DESCRIPTION(aValue, MF_E_PLATFORM_NOT_INITIALIZED, Platform not initialized.Please call MFStartup().);
		BIND_DESCRIPTION(aValue, MF_E_BUFFERTOOSMALL, The buffer was too small to carry out the requested action.);
		BIND_DESCRIPTION(aValue, MF_E_INVALIDREQUEST, The request is invalid in the current state.);
		BIND_DESCRIPTION(aValue, MF_E_INVALIDSTREAMNUMBER, The stream number provided was invalid.);
		BIND_DESCRIPTIONTRIPLE(aValue, MF_E_INVALIDMEDIATYPE, L"The data specified for the media type is invalid, inconsistent, or not supported by this object.");
		BIND_DESCRIPTION(aValue, MF_E_NOTACCEPTING, The callee is currently not accepting further input.);
		BIND_DESCRIPTION(aValue, MF_E_NOT_INITIALIZED, This object needs to be initialized before the requested operation can be carried out.);
		BIND_DESCRIPTION(aValue, MF_E_UNSUPPORTED_REPRESENTATION,	The requested representation is not supported by this object.);
		BIND_DESCRIPTION(aValue, MF_E_NO_MORE_TYPES, An object ran out of media types to suggest therefore the requested chain of streaming objects cannot be completed.);
		BIND_DESCRIPTION(aValue, MF_E_UNSUPPORTED_SERVICE, The object does not support the specified service.);
		BIND_DESCRIPTION(aValue, MF_E_UNEXPECTED, An unexpected error has occurred in the operation requested.);
		BIND_DESCRIPTION(aValue, MF_E_INVALIDNAME, Invalid name.);
		BIND_DESCRIPTION(aValue, MF_E_INVALIDTYPE, Invalid type.);
		BIND_DESCRIPTION(aValue, MF_E_INVALID_FILE_FORMAT, The file does not conform to the relevant file format specification.);
		BIND_DESCRIPTION(aValue, MF_E_INVALIDINDEX, Invalid index.);
		BIND_DESCRIPTION(aValue, MF_E_INVALID_TIMESTAMP, An invalid timestamp was given.);
		BIND_DESCRIPTION(aValue, MF_E_UNSUPPORTED_SCHEME, The scheme of the given URL is unsupported.);
		BIND_DESCRIPTION(aValue, MF_E_UNSUPPORTED_BYTESTREAM_TYPE, The byte stream type of the given URL is unsupported.);
		BIND_DESCRIPTION(aValue, MF_E_UNSUPPORTED_TIME_FORMAT, The given time format is unsupported.);
		BIND_DESCRIPTION(aValue, MF_E_NO_SAMPLE_TIMESTAMP, The Media Sample does not have a timestamp.);
		BIND_DESCRIPTION(aValue, MF_E_NO_SAMPLE_DURATION, The Media Sample does not have a duration.);
		BIND_DESCRIPTION(aValue, MF_E_INVALID_STREAM_DATA, The request failed because the data in the stream is corrupt.);
		BIND_DESCRIPTION(aValue, MF_E_RT_UNAVAILABLE, Real time services are not available.);
		BIND_DESCRIPTION(aValue, MF_E_UNSUPPORTED_RATE, The specified rate is not supported.);
		BIND_DESCRIPTION(aValue, MF_E_THINNING_UNSUPPORTED, This component does not support stream - thinning.);
		BIND_DESCRIPTION(aValue, MF_E_REVERSE_UNSUPPORTED, The call failed because no reverse playback rates are available.);
		BIND_DESCRIPTION(aValue, MF_E_UNSUPPORTED_RATE_TRANSITION, The requested rate transition cannot occur in the current state.);
		BIND_DESCRIPTION(aValue, MF_E_RATE_CHANGE_PREEMPTED, The requested rate change has been pre - empted and will not occur.);
		BIND_DESCRIPTION(aValue, MF_E_NOT_FOUND, The specified object or value does not exist.);
		BIND_DESCRIPTION(aValue, MF_E_NOT_AVAILABLE, The requested value is not available.);
		BIND_DESCRIPTION(aValue, MF_E_NO_CLOCK, The specified operation requires a clock and no clock is available.);
		BIND_DESCRIPTION(aValue, MF_E_MULTIPLE_BEGIN, This callback has already been passed in to this event generator.);
		BIND_DESCRIPTION(aValue, MF_E_MULTIPLE_SUBSCRIBERS, Some component is already listening to events on this event generator.);
		BIND_DESCRIPTION(aValue, MF_E_TIMER_ORPHANED, This timer was orphaned before its callback time arrived.);
		BIND_DESCRIPTION(aValue, MF_E_STATE_TRANSITION_PENDING, A state transition is already pending.);
		BIND_DESCRIPTION(aValue, MF_E_UNSUPPORTED_STATE_TRANSITION, The requested state transition is unsupported.);
		BIND_DESCRIPTION(aValue, MF_E_UNRECOVERABLE_ERROR_OCCURRED, An unrecoverable error has occurred.);
		BIND_DESCRIPTION(aValue, MF_E_SAMPLE_HAS_TOO_MANY_BUFFERS, The provided sample has too many buffers.);
		BIND_DESCRIPTION(aValue, MF_E_SAMPLE_NOT_WRITABLE, The provided sample is not writable.);
		BIND_DESCRIPTION(aValue, MF_E_INVALID_KEY, The specified key is not valid.);
		BIND_DESCRIPTION(aValue, MF_E_BAD_STARTUP_VERSION, You are calling MFStartup with the wrong MF_VERSION.Mismatched bits ? );
		BIND_DESCRIPTION(aValue, MF_E_UNSUPPORTED_CAPTION, The caption of the given URL is unsupported.);
		BIND_DESCRIPTION(aValue, MF_E_INVALID_POSITION, The operation on the current offset is not permitted.);
		BIND_DESCRIPTION(aValue, MF_E_ATTRIBUTENOTFOUND, The requested attribute was not found.);
		BIND_DESCRIPTION(aValue, MF_E_PROPERTY_TYPE_NOT_ALLOWED, The specified property type is not allowed in this context.);
		BIND_DESCRIPTION(aValue, MF_E_PROPERTY_TYPE_NOT_SUPPORTED, The specified property type is not supported.);
		BIND_DESCRIPTION(aValue, MF_E_PROPERTY_EMPTY, The specified property is empty.);
		BIND_DESCRIPTION(aValue, MF_E_PROPERTY_NOT_EMPTY, The specified property is not empty.);
		BIND_DESCRIPTION(aValue, MF_E_PROPERTY_VECTOR_NOT_ALLOWED, The vector property specified is not allowed in this context.);
		BIND_DESCRIPTION(aValue, MF_E_PROPERTY_VECTOR_REQUIRED, A vector property is required in this context.);
		BIND_DESCRIPTION(aValue, MF_E_OPERATION_CANCELLED, The operation is cancelled.);
		BIND_DESCRIPTION(aValue, MF_E_BYTESTREAM_NOT_SEEKABLE, The provided bytestream was expected to be seekable but it was not.);
		BIND_DESCRIPTION(aValue, MF_E_DISABLED_IN_SAFEMODE, The Media Foundation platform is disabled when the system is running in Safe Mode.);
		BIND_DESCRIPTION(aValue, MF_E_CANNOT_PARSE_BYTESTREAM, The Media Source could not parse the byte stream.);
		BIND_DESCRIPTION(aValue, MF_E_SOURCERESOLVER_MUTUALLY_EXCLUSIVE_FLAGS, Mutually exclusive flags have been specified to source resolver.This flag combination is invalid.);
		BIND_DESCRIPTION(aValue, MF_E_MEDIAPROC_WRONGSTATE, MediaProc is in the wrong state.);
		BIND_DESCRIPTION(aValue, MF_E_RT_THROUGHPUT_NOT_AVAILABLE, Real time I / O service can not provide requested throughput.);
		BIND_DESCRIPTION(aValue, MF_E_RT_TOO_MANY_CLASSES, The work queue cannot be registered with more classes.);
		BIND_DESCRIPTION(aValue, MF_E_RT_WOULDBLOCK, This operation cannot succeed because another thread owns this object.);
		BIND_DESCRIPTION(aValue, MF_E_NO_BITPUMP, Internal.Bitpump not found.);
		BIND_DESCRIPTION(aValue, MF_E_RT_OUTOFMEMORY, No more RT memory available.);
		BIND_DESCRIPTION(aValue, MF_E_RT_WORKQUEUE_CLASS_NOT_SPECIFIED, An MMCSS class has not been set for this work queue.);
		BIND_DESCRIPTION(aValue, MF_E_CANNOT_CREATE_SINK, Activate failed to create a media sink.Call OutputNode::GetUINT32(MF_TOPONODE_MAJORTYPE) for more information.);
		BIND_DESCRIPTION(aValue, MF_E_BYTESTREAM_UNKNOWN_LENGTH, The length of the provided bytestream is unknown.);
		BIND_DESCRIPTION(aValue, MF_E_SESSION_PAUSEWHILESTOPPED, The media session cannot pause from a stopped state.);
		BIND_DESCRIPTIONTRIPLE(aValue, MF_E_FORMAT_CHANGE_NOT_SUPPORTED, L"The data specified for the media type is supported, but would require a format change, which is not supported by this object.");
		BIND_DESCRIPTION(aValue, MF_E_INVALID_WORKQUEUE, The operation failed because an invalid combination of work queue ID and flags was specified.);
		BIND_DESCRIPTION(aValue, MF_E_DRM_UNSUPPORTED, No DRM support is available.);
		BIND_DESCRIPTION(aValue, MF_E_UNAUTHORIZED, This operation is not authorized.);
		BIND_DESCRIPTION(aValue, MF_E_OUT_OF_RANGE, The value is not in the specified or valid range.);
		BIND_DESCRIPTION(aValue, MF_E_INVALID_CODEC_MERIT, The registered codec merit is not valid.);
		BIND_DESCRIPTION(aValue, MF_E_HW_MFT_FAILED_START_STREAMING, Hardware MFT failed to start streaming due to lack of hardware resources.);
		BIND_DESCRIPTION(aValue, MF_E_ASF_PARSINGINCOMPLETE, Not enough data has been parsed to carry out the requested action.);
		BIND_DESCRIPTION(aValue, MF_E_ASF_MISSINGDATA, There is a gap in the ASF data provided.);
		BIND_DESCRIPTION(aValue, MF_E_ASF_INVALIDDATA, The data provided are not valid ASF.);
		BIND_DESCRIPTIONDOUBLE(aValue, MF_E_ASF_OPAQUEPACKET, L"The packet is opaque, so the requested information cannot be returned.");
		BIND_DESCRIPTION(aValue, MF_E_ASF_NOINDEX, The requested operation failed since there is no appropriate ASF index.);
		BIND_DESCRIPTION(aValue, MF_E_ASF_OUTOFRANGE, The value supplied is out of range for this operation.);
		BIND_DESCRIPTION(aValue, MF_E_ASF_INDEXNOTLOADED, The index entry requested needs to be loaded before it can be available.);
		BIND_DESCRIPTION(aValue, MF_E_ASF_TOO_MANY_PAYLOADS, The packet has reached the maximum number of payloads.);
		BIND_DESCRIPTION(aValue, MF_E_ASF_UNSUPPORTED_STREAM_TYPE, Stream type is not supported.);
		BIND_DESCRIPTION(aValue, MF_E_ASF_DROPPED_PACKET, One or more ASF packets were dropped.);
		BIND_DESCRIPTION(aValue, MF_E_NO_EVENTS_AVAILABLE, There are no events available in the queue.);
		BIND_DESCRIPTION(aValue, MF_E_INVALID_STATE_TRANSITION, A media source cannot go from the stopped state to the paused state.);
		BIND_DESCRIPTION(aValue, MF_E_END_OF_STREAM, The media stream cannot process any more samples because there are no more samples in the stream.);
		BIND_DESCRIPTION(aValue, MF_E_SHUTDOWN, The request is invalid because Shutdown() has been called.);
		BIND_DESCRIPTION(aValue, MF_E_MP3_NOTFOUND, The MP3 object was not found.);
		BIND_DESCRIPTION(aValue, MF_E_MP3_OUTOFDATA, The MP3 parser ran out of data before finding the MP3 object.);
		BIND_DESCRIPTION(aValue, MF_E_MP3_NOTMP3, The file is not really a MP3 file.);
		BIND_DESCRIPTION(aValue, MF_E_MP3_NOTSUPPORTED, The MP3 file is not supported.);
		BIND_DESCRIPTION(aValue, MF_E_NO_DURATION, The Media stream has no duration.);
		BIND_DESCRIPTION(aValue, MF_E_INVALID_FORMAT, The Media format is recognized but is invalid.);
		BIND_DESCRIPTION(aValue, MF_E_PROPERTY_NOT_FOUND, The property requested was not found.);
		BIND_DESCRIPTION(aValue, MF_E_PROPERTY_READ_ONLY, The property is read only.);
		BIND_DESCRIPTION(aValue, MF_E_PROPERTY_NOT_ALLOWED, The specified property is not allowed in this context.);
		BIND_DESCRIPTION(aValue, MF_E_MEDIA_SOURCE_NOT_STARTED, The media source has not started.);
		BIND_DESCRIPTION(aValue, MF_E_UNSUPPORTED_FORMAT, The Media format is recognized but not supported.);
		BIND_DESCRIPTION(aValue, MF_E_MP3_BAD_CRC, The MPEG frame has bad CRC.);
		BIND_DESCRIPTION(aValue, MF_E_NOT_PROTECTED, The file is not protected.);
		BIND_DESCRIPTION(aValue, MF_E_MEDIA_SOURCE_WRONGSTATE, The media source is in the wrong state.);
		BIND_DESCRIPTION(aValue, MF_E_MEDIA_SOURCE_NO_STREAMS_SELECTED, No streams are selected in the source presentation descriptor.);
		BIND_DESCRIPTION(aValue, MF_E_CANNOT_FIND_KEYFRAME_SAMPLE, No key frame sample was found.);
		BIND_DESCRIPTION(aValue, MF_E_NETWORK_RESOURCE_FAILURE, An attempt to acquire a network resource failed.);
		BIND_DESCRIPTION(aValue, MF_E_NET_WRITE, Error writing to the network.);
		BIND_DESCRIPTION(aValue, MF_E_NET_READ, Error reading from the network.);
		BIND_DESCRIPTION(aValue, MF_E_NET_REQUIRE_NETWORK, Internal.Entry cannot complete operation without network.);
		BIND_DESCRIPTION(aValue, MF_E_NET_REQUIRE_ASYNC, Internal.Async op is required.);
		BIND_DESCRIPTION(aValue, MF_E_NET_BWLEVEL_NOT_SUPPORTED, Internal.Bandwidth levels are not supported.);
		BIND_DESCRIPTION(aValue, MF_E_NET_STREAMGROUPS_NOT_SUPPORTED, Internal.Stream groups are not supported.);
		BIND_DESCRIPTION(aValue, MF_E_NET_MANUALSS_NOT_SUPPORTED, Manual stream selection is not supported.);
		BIND_DESCRIPTION(aValue, MF_E_NET_INVALID_PRESENTATION_DESCRIPTOR, Invalid presentation descriptor.);
		BIND_DESCRIPTION(aValue, MF_E_NET_CACHESTREAM_NOT_FOUND, Cannot find the cache stream.);
		BIND_DESCRIPTION(aValue, MF_E_NET_REQUIRE_INPUT, Internal.Entry cannot complete operation without input.);
		BIND_DESCRIPTION(aValue, MF_E_NET_REDIRECT, The client was redirected to another server.);
		BIND_DESCRIPTION(aValue, MF_E_NET_REDIRECT_TO_PROXY, The client was redirected to a proxy server.);
		BIND_DESCRIPTION(aValue, MF_E_NET_TOO_MANY_REDIRECTS, The client reached the maximum redirection limit.);
		BIND_DESCRIPTIONTRIPLE(aValue, MF_E_NET_TIMEOUT, L"The server, a computer set up to offer multimedia content to other computers, could not handle your request for multimedia content in a timely manner.Please try again later.");
		BIND_DESCRIPTION(aValue, MF_E_NET_CLIENT_CLOSE, The control socket is closed by the client.);
		BIND_DESCRIPTION(aValue, MF_E_NET_BAD_CONTROL_DATA, The server received invalid data from the client on the control connection.);
		BIND_DESCRIPTION(aValue, MF_E_NET_INCOMPATIBLE_SERVER, The server is not a compatible streaming media server.);
		BIND_DESCRIPTION(aValue, MF_E_NET_UNSAFE_URL, Url.);
		BIND_DESCRIPTION(aValue, MF_E_NET_CACHE_NO_DATA, Data is not available.);
		BIND_DESCRIPTION(aValue, MF_E_NET_EOL, End of line.);
		BIND_DESCRIPTION(aValue, MF_E_NET_BAD_REQUEST, The request could not be understood by the server.);
		BIND_DESCRIPTION(aValue, MF_E_NET_INTERNAL_SERVER_ERROR, The server encountered an unexpected condition which prevented it from fulfilling the request.);
		BIND_DESCRIPTION(aValue, MF_E_NET_SESSION_NOT_FOUND, Session not found.);
		BIND_DESCRIPTION(aValue, MF_E_NET_NOCONNECTION, There is no connection established with the Windows Media server.The operation failed.);
		BIND_DESCRIPTION(aValue, MF_E_NET_CONNECTION_FAILURE, The network connection has failed.);
		BIND_DESCRIPTION(aValue, MF_E_NET_INCOMPATIBLE_PUSHSERVER, The Server service that received the HTTP push request is not a compatible version of Windows Media Services(WMS).This error can indicate the push request was received by IIS instead of WMS.Ensure WMS has started and has the HTTP Server control protocol properly enabled and try again.);
		BIND_DESCRIPTION(aValue, MF_E_NET_SERVER_ACCESSDENIED, The Windows Media server is denying access.The username and / or password could be incorrect.);
		BIND_DESCRIPTION(aValue, MF_E_NET_PROXY_ACCESSDENIED, The proxy server is denying access.The username and / or password could be incorrect.);
		BIND_DESCRIPTION(aValue, MF_E_NET_CANNOTCONNECT, Unable to establish a connection to the server.);
		BIND_DESCRIPTION(aValue, MF_E_NET_INVALID_PUSH_TEMPLATE, The specified push template is invalid.);
		BIND_DESCRIPTION(aValue, MF_E_NET_INVALID_PUSH_PUBLISHING_POINT, The specified push publishing point is invalid.);
		BIND_DESCRIPTION(aValue, MF_E_NET_BUSY, The requested resource is in use.);
		BIND_DESCRIPTION(aValue, MF_E_NET_RESOURCE_GONE, The Publishing Point or file on the Windows Media Server is no longer available.);
		BIND_DESCRIPTION(aValue, MF_E_NET_ERROR_FROM_PROXY, The proxy experienced an error while attempting to contact the media server.);
		BIND_DESCRIPTION(aValue, MF_E_NET_PROXY_TIMEOUT, The proxy did not receive a timely response while attempting to contact the media server.);
		BIND_DESCRIPTION(aValue, MF_E_NET_SERVER_UNAVAILABLE, The server is currently unable to handle the request due to temporary overloading or maintenance of the server.);
		BIND_DESCRIPTION(aValue, MF_E_NET_TOO_MUCH_DATA, The encoding process was unable to keep up with the amount of data supplied.);
		BIND_DESCRIPTION(aValue, MF_E_NET_SESSION_INVALID, Session not found.);
		BIND_DESCRIPTION(aValue, MF_E_OFFLINE_MODE, The requested URL is not available in offline mode.);
		BIND_DESCRIPTION(aValue, MF_E_NET_UDP_BLOCKED, A device in the network is blocking UDP traffic.);
		BIND_DESCRIPTION(aValue, MF_E_NET_UNSUPPORTED_CONFIGURATION, The specified configuration value is not supported.);
		BIND_DESCRIPTION(aValue, MF_E_NET_PROTOCOL_DISABLED, The networking protocol is disabled.);
		BIND_DESCRIPTION(aValue, MF_E_ALREADY_INITIALIZED, This object has already been initialized and cannot be re - initialized at this time.);
		BIND_DESCRIPTION(aValue, MF_E_BANDWIDTH_OVERRUN, The amount of data passed in exceeds the given bitrate and buffer window.);
		BIND_DESCRIPTION(aValue, MF_E_LATE_SAMPLE, The sample was passed in too late to be correctly processed.);
		BIND_DESCRIPTION(aValue, MF_E_FLUSH_NEEDED, The requested action cannot be carried out until the object is flushed and the queue is emptied.);
		BIND_DESCRIPTION(aValue, MF_E_INVALID_PROFILE, The profile is invalid.);
		BIND_DESCRIPTION(aValue, MF_E_INDEX_NOT_COMMITTED, The index that is being generated needs to be committed before the requested action can be carried out.);
		BIND_DESCRIPTION(aValue, MF_E_NO_INDEX, The index that is necessary for the requested action is not found.);
		BIND_DESCRIPTION(aValue, MF_E_CANNOT_INDEX_IN_PLACE, The requested index cannot be added in - place to the specified ASF content.);
		BIND_DESCRIPTION(aValue, MF_E_MISSING_ASF_LEAKYBUCKET, The ASF leaky bucket parameters must be specified in order to carry out this request.);
		BIND_DESCRIPTION(aValue, MF_E_INVALID_ASF_STREAMID, The stream id is invalid.The valid range for ASF stream id is from 1 to 127.);
		BIND_DESCRIPTION(aValue, MF_E_STREAMSINK_REMOVED, The requested Stream Sink has been removed and cannot be used.);
		BIND_DESCRIPTION(aValue, MF_E_STREAMSINKS_OUT_OF_SYNC, The various Stream Sinks in this Media Sink are too far out of sync for the requested action to take place.);
		BIND_DESCRIPTION(aValue, MF_E_STREAMSINKS_FIXED, Stream Sinks cannot be added to or removed from this Media Sink because its set of streams is fixed.);
		BIND_DESCRIPTION(aValue, MF_E_STREAMSINK_EXISTS, The given Stream Sink already exists.);
		BIND_DESCRIPTION(aValue, MF_E_SAMPLEALLOCATOR_CANCELED, Sample allocations have been canceled.);
		BIND_DESCRIPTIONDOUBLE(aValue, MF_E_SAMPLEALLOCATOR_EMPTY, L"The sample allocator is currently empty, due to outstanding requests.");
		BIND_DESCRIPTION(aValue, MF_E_SINK_ALREADYSTOPPED, The stream sink is already stopped.);
		BIND_DESCRIPTION(aValue, MF_E_ASF_FILESINK_BITRATE_UNKNOWN, The ASF file sink could not reserve AVIO because the bitrate is unknown.);
		BIND_DESCRIPTION(aValue, MF_E_SINK_NO_STREAMS, No streams are selected in the sink presentation descriptor.);
		BIND_DESCRIPTION(aValue, MF_E_METADATA_TOO_LONG, A metadata item was too long to write to the output container.);
		BIND_DESCRIPTION(aValue, MF_E_SINK_NO_SAMPLES_PROCESSED, The operation failed because no samples were processed by the sink.);
		BIND_DESCRIPTION(aValue, MF_E_VIDEO_REN_NO_PROCAMP_HW, There is no available procamp hardware with which to perform color correction.);
		BIND_DESCRIPTION(aValue, MF_E_VIDEO_REN_NO_DEINTERLACE_HW, There is no available deinterlacing hardware with which to deinterlace the video stream.);
		BIND_DESCRIPTIONDOUBLE(aValue, MF_E_VIDEO_REN_COPYPROT_FAILED, L"A video stream requires copy protection to be enabled, but there was a failure in attempting to enable copy protection.");
		BIND_DESCRIPTION(aValue, MF_E_VIDEO_REN_SURFACE_NOT_SHARED, A component is attempting to access a surface for sharing that is not shared.);
		BIND_DESCRIPTION(aValue, MF_E_VIDEO_DEVICE_LOCKED, A component is attempting to access a shared device that is already locked by another component.);
		BIND_DESCRIPTION(aValue, MF_E_NEW_VIDEO_DEVICE, The device is no longer available.The handle should be closed and a new one opened.);
		BIND_DESCRIPTION(aValue, MF_E_NO_VIDEO_SAMPLE_AVAILABLE, A video sample is not currently queued on a stream that is required for mixing.);
		BIND_DESCRIPTION(aValue, MF_E_NO_AUDIO_PLAYBACK_DEVICE, No audio playback device was found.);
		BIND_DESCRIPTION(aValue, MF_E_AUDIO_PLAYBACK_DEVICE_IN_USE, The requested audio playback device is currently in use.);
		BIND_DESCRIPTION(aValue, MF_E_AUDIO_PLAYBACK_DEVICE_INVALIDATED, The audio playback device is no longer present.);
		BIND_DESCRIPTION(aValue, MF_E_AUDIO_SERVICE_NOT_RUNNING, The audio service is not running.);
		BIND_DESCRIPTION(aValue, MF_E_TOPO_INVALID_OPTIONAL_NODE, The topology contains an invalid optional node.Possible reasons are an incorrect number of outputs and inputs or an optional node is at the beginning or end of a segment.);
		BIND_DESCRIPTION(aValue, MF_E_TOPO_CANNOT_FIND_DECRYPTOR, No suitable transform was found to decrypt the content.);
		BIND_DESCRIPTION(aValue, MF_E_TOPO_CODEC_NOT_FOUND, No suitable transform was found to encode or decode the content.);
		BIND_DESCRIPTION(aValue, MF_E_TOPO_CANNOT_CONNECT, Unable to find a way to connect nodes.);
		BIND_DESCRIPTION(aValue, MF_E_TOPO_UNSUPPORTED, Unsupported operations in the topoloader.);
		BIND_DESCRIPTION(aValue, MF_E_TOPO_INVALID_TIME_ATTRIBUTES, The topology or its nodes contain incorrectly set time attributes.);
		BIND_DESCRIPTIONDOUBLE(aValue, MF_E_TOPO_LOOPS_IN_TOPOLOGY, L"The topology contains loops, which are unsupported in media foundation topologies.");
		BIND_DESCRIPTION(aValue, MF_E_TOPO_MISSING_PRESENTATION_DESCRIPTOR, A source stream node in the topology does not have a presentation descriptor.);
		BIND_DESCRIPTION(aValue, MF_E_TOPO_MISSING_STREAM_DESCRIPTOR, A source stream node in the topology does not have a stream descriptor.);
		BIND_DESCRIPTION(aValue, MF_E_TOPO_STREAM_DESCRIPTOR_NOT_SELECTED, A stream descriptor was set on a source stream node but it was not selected on the presentation descriptor.);
		BIND_DESCRIPTION(aValue, MF_E_TOPO_MISSING_SOURCE, A source stream node in the topology does not have a source.);
		BIND_DESCRIPTION(aValue, MF_E_TOPO_SINK_ACTIVATES_UNSUPPORTED, The topology loader does not support sink activates on output nodes.);
		BIND_DESCRIPTION(aValue, MF_E_SEQUENCER_UNKNOWN_SEGMENT_ID, The sequencer cannot find a segment with the given ID.);
		BIND_DESCRIPTION(aValue, MF_E_NO_SOURCE_IN_CACHE, Cannot find source in the source cache.);
		BIND_DESCRIPTION(aValue, MF_E_TRANSFORM_TYPE_NOT_SET, A valid type has not been set for this stream or a stream that it depends on.);
		BIND_DESCRIPTION(aValue, MF_E_TRANSFORM_STREAM_CHANGE, A stream change has occurred.Output cannot be produced until the streams have been renegotiated.);
		BIND_DESCRIPTION(aValue, MF_E_TRANSFORM_INPUT_REMAINING, The transform cannot take the requested action until all of the input data it currently holds is processed or flushed.);
		BIND_DESCRIPTION(aValue, MF_E_TRANSFORM_PROFILE_MISSING, The transform requires a profile but no profile was supplied or found.);
		BIND_DESCRIPTION(aValue, MF_E_TRANSFORM_PROFILE_INVALID_OR_CORRUPT, The transform requires a profile but the supplied profile was invalid or corrupt.);
		BIND_DESCRIPTION(aValue, MF_E_TRANSFORM_PROFILE_TRUNCATED, The transform requires a profile but the supplied profile ended unexpectedly while parsing.);
		BIND_DESCRIPTION(aValue, MF_E_TRANSFORM_PROPERTY_PID_NOT_RECOGNIZED, The property ID does not match any property supported by the transform.);
		BIND_DESCRIPTION(aValue, MF_E_TRANSFORM_PROPERTY_VARIANT_TYPE_WRONG, The variant does not have the type expected for this property ID.);
		BIND_DESCRIPTION(aValue, MF_E_TRANSFORM_PROPERTY_NOT_WRITEABLE, An attempt was made to set the value on a read - only property.);
		BIND_DESCRIPTION(aValue, MF_E_TRANSFORM_PROPERTY_ARRAY_VALUE_WRONG_NUM_DIM, The array property value has an unexpected number of dimensions.);
		BIND_DESCRIPTION(aValue, MF_E_TRANSFORM_PROPERTY_VALUE_SIZE_WRONG, The array or blob property value has an unexpected size.);
		BIND_DESCRIPTION(aValue, MF_E_TRANSFORM_PROPERTY_VALUE_OUT_OF_RANGE, The property value is out of range for this transform.);
		BIND_DESCRIPTION(aValue, MF_E_TRANSFORM_PROPERTY_VALUE_INCOMPATIBLE, The property value is incompatible with some other property or media type set on the transform.);
		BIND_DESCRIPTION(aValue, MF_E_TRANSFORM_NOT_POSSIBLE_FOR_CURRENT_OUTPUT_MEDIATYPE, The requested operation is not supported for the currently set output media type.);
		BIND_DESCRIPTION(aValue, MF_E_TRANSFORM_NOT_POSSIBLE_FOR_CURRENT_INPUT_MEDIATYPE, The requested operation is not supported for the currently set input media type.);
		BIND_DESCRIPTION(aValue, MF_E_TRANSFORM_NOT_POSSIBLE_FOR_CURRENT_MEDIATYPE_COMBINATION, The requested operation is not supported for the currently set combination of media types.);
		BIND_DESCRIPTION(aValue, MF_E_TRANSFORM_CONFLICTS_WITH_OTHER_CURRENTLY_ENABLED_FEATURES, The requested feature is not supported in combination with some other currently enabled feature.);
		BIND_DESCRIPTION(aValue, MF_E_TRANSFORM_NEED_MORE_INPUT, The transform cannot produce output until it gets more input samples.);
		BIND_DESCRIPTION(aValue, MF_E_TRANSFORM_NOT_POSSIBLE_FOR_CURRENT_SPKR_CONFIG, The requested operation is not supported for the current speaker configuration.);
		BIND_DESCRIPTION(aValue, MF_E_TRANSFORM_CANNOT_CHANGE_MEDIATYPE_WHILE_PROCESSING, The transform cannot accept media type changes in the middle of processing.);
		BIND_DESCRIPTION(aValue, MF_E_UNSUPPORTED_D3D_TYPE, The input type is not supported for D3D device.);
		BIND_DESCRIPTION(aValue, MF_E_TRANSFORM_ASYNC_LOCKED, The caller does not appear to support this transforms asynchronous capabilities.);
		BIND_DESCRIPTION(aValue, MF_E_TRANSFORM_CANNOT_INITIALIZE_ACM_DRIVER, An audio compression manager driver could not be initialized by the transform.);
		BIND_DESCRIPTION(aValue, MF_E_LICENSE_INCORRECT_RIGHTS, You are not allowed to open this file.Contact the content provider for further assistance.);
		BIND_DESCRIPTION(aValue, MF_E_LICENSE_OUTOFDATE, The license for this media file has expired.Get a new license or contact the content provider for further assistance.);
		BIND_DESCRIPTION(aValue, MF_E_LICENSE_REQUIRED, You need a license to perform the requested operation on this media file.);
		BIND_DESCRIPTION(aValue, MF_E_DRM_HARDWARE_INCONSISTENT, The licenses for your media files are corrupted.Contact Microsoft product support.);
		BIND_DESCRIPTION(aValue, MF_E_NO_CONTENT_PROTECTION_MANAGER, The APP needs to provide an IMFContentProtectionManager callback to access the protected media file.);
		BIND_DESCRIPTION(aValue, MF_E_LICENSE_RESTORE_NO_RIGHTS, Client does not have rights to restore licenses.);
		BIND_DESCRIPTION(aValue, MF_E_BACKUP_RESTRICTED_LICENSE, Licenses are restricted and hence cannot be backed up.);
		BIND_DESCRIPTION(aValue, MF_E_LICENSE_RESTORE_NEEDS_INDIVIDUALIZATION, License restore requires the machine to be individualized.);
		BIND_DESCRIPTION(aValue, MF_E_COMPONENT_REVOKED, Component is revoked.);
		BIND_DESCRIPTION(aValue, MF_E_TRUST_DISABLED, Trusted functionality is currently disabled on this component.);
		BIND_DESCRIPTION(aValue, MF_E_WMDRMOTA_NO_ACTION, No Action is set on WMDRM Output Trust Authority.);
		BIND_DESCRIPTION(aValue, MF_E_WMDRMOTA_ACTION_ALREADY_SET, Action is already set on WMDRM Output Trust Authority.);
		BIND_DESCRIPTION(aValue, MF_E_WMDRMOTA_DRM_HEADER_NOT_AVAILABLE, DRM Header is not available.);
		BIND_DESCRIPTION(aValue, MF_E_WMDRMOTA_DRM_ENCRYPTION_SCHEME_NOT_SUPPORTED, Current encryption scheme is not supported.);
		BIND_DESCRIPTION(aValue, MF_E_WMDRMOTA_ACTION_MISMATCH, Action does not match the current configuration.);
		BIND_DESCRIPTION(aValue, MF_E_WMDRMOTA_INVALID_POLICY, Invalid policy for WMDRM Output Trust Authority.);
		BIND_DESCRIPTION(aValue, MF_E_POLICY_UNSUPPORTED, The policies that the Input Trust Authority requires to be enforced are unsupported by the outputs.);
		BIND_DESCRIPTION(aValue, MF_E_OPL_NOT_SUPPORTED, The OPL that the license requires to be enforced are not supported by the Input Trust Authority.);
		BIND_DESCRIPTION(aValue, MF_E_TOPOLOGY_VERIFICATION_FAILED, The topology could not be successfully verified.);
		BIND_DESCRIPTION(aValue, MF_E_SIGNATURE_VERIFICATION_FAILED, Signature verification could not be completed successfully for this component.);
		BIND_DESCRIPTION(aValue, MF_E_DEBUGGING_NOT_ALLOWED, Running this process under a debugger while using protected content is not allowed.);
		BIND_DESCRIPTION(aValue, MF_E_CODE_EXPIRED, MF component has expired.);
		BIND_DESCRIPTION(aValue, MF_E_GRL_VERSION_TOO_LOW, The current GRL on the machine does not meet the minimum version requirements.);
		BIND_DESCRIPTION(aValue, MF_E_GRL_RENEWAL_NOT_FOUND, The current GRL on the machine does not contain any renewal entries for the specified revocation.);
		BIND_DESCRIPTION(aValue, MF_E_GRL_EXTENSIBLE_ENTRY_NOT_FOUND, The current GRL on the machine does not contain any extensible entries for the specified extension GUID.);
		BIND_DESCRIPTION(aValue, MF_E_KERNEL_UNTRUSTED, The kernel is not secure for high security level content.);
		BIND_DESCRIPTION(aValue, MF_E_PEAUTH_UNTRUSTED, The response from the protected environment driver is not valid.);
		BIND_DESCRIPTION(aValue, MF_E_NON_PE_PROCESS, A non - PE process tried to talk to PEAuth.);
		BIND_DESCRIPTION(aValue, MF_E_REBOOT_REQUIRED, We need to reboot the machine.);
		BIND_DESCRIPTION(aValue, MF_E_GRL_INVALID_FORMAT, The GRL file is not correctly formed : it may have been corrupted or overwritten.);
		BIND_DESCRIPTION(aValue, MF_E_GRL_UNRECOGNIZED_FORMAT, The GRL file is in a format newer than those recognized by this GRL Reader.);
		BIND_DESCRIPTION(aValue, MF_E_ALL_PROCESS_RESTART_REQUIRED, The GRL was reloaded and required all processes that can run protected media to restart.);
		BIND_DESCRIPTION(aValue, MF_E_PROCESS_RESTART_REQUIRED, The GRL was reloaded and the current process needs to restart.);
		BIND_DESCRIPTION(aValue, MF_E_USERMODE_UNTRUSTED, The user space is untrusted for protected content play.);
		BIND_DESCRIPTION(aValue, MF_E_PEAUTH_SESSION_NOT_STARTED, PEAuth communication session has not been started.);
		BIND_DESCRIPTION(aValue, MF_E_INSUFFICIENT_BUFFER, Insufficient memory for response.);
		BIND_DESCRIPTION(aValue, MF_E_PEAUTH_PUBLICKEY_REVOKED, PEAuths public key is revoked.);
		BIND_DESCRIPTION(aValue, MF_E_GRL_ABSENT, The GRL is absent.);
		BIND_DESCRIPTION(aValue, MF_E_PE_UNTRUSTED, The Protected Environment is untrusted.);
		BIND_DESCRIPTION(aValue, MF_E_PEAUTH_NOT_STARTED, The Protected Environment Authorization service(PEAUTH) has not been started.);
		BIND_DESCRIPTION(aValue, MF_E_INCOMPATIBLE_SAMPLE_PROTECTION, The sample protection algorithms supported by components are not compatible.);
		BIND_DESCRIPTION(aValue, MF_E_PE_SESSIONS_MAXED, No more protected environment sessions can be supported.);
		BIND_DESCRIPTION(aValue, MF_E_HIGH_SECURITY_LEVEL_CONTENT_NOT_ALLOWED, WMDRM ITA does not allow protected content with high security level for this release.);
		BIND_DESCRIPTION(aValue, MF_E_TEST_SIGNED_COMPONENTS_NOT_ALLOWED, WMDRM ITA cannot allow the requested action for the content as one or more components are not properly signed.);
		BIND_DESCRIPTION(aValue, MF_E_ITA_UNSUPPORTED_ACTION, WMDRM ITA does not support the requested action.);
		BIND_DESCRIPTION(aValue, MF_E_ITA_ERROR_PARSING_SAP_PARAMETERS, WMDRM ITA encountered an error in parsing the Secure Audio Path parameters.);
		BIND_DESCRIPTION(aValue, MF_E_POLICY_MGR_ACTION_OUTOFBOUNDS, The Policy Manager action passed in is invalid.);
		BIND_DESCRIPTION(aValue, MF_E_BAD_OPL_STRUCTURE_FORMAT, The structure specifying Output Protection Level is not the correct format.);
		BIND_DESCRIPTION(aValue, MF_E_ITA_UNRECOGNIZED_ANALOG_VIDEO_PROTECTION_GUID, WMDRM ITA does not recognize the Explicit Analog Video Output Protection guid specified in the license.);
		BIND_DESCRIPTION(aValue, MF_E_NO_PMP_HOST, IMFPMPHost object not available.);
		BIND_DESCRIPTION(aValue, MF_E_ITA_OPL_DATA_NOT_INITIALIZED, WMDRM ITA could not initialize the Output Protection Level data.);
		BIND_DESCRIPTION(aValue, MF_E_ITA_UNRECOGNIZED_ANALOG_VIDEO_OUTPUT, WMDRM ITA does not recognize the Analog Video Output specified by the OTA.);
		BIND_DESCRIPTION(aValue, MF_E_ITA_UNRECOGNIZED_DIGITAL_VIDEO_OUTPUT, WMDRM ITA does not recognize the Digital Video Output specified by the OTA.);
		BIND_DESCRIPTION(aValue, MF_E_CLOCK_INVALID_CONTINUITY_KEY, The continuity key supplied is not currently valid.);
		BIND_DESCRIPTION(aValue, MF_E_CLOCK_NO_TIME_SOURCE, No Presentation Time Source has been specified.);
		BIND_DESCRIPTION(aValue, MF_E_CLOCK_STATE_ALREADY_SET, The clock is already in the requested state.);
		BIND_DESCRIPTION(aValue, MF_E_CLOCK_NOT_SIMPLE, The clock has too many advanced features to carry out the request.);
		BIND_DESCRIPTION(aValue, MF_E_NO_MORE_DROP_MODES, The component does not support any more drop modes.);
		BIND_DESCRIPTION(aValue, MF_E_NO_MORE_QUALITY_LEVELS, The component does not support any more quality levels.);
		BIND_DESCRIPTION(aValue, MF_E_DROPTIME_NOT_SUPPORTED, The component does not support drop - time functionality.);
		BIND_DESCRIPTION(aValue, MF_E_QUALITYKNOB_WAIT_LONGER, The Quality Manager needs to wait longer before bumping the Quality Level up.);
		BIND_DESCRIPTION(aValue, MF_E_QM_INVALIDSTATE, The Quality Manager is in an invalid state.Quality Management is off at this moment.);
		BIND_DESCRIPTION(aValue, MF_E_TRANSCODE_NO_CONTAINERTYPE, No transcode output container type is specified.);
		BIND_DESCRIPTION(aValue, MF_E_TRANSCODE_PROFILE_NO_MATCHING_STREAMS, The profile does not have a media type configuration for any selected source streams.);
		BIND_DESCRIPTION(aValue, MF_E_TRANSCODE_NO_MATCHING_ENCODER,	Cannot find an encoder MFT that accepts the user preferred output type.);
		BIND_DESCRIPTION(aValue, MF_E_ALLOCATOR_NOT_INITIALIZED, Memory allocator is not initialized.);
		BIND_DESCRIPTION(aValue, MF_E_ALLOCATOR_NOT_COMMITED, Memory allocator is not committed yet.);
		BIND_DESCRIPTION(aValue, MF_E_ALLOCATOR_ALREADY_COMMITED, Memory allocator has already been committed.);
		BIND_DESCRIPTION(aValue, MF_E_STREAM_ERROR, An error occurred in the media stream.);
		BIND_DESCRIPTION(aValue, MF_E_INVALID_STREAM_STATE, The stream is not in a state to handle the request.);
		BIND_DESCRIPTION(aValue, MF_E_HW_STREAM_NOT_CONNECTED, The hardware stream is not connected yet.);
		BIND_DESCRIPTION(aValue, MF_S_MULTIPLE_BEGIN, This callback and state had already been passed in to this event generator earlier.);
		BIND_DESCRIPTION(aValue, MF_S_ACTIVATE_REPLACED, The activate could not be created in the remote process for some reason : it was replaced with empty one.);
		BIND_DESCRIPTION(aValue, MF_S_SINK_NOT_FINALIZED, The sink has not been finalized before shut down.This can cause the sink to generate corrupted content.);
		BIND_DESCRIPTION(aValue, MF_S_SEQUENCER_CONTEXT_CANCELED, The context was canceled.);
		BIND_DESCRIPTION(aValue, MF_S_SEQUENCER_SEGMENT_AT_END_OF_STREAM, Cannot update topology flags.);
		BIND_DESCRIPTION(aValue, MF_S_TRANSFORM_DO_NOT_PROPAGATE_EVENT, The caller should not propagate this event to downstream components.);
		BIND_DESCRIPTION(aValue, MF_S_PROTECTION_NOT_REQUIRED, Protection for stream is not required.);
		BIND_DESCRIPTION(aValue, MF_S_WAIT_FOR_POLICY_SET, Protection for this stream is not guaranteed to be enforced until the MEPolicySet event is fired.);
		BIND_DESCRIPTION(aValue, MF_S_VIDEO_DISABLED_WITH_UNKNOWN_SOFTWARE_OUTPUT, This video stream is disabled because it is being sent to an unknown software output.);
		BIND_DESCRIPTION(aValue, MF_S_PE_TRUSTED, The Protected Environment is trusted.);
		BIND_DESCRIPTION(aValue, MF_S_CLOCK_STOPPED, Timer::SetTimer returns this success code if call happened while timer is stopped.Timer is not going to be dispatched until clock is running.);
		BIND_DESCRIPTION(aValue, MF_S_ASF_PARSEINPROGRESS, Parsing is still in progress and is not yet complete.);
		BIND_DESCRIPTION(aValue, MF_I_MANUAL_PROXY, The proxy setting is manual.);
				
		return L"Unknown Error.";
	}
	
	HRESULT DataParser::readMediaType(
		IUnknown* aPtrMediaType,
		pugi::xml_node& aRefRootAttributeNode)
	{
		HRESULT lresult;

		using namespace Core;

		do
		{
			if (aPtrMediaType == nullptr)
			{
				lresult = E_POINTER;

				break;
			}

			CComQIPtrCustom<IMFMediaType> lMediaType(aPtrMediaType);

			if (!lMediaType)
			{
				lresult = E_INVALIDARG;

				break;
			}

			AttributesLock lLock(lMediaType);

			lresult = lLock.getResult();			

			if(FAILED(lresult))
				break;

			GUID lMajor_type;


			LOG_INVOKE_MF_METHOD(GetGUID, lMediaType,
				MF_MT_MAJOR_TYPE,
				&lMajor_type);

			GUID lSub_type;

			LOG_INVOKE_MF_METHOD(GetGUID, lMediaType,
				MF_MT_SUBTYPE,
				&lSub_type);

			if (lMajor_type == MFMediaType_Video)
			{

				UINT32 lItemCount = 0;

				LOG_INVOKE_MF_METHOD(GetCount, 
					lMediaType, 
					&lItemCount);

				if (lItemCount == 0)
					break;

				for (UINT32 lItemIndex = 0; lItemIndex < lItemCount; lItemIndex++)
				{

					auto lMediaTypeItemNode = aRefRootAttributeNode.append_child(L"MediaTypeItem");

					lresult = parsVideoMediaFormatAttributesValueByIndex(
						lMediaType, 
						lItemIndex, 
						lMediaTypeItemNode);

					if (lresult == S_FALSE)
					{
						aRefRootAttributeNode.remove_child(lMediaTypeItemNode);
					}
				}
			}
			else if (lMajor_type == MFMediaType_Audio)
			{

				UINT32 lItemCount = 0;

				LOG_INVOKE_MF_METHOD(GetCount, 
					lMediaType, 
					&lItemCount);

				if (lItemCount == 0)
					break;

				for (UINT32 lItemIndex = 0; lItemIndex < lItemCount; lItemIndex++)
				{

					auto lMediaTypeItemNode = aRefRootAttributeNode.append_child(L"MediaTypeItem");

					lresult = parsAudioMediaFormatAttributesValueByIndex(
						lMediaType,
						lItemIndex,
						lMediaTypeItemNode);

					if (FAILED(lresult) || lresult == S_FALSE)
					{
						aRefRootAttributeNode.remove_child(lMediaTypeItemNode);
					}
				}
			}
			else
			{
				lresult = E_UNEXPECTED;
			}


		} while (false);

		return lresult;
	}

	HRESULT DataParser::readSourceActivate(
		IUnknown* aPtrSourceActivate,
		pugi::xml_node& aRefRootAttributeNode)
	{
		HRESULT lresult;

		using namespace Core;

		do
		{
			if (aPtrSourceActivate == nullptr)
			{
				lresult = E_POINTER;

				break;
			}

			CComQIPtrCustom<IMFAttributes> lMediaType(aPtrSourceActivate);

			if (!lMediaType)
			{
				lresult = E_INVALIDARG;

				break;
			}



			PROPVARIANT lvar;

			PropVariantInit(&lvar);

			lresult = lMediaType->GetItem(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_HW_SOURCE, &lvar);

			PropVariantClear(&lvar);

			if (FAILED(lresult))
			{				
				PropVariantInit(&lvar);

				lresult = lMediaType->GetItem(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_HW_SOURCE, &lvar);

				PropVariantClear(&lvar);

				if (FAILED(lresult))
				{
					lMediaType->SetUINT32(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_HW_SOURCE, TRUE);
				}
			}					

			AttributesLock lLock(lMediaType);

			lresult = lLock.getResult();

			if(FAILED(lresult))
				break;


			UINT32 lItemCount = 0;

			LOG_INVOKE_MF_METHOD(GetCount,
				lMediaType, 
				&lItemCount);

			aRefRootAttributeNode.append_attribute(L"Title") = L"Attributes of Source";

			for (UINT32 lItemIndex = 0; lItemIndex < lItemCount; lItemIndex++)
			{
				lresult = parsSourceActivateAttributeValueByIndex(
					lMediaType, 
					lItemIndex, 
					aRefRootAttributeNode.append_child(L"Attribute"));
			}			

		} while (false);

		return lresult;
	}

	HRESULT DataParser::readPresentationDescriptor(
		IUnknown* aPtrPresentationDescriptor,
		pugi::xml_node& aRefRootAttributeNode)
	{
		HRESULT lresult;

		using namespace Core;

		do
		{
			if (aPtrPresentationDescriptor == nullptr)
			{
				lresult = E_POINTER;

				break;
			}

			CComQIPtrCustom<IMFAttributes> lMediaType(aPtrPresentationDescriptor);

			if (!lMediaType)
			{
				lresult = E_INVALIDARG;

				break;
			}

			AttributesLock lLock(lMediaType);

			lresult = lLock.getResult();

			if(FAILED(lresult))
				break;


			UINT32 lItemCount = 0;

			LOG_INVOKE_MF_METHOD(GetCount, lMediaType, &lItemCount);

			if (lItemCount == 0)
				break;

			auto lAttributes = aRefRootAttributeNode.append_child(L"PresentationDescriptor.Attributes");

			lAttributes.append_attribute(L"Title") = L"Attributes of Presentation";

			for (UINT32 lItemIndex = 0; lItemIndex < lItemCount; lItemIndex++)
			{
				auto lAttribute = lAttributes.append_child(L"Attribute");

				lresult = parsPresentationDescriptorAttributeValueByIndex(
					lMediaType,
					lItemIndex,
					lAttribute);


				if (lresult == S_FALSE || FAILED(lresult))
				{					
					lAttributes.remove_child(lAttribute);

					lresult = S_OK;
				}
			}

		} while (false);

		return lresult;
	}

	HRESULT DataParser::readStreamDescriptor(
		IUnknown* aPtrStreamDescriptor,
		pugi::xml_node& aRefRootAttributeNode)
	{
		HRESULT lresult;

		using namespace Core;

		do
		{
			if (aPtrStreamDescriptor == nullptr)
			{
				lresult = E_POINTER;

				break;
			}

			CComQIPtrCustom<IMFAttributes> lMFAttributes(aPtrStreamDescriptor);

			if (!lMFAttributes)
			{
				lresult = E_INVALIDARG;

				break;
			}

			AttributesLock lLock(lMFAttributes);

			lresult = lLock.getResult();

			if(FAILED(lresult))
				break;


			UINT32 lItemCount = 0;

			LOG_INVOKE_MF_METHOD(GetCount, lMFAttributes, &lItemCount);

			if (lItemCount == 0)
				break;

			auto lAttributes = aRefRootAttributeNode.append_child(L"StreamDescriptor.Attributes");

			lAttributes.append_attribute(L"Title") = L"Attributes of Stream";

			for (UINT32 lItemIndex = 0; lItemIndex < lItemCount; lItemIndex++)
			{
				auto lAttribute = lAttributes.append_child(L"Attribute");

				lresult = parsStreamDescriptorAttributeValueByIndex(
					lMFAttributes,
					lItemIndex,
					lAttribute);

				if(FAILED(lresult))
				{
					aRefRootAttributeNode.remove_child(lAttribute);

					lresult = S_OK;
				}
			}

		} while (false);

		return lresult;
	}

	HRESULT DataParser::parsSourceActivateAttributeValueByIndex(
		IUnknown* aPtrAttributes,
		DWORD aIndex,
		pugi::xml_node& aRefAttributeNode)
	{
		HRESULT lresult;

		using namespace Core;

		std::wstring guidValName;

		GUID lguid = { 0 };

		PROPVARIANT lvar;
		PropVariantInit(&lvar);

		do
		{

			if (aPtrAttributes == nullptr)
			{
				lresult = E_POINTER;

				break;
			}

			CComQIPtrCustom<IMFAttributes> lAttributes(aPtrAttributes);

			if (!lAttributes)
			{
				lresult = E_INVALIDARG;

				break;
			}

			LOG_INVOKE_MF_METHOD(GetItemByIndex, 
				lAttributes,
				aIndex,
				&lguid,
				&lvar);
			
			std::wstring lNameGUID;

			lresult = GetGUIDName(lguid, lNameGUID);

			if(FAILED(lresult))
				break;

			WCHAR *lptrName = nullptr;

			aRefAttributeNode.append_attribute(L"Name") = lNameGUID.c_str();

			lresult = StringFromCLSID(lguid, &lptrName);

			if(FAILED(lresult))
				break;

			aRefAttributeNode.append_attribute(L"GUID") = lptrName;

			CoTaskMemFree(lptrName);

			do
			{

				lresult = parsCaptureDeviceAttributeValue(
					lguid,
					lvar,
					aRefAttributeNode);
				
			} while (false);
			
		} while (false);

		PropVariantClear(&lvar);

		return lresult;
	}

	HRESULT DataParser::parsStreamDescriptorAttributeValueByIndex(
		IUnknown* aPtrAttributes,
		DWORD aIndex,
		pugi::xml_node& aRefAttributeNode)
	{
		HRESULT lresult;

		using namespace Core;

		std::wstring guidValName;

		GUID lguid = { 0 };

		PROPVARIANT lvar;
		PropVariantInit(&lvar);

		do
		{

			if (aPtrAttributes == nullptr)
			{
				lresult = E_POINTER;

				break;
			}

			CComQIPtrCustom<IMFAttributes> lAttributes(aPtrAttributes);

			if (!lAttributes)
			{
				lresult = E_INVALIDARG;

				break;
			}

			LOG_INVOKE_MF_METHOD(GetItemByIndex,
				lAttributes,
				aIndex,
				&lguid,
				&lvar);

			std::wstring lNameGUID;

			lresult = GetGUIDName(lguid, lNameGUID);

			if(FAILED(lresult))
				break;

			WCHAR *lptrName = nullptr;

			aRefAttributeNode.append_attribute(L"Name") = lNameGUID.c_str();

			lresult = StringFromCLSID(lguid, &lptrName);

			if(FAILED(lresult))
				break;

			aRefAttributeNode.append_attribute(L"GUID") = lptrName;

			CoTaskMemFree(lptrName);

			auto lValue = aRefAttributeNode.append_child(L"SingleValue");

			do
			{
				
				if (lguid == MF_SD_LANGUAGE)
				{
					readAttribute(
						lvar,
						lValue);

					aRefAttributeNode.append_attribute(L"Title") = L"Specifies the language for a stream.";

					aRefAttributeNode.append_attribute(L"Description") = L"The value of this attribute is an RFC 1766-compliant language tag. This attribute applies to stream descriptors.";
				}
				else if (lguid == MF_SD_MUTUALLY_EXCLUSIVE)
				{
					readAttribute(
						lvar,
						lValue);

					auto luintValue = lValue.attribute(L"Value").as_uint(FALSE);

					lValue.remove_attribute(L"Value");

					std::wstring lvalue = L"True";

					if (luintValue == FALSE)
					{
						lvalue = L"False";
					}

					lValue.append_attribute(L"Value") = lvalue.c_str();

					aRefAttributeNode.append_attribute(L"Title") = L"The mutual state of the stream.";

					aRefAttributeNode.append_attribute(L"Description") = L"Specifies whether a stream is mutually exclusive with other streams of the same type.";

				}
				else if (lguid == MF_SD_PROTECTED)
				{
					readAttribute(
						lvar,
						lValue);

					auto luintValue = lValue.attribute(L"Value").as_uint(FALSE);

					lValue.remove_attribute(L"Value");

					std::wstring lvalue = L"True";

					if (luintValue == FALSE)
					{
						lvalue = L"False";
					}

					lValue.append_attribute(L"Value") = lvalue.c_str();

					aRefAttributeNode.append_attribute(L"Title") = L"The stream contains protected content.";

					aRefAttributeNode.append_attribute(L"Description") = L"Indicates whether a stream contains protected content.";

				}
				else if (lguid == MF_SD_STREAM_NAME)
				{
					readAttribute(
						lvar,
						lValue);

					aRefAttributeNode.append_attribute(L"Title") = L"The name of a stream.";

					aRefAttributeNode.append_attribute(L"Description") = L"Contains the name of a stream.";

				}
				else if (lguid == MF_DEVICESTREAM_ATTRIBUTE_FRAMESOURCE_TYPES)
				{
					lresult = readAttribute(
						lvar,
						lValue);
					
					if (FAILED(lresult))
					{
						break;
					}

					auto lAttr = lValue.attribute(L"Value");

					auto lv = lAttr.as_uint();

					lValue.remove_attribute(lAttr);

					std::wstring ltype = L"unknown";

					switch (lv)
					{
					case 0x0001:
					{
						ltype = L"MFFrameSourceTypes_Color";
					}
					break;
					case 0x0002:
					{
						ltype = L"MFFrameSourceTypes_Infrared";
					}
					break;
					case 0x0004:
					{
						ltype = L"MFFrameSourceTypes_Depth";
					}
					break;
					case 0x0008:
					{
						ltype = L"MFFrameSourceTypes_Image";
					}
					break;					
					case 0x0080:
					{
						ltype = L"MFFrameSourceTypes_Custom";
					}
					break;

					default:
						break;
					}

					lValue.append_attribute(L"Value") = ltype.c_str();
					
					aRefAttributeNode.append_attribute(L"Title") = L"Represents the frame source type.";

					aRefAttributeNode.append_attribute(L"Description") = L"This value of this attribute should be a bitmask of one or more values from the MFFrameSourceTypes enumeration.";					
				}
				else if (lguid == MF_DEVICESTREAM_STREAM_CATEGORY)
				{
					lresult = readAttribute(
						lvar,
						lValue);

					if (FAILED(lresult))
					{
						break;
					}

					aRefAttributeNode.append_attribute(L"Title") = L"Represents the stream category.";

					aRefAttributeNode.append_attribute(L"Description") = L"This attribute represents the GUID that identifies the stream category.";
				}


				
				else
				{
					lresult = E_FAIL;
				}


			} while (false);
			
		} while (false);

		PropVariantClear(&lvar);

		return lresult;
	}

	HRESULT DataParser::parsPresentationDescriptorAttributeValueByIndex(
		IUnknown* aPtrAttributes, 
		DWORD aIndex,
		pugi::xml_node& aRefAttributeNode)
	{
		HRESULT lresult;

		using namespace Core;

		std::wstring guidValName;

		GUID lguid = { 0 };

		PROPVARIANT lvar;
		PropVariantInit(&lvar);

		do
		{

			if (aPtrAttributes == nullptr)
			{
				lresult = E_POINTER;

				break;
			}

			CComQIPtrCustom<IMFAttributes> lAttributes(aPtrAttributes);

			if (!lAttributes)
			{
				lresult = E_INVALIDARG;

				break;
			}
			
			LOG_INVOKE_MF_METHOD(GetItemByIndex,
				lAttributes,
				aIndex,
				&lguid,
				&lvar);
			
			std::wstring lNameGUID;

			lresult = GetGUIDName(lguid, lNameGUID);

			if(FAILED(lresult))
				break;

			WCHAR *lptrName = nullptr;

			aRefAttributeNode.append_attribute(L"Name") = lNameGUID.c_str();

			lresult = StringFromCLSID(lguid, &lptrName);

			if(FAILED(lresult))
				break;

			aRefAttributeNode.append_attribute(L"GUID") = lptrName;

			CoTaskMemFree(lptrName);

			do
			{
				if (lguid == MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME)
				{
					readAttribute(
						lvar,
						aRefAttributeNode.append_child(L"SingleValue"));

					aRefAttributeNode.append_attribute(L"Title") = L"The display name for a device.";

					aRefAttributeNode.append_attribute(L"Description") = L"The display name is a human-readable string, suitable for display in a user interface.";

				}
				else if (lguid == MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK)
				{
					readAttribute(
						lvar,
						aRefAttributeNode.append_child(L"SingleValue"));

					aRefAttributeNode.append_attribute(L"Title") = L"The symbolic link for a video capture driver.";

					aRefAttributeNode.append_attribute(L"Description") = L"Contains the unique symbolic link for a video capture driver.";
				}
				else if (lguid == MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_SYMBOLIC_LINK && lvar.vt == VT_LPWSTR)
				{
					readAttribute(
						lvar,
						aRefAttributeNode.append_child(L"SingleValue"));

					aRefAttributeNode.append_attribute(L"Title") = L"The symbolic link for a audio capture driver.";

					aRefAttributeNode.append_attribute(L"Description") = L"Contains the unique symbolic link for a audio capture driver.";
				}
				else if (lguid == MF_PD_AUDIO_ENCODING_BITRATE)
				{
					readAttribute(
						lvar,
						aRefAttributeNode.append_child(L"SingleValue"));

					aRefAttributeNode.append_attribute(L"Title") = L"Audio encoding bit rate.";

					aRefAttributeNode.append_attribute(L"Description") = L"Specifies the audio encoding bit rate for the presentation, in bits per second";
				}
				else if (lguid == MF_PD_AUDIO_ISVARIABLEBITRATE)
				{
					readAttribute(
						lvar,
						aRefAttributeNode.append_child(L"SingleValue"),
						true);

					aRefAttributeNode.append_attribute(L"Title") = L"Audio encoding bit rate is variable.";

					aRefAttributeNode.append_attribute(L"Description") = L"Specifies whether the audio streams in a presentation have a variable bit rate.";

				}
				else if (lguid == MF_PD_DURATION)
				{
					readAttribute(
						lvar,
						aRefAttributeNode.append_child(L"SingleValue"));

					aRefAttributeNode.append_attribute(L"Title") = L"Duration of a presentation.";

					aRefAttributeNode.append_attribute(L"Description") = L"Specifies the duration of a presentation, in 100-nanosecond units.";

				}
				else if (lguid == MF_PD_LAST_MODIFIED_TIME)
				{
					FILETIME ftWrite;
					SYSTEMTIME stUTC, stLocal;

					DWORD dwRet;

					ftWrite.dwLowDateTime = lvar.uhVal.LowPart;

					ftWrite.dwHighDateTime = lvar.uhVal.HighPart;

					// Convert the last-write time to local time.
					if (FileTimeToSystemTime(&ftWrite, &stUTC) == FALSE)
					{
						lresult = E_FAIL;

						break;
					}

					if (SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal) == FALSE)
					{
						lresult = E_FAIL;

						break;
					}
					

					TCHAR szBuf[MAX_PATH];

					// Build a string showing the date and time.
					dwRet = StringCchPrintf(szBuf, MAX_PATH,
						TEXT("%02d/%02d/%d  %02d:%02d"),
						stLocal.wMonth, stLocal.wDay, stLocal.wYear,
						stLocal.wHour, stLocal.wMinute);

					auto lValue = aRefAttributeNode.append_child(L"SingleValue");

					lValue.append_attribute(L"Title") = L"Time";

					if (S_OK == dwRet)
					{
						lValue.append_attribute(L"Value") = szBuf;
					}
					else
					{
						lValue.append_attribute(L"Value") = L"Undefined";
					}
					
					aRefAttributeNode.append_attribute(L"Title") = L"The date of last modification.";

					aRefAttributeNode.append_attribute(L"Description") = L"Specifies when a presentation was last modified.";

				}
				else if (lguid == MF_PD_MIME_TYPE)
				{
					readAttribute(
						lvar,
						aRefAttributeNode.append_child(L"SingleValue"));

					aRefAttributeNode.append_attribute(L"Title") = L"MIME type.";

					aRefAttributeNode.append_attribute(L"Description") = L"Specifies the MIME type of the content.";

				}
				else if (lguid == MF_PD_PLAYBACK_BOUNDARY_TIME)
				{
					FILETIME ftWrite;
					SYSTEMTIME stUTC = { 0 }, stLocal = { 0 };
					DWORD dwRet;

					ftWrite.dwLowDateTime = lvar.uhVal.LowPart;

					ftWrite.dwHighDateTime = lvar.uhVal.HighPart;

					// Convert the last-write time to local time.
					if (FileTimeToSystemTime(&ftWrite, &stUTC) == FALSE)
					{
						lresult = E_FAIL;

						break;
					}

					if (SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal) == FALSE)
					{
						lresult = E_FAIL;

						break;
					}

					TCHAR szBuf[MAX_PATH];

					// Build a string showing the date and time.
					dwRet = StringCchPrintf(szBuf, MAX_PATH,
						TEXT("%02d/%02d/%d  %02d:%02d"),
						stLocal.wMonth, stLocal.wDay, stLocal.wYear,
						stLocal.wHour, stLocal.wMinute);

					auto lValue = aRefAttributeNode.append_child(L"SingleValue");

					lValue.append_attribute(L"Title") = L"Time";

					if (S_OK == dwRet)
					{
						lValue.append_attribute(L"Value") = szBuf;
					}
					else
					{
						lValue.append_attribute(L"Value") = L"Undefined";
					}

					aRefAttributeNode.append_attribute(L"Title") = L"The beginning time of the presentation.";

					aRefAttributeNode.append_attribute(L"Description") = L"The time at which the presentation must begin, relative to the start of the media source.";

				}
				else if (lguid == MF_PD_PLAYBACK_ELEMENT_ID)
				{
					readAttribute(
						lvar,
						aRefAttributeNode.append_child(L"SingleValue"));

					aRefAttributeNode.append_attribute(L"Title") = L"The identifier of the playlist element in the presentation.";

					aRefAttributeNode.append_attribute(L"Description") = L"Contains the identifier of the playlist element in the presentation.";

				}
				else if (lguid == MF_PD_PREFERRED_LANGUAGE)
				{
					readAttribute(
						lvar,
						aRefAttributeNode.append_child(L"SingleValue"));

					aRefAttributeNode.append_attribute(L"Title") = L"The preferred RFC 1766 language of the media source.";

					aRefAttributeNode.append_attribute(L"Description") = L"Contains the preferred RFC 1766 language of the media source.";

				}
				else if (lguid == MF_PD_TOTAL_FILE_SIZE)
				{
					readAttribute(
						lvar,
						aRefAttributeNode.append_child(L"SingleValue"));

					aRefAttributeNode.append_attribute(L"Title") = L"The total size of the source file, in bytes.";

					aRefAttributeNode.append_attribute(L"Description") = L"Specifies the total size of the source file, in bytes. This attribute applies to presentation descriptors. A media source can optionally set this attribute.";

				}
				else if (lguid == MF_PD_VIDEO_ENCODING_BITRATE)
				{
					readAttribute(
						lvar,
						aRefAttributeNode.append_child(L"SingleValue"));

					aRefAttributeNode.append_attribute(L"Title") = L"The video encoding bit rate for the presentation, in bits per second.";

					aRefAttributeNode.append_attribute(L"Description") = L"Specifies the video encoding bit rate for the presentation, in bits per second. This attribute applies to presentation descriptors.";

				}
				else
				{
					lresult = E_FAIL;
				}

				if(FAILED(lresult))
				{
					break;
				}

				
			} while (false);

			if(FAILED(lresult))
			{
				break;						
			}

		} while (false);

		PropVariantClear(&lvar);

		return lresult;
	}

	HRESULT DataParser::parsVideoMediaFormatAttributesValueByIndex(
		IUnknown* aPtrAttributes,
		DWORD aIndex,
		pugi::xml_node& aRefAttributeNode)
	{
		HRESULT lresult;

		using namespace Core;

		std::wstring guidValName;

		GUID lguid = { 0 };

		PROPVARIANT lvar;
		PropVariantInit(&lvar);

		do
		{

			if (aPtrAttributes == nullptr)
			{
				lresult = E_POINTER;

				break;
			}

			CComQIPtrCustom<IMFAttributes> lAttributes(aPtrAttributes);

			if (!lAttributes)
			{
				lresult = E_INVALIDARG;

				break;
			}

			LOG_INVOKE_MF_METHOD(GetItemByIndex,
				lAttributes,
				aIndex,
				&lguid,
				&lvar);

			std::wstring lNameGUID;

			lresult = GetGUIDName(lguid, lNameGUID);

			if(FAILED(lresult))
				break;

			WCHAR *lptrName = nullptr;

			aRefAttributeNode.append_attribute(L"Name") = lNameGUID.c_str();

			lresult = StringFromCLSID(lguid, &lptrName);

			if(FAILED(lresult))
				break;

			aRefAttributeNode.append_attribute(L"GUID") = lptrName;

			CoTaskMemFree(lptrName);

			do
			{
				lresult = parsGeneralMediaTypeFormatAttributesValueByIndex(
					lguid,
					lvar,
					aRefAttributeNode);

				if (lresult == S_OK)
				{
					break;
				}

				lresult = parsVideoMediaFormatFormatAttributesValue(
					lguid,
					lvar,
					aRefAttributeNode);

			} while (false);

			if(FAILED(lresult))
			{
				break;
			}


		} while (false);

		PropVariantClear(&lvar);

		return lresult;
	}

	HRESULT DataParser::parsAudioMediaFormatAttributesValueByIndex(
		IUnknown* aPtrAttributes,
		DWORD aIndex,
		pugi::xml_node& aRefAttributeNode)
	{
		HRESULT lresult;

		using namespace Core;

		std::wstring guidValName;

		GUID lguid = { 0 };

		PROPVARIANT lvar;
		PropVariantInit(&lvar);

		do
		{

			if (aPtrAttributes == nullptr)
			{
				lresult = E_POINTER;

				break;
			}

			CComQIPtrCustom<IMFAttributes> lAttributes(aPtrAttributes);

			if (!lAttributes)
			{
				lresult = E_INVALIDARG;

				break;
			}

			LOG_INVOKE_MF_METHOD(GetItemByIndex,
				lAttributes,
				aIndex,
				&lguid,
				&lvar);

			std::wstring lNameGUID;

			lresult = GetGUIDName(lguid, lNameGUID);

			if(FAILED(lresult))
				break;

			WCHAR *lptrName = nullptr;

			aRefAttributeNode.append_attribute(L"Name") = lNameGUID.c_str();

			lresult = StringFromCLSID(lguid, &lptrName);

			if(FAILED(lresult))
				break;

			aRefAttributeNode.append_attribute(L"GUID") = lptrName;

			CoTaskMemFree(lptrName);

			do
			{
				lresult = parsGeneralMediaTypeFormatAttributesValueByIndex(
					lguid,
					lvar,
					aRefAttributeNode);

				if (lresult == S_OK)
				{
					break;
				}

				lresult = parsAudioMediaFormatFormatAttributesValue(
					lguid,
					lvar,
					aRefAttributeNode);

			} while (false);

			if(FAILED(lresult))
			{
				break;
			}


		} while (false);

		PropVariantClear(&lvar);

		return lresult;
	}

	HRESULT DataParser::parsVideoMediaFormatFormatAttributesValue(
			REFGUID aGUID,
			const PROPVARIANT& aVar,
			pugi::xml_node& aRefAttributeNode)
	{
		HRESULT lresult;

		using namespace Core;

		do
		{
			lresult = S_OK;
					

			if (aGUID == MF_MT_AVG_BIT_ERROR_RATE)
			{
				readAttribute(
					aVar,
					aRefAttributeNode.append_child(L"SingleValue"),
					true);

				if(FAILED(lresult))
				{
					break;
				}

				aRefAttributeNode.append_attribute(L"Title") = L"Data error rate.";

				aRefAttributeNode.append_attribute(L"Description") = L"Data error rate, in bit errors per second, for a video media type.";

			}
			else if (aGUID == MF_MT_AVG_BITRATE)
			{
				readAttribute(
					aVar,
					aRefAttributeNode.append_child(L"SingleValue"));

				if(FAILED(lresult))
				{
					break;
				}

				aRefAttributeNode.append_attribute(L"Title") = L"Approximate data rate of the video stream.";

				aRefAttributeNode.append_attribute(L"Description") = L"Approximate data rate of the video stream, in bits per second, for a video media type.";

			}
			else if (aGUID == MF_MT_DEFAULT_STRIDE)
			{
				readAttribute(
					aVar,
					aRefAttributeNode.append_child(L"SingleValue"),
					false,
					true);

				if(FAILED(lresult))
				{
					break;
				}

				aRefAttributeNode.append_attribute(L"Title") = L"Default surface stride.";

				aRefAttributeNode.append_attribute(L"Description") = L"Default surface stride, for an uncompressed video media type. Stride is the number of bytes needed to go from one row of pixels to the next.";

			}
			else if (aGUID == MF_MT_DRM_FLAGS)
			{
				auto lValue = aRefAttributeNode.append_child(L"SingleValue");

				lresult = readAttribute(
					aVar,
					lValue);

				if(FAILED(lresult))
				{
					break;
				}

				auto lAttr = lValue.attribute(L"Value");

				auto lv = lAttr.as_uint();

				lValue.remove_attribute(lAttr);

				std::wstring ltype = L"unknown";

				switch (lv)
				{
				case 0:
				{
						  ltype = L"MFVideoDRMFlag_None";
				}
					break;
				case 1:
				{
						  ltype = L"MFVideoDRMFlag_AnalogProtected";
				}
					break;
				case 2:
				{
						  ltype = L"MFVideoDRMFlag_DigitallyProtected";
				}
					break;

				default:
					break;
				}

				lValue.append_attribute(L"Value") = ltype.c_str();

				aRefAttributeNode.append_attribute(L"Title") = L"Specifies whether the video requires enforcing copy protection.";

				aRefAttributeNode.append_attribute(L"Description") = L"Specifies whether a video media type requires the enforcement of copy protection.";

			}
			else if (aGUID == MF_MT_FRAME_RATE)
			{
				auto lRatioValue = aRefAttributeNode.append_child(L"RatioValue");

				UINT32 lHigh = 0, lLow = 0;

				unpack2UINT32AsUINT64(aVar, lHigh, lLow);

				lRatioValue.append_attribute(L"Value") = (float)lHigh / (float)lLow;

				auto ValueParts = lRatioValue.append_child(L"Value.ValueParts");

				auto ValuePart = ValueParts.append_child(L"ValuePart");

				std::wstring lNameGUID;

				ValuePart.append_attribute(L"Title") = L"Numerator";
				
				ValuePart.append_attribute(L"Value") = lHigh;


				ValuePart = ValueParts.append_child(L"ValuePart");

				ValuePart.append_attribute(L"Title") = L"Denominator";

				ValuePart.append_attribute(L"Value") = lLow;

				aRefAttributeNode.append_attribute(L"Title") = L"Frame rate.";

				aRefAttributeNode.append_attribute(L"Description") = L"Frame rate of a video media type, in frames per second.";

			}
			else if (aGUID == MF_MT_FRAME_RATE_RANGE_MAX)
			{
				auto lRatioValue = aRefAttributeNode.append_child(L"RatioValue");

				UINT32 lHigh = 0, lLow = 0;

				unpack2UINT32AsUINT64(aVar, lHigh, lLow);

				lRatioValue.append_attribute(L"Value") = (float)lHigh / (float)lLow;

				auto ValueParts = lRatioValue.append_child(L"Value.ValueParts");

				auto ValuePart = ValueParts.append_child(L"ValuePart");

				std::wstring lNameGUID;

				ValuePart.append_attribute(L"Title") = L"Numerator";

				ValuePart.append_attribute(L"Value") = lHigh;


				ValuePart = ValueParts.append_child(L"ValuePart");

				ValuePart.append_attribute(L"Title") = L"Denominator";

				ValuePart.append_attribute(L"Value") = lLow;

				aRefAttributeNode.append_attribute(L"Title") = L"The maximum frame rate.";

				aRefAttributeNode.append_attribute(L"Description") = L"The maximum frame rate that is supported by a video capture device, in frames per second.";

			}
			else if (aGUID == MF_MT_FRAME_RATE_RANGE_MIN)
			{
				auto lRatioValue = aRefAttributeNode.append_child(L"RatioValue");

				UINT32 lHigh = 0, lLow = 0;

				unpack2UINT32AsUINT64(aVar, lHigh, lLow);

				lRatioValue.append_attribute(L"Value") = (float)lHigh / (float)lLow;

				auto ValueParts = lRatioValue.append_child(L"Value.ValueParts");

				auto ValuePart = ValueParts.append_child(L"ValuePart");

				std::wstring lNameGUID;

				ValuePart.append_attribute(L"Title") = L"Numerator";

				ValuePart.append_attribute(L"Value") = lHigh;


				ValuePart = ValueParts.append_child(L"ValuePart");

				ValuePart.append_attribute(L"Title") = L"Denominator";

				ValuePart.append_attribute(L"Value") = lLow;

				aRefAttributeNode.append_attribute(L"Title") = L"The minimum frame rate.";

				aRefAttributeNode.append_attribute(L"Description") = L"The minimum frame rate that is supported by a video capture device, in frames per second.";

			}
			else if (aGUID == MF_MT_FRAME_SIZE)
			{
				UINT32 lHigh = 0, lLow = 0;

				unpack2UINT32AsUINT64(aVar, lHigh, lLow);

				auto ValueParts = aRefAttributeNode.append_child(L"Value.ValueParts");

				auto ValuePart = ValueParts.append_child(L"ValuePart");

				std::wstring lNameGUID;

				ValuePart.append_attribute(L"Title") = L"Width";

				ValuePart.append_attribute(L"Value") = lHigh;


				ValuePart = ValueParts.append_child(L"ValuePart");

				ValuePart.append_attribute(L"Title") = L"Height";

				ValuePart.append_attribute(L"Value") = lLow;

				aRefAttributeNode.append_attribute(L"Title") = L"Width and height of the video frame.";

				aRefAttributeNode.append_attribute(L"Description") = L"Width and height of a video frame, in pixels.";

			}
			else if (aGUID == MF_MT_GEOMETRIC_APERTURE)
			{
				MFVideoArea* lPtrVideoArea;

				if (aVar.blob.cbSize == sizeof(MFVideoArea))
				{
					lPtrVideoArea = (MFVideoArea*)aVar.blob.pBlobData;
					
					auto ValueParts = aRefAttributeNode.append_child(L"Value.ValueParts");

					auto ValuePart = ValueParts.append_child(L"ValuePart");

					std::wstring lNameGUID;

					ValuePart.append_attribute(L"Title") = L"Width";

					ValuePart.append_attribute(L"Value") = lPtrVideoArea->Area.cx;


					ValuePart = ValueParts.append_child(L"ValuePart");

					ValuePart.append_attribute(L"Title") = L"Height";

					ValuePart.append_attribute(L"Value") = lPtrVideoArea->Area.cy;




					MFOffset lMFoffset = lPtrVideoArea->OffsetX;

					ValuePart = ValueParts.append_child(L"ValuePart");

					ValuePart.append_attribute(L"Title") = L"OffsetX";

					ValuePart.append_attribute(L"Value") = (float)((float)lMFoffset.value + (lMFoffset.fract / 65536.0f));



					lMFoffset = lPtrVideoArea->OffsetY;

					ValuePart = ValueParts.append_child(L"ValuePart");

					ValuePart.append_attribute(L"Title") = L"OffsetY";

					ValuePart.append_attribute(L"Value") = (float)((float)lMFoffset.value + (lMFoffset.fract / 65536.0f));
				}

				aRefAttributeNode.append_attribute(L"Title") = L"Geometric aperture.";

				aRefAttributeNode.append_attribute(L"Description") = L"Defines the geometric aperture for a video media type.";

			}
			else if (aGUID == MF_MT_INTERLACE_MODE)
			{

				auto lValue = aRefAttributeNode.append_child(L"SingleValue");

				lresult = readAttribute(
					aVar,
					lValue);

				if(FAILED(lresult))
				{
					break;
				}

				auto lAttr = lValue.attribute(L"Value");

				auto lv = lAttr.as_uint();

				lValue.remove_attribute(lAttr);

				std::wstring ltype = L"unknown";

				switch (lv)
				{
				case 0:
				{
						  ltype = L"MFVideoInterlace_Unknown";
				}
					break;
				case 1:
				{
				}
					break;
				case 2:
				{
						  ltype = L"MFVideoInterlace_Progressive";
				}
					break;
				case 3:
				{
						  ltype = L"MFVideoInterlace_FieldInterleavedUpperFirst";
				}
					break;
				case 4:
				{
						  ltype = L"MFVideoInterlace_FieldInterleavedLowerFirst";
				}
					break;
				case 5:
				{
						  ltype = L"MFVideoInterlace_FieldSingleUpper";
				}
					break;
				case 6:
				{
						  ltype = L"MFVideoInterlace_FieldSingleLower";
				}
					break;
				case 7:
				{
						  ltype = L"MFVideoInterlace_MixedInterlaceOrProgressive";
				}
					break;

				default:
					break;
				}

				lValue.append_attribute(L"Value") = ltype.c_str();

				aRefAttributeNode.append_attribute(L"Title") = L"Describes how the frames are interlaced.";

				aRefAttributeNode.append_attribute(L"Description") = L"Describes how the frames in a video media type are interlaced.";

			}
			else if (aGUID == MF_MT_MAX_KEYFRAME_SPACING)
			{
				readAttribute(
					aVar,
					aRefAttributeNode.append_child(L"SingleValue"));

				if(FAILED(lresult))
				{
					break;
				}

				aRefAttributeNode.append_attribute(L"Title") = L"Maximum number of frames from one key frame to the next.";

				aRefAttributeNode.append_attribute(L"Description") = L"Maximum number of frames from one key frame to the next, in a video media type.";

			}
			else if (aGUID == MF_MT_MINIMUM_DISPLAY_APERTURE)
			{
				MFVideoArea* lPtrVideoArea;

				if (aVar.blob.cbSize == sizeof(MFVideoArea))
				{
					lPtrVideoArea = (MFVideoArea*)aVar.blob.pBlobData;

					auto ValueParts = aRefAttributeNode.append_child(L"Value.ValueParts");

					auto ValuePart = ValueParts.append_child(L"ValuePart");

					std::wstring lNameGUID;

					ValuePart.append_attribute(L"Title") = L"Width";

					ValuePart.append_attribute(L"Value") = lPtrVideoArea->Area.cx;


					ValuePart = ValueParts.append_child(L"ValuePart");

					ValuePart.append_attribute(L"Title") = L"Height";

					ValuePart.append_attribute(L"Value") = lPtrVideoArea->Area.cy;




					MFOffset lMFoffset = lPtrVideoArea->OffsetX;

					ValuePart = ValueParts.append_child(L"ValuePart");

					ValuePart.append_attribute(L"Title") = L"OffsetX";

					ValuePart.append_attribute(L"Value") = (float)((float)lMFoffset.value + (lMFoffset.fract / 65536.0f));



					lMFoffset = lPtrVideoArea->OffsetY;

					ValuePart = ValueParts.append_child(L"ValuePart");

					ValuePart.append_attribute(L"Title") = L"OffsetY";

					ValuePart.append_attribute(L"Value") = (float)((float)lMFoffset.value + (lMFoffset.fract / 65536.0f));
				}

				aRefAttributeNode.append_attribute(L"Title") = L"Minimum display aperture.";

				aRefAttributeNode.append_attribute(L"Description") = L"Defines the display aperture, which is the region of a video frame that contains valid image data.";

			}
			else if (aGUID == MF_MT_MPEG_SEQUENCE_HEADER)
			{
				aRefAttributeNode.append_attribute(L"Title") = L"MPEG-1 or MPEG-2 sequence header.";

				aRefAttributeNode.append_attribute(L"Description") = L"Contains the MPEG-1 or MPEG-2 sequence header for a video media type.";

			}
			else if (aGUID == MF_MT_MPEG_START_TIME_CODE)
			{
				readAttribute(
					aVar,
					aRefAttributeNode.append_child(L"SingleValue"));

				if(FAILED(lresult))
				{
					break;
				}

				aRefAttributeNode.append_attribute(L"Title") = L"Group-of-pictures (GOP) start time code.";

				aRefAttributeNode.append_attribute(L"Description") = L"Group-of-pictures (GOP) start time code, for an MPEG-1 or MPEG-2 video media type.";

			}
			else if (aGUID == MF_MT_MPEG2_FLAGS)
			{
				readAttribute(
					aVar,
					aRefAttributeNode.append_child(L"SingleValue"));

				if(FAILED(lresult))
				{
					break;
				}

				aRefAttributeNode.append_attribute(L"Title") = L"Miscellaneous flags for MPEG-2 video.";

				aRefAttributeNode.append_attribute(L"Description") = L"Contains miscellaneous flags for an MPEG-2 video media type.";

			}
			else if (aGUID == MF_MT_MPEG2_LEVEL)
			{
				readAttribute(
					aVar,
					aRefAttributeNode.append_child(L"SingleValue"));

				if(FAILED(lresult))
				{
					break;
				}

				aRefAttributeNode.append_attribute(L"Title") = L"MPEG-2 or H.264 level.";

				aRefAttributeNode.append_attribute(L"Description") = L"Specifies the MPEG-2 or H.264 level in a video media type.";

			}
			else if (aGUID == MF_MT_MPEG2_PROFILE)
			{
				readAttribute(
					aVar,
					aRefAttributeNode.append_child(L"SingleValue"));

				if(FAILED(lresult))
				{
					break;
				}

				aRefAttributeNode.append_attribute(L"Title") = L"MPEG-2 or H.264 profile.";

				aRefAttributeNode.append_attribute(L"Description") = L"Specifies the MPEG-2 or H.264 profile in a video media type.";

			}
			else if (aGUID == MF_MT_ORIGINAL_4CC)
			{
				char lbuffer[16] = {0};

				DwordToFourCC(aVar.uintVal, lbuffer);

				aRefAttributeNode.append_child(L"SingleValue").append_attribute(L"Value") = lbuffer;

				if(FAILED(lresult))
				{
					break;
				}

				aRefAttributeNode.append_attribute(L"Title") = L"Contains the original codec FOURCC for a video stream.";

				aRefAttributeNode.append_attribute(L"Description") = L"Contains the original codec FOURCC for a video stream.";

			}
			else if (aGUID == MF_MT_PAD_CONTROL_FLAGS)
			{

				auto lValue = aRefAttributeNode.append_child(L"SingleValue");

				lresult = readAttribute(
					aVar,
					lValue);

				if(FAILED(lresult))
				{
					break;
				}

				auto lAttr = lValue.attribute(L"Value");

				auto lv = lAttr.as_uint();

				lValue.remove_attribute(lAttr);

				std::wstring ltype = L"unknown";

				switch (lv)
				{
				case 0:
				{
						  ltype = L"MFVideoPadFlag_PAD_TO_None";
				}
					break;
				case 1:
				{
						  ltype = L"MFVideoPadFlag_PAD_TO_4x3";
				}
					break;
				case 2:
				{
						  ltype = L"MFVideoPadFlag_PAD_TO_16x9";
				}
					break;
				default:
					break;
				}

				lValue.append_attribute(L"Value") = ltype.c_str();

				aRefAttributeNode.append_attribute(L"Title") = L"Aspect ratio of the output rectangle.";

				aRefAttributeNode.append_attribute(L"Description") = L"Specifies the aspect ratio of the output rectangle for a video media type.";

			}
			else if (aGUID == MF_MT_PALETTE)
			{

				aRefAttributeNode.append_attribute(L"Title") = L"Palette entries.";

				aRefAttributeNode.append_attribute(L"Description") = L"Contains the palette entries for a video media type. Use this attribute for palettized video formats, such as RGB 8.";

			}
			else if (aGUID == MF_MT_PAN_SCAN_APERTURE)
			{
				MFVideoArea* lPtrVideoArea;

				if (aVar.blob.cbSize == sizeof(MFVideoArea))
				{
					lPtrVideoArea = (MFVideoArea*)aVar.blob.pBlobData;

					auto ValueParts = aRefAttributeNode.append_child(L"Value.ValueParts");

					auto ValuePart = ValueParts.append_child(L"ValuePart");

					std::wstring lNameGUID;

					ValuePart.append_attribute(L"Title") = L"Width";

					ValuePart.append_attribute(L"Value") = lPtrVideoArea->Area.cx;


					ValuePart = ValueParts.append_child(L"ValuePart");

					ValuePart.append_attribute(L"Title") = L"Height";

					ValuePart.append_attribute(L"Value") = lPtrVideoArea->Area.cy;




					MFOffset lMFoffset = lPtrVideoArea->OffsetX;

					ValuePart = ValueParts.append_child(L"ValuePart");

					ValuePart.append_attribute(L"Title") = L"OffsetX";

					ValuePart.append_attribute(L"Value") = (float)((float)lMFoffset.value + (lMFoffset.fract / 65536.0f));



					lMFoffset = lPtrVideoArea->OffsetY;

					ValuePart = ValueParts.append_child(L"ValuePart");

					ValuePart.append_attribute(L"Title") = L"OffsetY";

					ValuePart.append_attribute(L"Value") = (float)((float)lMFoffset.value + (lMFoffset.fract / 65536.0f));
				}

				aRefAttributeNode.append_attribute(L"Title") = L"Defines the 4×3 region of video that should be displayed in pan/scan mode.";

				aRefAttributeNode.append_attribute(L"Description") = L"Defines the pan/scan aperture, which is the 4×3 region of video that should be displayed in pan/scan mode.";

			}
			else if (aGUID == MF_MT_PAN_SCAN_ENABLED)
			{
				readAttribute(
					aVar,
					aRefAttributeNode.append_child(L"SingleValue"),
					true);

				if(FAILED(lresult))
				{
					break;
				}

				aRefAttributeNode.append_attribute(L"Title") = L"Specifies whether pan/scan mode is enabled.";

				aRefAttributeNode.append_attribute(L"Description") = L"Specifies whether pan/scan mode is enabled.";

			}
			else if (aGUID == MF_MT_PIXEL_ASPECT_RATIO)
			{
				auto lRatioValue = aRefAttributeNode.append_child(L"RatioValue");

				UINT32 lHigh = 0, lLow = 0;

				unpack2UINT32AsUINT64(aVar, lHigh, lLow);

				lRatioValue.append_attribute(L"Value") = (float)lHigh / (float)lLow;

				auto ValueParts = lRatioValue.append_child(L"Value.ValueParts");

				auto ValuePart = ValueParts.append_child(L"ValuePart");

				std::wstring lNameGUID;

				ValuePart.append_attribute(L"Title") = L"Numerator";

				ValuePart.append_attribute(L"Value") = lHigh;


				ValuePart = ValueParts.append_child(L"ValuePart");

				ValuePart.append_attribute(L"Title") = L"Denominator";

				ValuePart.append_attribute(L"Value") = lLow;

				aRefAttributeNode.append_attribute(L"Title") = L"Pixel aspect ratio.";

				aRefAttributeNode.append_attribute(L"Description") = L"Pixel aspect ratio for a video media type.";

			}
			else if (aGUID == MF_MT_SOURCE_CONTENT_HINT)
			{

				auto lValue = aRefAttributeNode.append_child(L"SingleValue");

				lresult = readAttribute(
					aVar,
					lValue);

				if(FAILED(lresult))
				{
					break;
				}

				auto lAttr = lValue.attribute(L"Value");

				auto lv = lAttr.as_uint();

				lValue.remove_attribute(lAttr);

				std::wstring ltype = L"unknown";

				switch (lv)
				{
				case 0:
				{
						  ltype = L"MFVideoSrcContentHintFlag_None";
				}
					break;
				case 1:
				{
						  ltype = L"MFVideoSrcContentHintFlag_16x9";
				}
					break;
				case 2:
				{
						  ltype = L"MFVideoSrcContentHintFlag_235_1";
				}
					break;
				default:
					break;
				}

				lValue.append_attribute(L"Value") = ltype.c_str();

				aRefAttributeNode.append_attribute(L"Title") = L"Intended aspect ratio.";

				aRefAttributeNode.append_attribute(L"Description") = L"Describes the intended aspect ratio for a video media type.";

			}
			else if (aGUID == MF_MT_TRANSFER_FUNCTION)
			{

				auto lValue = aRefAttributeNode.append_child(L"SingleValue");

				lresult = readAttribute(
					aVar,
					lValue);

				if(FAILED(lresult))
				{
					break;
				}

				auto lAttr = lValue.attribute(L"Value");

				auto lv = lAttr.as_uint();

				lValue.remove_attribute(lAttr);

				std::wstring ltype = L"unknown";

				switch (lv)
				{
				case 0:
				{
						  ltype = L"MFVideoTransFunc_Unknown";
				}
					break;
				case 1:
				{
						  ltype = L"MFVideoTransFunc_10";
				}
					break;
				case 2:
				{
						  ltype = L"MFVideoTransFunc_18";
				}
					break;
				case 3:
				{
						  ltype = L"MFVideoTransFunc_20";
				}
					break;
				case 4:
				{
						  ltype = L"MFVideoTransFunc_22";
				}
					break;
				case 5:
				{
						  ltype = L"MFVideoTransFunc_709";
				}
					break;
				case 6:
				{
						  ltype = L"MFVideoTransFunc_240M";
				}
					break;
				case 7:
				{
						  ltype = L"MFVideoTransFunc_sRGB";
				}
					break;
				case 8:
				{
						  ltype = L"MFVideoTransFunc_28";
				}
					break;
				case 9:
				{
						  ltype = L"MFVideoTransFunc_Log_100";
				}
					break;
				case 10:
				{
						  ltype = L"MFVideoTransFunc_Log_316";
				}
					break;
				case 11:
					{
						ltype = L"MFVideoTransFunc_709_sym";
					}
					break;
				default:
					break;
				}

				lValue.append_attribute(L"Value") = ltype.c_str();

				aRefAttributeNode.append_attribute(L"Title") = L"Conversion function from RGB to R'G'B'.";

				aRefAttributeNode.append_attribute(L"Description") = L"Specifies the conversion function from RGB to R'G'B' for a video media type.";

			}
			else if (aGUID == MF_MT_VIDEO_3D)
			{
				readAttribute(
					aVar,
					aRefAttributeNode.append_child(L"SingleValue"),
					true);

				if(FAILED(lresult))
				{
					break;
				}

				aRefAttributeNode.append_attribute(L"Title") = L"Specifies whether a video stream contains 3D content.";

				aRefAttributeNode.append_attribute(L"Description") = L"Specifies whether a video stream contains 3D content.";

			}
			else if (aGUID == MF_MT_VIDEO_CHROMA_SITING)
			{
				auto lValue = aRefAttributeNode.append_child(L"SingleValue");

				lresult = readAttribute(
					aVar,
					lValue);

				if(FAILED(lresult))
				{
					break;
				}

				auto lAttr = lValue.attribute(L"Value");

				auto lv = lAttr.as_uint();

				lValue.remove_attribute(lAttr);

				std::wstring ltype = L"unknown";

				switch (lv)
				{
				case MFVideoChromaSubsampling_Unknown:
				{
						  ltype = L"MFVideoChromaSubsampling_Unknown";
				}
					break;
				case MFVideoChromaSubsampling_ProgressiveChroma:
				{
						  ltype = L"MFVideoChromaSubsampling_ProgressiveChroma";
				}
					break;
				case MFVideoChromaSubsampling_Horizontally_Cosited:
				{
						  ltype = L"MFVideoChromaSubsampling_Horizontally_Cosited";
				}
					break;
				case MFVideoChromaSubsampling_Vertically_Cosited:
				{
						  ltype = L"MFVideoChromaSubsampling_Vertically_Cosited";
				}
					break;
				case MFVideoChromaSubsampling_Vertically_AlignedChromaPlanes:
				{
						  ltype = L"MFVideoChromaSubsampling_Vertically_AlignedChromaPlanes";
				}
					break;
				case MFVideoChromaSubsampling_MPEG2:
				{
		                  ltype = L"MFVideoChromaSubsampling_MPEG2";
				}
					break;
				case MFVideoChromaSubsampling_DV_PAL:
				{
	                      ltype = L"MFVideoChromaSubsampling_DV_PAL";
				}
					break;
				case MFVideoChromaSubsampling_Cosited:
				{
				          ltype = L"MFVideoChromaSubsampling_Cosited";
				}
					break;

				default:
					break;
				}

				lValue.append_attribute(L"Value") = ltype.c_str();

				aRefAttributeNode.append_attribute(L"Title") = L"Describes how chroma was sampled for Y'Cb'Cr' video.";

				aRefAttributeNode.append_attribute(L"Description") = L"Describes how chroma was sampled for a Y'Cb'Cr' video media type.";

			}
			else if (aGUID == MF_MT_VIDEO_LIGHTING)
			{

				auto lValue = aRefAttributeNode.append_child(L"SingleValue");

				lresult = readAttribute(
					aVar,
					lValue);

				if(FAILED(lresult))
				{
					break;
				}

				auto lAttr = lValue.attribute(L"Value");

				auto lv = lAttr.as_uint();

				lValue.remove_attribute(lAttr);

				std::wstring ltype = L"unknown";

				switch (lv)
				{
				case 0:
				{
						  ltype = L"MFVideoLighting_Unknown";
				}
					break;
				case 1:
				{
						  ltype = L"MFVideoLighting_bright";
				}
					break;
				case 2:
				{
						  ltype = L"MFVideoLighting_office";
				}
					break;
				case 3:
				{
						  ltype = L"MFVideoLighting_dim";
				}
					break;
				case 4:
				{
						  ltype = L"MFVideoLighting_dark";
				}
					break;
				
				default:
					break;
				}
				
				lValue.append_attribute(L"Value") = ltype.c_str();

				aRefAttributeNode.append_attribute(L"Title") = L"Optimal lighting conditions for viewing.";

				aRefAttributeNode.append_attribute(L"Description") = L"Specifies the optimal lighting conditions for a video media type.";

			}
			else if (aGUID == MF_MT_VIDEO_NOMINAL_RANGE)
			{

				auto lValue = aRefAttributeNode.append_child(L"SingleValue");

				lresult = readAttribute(
					aVar,
					lValue);

				if(FAILED(lresult))
				{
					break;
				}

				auto lAttr = lValue.attribute(L"Value");

				auto lv = lAttr.as_uint();

				lValue.remove_attribute(lAttr);

				std::wstring ltype = L"unknown";

				switch (lv)
				{
				case 0:
				{
						  ltype = L"MFNominalRange_Unknown";
				}
					break;
				case 1:
				{
						  ltype = L"MFNominalRange_0_255";
				}
					break;
				case 2:
				{
						  ltype = L"MFNominalRange_16_235";
				}
					break;
				case 3:
				{
						  ltype = L"MFNominalRange_48_208";
				}
					break;
				case 4:
				{
						  ltype = L"MFNominalRange_64_127";
				}
					break;

				default:
					break;
				}
				
				lValue.append_attribute(L"Value") = ltype.c_str();

				aRefAttributeNode.append_attribute(L"Title") = L"Nominal range of the color information.";

				aRefAttributeNode.append_attribute(L"Description") = L"Specifies the nominal range of the color information in a video media type.";

			}
			else if (aGUID == MF_MT_VIDEO_PRIMARIES)
			{

				auto lValue = aRefAttributeNode.append_child(L"SingleValue");

				lresult = readAttribute(
					aVar,
					lValue);

				if(FAILED(lresult))
				{
					break;
				}

				auto lAttr = lValue.attribute(L"Value");

				auto lv = lAttr.as_uint();

				lValue.remove_attribute(lAttr);

				std::wstring ltype = L"unknown";

				switch (lv)
				{
				case 0:
				{
						  ltype = L"MFVideoPrimaries_Unknown";
				}
					break;
				case 1:
				{
						  ltype = L"MFVideoPrimaries_reserved";
				}
					break;
				case 2:
				{
						  ltype = L"MFVideoPrimaries_BT709";
				}
					break;
				case 3:
				{
						  ltype = L"MFVideoPrimaries_BT470_2_SysM";
				}
					break;
				case 4:
				{
						  ltype = L"MFVideoPrimaries_BT470_2_SysBG";
				}
					break;
				case 5:
				{
						  ltype = L"MFVideoPrimaries_SMPTE170M";
				}
					break;
				case 6:
				{
						  ltype = L"MFVideoPrimaries_SMPTE240M";
				}
					break;
				case 7:
				{
						  ltype = L"MFVideoPrimaries_EBU3213";
				}
					break;
				case 8:
				{
						  ltype = L"MFVideoPrimaries_SMPTE_C";
				}
					break;

				default:
					break;
				}

				lValue.append_attribute(L"Value") = ltype.c_str();

				aRefAttributeNode.append_attribute(L"Title") = L"Color primaries.";

				aRefAttributeNode.append_attribute(L"Description") = L"Specifies the color primaries for a video media type.";

			}
			else if (aGUID == MF_MT_VIDEO_ROTATION)
			{
				
				lresult = readAttribute(
					aVar,
					aRefAttributeNode.append_child(L"SingleValue"));

				if(FAILED(lresult))
				{
					break;
				}
				
				aRefAttributeNode.append_attribute(L"Title") = L"Specifies the rotation of a video frame in the counter-clockwise direction.";

				aRefAttributeNode.append_attribute(L"Description") = L"Specifies the rotation of a video frame in the counter-clockwise direction.";

			}
			else if (aGUID == MF_MT_YUV_MATRIX)
			{

				auto lValue = aRefAttributeNode.append_child(L"SingleValue");

				lresult = readAttribute(
					aVar,
					lValue);

				if(FAILED(lresult))
				{
					break;
				}

				auto lAttr = lValue.attribute(L"Value");

				auto lv = lAttr.as_uint();

				lValue.remove_attribute(lAttr);

				std::wstring ltype = L"unknown";

				switch (lv)
				{
				case 0:
				{
						  ltype = L"MFVideoTransferMatrix_Unknown";
				}
					break;
				case 1:
				{
						  ltype = L"MFVideoTransferMatrix_BT709";
				}
					break;
				case 2:
				{
						  ltype = L"MFVideoTransferMatrix_BT601";
				}
					break;
				case 3:
				{
						  ltype = L"MFVideoTransferMatrix_SMPTE240M";
				}
					break;

				default:
					break;
				}

				lValue.append_attribute(L"Value") = ltype.c_str();

				aRefAttributeNode.append_attribute(L"Title") = L"Conversion matrix from the Y'Cb'Cr' color space to the R'G'B' color space.";

				aRefAttributeNode.append_attribute(L"Description") = L"For YUV media types, defines the conversion matrix from the Y'Cb'Cr' color space to the R'G'B' color space.";

			}
			else if (aGUID == DirectShowPhysicalType)
			{

				auto lValue = aRefAttributeNode.append_child(L"SingleValue");

				lresult = readAttribute(
					aVar,
					lValue);

				if (FAILED(lresult))
				{
					break;
				}

				auto lAttr = lValue.attribute(L"Value");

				auto lv = lAttr.as_uint();

				lValue.remove_attribute(lAttr);

				std::wstring ltype = L"unknown";
				
				switch (lv)
				{
				case PhysConn_Video_Composite:
				{
					ltype = L"PhysConn_Video_Composite";
				}
					break;
				case PhysConn_Video_SVideo:
				{
					ltype = L"PhysConn_Video_SVideo";
				}
					break;
				case PhysConn_Video_USB:
				{
					ltype = L"PhysConn_Video_USB";
				}
					break;
				case PhysConn_Video_1394:
				{
					ltype = L"PhysConn_Video_1394";
				}
					break;

				default:
					break;
				}

				lValue.append_attribute(L"Value") = ltype.c_str();

				aRefAttributeNode.append_attribute(L"Title") = L"Physical type of input pin of Crossbar.";

				aRefAttributeNode.append_attribute(L"Description") = L"Physical type of input pin of Crossbar.";

			}
			else
			{
				lresult = S_FALSE;
			}
			
		} while (false);

		return lresult;
	}

	HRESULT DataParser::parsAudioMediaFormatFormatAttributesValue(
		REFGUID aGUID,
		const PROPVARIANT& aVar,
		pugi::xml_node& aRefAttributeNode)
	{
		HRESULT lresult;

		using namespace Core;

		do
		{
			lresult = S_OK;

			if (aGUID == MF_MT_AAC_AUDIO_PROFILE_LEVEL_INDICATION)
			{
				readAttribute(
					aVar,
					aRefAttributeNode.append_child(L"SingleValue"));

				if(FAILED(lresult))
				{
					break;
				}

				aRefAttributeNode.append_attribute(L"Title") = L"The audio profile and level of an Advanced Audio Coding (AAC) stream.";

				aRefAttributeNode.append_attribute(L"Description") = L"This attribute contains the value of the audioProfileLevelIndication field, as defined by ISO/IEC 14496-3.";

			}
			else if (aGUID == MF_MT_AAC_PAYLOAD_TYPE)
			{
				auto lValue = aRefAttributeNode.append_child(L"SingleValue");

				lresult = readAttribute(
					aVar,
					lValue);

				if(FAILED(lresult))
				{
					break;
				}

				auto lAttr = lValue.attribute(L"Value");

				auto lv = lAttr.as_uint();

				lValue.remove_attribute(lAttr);

				std::wstring ltype = L"unknown";

				switch (lv)
				{
				case 0:
				{
						  ltype = L"The stream contains raw_data_block elements only.";
				}
					break;
				case 1:
				{
						  ltype = L"Audio Data Transport Stream (ADTS). The stream contains an adts_sequence, as defined by MPEG-2.";
				}
					break;
				case 2:
				{
						  ltype = L"Audio Data Interchange Format (ADIF). The stream contains an adif_sequence, as defined by MPEG-2.";
				}
					break;
				case 3:
				{
						  ltype = L"The stream contains an MPEG-4 audio transport stream with a synchronization layer (LOAS) and a multiplex layer (LATM).";
				}
					break;

				default:
					break;
				}

				lValue.append_attribute(L"Value") = ltype.c_str();

				aRefAttributeNode.append_attribute(L"Title") = L"The payload type for an Advanced Audio Coding (AAC) stream.";

				aRefAttributeNode.append_attribute(L"Description") = L"Specifies the payload type of an Advanced Audio Coding (AAC) stream.";

			}
			else if (aGUID == MF_MT_AUDIO_AVG_BYTES_PER_SECOND)
			{
				readAttribute(
					aVar,
					aRefAttributeNode.append_child(L"SingleValue"));

				if(FAILED(lresult))
				{
					break;
				}

				aRefAttributeNode.append_attribute(L"Title") = L"Average number of bytes per second.";

				aRefAttributeNode.append_attribute(L"Description") = L"Average number of bytes per second in an audio media type.";

			}
			else if (aGUID == MF_MT_AUDIO_BITS_PER_SAMPLE)
			{
				readAttribute(
					aVar,
					aRefAttributeNode.append_child(L"SingleValue"));

				if(FAILED(lresult))
				{
					break;
				}

				aRefAttributeNode.append_attribute(L"Title") = L"Number of bits per audio sample.";

				aRefAttributeNode.append_attribute(L"Description") = L"Number of bits per audio sample in an audio media type.";

			}
			else if (aGUID == MF_MT_AUDIO_BLOCK_ALIGNMENT)
			{
				readAttribute(
					aVar,
					aRefAttributeNode.append_child(L"SingleValue"));

				if(FAILED(lresult))
				{
					break;
				}

				aRefAttributeNode.append_attribute(L"Title") = L"Block alignment, in bytes.";

				aRefAttributeNode.append_attribute(L"Description") = L"Block alignment, in bytes, for an audio media type. The block alignment is the minimum atomic unit of data for the audio format.";

			}
			else if (aGUID == MF_MT_AUDIO_CHANNEL_MASK)
			{
				auto lValue = aRefAttributeNode.append_child(L"SingleValue");

				lresult = readAttribute(
					aVar,
					lValue);

				if(FAILED(lresult))
				{
					break;
				}

				auto lAttr = lValue.attribute(L"Value");

				auto lv = lAttr.as_uint();

				lValue.remove_attribute(lAttr);

				aRefAttributeNode.remove_child(lValue);

				auto ValueParts = aRefAttributeNode.append_child(L"Value.ValueParts");

				ADD_IF_EQUAL(lv, SPEAKER_FRONT_LEFT, ValueParts);

				ADD_IF_EQUAL(lv, SPEAKER_FRONT_RIGHT, ValueParts);

				ADD_IF_EQUAL(lv, SPEAKER_FRONT_CENTER, ValueParts);

				ADD_IF_EQUAL(lv, SPEAKER_LOW_FREQUENCY, ValueParts);

				ADD_IF_EQUAL(lv, SPEAKER_BACK_LEFT, ValueParts);

				ADD_IF_EQUAL(lv, SPEAKER_BACK_RIGHT, ValueParts);

				ADD_IF_EQUAL(lv, SPEAKER_FRONT_LEFT_OF_CENTER, ValueParts);

				ADD_IF_EQUAL(lv, SPEAKER_FRONT_RIGHT_OF_CENTER, ValueParts);

				ADD_IF_EQUAL(lv, SPEAKER_BACK_CENTER, ValueParts);

				ADD_IF_EQUAL(lv, SPEAKER_SIDE_LEFT, ValueParts);

				ADD_IF_EQUAL(lv, SPEAKER_SIDE_RIGHT, ValueParts);

				ADD_IF_EQUAL(lv, SPEAKER_TOP_CENTER, ValueParts);

				ADD_IF_EQUAL(lv, SPEAKER_TOP_FRONT_LEFT, ValueParts);

				ADD_IF_EQUAL(lv, SPEAKER_TOP_FRONT_CENTER, ValueParts);

				ADD_IF_EQUAL(lv, SPEAKER_TOP_FRONT_RIGHT, ValueParts);

				ADD_IF_EQUAL(lv, SPEAKER_TOP_BACK_LEFT, ValueParts);

				ADD_IF_EQUAL(lv, SPEAKER_TOP_BACK_CENTER, ValueParts);

				ADD_IF_EQUAL(lv, SPEAKER_TOP_BACK_RIGHT, ValueParts);

				aRefAttributeNode.append_attribute(L"Title") = L"Specifies the assignment of audio channels to speaker positions.";

				aRefAttributeNode.append_attribute(L"Description") = L"In an audio media type, specifies the assignment of audio channels to speaker positions.";

			}
			else if (aGUID == MF_MT_AUDIO_FLOAT_SAMPLES_PER_SECOND)
			{
				readAttribute(
					aVar,
					aRefAttributeNode.append_child(L"SingleValue"));

				if(FAILED(lresult))
				{
					break;
				}

				aRefAttributeNode.append_attribute(L"Title") = L"Number of audio samples per second (floating-point value).";

				aRefAttributeNode.append_attribute(L"Description") = L"Number of audio samples per second in an audio media type.";

			}
			else if (aGUID == MF_MT_AUDIO_FOLDDOWN_MATRIX)
			{
				aRefAttributeNode.append_attribute(L"Title") = L"Specifies how an audio decoder should transform multichannel audio to stereo output.";

				aRefAttributeNode.append_attribute(L"Description") = L"Specifies how an audio decoder should transform multichannel audio to stereo output. This process is also called fold-down.";

			}
			else if (aGUID == MF_MT_AUDIO_NUM_CHANNELS)
			{
				readAttribute(
					aVar,
					aRefAttributeNode.append_child(L"SingleValue"));

				if(FAILED(lresult))
				{
					break;
				}

				aRefAttributeNode.append_attribute(L"Title") = L"Number of audio channels.";

				aRefAttributeNode.append_attribute(L"Description") = L"Number of audio channels in an audio media type.";

			}
			else if (aGUID == MF_MT_AUDIO_PREFER_WAVEFORMATEX)
			{
				readAttribute(
					aVar,
					aRefAttributeNode.append_child(L"SingleValue"),
					true);

				if(FAILED(lresult))
				{
					break;
				}

				aRefAttributeNode.append_attribute(L"Title") = L"Specifies the preferred legacy format structure to use when converting an audio media type.";

				aRefAttributeNode.append_attribute(L"Description") = L"Specifies the preferred legacy format structure to use when converting an audio media type.";

			}
			else if (aGUID == MF_MT_AUDIO_SAMPLES_PER_BLOCK)
			{
				readAttribute(
					aVar,
					aRefAttributeNode.append_child(L"SingleValue"));

				if(FAILED(lresult))
				{
					break;
				}

				aRefAttributeNode.append_attribute(L"Title") = L"Number of audio samples contained in one compressed block of audio data.";

				aRefAttributeNode.append_attribute(L"Description") = L"Number of audio samples contained in one compressed block of audio data. ";

			}
			else if (aGUID == MF_MT_AUDIO_SAMPLES_PER_SECOND)
			{
				readAttribute(
					aVar,
					aRefAttributeNode.append_child(L"SingleValue"));

				if(FAILED(lresult))
				{
					break;
				}

				aRefAttributeNode.append_attribute(L"Title") = L"Number of audio samples per second (integer value).";

				aRefAttributeNode.append_attribute(L"Description") = L"Number of audio samples per second in an audio media type.";

			}
			else if (aGUID == MF_MT_AUDIO_VALID_BITS_PER_SAMPLE)
			{
				readAttribute(
					aVar,
					aRefAttributeNode.append_child(L"SingleValue"));

				if(FAILED(lresult))
				{
					break;
				}

				aRefAttributeNode.append_attribute(L"Title") = L"Number of valid bits of audio data in each audio sample.";

				aRefAttributeNode.append_attribute(L"Description") = L"Number of valid bits of audio data in each audio sample.";

			}
			else if (aGUID == MF_MT_AUDIO_WMADRC_AVGREF)
			{
				readAttribute(
					aVar,
					aRefAttributeNode.append_child(L"SingleValue"));

				if(FAILED(lresult))
				{
					break;
				}

				aRefAttributeNode.append_attribute(L"Title") = L"Reference average volume level of a Windows Media Audio file.";

				aRefAttributeNode.append_attribute(L"Description") = L"Reference average volume level of a Windows Media Audio file.";

			}
			else if (aGUID == MF_MT_AUDIO_WMADRC_AVGTARGET)
			{
				readAttribute(
					aVar,
					aRefAttributeNode.append_child(L"SingleValue"));

				if(FAILED(lresult))
				{
					break;
				}

				aRefAttributeNode.append_attribute(L"Title") = L"Target average volume level of a Windows Media Audio file.";

				aRefAttributeNode.append_attribute(L"Description") = L"Target average volume level of a Windows Media Audio file.";

			}
			else if (aGUID == MF_MT_AUDIO_WMADRC_PEAKREF)
			{
				readAttribute(
					aVar,
					aRefAttributeNode.append_child(L"SingleValue"));

				if(FAILED(lresult))
				{
					break;
				}

				aRefAttributeNode.append_attribute(L"Title") = L"Reference peak volume level of a Windows Media Audio file.";

				aRefAttributeNode.append_attribute(L"Description") = L"Reference peak volume level of a Windows Media Audio file.";

			}
			else if (aGUID == MF_MT_AUDIO_WMADRC_PEAKTARGET)
			{
				readAttribute(
					aVar,
					aRefAttributeNode.append_child(L"SingleValue"));

				if(FAILED(lresult))
				{
					break;
				}

				aRefAttributeNode.append_attribute(L"Title") = L"Target peak volume level of a Windows Media Audio file.";

				aRefAttributeNode.append_attribute(L"Description") = L"Target peak volume level of a Windows Media Audio file.";

			}
			else if (aGUID == MF_MT_ORIGINAL_WAVE_FORMAT_TAG)
			{
				readAttribute(
					aVar,
					aRefAttributeNode.append_child(L"SingleValue"));

				if(FAILED(lresult))
				{
					break;
				}

				aRefAttributeNode.append_attribute(L"Title") = L"Contains the original WAVE format tag for an audio stream.";

				aRefAttributeNode.append_attribute(L"Description") = L"Contains the original WAVE format tag for an audio stream.";

			}			
			else
			{
				lresult = S_FALSE;
			}
									
		} while (false);
		
		return lresult;
	}

	HRESULT DataParser::parsGeneralMediaTypeFormatAttributesValueByIndex(
		REFGUID aGUID,
		const PROPVARIANT& aVar,
		pugi::xml_node& aRefAttributeNode)
	{
		HRESULT lresult;

		using namespace Core;

			do
			{
				lresult = S_OK;

				if (aGUID == MF_MT_ALL_SAMPLES_INDEPENDENT)
				{
					readAttribute(
						aVar,
						aRefAttributeNode.append_child(L"SingleValue"),
						true);

					aRefAttributeNode.append_attribute(L"Title") = L"Independent of samples.";

					aRefAttributeNode.append_attribute(L"Description") = L"Specifies for a media type whether each sample is independent of the other samples in the stream.";

				}
				else if (aGUID == MF_MT_AM_FORMAT_TYPE)
				{
					readAttribute(
						aVar,
						aRefAttributeNode.append_child(L"SingleValue"));

					aRefAttributeNode.append_attribute(L"Title") = L"DirectShow format GUID for a media type.";

					aRefAttributeNode.append_attribute(L"Description") = L"Contains a DirectShow format GUID for a media type.";

				}
				else if (aGUID == MF_MT_COMPRESSED)
				{
					readAttribute(
						aVar,
						aRefAttributeNode.append_child(L"SingleValue"),
						true);

					aRefAttributeNode.append_attribute(L"Title") = L"The media type is a compressed format.";

					aRefAttributeNode.append_attribute(L"Description") = L"Specifies for a media type whether the media data is compressed.";

				}
				else if (aGUID == MF_MT_FIXED_SIZE_SAMPLES)
				{
					readAttribute(
						aVar,
						aRefAttributeNode.append_child(L"SingleValue"),
						true);

					aRefAttributeNode.append_attribute(L"Title") = L"The fixed size of samples in stream.";

					aRefAttributeNode.append_attribute(L"Description") = L"Specifies for a media type whether the samples have a fixed size.";

				}
				else if (aGUID == MF_MT_MAJOR_TYPE)
				{
					readAttribute(
						aVar,
						aRefAttributeNode.append_child(L"SingleValue"));

					aRefAttributeNode.append_attribute(L"Title") = L"Major type GUID for a media type.";

					aRefAttributeNode.append_attribute(L"Description") = L"The major type defines the overall category of the media data.";

				}
				else if (aGUID == MF_MT_SAMPLE_SIZE)
				{
					readAttribute(
						aVar,
						aRefAttributeNode.append_child(L"SingleValue"));

					aRefAttributeNode.append_attribute(L"Title") = L"The fixed size of each sample in stream.";

					aRefAttributeNode.append_attribute(L"Description") = L"Specifies the size of each sample, in bytes, in a media type.";

				}
				else if (aGUID == MF_MT_SUBTYPE)
				{
					readAttribute(
						aVar,
						aRefAttributeNode.append_child(L"SingleValue"));

					aRefAttributeNode.append_attribute(L"Title") = L"Subtype GUID for a media type.";

					aRefAttributeNode.append_attribute(L"Description") = L"The subtype GUID defines a specific media format type within a major type.";

				}				
				else
				{
					lresult = S_FALSE;
				}
				
			} while (false);
			
		return lresult;
	}

	HRESULT DataParser::GetGUIDName(
		REFGUID aGUID, 
		std::wstring& aNameGUID)
	{
		HRESULT lresult;

		do
		{

			WCHAR *lptrName = nullptr;

			LPCWSTR lptrConstName = GetGUIDNameConst(aGUID);

			if (lptrConstName == nullptr)
			{
				lresult = StringFromCLSID(aGUID, &lptrName);

				if(FAILED(lresult))
					break;
					
				aNameGUID = std::wstring(lptrName);

				CoTaskMemFree(lptrName);
				
			}
			else
			{
				aNameGUID = std::wstring(lptrConstName);

				lresult = S_OK;
			}

		} while (false);

		return lresult;
	}

	void DataParser::unpack2UINT32AsUINT64(const PROPVARIANT& aVar, UINT32& aHigh, UINT32& aLow)
	{
		Unpack2UINT32AsUINT64(aVar.uhVal.QuadPart, &aHigh, &aLow);
	}

	HRESULT DataParser::readAttribute(
		const PROPVARIANT& aVar,
		pugi::xml_node& aRefValueNode,
		bool aIsBoolable,
		bool aIsSigned)
	{
		HRESULT lresult(S_OK);

		do
		{
			if (aVar.vt == VT_LPWSTR)
			{
				aRefValueNode.append_attribute(L"Value") = aVar.pwszVal;
			}
			else if (aVar.vt == VT_CLSID)
			{
				std::wstring lNameGUID;

				std::wstring lRawGUID;

				WCHAR* lptrName;

				LPCWSTR lptrConstName = GetGUIDNameConst(*aVar.puuid);

				lresult = StringFromCLSID(*aVar.puuid, &lptrName);

				if(FAILED(lresult))
					break;

				lRawGUID = std::wstring(lptrName);

				CoTaskMemFree(lptrName);

				if (lptrConstName == nullptr)
				{
					lNameGUID = lRawGUID;
				}
				else
				{
					lNameGUID = std::wstring(lptrConstName);
				}

				aRefValueNode.append_attribute(L"Value") = lNameGUID.c_str();

				aRefValueNode.append_attribute(L"GUID") = lRawGUID.c_str();
			}
			else if (aVar.vt == VT_UI4 && aIsBoolable)
			{
				std::wstring lvalue = L"True";

				if (aVar.uintVal == FALSE)
				{
					lvalue = L"False";
				}

				aRefValueNode.append_attribute(L"Value") = lvalue.c_str();
			}
			else if (aVar.vt == VT_UI4 && !aIsBoolable && !aIsSigned)
			{
				aRefValueNode.append_attribute(L"Value") = aVar.uintVal;
			}
			else if (aVar.vt == VT_UI4 && !aIsBoolable && aIsSigned)
			{
				aRefValueNode.append_attribute(L"Value") = (int)aVar.uintVal;
			}
			else if (aVar.vt == VT_UI8)
			{
				aRefValueNode.append_attribute(L"Value").set_value(aVar.uhVal.QuadPart);
			}
			else if (aVar.vt == VT_R8)
			{
				aRefValueNode.append_attribute(L"Value").set_value(aVar.dblVal);
			}
			else
			{
				lresult = S_FALSE;
			}

		} while (false);

		return lresult;
	}

	HRESULT DataParser::parsCaptureDeviceAttributeValue(
		REFGUID aGUID, 
		const PROPVARIANT& aVar,
		pugi::xml_node& Value)
	{
		HRESULT lresult(S_OK);

		do
		{
			if (aGUID == MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME)
			{
				readAttribute(
					aVar, 
					Value.append_child(L"SingleValue"));

				Value.append_attribute(L"Title") = L"The display name for a device.";
				
				Value.append_attribute(L"Description") = L"The display name is a human-readable string, suitable for display in a user interface.";

			}
			else if (aGUID == MF_DEVSOURCE_ATTRIBUTE_MEDIA_TYPE && aVar.vt == 4113)
			{
				MFT_REGISTER_TYPE_INFO* lptrInfo = (MFT_REGISTER_TYPE_INFO*)aVar.blob.pBlobData;
				
				Value.append_attribute(L"Title") = L"The device's output format.";

				Value.append_attribute(L"Description") = L"Specifies the output format of a device.";


				auto ValueParts = Value.append_child(L"Value.ValueParts");

				auto ValuePart = ValueParts.append_child(L"ValuePart");

				std::wstring lNameGUID;

				ValuePart.append_attribute(L"Title") = L"MajorType";

				lresult = GetGUIDName(lptrInfo->guidMajorType, lNameGUID);

				if(FAILED(lresult))
				{
					WCHAR *lptrName = nullptr;

					lresult = StringFromCLSID(lptrInfo->guidMajorType, &lptrName);

					if(FAILED(lresult))
						return lresult;

					lNameGUID = std::wstring(lptrName);

					CoTaskMemFree(lptrName);
				}

				ValuePart.append_attribute(L"Value") = lNameGUID.c_str();


				ValuePart = ValueParts.append_child(L"ValuePart");

				ValuePart.append_attribute(L"Title") = L"SubType";

				lresult = GetGUIDName(lptrInfo->guidSubtype, lNameGUID);

				if(FAILED(lresult))
				{
					WCHAR *lptrName = nullptr;

					lresult = StringFromCLSID(lptrInfo->guidSubtype, &lptrName);

					if(FAILED(lresult))
						return lresult;

					lNameGUID = std::wstring(lptrName);

					CoTaskMemFree(lptrName);
				}

				ValuePart.append_attribute(L"Value") = lNameGUID.c_str();
			}
			else if (aGUID == MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE)
			{
				readAttribute(
					aVar,
					Value.append_child(L"SingleValue"));

				Value.append_attribute(L"Title") = L"The type of device.";

				Value.append_attribute(L"Description") = L"Specifies a device's type, such as audio capture or video capture.";

			}			
			else if (aGUID == MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_ENDPOINT_ID)
			{
				readAttribute(
					aVar,
					Value.append_child(L"SingleValue"));

				Value.append_attribute(L"Title") = L"The endpoint ID for an audio capture device.";

				Value.append_attribute(L"Description") = L"Specifies the endpoint ID for an audio capture device.";
			}
			else if (aGUID == MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_ROLE)
			{
				auto lSingleValue = Value.append_child(L"SingleValue");

				readAttribute(
					aVar,
					lSingleValue);

				Value.append_attribute(L"Title") = L"The device role for an audio capture device.";

				Value.append_attribute(L"Description") = L"Specifies the device role for an audio capture device.";

				auto luintValue = lSingleValue.attribute(L"Value").as_uint(ERole_enum_count);

				std::wstring lvalue;

				switch (luintValue)
				{
				case eConsole:
					lvalue = L"eConsole";
					break;
				case eMultimedia:
					lvalue = L"eMultimedia";
					break;
				case eCommunications:
					lvalue = L"eCommunications";
					break;
				default:
					lvalue = L"unknown";
					break;
				}

				lSingleValue.remove_attribute(L"Value");
				
				lSingleValue.append_attribute(L"Value") = lvalue.c_str();
			}
			else if (aGUID == MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_CATEGORY)
			{
				readAttribute(
					aVar,
					Value.append_child(L"SingleValue"));

				Value.append_attribute(L"Title") = L"The device category for a video device.";

				Value.append_attribute(L"Description") = L"Specifies the device category for a video capture device.";
			}
			else if (aGUID == MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_HW_SOURCE)
			{
				auto lSingleValue = Value.append_child(L"SingleValue");

				readAttribute(
					aVar,
					lSingleValue);
				
				auto luintValue = lSingleValue.attribute(L"Value").as_uint(FALSE);

				lSingleValue.remove_attribute(L"Value");
				
				std::wstring lvalue = L"Hardware device";

				if (luintValue == FALSE)
				{
					lvalue = L"Software device";
				}
							
				lSingleValue.append_attribute(L"Value") = lvalue.c_str();

				Value.append_attribute(L"Title") = L"The type of video capture source.";

				Value.append_attribute(L"Description") = L"Specifies whether a video capture source is a hardware device or a software device.";

			}
			else if (aGUID == MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_HW_SOURCE)
			{
				auto lSingleValue = Value.append_child(L"SingleValue");

				readAttribute(
					aVar,
					lSingleValue);

				auto luintValue = lSingleValue.attribute(L"Value").as_uint(FALSE);

				lSingleValue.remove_attribute(L"Value");

				std::wstring lvalue = L"Hardware device";

				if (luintValue == FALSE)
				{
					lvalue = L"Software device";
				}

				lSingleValue.append_attribute(L"Value") = lvalue.c_str();

				Value.append_attribute(L"Title") = L"The type of audio capture source.";

				Value.append_attribute(L"Description") = L"Specifies whether a audio capture source is a hardware device or a software device.";

			}
			else if (aGUID == MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_MAX_BUFFERS)
			{
				readAttribute(
					aVar,
					Value.append_child(L"SingleValue"));

				Value.append_attribute(L"Title") = L"Size of the video capture source buffer.";

				Value.append_attribute(L"Description") = L"Specifies the maximum number of frames that the video capture source will buffer.";
			}
			else if (aGUID == MFT_HW_TIMESTAMP_WITH_QPC_Attribute)
			{
				auto lSingleValue = Value.append_child(L"SingleValue");

				readAttribute(
					aVar,
					lSingleValue);

				auto luintValue = lSingleValue.attribute(L"Value").as_uint(FALSE);

				lSingleValue.remove_attribute(L"Value");

				std::wstring lvalue = L"True";

				if (luintValue == FALSE)
				{
					lvalue = L"False";
				}

				lSingleValue.append_attribute(L"Value") = lvalue.c_str();

				Value.append_attribute(L"Title") = L"The device source uses the system time for time stamps.";

				Value.append_attribute(L"Description") = L"Specifies whether a hardware device source uses the system time for time stamps.";

			}
			else if (aGUID == MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK && aVar.vt == VT_LPWSTR)
			{
				readAttribute(
					aVar,
					Value.append_child(L"SingleValue"));

				Value.append_attribute(L"Title") = L"The symbolic link for a video capture driver.";

				Value.append_attribute(L"Description") = L"Contains the unique symbolic link for a video capture driver.";
			}
			else if (aGUID == MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_SYMBOLIC_LINK && aVar.vt == VT_LPWSTR)
			{
				readAttribute(
					aVar,
					Value.append_child(L"SingleValue"));

				Value.append_attribute(L"Title") = L"The symbolic link for a audio capture driver.";

				Value.append_attribute(L"Description") = L"Contains the unique symbolic link for a audio capture driver.";
			}
			else
			{
				lresult = S_FALSE;
			}

		} while (false);

		return lresult;
	}
	
}