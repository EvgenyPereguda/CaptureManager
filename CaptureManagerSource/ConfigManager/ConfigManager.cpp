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

#include "ConfigManager.h"
#include "../Common/MFHeaders.h"
#include "../DirectXManager/DXGIManager.h"
#include "../DirectXManager/Direct3D11Manager.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/ComPtrCustom.h"
#include "../Common/Common.h"
#include "../Common/Singleton.h"
#include <dxgi1_3.h>

namespace CaptureManager
{
	namespace Core
	{
		using namespace Core;
		using namespace Core::DXGI;
		using namespace Core::Direct3D11;

		ConfigManager::ConfigManager():
			BaseConfigManager()
		{}
		
		ConfigManager::~ConfigManager()
		{
		}

		DWORD ConfigManager::getMaxVideoRenderStreamCount(IUnknown* aPtrDevice)
		{
			return 2;
		}

		DWORD ConfigManager::getMaxAudioInputMixerNodeAmount()
		{
			return 2;
		}

	}
}