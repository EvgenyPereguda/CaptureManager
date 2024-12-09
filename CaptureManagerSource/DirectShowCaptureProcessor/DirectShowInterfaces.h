#pragma once

#include <Unknwnbase.h>
#include <strmif.h>

namespace CaptureManager
{
	namespace Sources
	{
		namespace DirectShow
		{			
			class DECLSPEC_UUID("BF87B6E1-8C27-11d0-B3F0-00AA003761C5")
				CLSID_CaptureGraphBuilder2Proxy;

			class DECLSPEC_UUID("e436ebb3-524f-11ce-9f53-0020af0ba770")
				CLSID_FilterGraphProxy;

			class DECLSPEC_UUID("fb6c4282-0353-11d1-905f-0000c0cc16ba")
				PIN_CATEGORY_PREVIEWProxy;

			class DECLSPEC_UUID("fb6c4281-0353-11d1-905f-0000c0cc16ba")
				PIN_CATEGORY_CAPTUREProxy;

			class DECLSPEC_UUID("fb6c428a-0353-11d1-905f-0000c0cc16ba")
				PIN_CATEGORY_STILLProxy;

			class DECLSPEC_UUID("73646976-0000-0010-8000-00AA00389B71")
				MEDIATYPE_VideoProxy;

			class DECLSPEC_UUID("C1F400A0-3F08-11d3-9F0B-006008039E37")
				CLSID_SampleGrabberProxy;

			class DECLSPEC_UUID("C1F400A4-3F08-11d3-9F0B-006008039E37")
				CLSID_NullRendererProxy;

			// {0579154A-2B53-4994-B0D0-E773148EFF85}
			DEFINE_GUID(IID_ISampleGrabberCB,
				0x0579154A, 0x2B53, 0x4994, 0xB0, 0xD0, 0xE7, 0x73, 0x14, 0x8E, 0xFF, 0x85);

			MIDL_INTERFACE("0579154A-2B53-4994-B0D0-E773148EFF85")
			ISampleGrabberCB : public IUnknown
			{
			public:
				virtual HRESULT STDMETHODCALLTYPE SampleCB(
					double SampleTime,
					IMediaSample *pSample) = 0;

				virtual HRESULT STDMETHODCALLTYPE BufferCB(
					double SampleTime,
					BYTE *pBuffer,
					long BufferLen) = 0;

			};

			// {6B652FFF-11FE-4fce-92AD-0266B5D7C78F}
			DEFINE_GUID(IID_ISampleGrabber,
				0x6B652FFF, 0x11FE, 0x4fce, 0x92, 0xAD, 0x02, 0x66, 0xB5, 0xD7, 0xC7, 0x8F);

			MIDL_INTERFACE("6B652FFF-11FE-4fce-92AD-0266B5D7C78F")
			ISampleGrabber : public IUnknown
			{
			public:
				virtual HRESULT STDMETHODCALLTYPE SetOneShot(
					BOOL OneShot) = 0;

				virtual HRESULT STDMETHODCALLTYPE SetMediaType(
					const AM_MEDIA_TYPE *pType) = 0;

				virtual HRESULT STDMETHODCALLTYPE GetConnectedMediaType(
					AM_MEDIA_TYPE *pType) = 0;

				virtual HRESULT STDMETHODCALLTYPE SetBufferSamples(
					BOOL BufferThem) = 0;

				virtual HRESULT STDMETHODCALLTYPE GetCurrentBuffer(
					/* [out][in] */ long *pBufferSize,
					/* [out] */ long *pBuffer) = 0;

				virtual HRESULT STDMETHODCALLTYPE GetCurrentSample(
					/* [retval][out] */ IMediaSample **ppSample) = 0;

				virtual HRESULT STDMETHODCALLTYPE SetCallback(
					ISampleGrabberCB *pCallback,
					long WhichMethodToCallback) = 0;

			};


		}
	}
}