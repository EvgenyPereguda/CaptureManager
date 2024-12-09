#pragma once

#include "ComPtrCustom.h"
#include "../CaptureManager/CaptureManagerTypeInfo.h"
#include <vector>

class CaptureManagerProxy sealed
{
public:

	static CaptureManagerProxy& getInstance();

	bool isInitialized();

	bool release();
	
	bool getISourceControl(ISourceControl** aPtrPtrISourceControl);

	bool getISinkControl(ISinkControl** aPtrPtrISinkControl);

	bool getIStrideForBitmap(IStrideForBitmap** aPtrPtrIStrideForBitmap);

	bool getISessionControl(ISessionControl** aPtrPtrISessionControl);

	bool getISession(std::vector<IUnknown*> aSourcesList, ISession** aPtrPtrISession);

	//    bool getIWebCamControl(QString aSymbolicLink, IWebCamControl** aPtrPtrIWebCamControl);

	//    bool getIEncoderControl(IEncoderControl** aPtrPtrIEncoderControl);

	//    bool getIEncoderNodeFactory(IID aEncoderCLSID, IEncoderNodeFactory** aPtrPtrIEncoderNodeFactory);

	//    bool getOutputNodes(QString aFilePath, QList<IUnknown*> aCompressedMediaTypeList, QList<IUnknown*>& aOutputNodeList, IFileSinkFactory* aPtrIFileSinkFactory);

	//    bool getOutputNodes(IUnknown* aPtrByteStream, QList<IUnknown*> aCompressedMediaTypeList, QList<IUnknown*>& aOutputNodeList, IByteStreamSinkFactory* aPtrIByteStreamSinkFactory);


private:


	bool mIsInitialized;

	//CComPtrCustom<IUnknown> mILogPrintOutControl;

	CComPtrCustom<ICaptureManagerControl> mICaptureManagerControl;
	
	CaptureManagerProxy();


};

