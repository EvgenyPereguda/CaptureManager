#include "pch.h"
#include "CaptureManagerProxy.h"

#pragma comment(lib, "OleAut32.lib")

using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::UI::Core;

CaptureManagerProxy::CaptureManagerProxy()
{

	bool lresult = false;

	do
	{
		do
		{

			const GUID lCLSID_CoLogPrintOut = { 0x4563EE3E, 0xDA1E, 0x4911, { 0x9F, 0x40, 0x88, 0xA2, 0x84, 0xE2, 0xDD, 0x69 } };

			const GUID lCLSID_CoCaptureManager = { 0xD5F07FB8, 0xCE60, 0x4017, { 0xB2, 0x15, 0x95, 0xC8, 0xA0, 0xDD, 0xF4, 0x2A } };

			
			HRESULT hr;

			MULTI_QI arrMultiQI[1] = { &__uuidof(ILogPrintOutControl), nullptr, S_OK };
			
			hr = CoCreateInstanceFromApp(lCLSID_CoLogPrintOut, nullptr, CLSCTX_ALL, nullptr, 1, arrMultiQI);

			if (FAILED(hr) || arrMultiQI[0].pItf == nullptr)
			{
				break;
			}

			CComPtrCustom<ILogPrintOutControl> lILogPrintOutControl;

			arrMultiQI[0].pItf->QueryInterface(IID_PPV_ARGS(&lILogPrintOutControl));

			arrMultiQI[0].pItf->Release();

			arrMultiQI[0].pItf = nullptr;


			StorageFolder^ localFolder = ApplicationData::Current->LocalFolder;

			auto lLogFilePath = localFolder->Path + "\\Log.txt";



			auto lSize = lLogFilePath->Length();

			std::unique_ptr<OLECHAR> lBSTRFilePath(new OLECHAR[lSize + 1]);

			memcpy(lBSTRFilePath.get(),
				lLogFilePath->Data(),
				(lSize + 1)*sizeof(OLECHAR));

			lBSTRFilePath.get()[lSize] = '\0';
			
			lILogPrintOutControl->addPrintOutDestination(ERROR_LEVEL, lBSTRFilePath.get());




			arrMultiQI[0] = { &__uuidof(ICaptureManagerControl), nullptr, S_OK };

			hr = CoCreateInstanceFromApp(lCLSID_CoCaptureManager, nullptr, CLSCTX_ALL, nullptr, 1, arrMultiQI);

			if (FAILED(hr) || arrMultiQI[0].pItf == nullptr)
			{
				break;
			}

			arrMultiQI[0].pItf->QueryInterface(IID_PPV_ARGS(&mICaptureManagerControl));

			arrMultiQI[0].pItf->Release();

			arrMultiQI[0].pItf = nullptr;
			
			
			lresult = true;

		} while (false);

	} while (false);

	mIsInitialized = lresult;
	
}

CaptureManagerProxy& CaptureManagerProxy::getInstance()
{
	static CaptureManagerProxy lInstance;

	return lInstance;
}

bool CaptureManagerProxy::isInitialized()
{
	return mIsInitialized;
}

bool CaptureManagerProxy::release()
{
	if (mIsInitialized)
	{
		mICaptureManagerControl.Release();

		mIsInitialized = false;
	}

	return mIsInitialized;
}

bool CaptureManagerProxy::getISourceControl(ISourceControl** aPtrPtrISourceControl)
{
    bool lresult = false;

    do
    {
        if(!mIsInitialized)
            break;

        if(aPtrPtrISourceControl == nullptr)
            break;

		if (mICaptureManagerControl == nullptr)
            break;

		auto lhresult = mICaptureManagerControl->createControl(
					__uuidof(ISourceControl),
                    (IUnknown**)aPtrPtrISourceControl);
		
        if(lhresult != 0)
            break;

        lresult = true;
    }
    while(false);

    return lresult;
}

bool CaptureManagerProxy::getISinkControl(ISinkControl** aPtrPtrISinkControl)
{
    bool lresult = false;

    do
    {
        if(!mIsInitialized)
            break;

        if(aPtrPtrISinkControl == nullptr)
            break;

		auto lhresult = mICaptureManagerControl->createControl(
					__uuidof(ISinkControl),
                    (IUnknown**)aPtrPtrISinkControl);

        if(lhresult != 0)
            break;

        lresult = true;
    }
    while(false);

    return lresult;
}

