#pragma once

#include <guiddef.h>
#include <string>
#include <vector>

namespace CaptureManager
{
	struct StreamControlInfo
	{
		GUID mGUID;

		std::wstring mName;

		std::wstring mTitle;

		bool mIsInputPortCountConstant;

		bool mIsOutputPortCountConstant;

		std::size_t mInputPortCount;

		std::size_t mOutputPortCount;

		std::vector<StreamControlInfo> mCollection;
	};
}
