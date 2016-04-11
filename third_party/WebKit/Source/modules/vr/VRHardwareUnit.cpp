// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "modules/vr/VRHardwareUnit.h"

#include "modules/vr/HMDVRDevice.h"
#include "modules/vr/NavigatorVRDevice.h"
#include "modules/vr/PositionSensorVRDevice.h"
#include "modules/vr/VRController.h"
#include "modules/vr/VRDevice.h"
#include "public/platform/Platform.h"

namespace blink {

VRHardwareUnit::VRHardwareUnit(NavigatorVRDevice* navigatorVRDevice)
    : m_nextDeviceId(1)
    , m_frameIndex(0)
    , m_navigatorVRDevice(navigatorVRDevice)
    , m_canUpdatePoseState(true)
{
    m_poseState = VRPoseState::create();
}

VRHardwareUnit::~VRHardwareUnit()
{
    if (!m_canUpdatePoseState)
        Platform::current()->currentThread()->removeTaskObserver(this);
}

void VRHardwareUnit::updateFromWebVRDevice(const WebVRDevice& device)
{
    m_index = device.index;
    m_hardwareUnitId = String::number(device.index);

    if (device.flags & WebVRDeviceTypeHMD) {
        if (!m_hmd)
            m_hmd = new HMDVRDevice(this, m_nextDeviceId++);
        m_hmd->updateFromWebVRDevice(device);
    } else if (m_hmd) {
        m_hmd.clear();
    }

    if (device.flags & WebVRDeviceTypePosition) {
        if (!m_poseSensor)
            m_poseSensor = new PositionSensorVRDevice(this, m_nextDeviceId++);
        m_poseSensor->updateFromWebVRDevice(device);
    } else if (m_poseSensor) {
        m_poseSensor.clear();
    }
}

void VRHardwareUnit::addDevicesToVector(HeapVector<Member<VRDevice>>& vrDevices)
{
    if (m_hmd)
        vrDevices.append(m_hmd);

    if (m_poseSensor)
        vrDevices.append(m_poseSensor);
}

VRController* VRHardwareUnit::controller()
{
    return m_navigatorVRDevice->controller();
}

VRPoseState* VRHardwareUnit::getSensorState()
{
    if (m_canUpdatePoseState) {
        m_poseState = getImmediateSensorState(true);
        Platform::current()->currentThread()->addTaskObserver(this);
        m_canUpdatePoseState = false;
    }

    return m_poseState;
}

VRPoseState* VRHardwareUnit::getImmediateSensorState(bool updateFrameIndex)
{
    WebHMDSensorState state;
    controller()->getSensorState(m_index, state);
    if (updateFrameIndex)
        m_frameIndex = state.frameIndex;

    VRPoseState* immediatePoseState = VRPoseState::create();
    immediatePoseState->setState(state);
    return immediatePoseState;
}

void VRHardwareUnit::getOrientation(float &x, float &y, float &z, float &w) {
    x = m_poseState->orientation()->x();
    y = m_poseState->orientation()->y();
    z = m_poseState->orientation()->z();
    w = m_poseState->orientation()->w();
}

void VRHardwareUnit::didProcessTask()
{
    // State should be stable until control is returned to the user agent.
    if (!m_canUpdatePoseState) {
        Platform::current()->currentThread()->removeTaskObserver(this);
        m_canUpdatePoseState = true;
    }
}

DEFINE_TRACE(VRHardwareUnit)
{
    visitor->trace(m_navigatorVRDevice);
    visitor->trace(m_poseState);
    visitor->trace(m_hmd);
    visitor->trace(m_poseSensor);
}

} // namespace blink
