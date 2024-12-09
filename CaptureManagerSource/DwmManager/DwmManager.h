#pragma once

#include "../Common/BaseUnknown.h"
#include "../Common/MFHeaders.h"
#include "../Common/ComPtrCustom.h"

namespace CaptureManager
{
	namespace Core
	{
		namespace Dwm
		{			
			class DwmManager
			{
			public:
				BOOL EnableDwmQueuing(HWND a_Hwnd);
			protected:
				DwmManager();
				virtual ~DwmManager();
			private:
				void initialize();
			};
		}
	}
}