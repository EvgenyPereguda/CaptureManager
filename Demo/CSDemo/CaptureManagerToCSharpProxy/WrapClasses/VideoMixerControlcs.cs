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
    class VideoMixerControl : IVideoMixerControl, IVideoMixerControlAsync
    {
        private CaptureManagerLibrary.IVideoMixerControl mIVideoMixerControl = null;

        public VideoMixerControl(
            CaptureManagerLibrary.IVideoMixerControl aIVideoMixerControl)
        {
            mIVideoMixerControl = aIVideoMixerControl;
        }

        private async Task<bool> flushTask(object aPtrVideoMixerNode, bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                bool lresult = false;

                do
                {
                    if (mIVideoMixerControl == null)
                        break;

                    if (aPtrVideoMixerNode == null)
                        break;

                    try
                    {
                        mIVideoMixerControl.flush(
                            aPtrVideoMixerNode);

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

        public bool flush(object aPtrVideoMixerNode)
        {
            return flushTask(aPtrVideoMixerNode, false).Result;
        }

        public async Task<bool> flushAsync(object aPtrVideoMixerNode)
        {
            return await flushTask(aPtrVideoMixerNode, true);
        }

        private async Task<bool> setOpacityTask(object aPtrVideoMixerNode, float aOpacity, bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                bool lresult = false;

                do
                {
                    if (mIVideoMixerControl == null)
                        break;

                    if (aPtrVideoMixerNode == null)
                        break;

                    try
                    {
                        mIVideoMixerControl.setOpacity(
                            aPtrVideoMixerNode,
                            aOpacity);

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

        public bool setOpacity(object aPtrVideoMixerNode, float aOpacity)
        {
            return setOpacityTask(aPtrVideoMixerNode, aOpacity, false).Result;
        }

        public async Task<bool> setOpacityAsync(object aPtrVideoMixerNode, float aOpacity)
        {
            return await setOpacityTask(aPtrVideoMixerNode, aOpacity, true);
        }

        private async Task<bool> setPositionTask(object aPtrVideoMixerNode, float aLeft, float aRight, float aTop, float aBottom, bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                bool lresult = false;

                do
                {
                    if (mIVideoMixerControl == null)
                        break;

                    if (aPtrVideoMixerNode == null)
                        break;

                    try
                    {
                        mIVideoMixerControl.setPosition(
                            aPtrVideoMixerNode,
                            aLeft,
                            aRight,
                            aTop,
                            aBottom);

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

        public bool setPosition(object aPtrVideoMixerNode, float aLeft, float aRight, float aTop, float aBottom)
        {
            return setPositionTask(aPtrVideoMixerNode, aLeft, aRight, aTop, aBottom, false).Result;
        }

        public async Task<bool> setPositionAsync(object aPtrVideoMixerNode, float aLeft, float aRight, float aTop, float aBottom)
        {
            return await setPositionTask(aPtrVideoMixerNode, aLeft, aRight, aTop, aBottom, true);
        }

        private async Task<bool> setSrcPositionTask(object aPtrVideoMixerNode, float aLeft, float aRight, float aTop, float aBottom, bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                bool lresult = false;

                do
                {
                    if (mIVideoMixerControl == null)
                        break;

                    if (aPtrVideoMixerNode == null)
                        break;

                    try
                    {
                        mIVideoMixerControl.setSrcPosition(
                            aPtrVideoMixerNode,
                            aLeft,
                            aRight,
                            aTop,
                            aBottom);

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

        public bool setSrcPosition(object aPtrVideoMixerNode, float aLeft, float aRight, float aTop, float aBottom)
        {
            return setSrcPositionTask(aPtrVideoMixerNode, aLeft, aRight, aTop, aBottom, false).Result;
        }

        public async Task<bool> setSrcPositionAsync(object aPtrVideoMixerNode, float aLeft, float aRight, float aTop, float aBottom)
        {
            return await setSrcPositionTask(aPtrVideoMixerNode, aLeft, aRight, aTop, aBottom, true);
        }

        private async Task<bool> setZOrderTask(object aPtrVideoMixerNode, uint aZOrder, bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                bool lresult = false;

                do
                {
                    if (mIVideoMixerControl == null)
                        break;

                    if (aPtrVideoMixerNode == null)
                        break;

                    try
                    {
                        mIVideoMixerControl.setZOrder(
                            aPtrVideoMixerNode,
                            aZOrder);

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

        public bool setZOrder(object aPtrVideoMixerNode, uint aZOrder)
        {
            return setZOrderTask(aPtrVideoMixerNode, aZOrder, false).Result;
        }

        public async Task<bool> setZOrderAsync(object aPtrVideoMixerNode, uint aZOrder)
        {
            return await setZOrderTask(aPtrVideoMixerNode, aZOrder, true);
        }
    }
}
