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
    class SARVolumeControl: ISARVolumeControl, ISARVolumeControlAsync
    {
        private CaptureManagerLibrary.ISARVolumeControl mISARVolumeControl = null;

        public SARVolumeControl(
            CaptureManagerLibrary.ISARVolumeControl aISARVolumeControl)
        {
            mISARVolumeControl = aISARVolumeControl;
        }

        private async Task<uint> getChannelCountTask(object aPtrSARNode, bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                uint lresult = 0;

                do
                {
                    if (mISARVolumeControl == null)
                        break;

                    if (aPtrSARNode == null)
                        break;

                    try
                    {
                        mISARVolumeControl.getChannelCount(aPtrSARNode, out lresult);
                    }
                    catch (Exception exc)
                    {
                        LogManager.getInstance().write(exc.Message);
                    }

                } while (false);

                return lresult;
            }).ConfigureAwait(aIsAwait);
        }

        public bool getChannelCount(object aPtrSARNode, out uint aPtrCount)
        {
            bool lresult = false;

            aPtrCount = getChannelCountTask(aPtrSARNode, false).Result;

            lresult = aPtrCount != 0;

            return lresult;
        }

        public async Task<uint> getChannelCountAsync(object aPtrSARNode)
        {
            return await getChannelCountTask(aPtrSARNode, true);
        }

        private async Task<float> getChannelVolumeTask(object aPtrSARNode, uint aIndex, bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                float lresult = 0.0f;

                do
                {
                    if (mISARVolumeControl == null)
                        break;

                    if (aPtrSARNode == null)
                        break;

                    try
                    {
                        mISARVolumeControl.getChannelVolume(aPtrSARNode, aIndex, out lresult);
                    }
                    catch (Exception exc)
                    {
                        LogManager.getInstance().write(exc.Message);
                    }

                } while (false);

                return lresult;
            }).ConfigureAwait(aIsAwait);
        }

        public bool getChannelVolume(object aPtrSARNode, uint aIndex, out float aPtrLevel)
        {
            bool lresult = false;

            aPtrLevel = getChannelVolumeTask(aPtrSARNode, aIndex, false).Result;

            lresult = aPtrLevel != 0;

            return lresult;
        }

        public async Task<float> getChannelVolumeAsync(object aPtrSARNode, uint aIndex)
        {
            return await getChannelVolumeTask(aPtrSARNode, aIndex, true);
        }

        private async Task<bool> setChannelVolumeTask(object aPtrSARNode, uint aIndex, float aLevel, bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                bool lresult = false;

                do
                {
                    if (mISARVolumeControl == null)
                        break;

                    if (aPtrSARNode == null)
                        break;

                    try
                    {
                        mISARVolumeControl.setChannelVolume(aPtrSARNode, aIndex, aLevel);

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

        public bool setChannelVolume(object aPtrSARNode, uint aIndex, float aLevel)
        {
            return setChannelVolumeTask(aPtrSARNode, aIndex, aLevel, false).Result;
        }

        public async Task<bool> setChannelVolumeAsync(object aPtrSARNode, uint aIndex, float aLevel)
        {
            return await setChannelVolumeTask(aPtrSARNode, aIndex, aLevel, true);
        }
    }
}
