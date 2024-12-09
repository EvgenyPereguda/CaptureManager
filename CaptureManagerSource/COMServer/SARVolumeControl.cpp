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

#include <string>
#include <algorithm>


#include "SARVolumeControl.h"
#include "../Common/ComPtrCustom.h"
#include "../Common/MFHeaders.h"
#include "../Common/InstanceMaker.h"
#include "SinkCollection.h"
#include "../CaptureManagerBroker/CaptureManagerBroker.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Common.h"
#include "../Common/Singleton.h"

namespace CaptureManager
{
	namespace COMServer
	{
		SARVolumeControl::SARVolumeControl()
		{
		}


		SARVolumeControl::~SARVolumeControl()
		{
		}

		//	ISARVolumeControl implements
			   
		HRESULT STDMETHODCALLTYPE SARVolumeControl::getChannelCount(
			/* [in] */ IUnknown *aPtrSARNode,
			/* [out] */ UINT32 *aPtrCount)
		{

			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrSARNode);

				LOG_CHECK_PTR_MEMORY(aPtrCount);

				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().getSARChannelCount,
					aPtrSARNode,
					aPtrCount);

			} while (false);

			return lresult;
		}

		HRESULT STDMETHODCALLTYPE SARVolumeControl::setChannelVolume(
			/* [in] */ IUnknown *aPtrSARNode,
			/* [in] */ UINT32 aIndex,
			/* [in] */ const float aLevel)
		{

			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrSARNode);

				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().setSARChannelVolume,
					aPtrSARNode,
					aIndex,
					aLevel);

			} while (false);

			return lresult;
		}

		HRESULT STDMETHODCALLTYPE SARVolumeControl::getChannelVolume(
			/* [in] */ IUnknown *aPtrSARNode,
			/* [in] */ UINT32 aIndex,
			/* [out] */ float *aPtrLevel)
		{

			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrSARNode);

				LOG_CHECK_PTR_MEMORY(aPtrLevel);

				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().getSARChannelVolume,
					aPtrSARNode,
					aIndex,
					aPtrLevel);

			} while (false);

			return lresult;
		}
	}
}