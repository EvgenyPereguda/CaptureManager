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
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CaptureManagerToCSharpProxy.Interfaces;

namespace CaptureManagerToCSharpProxy.WrapClasses
{
    class EncoderNodeFactory : IEncoderNodeFactory, IEncoderNodeFactoryAsync
    {
        private CaptureManagerLibrary.IEncoderNodeFactory mIEncoderNodeFactory;

        public EncoderNodeFactory(
            CaptureManagerLibrary.IEncoderNodeFactory aIEncoderNodeFactory)
        {
            mIEncoderNodeFactory = aIEncoderNodeFactory;
        }

        private async Task<object> createCompressedMediaTypeTask(
            object aUncompressedMediaType, 
            Guid aEncodingModeGUID, 
            uint aEncodingModeValue, 
            uint aIndexCompressedMediaType,
            bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                object lresult = null;
                
                do
                {
                    if (mIEncoderNodeFactory == null)
                        break;

                    try
                    {

                        mIEncoderNodeFactory.createCompressedMediaType(
                            aUncompressedMediaType,
                            aEncodingModeGUID,
                            aEncodingModeValue,
                            aIndexCompressedMediaType,
                            out lresult);
                    }
                    catch (Exception exc)
                    {
                        LogManager.getInstance().write(exc.Message);
                    }

                } while (false);

                return lresult;
            }).ConfigureAwait(aIsAwait);
        }
                       
        public async Task<object> createCompressedMediaTypeAsync(object aUncompressedMediaType, Guid aEncodingModeGUID, uint aEncodingModeValue, uint aIndexCompressedMediaType)
        {
            return await createCompressedMediaTypeTask(
                aUncompressedMediaType, 
                aEncodingModeGUID, 
                aEncodingModeValue, 
                aIndexCompressedMediaType, 
                true);
        }

        public bool createCompressedMediaType(
            object aUncompressedMediaType, 
            Guid aEncodingModeGUID, 
            uint aEncodingModeValue, 
            uint aIndexCompressedMediaType, 
            out object aCompressedMediaType)
        {
            bool lresult = false;

            aCompressedMediaType = createCompressedMediaTypeTask(
                aUncompressedMediaType,
                aEncodingModeGUID,
                aEncodingModeValue,
                aIndexCompressedMediaType,
                false).Result;

            lresult = aCompressedMediaType != null;

            return lresult;
        }

        private async Task<object> createEncoderNodeTask(
            object aUncompressedMediaType,
            Guid aEncodingModeGUID,
            uint aEncodingModeValue,
            uint aIndexCompressedMediaType,
            object aDownStreamNode,
            bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                object lresult = null;

                do
                {
                    if (mIEncoderNodeFactory == null)
                        break;

                    try
                    {

                        mIEncoderNodeFactory.createEncoderNode(
                            aUncompressedMediaType,
                            aEncodingModeGUID,
                            aEncodingModeValue,
                            aIndexCompressedMediaType,
                            aDownStreamNode,
                            out lresult);

                    }
                    catch (Exception exc)
                    {
                        LogManager.getInstance().write(exc.Message);
                    }

                } while (false);

                return lresult;
            }).ConfigureAwait(aIsAwait);
        }

        public async Task<object> createEncoderNodeAsync(object aUncompressedMediaType, Guid aEncodingModeGUID, uint aEncodingModeValue, uint aIndexCompressedMediaType, object aDownStreamNode)
        {
            return await createEncoderNodeTask(
                aUncompressedMediaType,
                aEncodingModeGUID,
                aEncodingModeValue,
                aIndexCompressedMediaType,
                aDownStreamNode,
                true);
        }

        public bool createEncoderNode(
            object aUncompressedMediaType, 
            Guid aEncodingModeGUID, 
            uint aEncodingModeValue, 
            uint aIndexCompressedMediaType, 
            object aDownStreamNode,
            out object aEncoderNode)
        {
            bool lresult = false;

            aEncoderNode = createEncoderNodeTask(
                aUncompressedMediaType,
                aEncodingModeGUID,
                aEncodingModeValue,
                aIndexCompressedMediaType,
                aDownStreamNode,
                false).Result;

            lresult = aEncoderNode != null;

            return lresult;
        }
    }
}
