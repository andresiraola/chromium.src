// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/browser/vr/openvr/open_vr_device_provider.h"

#include <algorithm>

#include "base/logging.h"
#include "content/browser/vr/openvr/open_vr_device.h"
#include "third_party/openvr/openvr/headers/openvr.h"

namespace content {

OpenVRDeviceProvider::OpenVRDeviceProvider()
    : VRDeviceProvider()
    , initialized_(false)
    , device_(nullptr)
    , vr_system_(nullptr) {
}

OpenVRDeviceProvider::~OpenVRDeviceProvider() {
  Shutdown();
}

void OpenVRDeviceProvider::GetDevices(std::vector<VRDevice*>* devices) {
  Initialize();

  if (!initialized_ || !vr_system_)
    return;

  LOG(INFO) << "OpenVRDeviceProvider::GetDevices";

  vr::TrackedDevicePose_t tracked_devices_poses[vr::k_unMaxTrackedDeviceCount];
  vr_system_->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding,
      0, tracked_devices_poses, vr::k_unMaxTrackedDeviceCount);

  if (!device_) {
    for (uint32_t i = 0; i < vr::k_unMaxTrackedDeviceCount; ++i) {
      if (vr_system_->GetTrackedDeviceClass(i) != vr::TrackedDeviceClass_HMD)
        continue;

      device_ = new OpenVRDevice(this, vr_system_, i);
      break;
    }
  }

  if (device_)
    devices->push_back(device_);
}

void OpenVRDeviceProvider::Initialize() {
  if (!initialized_) {
    LOG(INFO) << "OpenVRDeviceProvider::Initialize";
    // Loading the SteamVR Runtime
    vr::HmdError error = vr::HmdError_None;
    vr_system_ = vr::VR_Init( &error );

    if (error != vr::HmdError_None) {
      vr_system_ = nullptr;
      return;
    }

    initialized_ = true;
  }
}

void OpenVRDeviceProvider::Shutdown() {
  if (device_) {
    delete device_;
    device_ = nullptr;
  }

  if (initialized_) {
    vr::VR_Shutdown();
    vr_system_ = nullptr;
    initialized_ = false;
  }
}

}  // namespace content
