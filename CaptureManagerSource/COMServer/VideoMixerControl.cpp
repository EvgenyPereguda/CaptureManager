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

#include "VideoMixerControl.h"
#include "../Common/ComPtrCustom.h"
#include "../Common/Singleton.h"
#include "../Common/Common.h"
#include "../CaptureManagerBroker/CaptureManagerBroker.h"
#include "../LogPrintOut/LogPrintOut.h"


namespace CaptureManager
{
	namespace COMServer
	{

		VideoMixerControl::VideoMixerControl()
		{
		}


		VideoMixerControl::~VideoMixerControl()
		{
		}

		// IVideoMixerControl methods

		HRESULT STDMETHODCALLTYPE VideoMixerControl::setPosition(
			/* [in] */ IUnknown *aPtrVideoMixerNode,
			/* [in] */ FLOAT aLeft,
			/* [in] */ FLOAT aRight,
			/* [in] */ FLOAT aTop,
			/* [in] */ FLOAT aBottom)
		{

			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrVideoMixerNode);

				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().setVideoMixerPosition,
					aPtrVideoMixerNode,
					aLeft,
					aRight,
					aTop,
					aBottom);

			} while (false);

			return lresult;
		}

		HRESULT STDMETHODCALLTYPE VideoMixerControl::setSrcPosition(
			/* [in] */ IUnknown *aPtrVideoMixerNode,
			/* [in] */ FLOAT aLeft,
			/* [in] */ FLOAT aRight,
			/* [in] */ FLOAT aTop,
			/* [in] */ FLOAT aBottom)
		{

			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrVideoMixerNode);

				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().setVideoMixerSrcPosition,
					aPtrVideoMixerNode,
					aLeft,
					aRight,
					aTop,
					aBottom);

			} while (false);

			return lresult;
		}

		HRESULT STDMETHODCALLTYPE VideoMixerControl::setZOrder(
			/* [in] */ IUnknown *aPtrVideoMixerNode,
			/* [in] */ DWORD aZOrder)
		{

			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrVideoMixerNode);

				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().setVideoMixerZOrder,
					aPtrVideoMixerNode,
					aZOrder);

			} while (false);

			return lresult;
		}

		HRESULT STDMETHODCALLTYPE VideoMixerControl::setOpacity(
			/* [in] */ IUnknown *aPtrVideoMixerNode,
			/* [in] */ FLOAT aOpacity)
		{

			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrVideoMixerNode);

				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().setVideoMixerOpacity,
					aPtrVideoMixerNode,
					aOpacity);

			} while (false);

			return lresult;
		}

		HRESULT STDMETHODCALLTYPE VideoMixerControl::flush(
			/* [in] */ IUnknown *aPtrVideoMixerNode)
		{

			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrVideoMixerNode);

				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().setVideoMixerFlush,
					aPtrVideoMixerNode);

			} while (false);

			return lresult;
		}
	}
}