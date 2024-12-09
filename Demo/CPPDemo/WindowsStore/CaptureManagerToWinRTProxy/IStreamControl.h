#pragma once

#include "pch.h"
#include "ISpreaderNodeFactory.h"

using namespace Platform;


namespace CaptureManagerToWinRTProxy
{
	public interface class IStreamControl
	{
		String^ getCollectionOfStreamControlNodeFactories();

		ISpreaderNodeFactory^ createSpreaderNodeFactory();

		ISpreaderNodeFactory^ createStreamControlNodeFactory(Guid aREFIID);
	};
}