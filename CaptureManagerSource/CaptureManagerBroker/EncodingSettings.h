#pragma once

namespace CaptureManager
{
	struct EncodingSettings
	{
		enum EncodingMode: unsigned int
		{
			NONE = 0,
			CBR = NONE + 1,
			VBR = CBR + 1,
			StreamingCBR = VBR + 1
		};

		EncodingMode mEncodingMode = CBR;

		unsigned int mEncodingModeValue = 3000;

		unsigned int mAverageBitrateValue = 100000000;

	};
}