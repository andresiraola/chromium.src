// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "modules/vr/HMDVRDevice.h"

#include "core/html/HTMLCanvasElement.h"
#include "modules/vr/VRController.h"
#include "modules/vr/VRFieldOfView.h"
#include "modules/webgl/WebGLRenderingContextBase.h"
#include "platform/graphics/gpu/DrawingBuffer.h"

namespace blink {

namespace {

VREye stringToVREye(const String& whichEye)
{
    if (whichEye == "left")
        return VREyeLeft;
    if (whichEye == "right")
        return VREyeRight;
    return VREyeNone;
}

} // namepspace

HMDVRDevice::HMDVRDevice(VRHardwareUnit* hardwareUnit, unsigned deviceId)
    : VRDevice(hardwareUnit, deviceId)
    , m_fullscreenElement(0)
{
    m_eyeParametersLeft = VREyeParameters::create();
    m_eyeParametersRight = VREyeParameters::create();
}

void HMDVRDevice::updateFromWebVRDevice(const WebVRDevice& device)
{
    VRDevice::updateFromWebVRDevice(device);
    const WebVRHMDInfo &hmdInfo = device.hmdInfo;

    m_eyeParametersLeft->setFromWebVREyeParameters(hmdInfo.leftEye);
    m_eyeParametersRight->setFromWebVREyeParameters(hmdInfo.rightEye);
}

VREyeParameters* HMDVRDevice::getEyeParameters(const String& whichEye)
{
    switch (stringToVREye(whichEye)) {
    case VREyeLeft:
        return m_eyeParametersLeft;
    case VREyeRight:
        return m_eyeParametersRight;
    default:
        return nullptr;
    }
}

void HMDVRDevice::setFieldOfView(VRFieldOfView* leftFov, VRFieldOfView* rightFov)
{
    // FIXME: Currently min == max == recommended FOV, but when that changes
    // this function will need to perform clamping and track the set value
}

void HMDVRDevice::configureRendering(Node* node, bool fullscreen)
{
    if(isHTMLCanvasElement(node)) {
        HTMLCanvasElement* canvasElement = static_cast<HTMLCanvasElement*>(node);
        WebGLRenderingContextBase* webgl_context = toWebGLRenderingContextBase(canvasElement->renderingContext());
        if (webgl_context) {
            DrawingBuffer* buffer = webgl_context->drawingBuffer();
            if (fullscreen) {
                buffer->startVRCompositing(hardwareUnit());
            } else {
                buffer->endVRCompositing();
            }
        }
        return;
    }
    for (Node* child = node->firstChild(); child; child = child->nextSibling()) {
        configureRendering(child, fullscreen);
    }
}

void HMDVRDevice::didEnterFullScreenForElement(Element* element)
{
    m_fullscreenElement = element;
    configureRendering(m_fullscreenElement, true);
}

void HMDVRDevice::exitFullscreen()
{
    configureRendering(m_fullscreenElement, false);
    m_fullscreenElement = 0;
}

DEFINE_TRACE(HMDVRDevice)
{
    visitor->trace(m_eyeParametersLeft);
    visitor->trace(m_eyeParametersRight);

    VRDevice::trace(visitor);
}

} // namespace blink
