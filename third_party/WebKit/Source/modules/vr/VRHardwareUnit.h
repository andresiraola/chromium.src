// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VRHardwareUnit_h
#define VRHardwareUnit_h

#include "modules/vr/VRFieldOfView.h"
#include "modules/vr/VRPoseState.h"
#include "platform/graphics/gpu/DrawingBuffer.h"
#include "platform/heap/Handle.h"
#include "public/platform/WebThread.h"
#include "wtf/text/WTFString.h"

namespace blink {

class VRController;
class VRDevice;
class HMDVRDevice;
class NavigatorVRDevice;
class PoseSensorVRDevice;

enum VREye {
    VREyeLeft,
    VREyeRight,
    VREyeNone,
};

class VRHardwareUnit : public GarbageCollectedFinalized<VRHardwareUnit>, public WebThread::TaskObserver, public DrawingBuffer::OrientationProvider {
public:
    explicit VRHardwareUnit(NavigatorVRDevice*);
    virtual ~VRHardwareUnit();

    void updateFromWebVRDevice(const WebVRDevice&);

    void addDevicesToVector(HeapVector<Member<VRDevice>>&);

    virtual unsigned index() const { return m_index; }
    const String& hardwareUnitId() const { return m_hardwareUnitId; }

    unsigned frameIndex() const { return m_frameIndex; }

    VRController* controller();

    // VRController queries
    VRPoseState* getSensorState();
    VRPoseState* getImmediateSensorState(bool updateFrameIndex);

    HMDVRDevice* hmd() const { return m_hmd; }
    PoseSensorVRDevice* poseSensor() const { return m_poseSensor; }

    void getOrientation(float &x, float &y, float &z, float &w) override;

    DECLARE_VIRTUAL_TRACE();

private:
    // TaskObserver implementation.
    void didProcessTask() override;
    void willProcessTask() override { }

    unsigned m_index;
    String m_hardwareUnitId;
    unsigned m_nextDeviceId;

    unsigned m_frameIndex;

    Member<NavigatorVRDevice> m_navigatorVRDevice;
    Member<VRPoseState> m_poseState;
    bool m_canUpdatePoseState;

    // Device types
    Member<HMDVRDevice> m_hmd;
    Member<PoseSensorVRDevice> m_poseSensor;
};

} // namespace blink

#endif // VRHardwareUnit_h
