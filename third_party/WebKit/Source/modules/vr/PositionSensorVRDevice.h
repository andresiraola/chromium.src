// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PositionSensorVRDevice_h
#define PositionSensorVRDevice_h

#include "modules/vr/PoseSensorVRDevice.h"

namespace blink {

class PositionSensorVRDevice final : public PoseSensorVRDevice {
    DEFINE_WRAPPERTYPEINFO();
public:
    PositionSensorVRDevice(VRHardwareUnit*, unsigned);

    DECLARE_VIRTUAL_TRACE();
};

} // namespace blink

#endif // PositionSensorVRDevice_h