bool CaptureManagerProxy::getIStrideForBitmap(IStrideForBitmap** aPtrPtrIStrideForBitmap)
{
    bool lresult = false;

    do
    {
        if(!mIsInitialized)
            break;

        if(aPtrPtrIStrideForBitmap == nullptr)
            break;

		auto lhresult = mICaptureManagerControl->createMisc(
					__uuidof(IStrideForBitmap),
                    (IUnknown**)aPtrPtrIStrideForBitmap);

        if(lhresult != 0)
            break;

        lresult = true;
    }
    while(false);

    return lresult;
}

bool CaptureManagerProxy::getISessionControl(ISessionControl** aPtrPtrISessionControl)
{
    bool lresult = false;

    do
    {
        if(!mIsInitialized)
            break;

        if(aPtrPtrISessionControl == nullptr)
            break;

		auto lhresult = mICaptureManagerControl->createControl(
					__uuidof(ISessionControl),
                    (IUnknown**)aPtrPtrISessionControl);
		
        if(lhresult != 0)
            break;

        lresult = true;
    }
    while(false);

    return lresult;
}

bool CaptureManagerProxy::getISession(std::vector<IUnknown*> aSourcesList, ISession** aPtrPtrISession)
{
    bool lresult = false;

    do
    {
        if(!mIsInitialized)
            break;

        if(aPtrPtrISession == nullptr)
            break;

        if(aSourcesList.empty())
            break;

        ISessionControl* lISessionControl;

        if(getISessionControl(&lISessionControl))
        {
            SAFEARRAY *lPtrSAFEARRAY = nullptr;

            SAFEARRAYBOUND lbound[1];

            lbound[0].lLbound = 0;

			lbound[0].cElements = aSourcesList.size();

            lPtrSAFEARRAY = SafeArrayCreate(
                        VT_VARIANT,
                        1,
                        lbound);


            for(LONG lIndex = 0;
				lIndex < aSourcesList.size();
                lIndex++)
            {
                auto lISourceTopologyNode = aSourcesList.at(lIndex);

                VARIANT lVar;

                VariantInit(&lVar);

                lVar.vt = VT_UNKNOWN;

                lVar.punkVal = lISourceTopologyNode;

                SafeArrayPutElement(lPtrSAFEARRAY, &lIndex, &lVar);

            }


             VARIANT lsourceNodesSAFEARRAY;

             VariantInit(&lsourceNodesSAFEARRAY);

             lsourceNodesSAFEARRAY.vt = VT_UNKNOWN | VT_SAFEARRAY;

             lsourceNodesSAFEARRAY.parray = lPtrSAFEARRAY;

             auto lhresult = lISessionControl->createSession(
                         lsourceNodesSAFEARRAY,
						 __uuidof(ISession),
                         (IUnknown**)aPtrPtrISession);

             SafeArrayDestroy(lPtrSAFEARRAY);


             if(lhresult != 0)
                 break;

             lresult = true;

        }

    }
    while(false);

    return lresult;
}

//bool CaptureManagerProxy::getIWebCamControl(QString aSymbolicLink, IWebCamControl** aPtrPtrIWebCamControl)
//{
//    bool lresult = false;

//    do
//    {
//        if(!mIsInitialized)
//            break;

//        if(aPtrPtrIWebCamControl == nullptr)
//            break;

//        if(aSymbolicLink.isEmpty())
//            break;

//        ISourceControl* lISourceControl;

//        if(getISourceControl(&lISourceControl))
//        {
//            auto lSize = aSymbolicLink.toStdWString().size();

//            std::unique_ptr<OLECHAR> lBSTRSymbolicLink(new OLECHAR[lSize + 1]);

//            memcpy(lBSTRSymbolicLink.get(),
//                    aSymbolicLink.toStdWString().c_str(),
//                    (lSize + 1)*sizeof(OLECHAR));

//            lBSTRSymbolicLink.get()[lSize] = '\0';

//            auto lhresult= lISourceControl->createSourceControl(
//                        lBSTRSymbolicLink.get(),
//                        IID_IWebCamControl,
//                        (IUnknown**)aPtrPtrIWebCamControl);


//             if(lhresult != 0)
//                 break;

//             lresult = true;

//        }

//    }
//    while(false);

//    return lresult;
//}

//bool CaptureManagerProxy::getIEncoderControl(IEncoderControl** aPtrPtrIEncoderControl)
//{
//    bool lresult = false;

//    do
//    {
//        if(!mIsInitialized)
//            break;

//        if(aPtrPtrIEncoderControl == nullptr)
//            break;

//        ICaptureManagerControl* lPtrICaptureManagerControl = nullptr;

