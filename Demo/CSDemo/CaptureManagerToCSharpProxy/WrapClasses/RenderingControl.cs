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
    class RenderingControl : IRenderingControl, IRenderingControlAsync
    {
        private CaptureManagerLibrary.IRenderingControl mIRenderingControl = null;

        public RenderingControl(
            CaptureManagerLibrary.IRenderingControl aIRenderingControl)
        {
            mIRenderingControl = aIRenderingControl;
        }

        private async Task<bool> enableInnerRenderingTask(object aPtrEVROutputNode, bool aIsInnerRendering, bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                bool lresult = false;

                do
                {
                    if (mIRenderingControl == null)
                        break;

                    if (aPtrEVROutputNode == null)
                        break;

                    try
                    {
                        mIRenderingControl.enableInnerRendering(aPtrEVROutputNode,
                            aIsInnerRendering ? 1 : 0);

                        lresult = true;
                    }
                    catch (Exception exc)
                    {
                        LogManager.getInstance().write(exc.Message);
                    }

                } while (false);

                return lresult;
            }).ConfigureAwait(aIsAwait);
        }

        public bool enableInnerRendering(object aPtrEVROutputNode, bool aIsInnerRendering)
        {
            return enableInnerRenderingTask(aPtrEVROutputNode, aIsInnerRendering, false).Result;
        }

        public async Task<bool> enableInnerRenderingAsync(object aPtrEVROutputNode, bool aIsInnerRendering)
        {
            return await enableInnerRenderingTask(aPtrEVROutputNode, aIsInnerRendering, true);
        }

        private async Task<bool> renderToTargetTask(object aPtrEVROutputNode, object aPtrRenderTarget, bool aCopyMode, bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                bool lresult = false;

                do
                {
                    if (mIRenderingControl == null)
                        break;

                    if (aPtrEVROutputNode == null)
                        break;

                    if (aPtrRenderTarget == null)
                        break;

                    try
                    {
                        mIRenderingControl.renderToTarget(aPtrEVROutputNode,
                            aPtrRenderTarget,
                            aCopyMode ? 1 : 0);

                        lresult = true;
                    }
                    catch (Exception exc)
                    {
                        LogManager.getInstance().write(exc.Message);
                    }

                } while (false);

                return lresult;
            }).ConfigureAwait(aIsAwait);
        }

        public bool renderToTarget(object aPtrEVROutputNode, object aPtrRenderTarget, bool aCopyMode)
        {
            return renderToTargetTask(aPtrEVROutputNode, aPtrRenderTarget, aCopyMode, false).Result;
        }

        public async Task<bool> renderToTargetAsync(object aPtrEVROutputNode, object aPtrRenderTarget, bool aCopyMode)
        {
            return await renderToTargetTask(aPtrEVROutputNode, aPtrRenderTarget, aCopyMode, true);
        }
    }
}
