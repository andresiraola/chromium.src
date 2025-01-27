// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "modules/vr/PositionSensorVRDevice.h"

#include "modules/vr/VRController.h"
#include "public/platform/Platform.h"

namespace blink {

PositionSensorVRDevice::PositionSensorVRDevice(VRHardwareUnit* hardwareUnit, unsigned deviceId)
    : PoseSensorVRDevice(hardwareUnit, deviceId)
{
}

DEFINE_TRACE(PositionSensorVRDevice)
{
    PoseSensorVRDevice::trace(visitor);
}

} // namespace blink
