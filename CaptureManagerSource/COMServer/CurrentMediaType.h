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
		class CurrentMediaType :
			public BaseUnknown < ICurrentMediaType >
		{
		public:
			CurrentMediaType();

			DWORD mStreamIndex;

			DWORD mMediaTypeIndex;

			CComQIPtrCustom<IMFMediaType> mCurrentMediaType;

			//    ICurrentMediaType methods

			virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE getMediaTypeIndex(
				/* [out] */ DWORD *aPtrMediaTypeIndex)override;

			virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE getStreamIndex(
				/* [out] */ DWORD *aPtrStreamIndex)override;
			
			virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE getMediaType(
				/* [out] */ IUnknown **aPtrMediaType)override;

		private:
			virtual ~CurrentMediaType();
		};

	}
}