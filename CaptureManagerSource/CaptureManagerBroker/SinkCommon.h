#pragma once

#include <guiddef.h>
#include <string>
#include <vector>


namespace CaptureManager
{
	namespace Core
	{
		class IMaker;
	}

	enum SinkType
	{
		File = 0,
		ByteStream = File + 1,
		SampleGrabberCall = ByteStream + 1
	};

	typedef bool(*IsAvailabilityFn)();

	struct GUIDToNamePair
	{
		GUID mGUID;

		std::wstring mName;

		std::wstring mTitle;

		std::wstring mMIME;

		size_t mMaxPortCount;

		int mPriority;

		std::vector<GUIDToNamePair> mContainers;

		std::vector<SinkType> mSinkTypes;

		IsAvailabilityFn mIsAvailability;
	};
}

