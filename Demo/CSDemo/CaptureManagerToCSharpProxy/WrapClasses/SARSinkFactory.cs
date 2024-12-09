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

using CaptureManagerToCSharpProxy.Interfaces;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CaptureManagerToCSharpProxy.WrapClasses
{
    class SARSinkFactory : ISARSinkFactory, ISARSinkFactoryAsync
    {
        private CaptureManagerLibrary.ISARSinkFactory mISARSinkFactory;

        public SARSinkFactory(object aIUnknown)
        {
            mISARSinkFactory = aIUnknown as CaptureManagerLibrary.ISARSinkFactory;
        }

        private async Task<object> createOutputNodeTask(bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                object lresult = null;

                do
                {
                    if (mISARSinkFactory == null)
                        break;


                    try
                    {
                        object lArrayMediaNodes = new Object();

                        mISARSinkFactory.createOutputNode(
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

        public bool createOutputNode(out object aTopologyOutputNode)
        {
            bool lresult = false;

            aTopologyOutputNode = createOutputNodeTask(false).Result;

            lresult = aTopologyOutputNode != null;

            return lresult;
        }

        public async Task<object> createOutputNodeAsync()
        {
            return await createOutputNodeTask(true);
        }
    }
}
