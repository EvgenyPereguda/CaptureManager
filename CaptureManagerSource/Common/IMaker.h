#pragma once

struct IUnknown;

namespace CaptureManager
{
	namespace Core
	{
		class IMaker
		{
		public:
			virtual IUnknown * make() const = 0;

			virtual ~IMaker()
			{
			}
		};
	}
}
