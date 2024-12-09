#include "pch.h"
#include "LogPrintOutControl.h"


namespace CaptureManagerToWinRTProxy
{
	LogPrintOutControl::LogPrintOutControl(IntPtr aIntPtr)
	{
		do
		{
			auto lPtrUnk = reinterpret_cast<IUnknown*>((void*)aIntPtr);

			if (lPtrUnk == nullptr)
				break;

			lPtrUnk->QueryInterface(IID_PPV_ARGS(&mILogPrintOutControl));

		} while (false);
	}

	void LogPrintOutControl::setVerbose(
		int32 aLevelType,
		String^ aFilePath,
		Boolean aState)
	{
		do
		{
			if (!mILogPrintOutControl)
				break;
			
			auto lSize = aFilePath->Length();

			std::unique_ptr<OLECHAR> lBSTRFilePath(new OLECHAR[lSize + 1]);

			memcpy(lBSTRFilePath.get(),
				aFilePath->Data(),
				(lSize + 1)*sizeof(OLECHAR));

			lBSTRFilePath.get()[lSize] = '\0';

			mILogPrintOutControl->setVerbose(
				aLevelType,
				lBSTRFilePath.get(),
				aState);

		} while (false);

	}

	void LogPrintOutControl::addPrintOutDestination(
		int32 aLevelType,
		String^ aFilePath)
	{
		do
		{
			if (!mILogPrintOutControl)
				break;

			auto lSize = aFilePath->Length();

			std::unique_ptr<OLECHAR> lBSTRFilePath(new OLECHAR[lSize + 1]);

			memcpy(lBSTRFilePath.get(),
				aFilePath->Data(),
				(lSize + 1)*sizeof(OLECHAR));

			lBSTRFilePath.get()[lSize] = '\0';

			mILogPrintOutControl->addPrintOutDestination(
				aLevelType,
				lBSTRFilePath.get());

		} while (false);
	}

	void LogPrintOutControl::removePrintOutDestination(
		int32 aLevelType,
		String^ aFilePath)
	{
		do
		{
			if (!mILogPrintOutControl)
				break;

			auto lSize = aFilePath->Length();

			std::unique_ptr<OLECHAR> lBSTRFilePath(new OLECHAR[lSize + 1]);

			memcpy(lBSTRFilePath.get(),
				aFilePath->Data(),
				(lSize + 1)*sizeof(OLECHAR));

			lBSTRFilePath.get()[lSize] = '\0';

			mILogPrintOutControl->removePrintOutDestination(
				aLevelType,
				lBSTRFilePath.get());

		} while (false);
	}
}