//        auto lhresult = mCaptureManagerObject->queryInterface(
//                    IID_ICaptureManagerControl,
//                    (void**)&lPtrICaptureManagerControl);

//        if(lhresult != 0)
//            break;

//        if(lPtrICaptureManagerControl == nullptr)
//            break;

//        lhresult = lPtrICaptureManagerControl->createControl(
//                    IID_IEncoderControl,
//                    (IUnknown**)aPtrPtrIEncoderControl);

//        lPtrICaptureManagerControl->Release();

//        if(lhresult != 0)
//            break;

//        lresult = true;
//    }
//    while(false);

//    return lresult;
//}

//bool CaptureManagerProxy::getIEncoderNodeFactory(IID aEncoderCLSID, IEncoderNodeFactory** aPtrPtrIEncoderNodeFactory)
//{
//    bool lresult = false;

//    do
//    {
//        if(!mIsInitialized)
//            break;

//        if(aPtrPtrIEncoderNodeFactory == nullptr)
//            break;

//        IEncoderControl* lPtrIEncoderControl = nullptr;

//        if(getIEncoderControl(&lPtrIEncoderControl))
//        {
//            auto lhresult = lPtrIEncoderControl->createEncoderNodeFactory(
//                        aEncoderCLSID,
//                        IID_IEncoderNodeFactory,
//                        (IUnknown**)aPtrPtrIEncoderNodeFactory);

//            lPtrIEncoderControl->Release();

//            if(lhresult != 0)
//                break;
//        }


//        lresult = true;
//    }
//    while(false);

//    return lresult;
//}

//bool CaptureManagerProxy::getOutputNodes(
//        QString aFilePath,
//        QList<IUnknown*> aCompressedMediaTypeList,
//        QList<IUnknown*>& aOutputNodeList,
//        IFileSinkFactory* aPtrIFileSinkFactory)
//{
//    bool lresult = false;

//    do
//    {
//        if(!mIsInitialized)
//            break;

//        if(aPtrIFileSinkFactory == nullptr)
//            break;

//        if(aCompressedMediaTypeList.isEmpty())
//            break;

//        ISessionControl* lISessionControl;

//        if(getISessionControl(&lISessionControl))
//        {
//            SAFEARRAY *lPtrSAFEARRAY = nullptr;

//            SAFEARRAYBOUND lbound[1];

//            lbound[0].lLbound = 0;

//            lbound[0].cElements = aCompressedMediaTypeList.length();

//            lPtrSAFEARRAY = SafeArrayCreate(
//                        VT_VARIANT,
//                        1,
//                        lbound);


//            for(LONG lIndex = 0;
//                lIndex < aCompressedMediaTypeList.length();
//                lIndex++)
//            {
//                auto lISourceTopologyNode = aCompressedMediaTypeList.at(lIndex);

//                VARIANT lVar;

//                VariantInit(&lVar);

//                lVar.vt = VT_UNKNOWN;

//                lVar.punkVal = lISourceTopologyNode;

//                SafeArrayPutElement(lPtrSAFEARRAY, &lIndex, &lVar);

//            }


//             VARIANT lCompressedMediaTypeSAFEARRAY;

//             VariantInit(&lCompressedMediaTypeSAFEARRAY);

//             lCompressedMediaTypeSAFEARRAY.vt = VT_UNKNOWN | VT_SAFEARRAY;

//             lCompressedMediaTypeSAFEARRAY.parray = lPtrSAFEARRAY;

//             VARIANT lOutputNodeSAFEARRAY;




//             auto lSize = aFilePath.toStdWString().size();

//             std::unique_ptr<OLECHAR> lBSTRFilePath(new OLECHAR[lSize + 1]);

//             memcpy(lBSTRFilePath.get(),
//                     aFilePath.toStdWString().c_str(),
//                     (lSize + 1)*sizeof(OLECHAR));

//             lBSTRFilePath.get()[lSize] = '\0';


//             auto lhresult = aPtrIFileSinkFactory->createOutputNodes(
//                         lCompressedMediaTypeSAFEARRAY,
//                         lBSTRFilePath.get(),
//                         &lOutputNodeSAFEARRAY);

//             SafeArrayDestroy(lPtrSAFEARRAY);


//             if(lhresult != 0)
//                 break;

//             if(lOutputNodeSAFEARRAY.vt & VT_UNKNOWN &&
//                     lOutputNodeSAFEARRAY.vt & VT_SAFEARRAY)
//             {

//                 SAFEARRAY* lPtrSAoutputNodes;

//                 lPtrSAoutputNodes = lOutputNodeSAFEARRAY.parray;

