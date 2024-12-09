#pragma once

namespace CaptureManager
{
	namespace Sources
	{
		enum class SourceState :int
		{
			SourceStateUninitialized = 0,
			SourceStateInitialized = SourceStateUninitialized + 1,
			SourceStateStopped = SourceStateInitialized + 1,
			SourceStatePaused = SourceStateStopped + 1,
			SourceStateStarted = SourceStatePaused + 1,
			SourceStateShutdown = SourceStateStarted + 1
		};
	}
}