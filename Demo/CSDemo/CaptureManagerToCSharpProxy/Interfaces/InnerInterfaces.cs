/*
MIT License

Copyright(c) 2020 Evgeny Pereguda

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/



using System;
using System.Runtime.InteropServices;
namespace CaptureManagerToCSharpProxy
{
    [Guid("1276CC17-BCA8-4200-87BB-7180EF562447")]
    [TypeLibType(4160)]
    internal interface ISourceControlInner
    {
        void getCollectionOfSources(out IntPtr aPtrXMLstring);
    }

    [Guid("C6BA3732-197E-438B-8E73-277759A7B58F")]
    [TypeLibType(4160)]
    internal interface ISinkControlInner
    {
        void getCollectionOfSinks(out IntPtr aPtrXMLstring);
    }

    [Guid("96223507-D8FF-4EC1-B125-71AA7F9726A4")]
    [TypeLibType(4160)]
    internal interface IEncoderControlInner
    {
        void getCollectionOfEncoders(out IntPtr aPtrXMLstring);
        void getMediaTypeCollectionOfEncoder(IntPtr aPtrUncompressedMediaType, ref Guid aRefEncoderCLSID, out IntPtr aPtrXMLstring);
    }

    [Guid("74F0DC2B-E470-4359-A1E7-467B521BDFE1")]
    [TypeLibType(4160)]
    internal interface IMediaTypeParserInner
    {
        void parse(IntPtr aPtrMediaType, out IntPtr aPtrXMLstring);
    }

    [Guid("3BD92C4C-5E06-4901-AE0B-D97E3902EAFC")]
    [TypeLibType(4096)]
    internal interface IWebCamControlInner
    {
        void getCamParametrs(out IntPtr aXMLstring);
    }

    [Guid("47F9883C-77B1-4A0B-9233-B3EAFA8F387E")]
    [TypeLibType(4160)]
    internal interface IEVRStreamControlInner
    {
        [DispId(1)]
        void setPosition(object aPtrEVROutputNode, float aLeft, float aRight, float aTop, float aBottom);
        [DispId(2)]
        void setZOrder(object aPtrEVROutputNode, uint aZOrder);
        [DispId(3)]
        void getPosition(object aPtrEVROutputNode, out float aPtrLeft, out float aPtrRight, out float aPtrTop, out float aPtrBottom);
        [DispId(4)]
        void getZOrder(IntPtr aPtrEVROutputNode, out IntPtr aPtrZOrder);
        [DispId(5)]
        void flush(object aPtrEVROutputNode);
        [DispId(6)]
        void setSrcPosition(object aPtrEVROutputNode, float aLeft, float aRight, float aTop, float aBottom);
        [DispId(7)]
        void getSrcPosition(object aPtrEVROutputNode, out float aPtrLeft, out float aPtrRight, out float aPtrTop, out float aPtrBottom);
        [DispId(8)]
        void getCollectionOfFilters(IntPtr aPtrEVROutputNode, out IntPtr aPtrXMLstring);
        [DispId(9)]
        void setFilterParametr(object aPtrEVROutputNode, uint aParametrIndex, int aNewValue, int aIsEnabled);
        [DispId(10)]
        void getCollectionOfOutputFeatures(IntPtr aPtrEVROutputNode, out IntPtr aPtrXMLstring);
        [DispId(11)]
        void setOutputFeatureParametr(object aPtrEVROutputNode, uint aParametrIndex, int aNewValue);
    }

    [Guid("39DC3AEF-3B59-4C0D-A1B2-54BF2653C056")]
    [TypeLibType(4160)]
    internal interface IVersionControlInner
    {
        void getVersion(out uint aPtrMAJOR, out uint aPtrMINOR, out uint aPtrPATCH, out IntPtr aPtrAdditionalLabel);
        void getXMLStringVersion(out IntPtr aPtrXMLstring);
        void checkVersion(uint aMAJOR, uint aMINOR, uint aPATCH, out sbyte aPtrResult);
    }

    [Guid("E8F25B4A-8C71-4C9E-BD8C-82260DC4C21B")]
    [TypeLibType(4160)]
    internal interface IStreamControlInner
    {
        void getCollectionOfStreamControlNodeFactories(out IntPtr aPtrXMLstring);
        void createStreamControlNodeFactory(ref Guid aREFIID, out object aPtrPtrStreamControlNodeFactory);     
    }
}