//
// Copyright (c) 2008-2016 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#ifdef RPI

#include "../Graphics/TFTDisplay.h"

namespace Urho3D
{

TFTDisplay::TFTDisplay(Context* context) :
    Object(context),
    fbfd_(-1),
    fbp_(0)
{
    // Initialize Things?
}

TFTDisplay::~TFTDisplay()
{
    if (fbp_ > 0)
        munmap(fbp_, finfo_.smem_len);
    if (fbfd_ >= 0)
        close(fbfd_);
    if (screenResource_)
        vc_dispmanx_resource_delete(screenResource_);
    if (display_)
        vc_dispmanx_display_close(display_);
}

int TFTDisplay::InitializeTFT(const String& bufferAddress)
{
    bcm_host_init();

    if (fbfd_ >= 0)
        close(fbfd_);

    fbfd_ = open(bufferAddress.CString(), O_RDWR);
    if (fbfd_ < 0)
    {
        return 0;
    }

    if (ioctl(fbfd_, FBIOGET_FSCREENINFO, &finfo_))
    {
        close(fbfd_);
        fbfd_ = -1;
        return 0;
    }

    if (ioctl(fbfd_, FBIOGET_VSCREENINFO, &vinfo_))
    {
        close(fbfd_);
        fbfd_ = -1;
        return 0;
    }

    screenResource_ = vc_dispmanx_resource_create(VC_IMAGE_RGB565, vinfo_.xres, vinfo_.yres, &imagePrt_);
    if (!screenResource_)
    {
        close(fbfd_);
        fbfd_ = -1;
        return 0;
    }

    return 1;
}

int TFTDisplay::InitializeSourceBuffer(uint32_t displayNumber)
{
    bcm_host_init();

    display_ = vc_dispmanx_display_open(displayNumber);
    if (!display_)
    {
        return 0;
    }

    if (vc_dispmanx_display_get_info(display_, &displayInfo_))
    {
        return 0;
    }

    return 1;
}

int TFTDisplay::InitializeMemoryMap()
{
    bcm_host_init();

    if (!screenResource_ || fbfd_ < 0)
    {
        return 0;
    }

    fbp_ = (char*)mmap(0, finfo_.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd_, 0);
    if (fbp_ <= 0)
    {
        return 0;
    }

    vc_dispmanx_rect_set(&rect_, 0, 0, vinfo_.xres, vinfo_.yres);

    return 1;
}

void TFTDisplay::CopyToTFT()
{
    if (display_ && screenResource_ && fbp_ > 0)
    {
        vc_dispmanx_snapshot(display_, screenResource_, (DISPMANX_TRANSFORM_T)0);
        vc_dispmanx_resource_read_data(screenResource_, &rect_, fbp_, vinfo_.xres * vinfo_.bits_per_pixel / 8);
    }
}

int TFTDisplay::GetWidth()
{
    if (fbfd_ > 0)
    {
        return (int)vinfo_.xres;
    }

    return 0;
}

int TFTDisplay::GetHeight()
{
    if (fbfd_ > 0)
    {
        return (int)vinfo_.yres;
    }

    return 0;
}

}
#endif
