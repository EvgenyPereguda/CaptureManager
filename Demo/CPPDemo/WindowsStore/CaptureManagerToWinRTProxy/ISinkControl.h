#pragma once

#include "pch.h"
#include "IFileSinkFactory.h"
#include "IByteStreamSinkFactory.h"
#include "IEVRSinkFactory.h"
#include "ISampleGrabberCallbackSinkFactory.h"
#include "ISampleGrabberCallSinkFactory.h"


using namespace Platform;


namespace CaptureManagerToWinRTProxy
{
	public interface class ISinkControl
	{
		String^ getCollectionOfSinks();

		IFileSinkFactory^ createIFileSinkFactory(
			Guid aRefContainerTypeGUID);

		//IByteStreamSinkFactory^ createIByteStreamSinkFactory(
		//	Guid aRefContainerTypeGUID);

		IEVRSinkFactory^ createIEVRSinkFactory();

		ISampleGrabberCallbackSinkFactory^ createISampleGrabberCallbackSinkFactory(
			Guid aRefContainerTypeGUID);

		ISampleGrabberCallSinkFactory^ createISampleGrabberCallSinkFactory(
			Guid aRefContainerTypeGUID);	
		
	};
}