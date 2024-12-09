#pragma once

#include <Unknwnbase.h>

namespace CaptureManager
{
	namespace Core
	{
		class MCSSManager
		{
		public:

			HRESULT initialize();

			HRESULT createCaptureWorkQueue(DWORD* aPtrWorkQueue);

			HRESULT createProAudioWorkQueue(DWORD* aPtrWorkQueue);

			HRESULT unlockWorkQueue(DWORD aWorkQueue);

		protected:
			MCSSManager();
			virtual ~MCSSManager();

		private:

			static const wchar_t* mProAudioTaskName;

			static const wchar_t* mCaptureTaskName;

			static const wchar_t* mLowLatencyTaskName;

			static DWORD mProAudioTaskID;

			static DWORD mCaptureTaskID;

		};
	}
}