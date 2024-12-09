#pragma once

namespace CaptureManager
{
	namespace Core
	{
		class BaseConfigManager
		{
		private:

			bool m_IsWindows8_Or_Greater;

			bool m_IsWindows8Point1_Or_Greater;

			bool m_IsWindows10_Or_Greater;

			bool m_IsWindows10_With_VP_Or_Greater;



			bool isWindows8_Or_Greater_Inner();

			bool isWindows8Point1_Or_Greater_Inner();

			bool isWindows10_Or_Greater_Inner();

			bool isWindows10_With_VP_Or_Greater_Inner();
			
		protected:
			BaseConfigManager();
			virtual ~BaseConfigManager();

		public:
			bool isWindows8_Or_Greater();

			bool isWindows8Point1_Or_Greater();

			bool isWindows10_Or_Greater();

			bool isWindows10_With_VP_Or_Greater();
		};
	}
}
