#pragma once

#include <Unknwnbase.h>

struct IMFMediaType;

namespace CaptureManager
{
	namespace Transform
	{
		namespace Encoder
		{
			class BaseEncoderManager
			{
			public:
				BaseEncoderManager();
				virtual ~BaseEncoderManager();

			protected:
				HRESULT modifySubTypeOFMediaType(
					IMFMediaType** aPtrPtrInputMediaType,
					REFGUID aNewSubType);
			};
		}
	}
}