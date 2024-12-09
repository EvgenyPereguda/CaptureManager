#pragma once

#include "pch.h"

using namespace Platform;


namespace CaptureManagerToWinRTProxy
{
	public interface class ILogPrintOutControl
	{
		void setVerbose(
			int32 aLevelType,
			String^ aFilePath,
			Boolean aState);

		void addPrintOutDestination(
			int32 aLevelType,
			String^ aFilePath);

		void removePrintOutDestination(
			int32 aLevelType,
			String^ aFilePath);
	};
}