#include "pch.h"
#include "WrapperIntPtr.h"

namespace CaptureManagerToWinRTProxy
{
	
		WrapperIntPtr::WrapperIntPtr(Platform::IntPtr aIntPtr)
		{
			if ((void*)aIntPtr != nullptr)
			{
				auto lPtrUnk = reinterpret_cast<IUnknown*>((void*)aIntPtr);

				if (lPtrUnk == nullptr)
					return;

				lPtrUnk->QueryInterface(IID_PPV_ARGS(&m_Unk));
			}
		}

		Platform::IntPtr WrapperIntPtr::getIntPtr()
		{
			Platform::IntPtr lresult = nullptr;

			do
			{
				lresult = Platform::IntPtr(m_Unk.Get());

			} while (false);

			return lresult;
		}
}