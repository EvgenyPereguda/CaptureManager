using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Foundation.Metadata;
using Windows.UI.Xaml.Controls;

namespace CameraSession
{   
 
    public interface ICameraSession
    {
        Page getDisplayPage();

        void start();

        void stop();

        void close();


        void hideElements();

        void showElements();
    }
}
