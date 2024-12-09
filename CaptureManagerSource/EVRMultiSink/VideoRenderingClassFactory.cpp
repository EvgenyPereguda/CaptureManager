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

#include "VideoRenderingClassFactory.h"
#include "EVRMediaSink.h"
#include "DirectShowEVRMultiFilter.h"
#include "IPresenter.h"
#include "IIDCollection.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Common.h"
#include "../Common/Singleton.h"
#include "../WinmmManager/WinmmManager.h"


namespace EVRMultiSink
{
	using namespace CaptureManager;

	VideoRenderingClassFactory::VideoRenderingClassFactory()
	{
	}
		
	VideoRenderingClassFactory::~VideoRenderingClassFactory()
	{
	}
		
	HRESULT VideoRenderingClassFactory::createVideoRenderingClass(
		IUnknown* aPtrUnkPresenter,
		IUnknown* aPtrUnkMixer,
		DWORD aMixerStreamID,
		REFIID aRefIID,
		IUnknown** aPtrPtrVideoRenderingClass)
	{
		using namespace CaptureManager;

		using namespace Sinks;

		HRESULT lresult(E_FAIL);

		do
		{
			LOG_CHECK_PTR_MEMORY(aPtrUnkPresenter);

			LOG_CHECK_PTR_MEMORY(aPtrUnkMixer);

			LOG_CHECK_PTR_MEMORY(aPtrPtrVideoRenderingClass);

			CComPtrCustom<IPresenter> mPresenter;

			LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrUnkPresenter, &mPresenter);

			LOG_CHECK_PTR_MEMORY(mPresenter);


			CComPtrCustom<IMFTransform> mMixer;

			LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrUnkMixer, &mMixer);

			LOG_CHECK_PTR_MEMORY(mMixer);

			if (__uuidof(IMFMediaSink) == aRefIID)
			{
				using namespace Sinks::EVR;

				CComPtrCustom<IMFMediaSink> lEVRMediaSink;

				LOG_INVOKE_FUNCTION(EVRMediaSink::createEVRMediaSink,
					mPresenter,
					mMixer,
					aMixerStreamID,
					&lEVRMediaSink);

				LOG_CHECK_PTR_MEMORY(lEVRMediaSink);

				LOG_INVOKE_QUERY_INTERFACE_METHOD(lEVRMediaSink, aPtrPtrVideoRenderingClass);
			}
			else if (__uuidof(IBaseFilter) == aRefIID)
			{
				using namespace Filters::EVR;

				Singleton<Core::Winmm::WinmmManager>::getInstance().initialize();

				CComPtrCustom<IBaseFilter> lBaseFilter;

				LOG_INVOKE_FUNCTION(DirectShowEVRMultiFilter::createFilter,
					mPresenter,
					mMixer,
					aMixerStreamID,
					&lBaseFilter);

				LOG_CHECK_PTR_MEMORY(lBaseFilter);

				LOG_INVOKE_QUERY_INTERFACE_METHOD(lBaseFilter, aPtrPtrVideoRenderingClass);
			}
			else
			{
				lresult = E_NOINTERFACE;
			}

		} while (false);

		return lresult;
	}
}