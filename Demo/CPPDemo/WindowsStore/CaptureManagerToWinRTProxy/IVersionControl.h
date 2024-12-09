#pragma once

#include "pch.h"

using namespace Platform;


namespace CaptureManagerToWinRTProxy
{
	public interface class IVersionControl
	{
		String^ getVersion(
			uint32 *aPtrMAJOR,
			uint32 *aPtrMINOR,
			uint32 *aPtrPATCH);

		String^ getXMLStringVersion();

		Boolean checkVersion(
			uint32 aMAJOR,
			uint32 aMINOR,
			uint32 aPATCH);
	};
}