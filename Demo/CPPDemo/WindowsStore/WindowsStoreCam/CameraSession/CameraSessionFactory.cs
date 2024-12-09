using CaptureManagerToWinRTProxy;
using PhotoDisplayComponent;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Text;
using System.Threading.Tasks;
using Windows.Data.Xml.Dom;
using Windows.Devices.Enumeration;
using Windows.Foundation;
using Windows.Storage;
using Windows.UI.Core;

namespace CameraSession
{
    public sealed class CameraSessionFactory
    {
        private static CameraSessionFactory mInstance = new CameraSessionFactory();

        private CameraSessionFactory() { }

        public static CameraSessionFactory getInstance()
        {
            return mInstance;
        }

        public ICameraSession createICameraSession(
            string aSymbolicLink,
            IPhotoDisplayControl aIPhotoDisplayControl)
        {
            ICameraSession lresult = null;

            do
            {
                if (aSymbolicLink == null)
                    break;

                var lSourceControl = CaptureManager.getInstance().getISourceControl();

                if (lSourceControl == null)
                    break;

                var lSourceCollection = lSourceControl.getCollectionOfSources();

                if (lSourceCollection == null)
                    break;

                XmlDocument doc = new XmlDocument();

                doc.LoadXml(lSourceCollection);

                var lSourceXmlNodeList = doc.SelectNodes("Sources/Source");

                if (lSourceXmlNodeList == null)
                    break;

                foreach (var item in lSourceXmlNodeList)
                {
                    var lSymbolicLink = item.SelectSingleNode("Source.Attributes/Attribute[@Name='MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK']/SingleValue/@Value");

                    if (lSymbolicLink != null && lSymbolicLink.NodeValue != null)
                    {
                        if (lSymbolicLink.NodeValue.ToString() == aSymbolicLink)
                        {
                            lresult = CameraSession.createICameraSession(item, aIPhotoDisplayControl);

                            break;
                        }
                    }
                }

            } while (false);
            
            return lresult;
        }

    }
}
