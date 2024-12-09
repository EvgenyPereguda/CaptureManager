#pragma once

#include "pch.h"

using namespace Platform;
using namespace Windows::Foundation::Collections;

namespace CaptureManagerToWinRTProxy
{
	public interface class IInitilaizeCaptureSource
	{
		void setPresentationDescriptor(String^ aPresentationDescriptor);
	};

}