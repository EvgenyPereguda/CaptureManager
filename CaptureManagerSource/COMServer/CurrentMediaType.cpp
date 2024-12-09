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

#include "CurrentMediaType.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../VideoRendererManager/Direct3D11VideoProcessor.h"
#include "../DirectXManager/Direct3D11Manager.h"
#include "../DirectXManager/DXGIManager.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Singleton.h"
#include "../Common/Common.h"
#include "../Common/GUIDs.h"
#include "../ConfigManager/ConfigManager.h"


namespace CaptureManager
{
	namespace COMServer
	{
		CurrentMediaType::CurrentMediaType() :
			mStreamIndex(0),
			mMediaTypeIndex(0)
		{}

		DWORD mStreamIndex;

		DWORD mMediaTypeIndex;

		CComQIPtrCustom<IMFMediaType> mCurrentMediaType;

		//    ICurrentMediaType methods

		HRESULT STDMETHODCALLTYPE CurrentMediaType::getMediaTypeIndex(
			/* [out] */ DWORD *aPtrMediaTypeIndex)
		{
			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrMediaTypeIndex);

				*aPtrMediaTypeIndex = mMediaTypeIndex;

				lresult = S_OK;

			} while (false);

			return lresult;
		}

		HRESULT STDMETHODCALLTYPE CurrentMediaType::getStreamIndex(
			/* [out] */ DWORD *aPtrStreamIndex)
		{
			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrStreamIndex);

				*aPtrStreamIndex = mStreamIndex;

				lresult = S_OK;

			} while (false);

			return lresult;
		}

		HRESULT STDMETHODCALLTYPE CurrentMediaType::getMediaType(
			/* [out] */ IUnknown **aPtrMediaType)
		{
			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrMediaType);

				LOG_CHECK_PTR_MEMORY(mCurrentMediaType);

				LOG_INVOKE_QUERY_INTERFACE_METHOD(mCurrentMediaType, aPtrMediaType);

			} while (false);

			return lresult;
		}


		CurrentMediaType::~CurrentMediaType()
		{
		}
	}
}