//                 LONG lBoundSourcesNodes;

//                 LONG uBoundSourcesNodes;

//                 SafeArrayGetUBound(lPtrSAoutputNodes, 1, &uBoundSourcesNodes);

//                 SafeArrayGetLBound(lPtrSAoutputNodes, 1, &lBoundSourcesNodes);

//                 for (LONG lIndex = lBoundSourcesNodes; lIndex <= uBoundSourcesNodes; lIndex++)
//                 {
//                     VARIANT lVar;

//                     SafeArrayGetElement(lPtrSAoutputNodes, &lIndex, &lVar);

//                     if (lVar.vt == VT_UNKNOWN)
//                     {
//                         aOutputNodeList.push_back(lVar.punkVal);
//                     }
//                 }

//             }

//             VariantClear(&lOutputNodeSAFEARRAY);


//             lresult = true;

//        }

//    }
//    while(false);

//    return lresult;
//}

//bool CaptureManagerProxy::getOutputNodes(
//        IUnknown* aPtrByteStream,
//        QList<IUnknown*> aCompressedMediaTypeList,
//        QList<IUnknown*>& aOutputNodeList,
//        IByteStreamSinkFactory* aPtrIByteStreamSinkFactory)
//{
//    bool lresult = false;

//    do
//    {
//        if(!mIsInitialized)
//            break;

//        if(aPtrIByteStreamSinkFactory == nullptr)
//            break;

//        if(aCompressedMediaTypeList.isEmpty())
//            break;

//        ISessionControl* lISessionControl;

//        if(getISessionControl(&lISessionControl))
//        {
//            SAFEARRAY *lPtrSAFEARRAY = nullptr;

//            SAFEARRAYBOUND lbound[1];

//            lbound[0].lLbound = 0;

//            lbound[0].cElements = aCompressedMediaTypeList.length();

//            lPtrSAFEARRAY = SafeArrayCreate(
//                        VT_VARIANT,
//                        1,
//                        lbound);


//            for(LONG lIndex = 0;
//                lIndex < aCompressedMediaTypeList.length();
//                lIndex++)
//            {
//                auto lISourceTopologyNode = aCompressedMediaTypeList.at(lIndex);

//                VARIANT lVar;

//                VariantInit(&lVar);

//                lVar.vt = VT_UNKNOWN;

//                lVar.punkVal = lISourceTopologyNode;

//                SafeArrayPutElement(lPtrSAFEARRAY, &lIndex, &lVar);

//            }


//             VARIANT lCompressedMediaTypeSAFEARRAY;

//             VariantInit(&lCompressedMediaTypeSAFEARRAY);

//             lCompressedMediaTypeSAFEARRAY.vt = VT_UNKNOWN | VT_SAFEARRAY;

//             lCompressedMediaTypeSAFEARRAY.parray = lPtrSAFEARRAY;

//             VARIANT lOutputNodeSAFEARRAY;



//             auto lhresult = aPtrIByteStreamSinkFactory->createOutputNodes(
//                         lCompressedMediaTypeSAFEARRAY,
//                         aPtrByteStream,
//                         &lOutputNodeSAFEARRAY);

//             SafeArrayDestroy(lPtrSAFEARRAY);


//             if(lhresult != 0)
//                 break;

//             if(lOutputNodeSAFEARRAY.vt & VT_UNKNOWN &&
//                     lOutputNodeSAFEARRAY.vt & VT_SAFEARRAY)
//             {

//                 SAFEARRAY* lPtrSAoutputNodes;

//                 lPtrSAoutputNodes = lOutputNodeSAFEARRAY.parray;

//                 LONG lBoundSourcesNodes;

//                 LONG uBoundSourcesNodes;

//                 SafeArrayGetUBound(lPtrSAoutputNodes, 1, &uBoundSourcesNodes);

//                 SafeArrayGetLBound(lPtrSAoutputNodes, 1, &lBoundSourcesNodes);

//                 for (LONG lIndex = lBoundSourcesNodes; lIndex <= uBoundSourcesNodes; lIndex++)
//                 {
//                     VARIANT lVar;

//                     SafeArrayGetElement(lPtrSAoutputNodes, &lIndex, &lVar);

//                     if (lVar.vt == VT_UNKNOWN)
//                     {
//                         aOutputNodeList.push_back(lVar.punkVal);
//                     }
//                 }

//             }

//             VariantClear(&lOutputNodeSAFEARRAY);


//             lresult = true;

//        }

//    }
//    while(false);

//    return lresult;
//}