#pragma once

#include "CaptureManagerTypeInfo.h"
#include "..\Common\BaseUnknown.h"
#include "..\Common\ComPtrCustom.h"
#include "..\Common\MFHeaders.h"
#include "..\PugiXML\pugixml.hpp"
#include <unordered_map>



namespace CaptureManager
{
	namespace COMServer
	{

		#define DeviceType_None		0
		#define DeviceType_Video	1
		#define DeviceType_Audio	2


		typedef std::unordered_map<DWORD, CaptureManager::CComPtrCustom<IMFMediaType>> StreamMediaType;
		

		class InitilaizeCaptureSource :
			public BaseUnknown<IInitilaizeCaptureSource>
		{
			CComPtrCustom<IMFDXGIDeviceManager> mDeviceManager;

			CComPtrCustom<IMFTransform> mMixer;

			CComPtrCustom<ID3D11Device> mID3D11Device;

			CComPtrCustom<ID3D11Texture2D> mCaptureID3D11Texture2D;

			CComPtrCustom<ID3D11Texture2D> mID3D11Texture2D;

			CComQIPtrCustom<ID3D11Texture2D> mSharedID3D11Texture2D;

			UINT mDeviceResetToken;

			HRESULT createDevice(ID3D11Device** aPtrPtrDevice);

			HRESULT createUncompressedVideoType(
				GUID				 subtype,  // FOURCC or D3DFORMAT value.     
				UINT32               width,
				UINT32               height,
				MFVideoInterlaceMode interlaceMode,
				const MFRatio&       frameRate,
				const MFRatio&       par,
				IMFMediaType         **ppType
				);	
			
			INT mDeviceType = DeviceType_None;

		public:
			InitilaizeCaptureSource();

			HRESULT processVideoMediaType(
				pugi::xml_node& aMediaType,
				IMFMediaType* aPtrVideoMediaType);

			HRESULT processAudioMediaType(
				pugi::xml_node& aMediaType,
				IMFMediaType* aPtrAudioMediaType);

			HRESULT processStreamDescriptor(pugi::xml_node& aStreamDescriptor,
				StreamMediaType& aStreamMediaType);

			//    IInitilaizeCaptureSource methods
			HRESULT STDMETHODCALLTYPE setPresentationDescriptor(
				/* [in] */ BSTR aXMLPresentationDescriptor);

			std::unordered_map<DWORD, StreamMediaType> mStreams;

			HRESULT getOutputTexture(ID3D11Texture2D** aPtrPtrOutputTexture);

			HRESULT getSharedTexture(ID3D11Texture2D** aPtrPtrSharedTexture);

			HRESULT getCaptureTexture(ID3D11Texture2D** aPtrPtrCaptureTexture);

			HRESULT getImmediateContext(ID3D11DeviceContext** aPtrPtrImmediateContext);

			HRESULT getDeviceManager(IMFDXGIDeviceManager** aPtrPtrDeviceManager);

			HRESULT getMixer(IMFTransform** aPtrPtrMixer);

			INT getDeviceType();

			std::wstring mStreamName;

		private:
			virtual ~InitilaizeCaptureSource();
		};

	}
}