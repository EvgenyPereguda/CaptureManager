#pragma once
#include "ILogPrintOutControl.h"
#include "CaptureManagerTypeInfo.h"
#include "ComPtrCustom.h"


namespace CaptureManagerToWinRTProxy
{
	ref class LogPrintOutControl sealed :
		public ILogPrintOutControl
	{
	private:

		CComPtrCustom<::ILogPrintOutControl> mILogPrintOutControl;
	public:
		LogPrintOutControl(IntPtr aIntPtr);
		
		virtual void setVerbose(
			int32 aLevelType,
			String^ aFilePath,
			Boolean aState);

		virtual void addPrintOutDestination(
			int32 aLevelType,
			String^ aFilePath);

		virtual void removePrintOutDestination(
			int32 aLevelType,
			String^ aFilePath);
	};
}