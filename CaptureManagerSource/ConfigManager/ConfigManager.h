#pragma once

#include <Unknwnbase.h>

#include "../BaseConfigManager/BaseConfigManager.h"

namespace CaptureManager
{
	namespace Core
	{
		class ConfigManager: public BaseConfigManager
		{
		public:

			DWORD getMaxVideoRenderStreamCount(IUnknown* aPtrDevice = nullptr);
			
			DWORD getMaxAudioInputMixerNodeAmount();
			
		protected:
			ConfigManager();
			virtual ~ConfigManager();
		};
	}
}
