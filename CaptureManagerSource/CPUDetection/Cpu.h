#pragma once


namespace CaptureManager
{
	namespace Core
	{
		namespace CPUDetection
		{
			class Cpu
			{
			public:

				bool is_SSE4_2();

				bool is_SSSE3();

				bool is_SSE2();				

			protected:
				Cpu();
				virtual ~Cpu();
			};
		}
	}
}