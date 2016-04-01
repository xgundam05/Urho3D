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

#pragma once

// Only include this if we are compiling for the Pi
#ifdef RPI

#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <bcm_host.h>

#include "../Precompiled.h"
#include "../Core/Context.h"
#include "../Core/Object.h"

namespace Urho3D
{

class URHO3D_API TFTDisplay : public Object
{
    URHO3D_OBJECT(TFTDisplay, Object);

public:
    // Construct
    TFTDisplay(Context* context);
    ~TFTDisplay();

    // Initialize the TFT display
    int InitializeTFT(const String& bufferAddress = "/dev/fb1");

    // Grab the source framebuffer, wait until display is initialized by engine
    int InitializeSourceBuffer(uint32_t displayNumber = 0);

    // Set it up to actually copy
    int InitializeMemoryMap();

    // Copy the Framebuffer
    void CopyToTFT();

    // Get the TFT Width
    int GetWidth();

    // Get the TFT Height
    int GetHeight();

private:
    DISPMANX_DISPLAY_HANDLE_T display_;
    DISPMANX_MODEINFO_T displayInfo_;
    DISPMANX_RESOURCE_HANDLE_T screenResource_;
    VC_IMAGE_TRANSFORM_T transform_;
    uint32_t imagePrt_;
    VC_RECT rect_;
    int fbfd_;
    char* fbp_;

    struct fb_var_screeninfo vinfo_;
    struct fb_fix_screeninfo finfo_;
}

}

#endif
