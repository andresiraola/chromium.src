// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_BROWSER_VR_OCULUS_VR_DEVICE_H
#define CONTENT_BROWSER_VR_OCULUS_VR_DEVICE_H

#include "content/browser/vr/vr_device.h"
#include "third_party/libovr/LibOVR/Include/OVR_CAPI.h"

namespace content {

class OculusVRDevice : public VRDevice {
 public:
  OculusVRDevice(VRDeviceProvider* provider, ovrSession session);
  ~OculusVRDevice() override;

  VRDeviceInfoPtr GetVRDevice() override;
  VRSensorStatePtr GetSensorState() override;
  void ResetSensor() override;

 private:
  void EnsureSensorStarted();

  ovrSession session_;
  bool sensor_started_;
  unsigned int frame_index_;
};

}  // namespace content

#endif  // CONTENT_BROWSER_VR_OCULUS_VR_DEVICE_H
