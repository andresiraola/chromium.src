// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_BROWSER_VR_OPENVR_DEVICE_H
#define CONTENT_BROWSER_VR_OPENVR_DEVICE_H

#include "content/browser/vr/vr_device.h"
#include "third_party/openvr/openvr/headers/openvr.h"

namespace content {

class OpenVRDevice : public VRDevice {
 public:
  OpenVRDevice(VRDeviceProvider* provider,
               vr::IVRSystem* vr_system,
               vr::TrackedDeviceIndex_t device_index);
  ~OpenVRDevice() override;

  VRDeviceInfoPtr GetVRDevice() override;
  VRSensorStatePtr GetSensorState() override;
  void ResetSensor() override;

 private:
  void EnsureSensorStarted();
  std::string getOpenVRString(vr::TrackedDeviceProperty prop);

  vr::IVRSystem* vr_system_;
  vr::TrackedDeviceIndex_t device_index_;
  unsigned int frame_index_;
};

}  // namespace content

#endif  // CONTENT_BROWSER_VR_OPENVR_DEVICE_H
