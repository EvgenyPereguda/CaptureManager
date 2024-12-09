using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Media.Devices;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

namespace CameraSession
{
    public sealed partial class VideoDeviceParametr : UserControl
    {
        MediaDeviceControl mMediaDeviceControl;

        public VideoDeviceParametr(string aTitle, MediaDeviceControl aMediaDeviceControl)
        {
            this.InitializeComponent();

            mTitle.Text = aTitle;

            mMediaDeviceControl = aMediaDeviceControl;

            mValueSlider.Minimum = mMediaDeviceControl.Capabilities.Min;

            mValueSlider.Maximum = mMediaDeviceControl.Capabilities.Max;

            mValueSlider.StepFrequency = mMediaDeviceControl.Capabilities.Step;

            mValueSlider.SmallChange = mMediaDeviceControl.Capabilities.Step;

            double lvalue = 0.0;

            if (mMediaDeviceControl.TryGetValue(out lvalue))
            {
                mValueSlider.Value = lvalue;
            }

            mValueSlider.ValueChanged += mValueSlider_ValueChanged;

            if(!mMediaDeviceControl.Capabilities.AutoModeSupported)
            {
                mModeCheckBox.IsEnabled = false;

                mModeCheckBox.IsChecked = true;
            }
            else
            {
                bool lstate = false;

                if(mMediaDeviceControl.TryGetAuto(out lstate))
                {
                    mModeCheckBox.IsChecked = !lstate;
                }
            }
        }

        private void mValueSlider_ValueChanged(object sender, RangeBaseValueChangedEventArgs e)
        {            
            if (mMediaDeviceControl.TrySetValue(mValueSlider.Value))
            {

            }            
        }

        private void mModeCheckBox_Checked(object sender, RoutedEventArgs e)
        {
            mMediaDeviceControl.TrySetAuto(!(bool)mModeCheckBox.IsChecked);
        }
    }
}
