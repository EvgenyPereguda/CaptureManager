using CaptureManagerToWinRTProxy;
using PhotoDisplayComponent;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Data.Xml.Dom;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Graphics.Imaging;
using Windows.Media.Capture;
using Windows.Storage;
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Markup;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Imaging;
using Windows.UI.Xaml.Navigation;


namespace CameraSession
{
    public sealed class TakePhotoSampleGrabberCall
    {
        public ISampleGrabberCall mISampleGrabberCall { get; set; }

        public bool mIsMJPG { get; set; }

        public uint mBufferSize { get; set; }

        public uint mImageWidth { get; set; }

        public uint mImageHeight { get; set; }
    }

    public delegate void StartRecordDelegate();

    public delegate void StopRecordDelegate();

    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class CameraDisplayPanel : Page, IWriteableBitmapCallback
    {
        MediaCapture mediaCapture;

        public event StartRecordDelegate StartRecordEvent;

        public event StopRecordDelegate StopRecordEvent;

        public TakePhotoSampleGrabberCall takePhotoSampleGrabberCall { get; set; }

        public IPhotoDisplayControl PhotoDisplayControl { get; set; }

        public CameraDisplayPanel()
        {
            this.InitializeComponent();
        }

        public void invoke(Guid aGUIDMajorMediaType, uint aSampleFlags, long aSampleTime, long aSampleDuration, Windows.UI.Xaml.Media.Imaging.WriteableBitmap aWriteableBitmap)
        {
            mDisplayImage.Source = null;

            mDisplayImage.Source = aWriteableBitmap;
        }


        public Windows.UI.Core.CoreDispatcher getDispatcher()
        {
            return this.Dispatcher;
        }

        string laSymbolicLink;

        public async void processControl(string aSymbolicLink)
        {
            do
            {
                laSymbolicLink = aSymbolicLink;


                try
                {
                    if (takePhotoSampleGrabberCall.mISampleGrabberCall != null && 
                        PhotoDisplayControl != null)
                    {
                        mTakePhotoBtn.Visibility = Windows.UI.Xaml.Visibility.Visible;

                        mTakePhotoBtn.Click += new RoutedEventHandler((object sender, RoutedEventArgs e)=>
                        {
                            byte[] lData = new byte[takePhotoSampleGrabberCall.mBufferSize];

                            uint lByteSize = (uint)lData.Length;

                            try
                            {
                                takePhotoSampleGrabberCall.mISampleGrabberCall.readData(lData, out lByteSize);
                            }
                            catch (Exception)
                            {
                            }
                            finally
                            {

                                if (lByteSize > 0)
                                {
                                    {
                                        InMemoryRandomAccessStream stream = new InMemoryRandomAccessStream();

                                        using (DataWriter writer = new DataWriter(stream.GetOutputStreamAt(0)))
                                        {
                                            writer.WriteBytes(lData);

                                            writer.StoreAsync().GetResults();

                                            stream.Seek(0);

                                            PhotoDisplayControl.addPhoto(
                                                stream,
                                                takePhotoSampleGrabberCall.mIsMJPG);
                                        }
                                    }

                                }
                            }
                        });
                    }

                    mediaCapture = new MediaCapture();

                    var settings = new MediaCaptureInitializationSettings();

                    settings.VideoDeviceId = laSymbolicLink;

                    await mediaCapture.InitializeAsync(settings);
                                        
                    GridView lVideoParametrGridView = new GridView();
                    
                    string template =
                                    "<ItemsPanelTemplate xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\" xmlns:x=\"http://schemas.microsoft.com/winfx/2006/xaml\"><ItemsWrapGrid VerticalAlignment = \"Top\" "
                                    + " MaximumRowsOrColumns=\"4\" Orientation=\"Vertical\"/></ItemsPanelTemplate> ";
                    lVideoParametrGridView.ItemsPanel = (ItemsPanelTemplate)XamlReader.Load(template);


                    var lMediaDeviceControl = mediaCapture.VideoDeviceController.Brightness;

                    if (lMediaDeviceControl.Capabilities.Supported)
                    {
                        VideoDeviceParametr lVideoDeviceParametr = new VideoDeviceParametr(
                            "Brightness",
                            lMediaDeviceControl);

                        lVideoParametrGridView.Items.Add(lVideoDeviceParametr);
                    }

                    lMediaDeviceControl = mediaCapture.VideoDeviceController.Contrast;

                    if (lMediaDeviceControl.Capabilities.Supported)
                    {
                        VideoDeviceParametr lVideoDeviceParametr = new VideoDeviceParametr(
                            "Contrast",
                            lMediaDeviceControl);

                        lVideoParametrGridView.Items.Add(lVideoDeviceParametr);
                    }

                    lMediaDeviceControl = mediaCapture.VideoDeviceController.BacklightCompensation;

                    if (lMediaDeviceControl.Capabilities.Supported)
                    {
                        VideoDeviceParametr lVideoDeviceParametr = new VideoDeviceParametr(
                            "Backlight Compensation",
                            lMediaDeviceControl);

                        lVideoParametrGridView.Items.Add(lVideoDeviceParametr);
                    }

                    lMediaDeviceControl = mediaCapture.VideoDeviceController.Hue;

                    if (lMediaDeviceControl.Capabilities.Supported)
                    {
                        VideoDeviceParametr lVideoDeviceParametr = new VideoDeviceParametr(
                            "Hue",
                            lMediaDeviceControl);

                        lVideoParametrGridView.Items.Add(lVideoDeviceParametr);
                    }

                    lMediaDeviceControl = mediaCapture.VideoDeviceController.WhiteBalance;

                    if (lMediaDeviceControl.Capabilities.Supported)
                    {
                        VideoDeviceParametr lVideoDeviceParametr = new VideoDeviceParametr(
                            "White Balance",
                            lMediaDeviceControl);

                        lVideoParametrGridView.Items.Add(lVideoDeviceParametr);
                    }

                    if(lVideoParametrGridView.Items.Count > 0)
                    {
                        Button lButton = new Button(){
                        Content="Video"};

                        mControlStack.Children.Add(lButton);

                        lButton.Tag = lVideoParametrGridView;

                        lButton.Click += lButton_Click;
                    }



                    GridView lCameraParametrGridView = new GridView();

                    lCameraParametrGridView.ItemsPanel = (ItemsPanelTemplate)XamlReader.Load(template);
                    
                    lMediaDeviceControl = mediaCapture.VideoDeviceController.Exposure;

                    if (lMediaDeviceControl.Capabilities.Supported)
                    {
                        VideoDeviceParametr lVideoDeviceParametr = new VideoDeviceParametr(
                            "Exposure",
                            lMediaDeviceControl);

                        lCameraParametrGridView.Items.Add(lVideoDeviceParametr);
                    }

                    lMediaDeviceControl = mediaCapture.VideoDeviceController.Focus;

                    if (lMediaDeviceControl.Capabilities.Supported)
                    {
                        VideoDeviceParametr lVideoDeviceParametr = new VideoDeviceParametr(
                            "Focus",
                            lMediaDeviceControl);

                        lCameraParametrGridView.Items.Add(lVideoDeviceParametr);
                    }

                    lMediaDeviceControl = mediaCapture.VideoDeviceController.Pan;

                    if (lMediaDeviceControl.Capabilities.Supported)
                    {
                        VideoDeviceParametr lVideoDeviceParametr = new VideoDeviceParametr(
                            "Pan",
                            lMediaDeviceControl);

                        lCameraParametrGridView.Items.Add(lVideoDeviceParametr);
                    }

                    lMediaDeviceControl = mediaCapture.VideoDeviceController.Roll;

                    if (lMediaDeviceControl.Capabilities.Supported)
                    {
                        VideoDeviceParametr lVideoDeviceParametr = new VideoDeviceParametr(
                            "Roll",
                            lMediaDeviceControl);

                        lCameraParametrGridView.Items.Add(lVideoDeviceParametr);
                    }

                    lMediaDeviceControl = mediaCapture.VideoDeviceController.Tilt;

                    if (lMediaDeviceControl.Capabilities.Supported)
                    {
                        VideoDeviceParametr lVideoDeviceParametr = new VideoDeviceParametr(
                            "Tilt",
                            lMediaDeviceControl);

                        lCameraParametrGridView.Items.Add(lVideoDeviceParametr);
                    }

                    lMediaDeviceControl = mediaCapture.VideoDeviceController.Zoom;

                    if (lMediaDeviceControl.Capabilities.Supported)
                    {
                        VideoDeviceParametr lVideoDeviceParametr = new VideoDeviceParametr(
                            "Zoom",
                            lMediaDeviceControl);

                        lCameraParametrGridView.Items.Add(lVideoDeviceParametr);
                    }

                    if (lCameraParametrGridView.Items.Count > 0)
                    {
                        Button lButton = new Button()
                        {
                            Content = "Camera"
                        };

                        mControlStack.Children.Add(lButton);

                        lButton.Tag = lCameraParametrGridView;

                        lButton.Click += lButton_Click;
                    }
                    
                }
                catch (Exception)
                {
                    return;
                }

                
                
                
            } while (false);

        }

        

        void lButton_Click(object sender, RoutedEventArgs e)
        {
            Button lButton = sender as Button;

            if (lButton == null)
                return;

            var lGridView = lButton.Tag as GridView;

            if (lGridView == null)
                return;
                        
            var lGeneraTransform = lButton.TransformToVisual(Window.Current.Content);

            Point screenCoords = lGeneraTransform.TransformPoint(new Point(0, 0));


            Border border = new Border
            {
                Child = lGridView,
                Background = this.Resources["ApplicationPageBackgroundThemeBrush"] as SolidColorBrush,
                BorderBrush = this.Resources["ApplicationForegroundThemeBrush"] as SolidColorBrush,
                BorderThickness = new Thickness(1),
                Padding = new Thickness(10),
            };

            Popup popup = new Popup
            {
                Child = border,
                IsLightDismissEnabled = true
            };

            popup.Opacity = 0.75;

            popup.HorizontalOffset = screenCoords.X + lButton.ActualWidth;

            popup.VerticalOffset = screenCoords.Y;

            popup.IsOpen = true;
        }


        public void hideElements()
        {
            mControlStack.Visibility = Windows.UI.Xaml.Visibility.Collapsed;

            mCommandStack.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
            
        }

        public void showElements()
        {
            mControlStack.Visibility = Windows.UI.Xaml.Visibility.Visible;

            mCommandStack.Visibility = Windows.UI.Xaml.Visibility.Visible;
        }

        private void mStartStopRecordBtn_Checked(object sender, RoutedEventArgs e)
        {
            if (StartRecordEvent != null)
                StartRecordEvent();
        }

        private void mStartStopRecordBtn_Unchecked(object sender, RoutedEventArgs e)
        {
            if (StopRecordEvent != null)
                StopRecordEvent();
            
        }

    }
}
