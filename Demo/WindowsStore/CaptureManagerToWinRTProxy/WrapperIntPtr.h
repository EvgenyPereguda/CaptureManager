#pragma once

#include "pch.h"
#include "Interfaces/IIntPtr.h"

namespace CaptureManagerToWinRTProxy
{
	public ref class WrapperIntPtr sealed : public IIntPtr
	{
	public:
		WrapperIntPtr(Platform::IntPtr aIntPtr);

		Platform::IntPtr getIntPtr();

	private:

		Microsoft::WRL::ComPtr<IUnknown> m_Unk;
	};
}