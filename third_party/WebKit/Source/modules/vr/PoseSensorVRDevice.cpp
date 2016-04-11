// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "modules/vr/PoseSensorVRDevice.h"

#include "modules/vr/VRController.h"
#include "modules/vr/VRHardwareUnit.h"

namespace blink {

PoseSensorVRDevice::PoseSensorVRDevice(VRHardwareUnit* hardwareUnit, unsigned deviceId)
    : VRDevice(hardwareUnit, deviceId)
{
}

VRPoseState* PoseSensorVRDevice::getState()
{
    return hardwareUnit()->getSensorState();
}

VRPoseState* PoseSensorVRDevice::getImmediateState()
{
    return hardwareUnit()->getImmediateSensorState(false);
}

void PoseSensorVRDevice::resetSensor()
{
    controller()->resetSensor(index());
}

DEFINE_TRACE(PoseSensorVRDevice)
{
    VRDevice::trace(visitor);
}

} // namespace blink
