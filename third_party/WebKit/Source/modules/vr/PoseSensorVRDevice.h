// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PoseSensorVRDevice_h
#define PoseSensorVRDevice_h

#include "modules/vr/VRDevice.h"
#include "modules/vr/VRPoseState.h"
#include "platform/heap/Handle.h"
//#include "wtf/Forward.h"

namespace blink {

class PoseSensorVRDevice : public VRDevice {
    DEFINE_WRAPPERTYPEINFO();
public:
    PoseSensorVRDevice(VRHardwareUnit*, unsigned);

    VRPoseState* getState();
    VRPoseState* getImmediateState();
    void resetSensor();

    DECLARE_VIRTUAL_TRACE();
};

} // namespace blink

#endif // PoseSensorVRDevice_h
