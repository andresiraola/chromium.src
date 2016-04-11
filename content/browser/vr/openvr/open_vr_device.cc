// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/browser/vr/openvr/open_vr_device.h"

#include <algorithm>
#include <math.h>

#include "base/logging.h"
#include "base/strings/stringprintf.h"
#include "third_party/openvr/openvr/headers/openvr.h"
#include "ui/gfx/transform.h"
#include "ui/gfx/transform_util.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace content {

namespace {

VRFieldOfViewPtr openVRFovToWebVR(vr::IVRSystem* vr_system, vr::Hmd_Eye eye) {
  VRFieldOfViewPtr out = VRFieldOfView::New();

  float upTan, downTan, leftTan, rightTan;
  vr_system->GetProjectionRaw(eye, &leftTan, &rightTan, &upTan, &downTan);
  out->upDegrees = -(atanf(upTan) * 180.0 / M_PI);
  out->downDegrees = atanf(downTan) * 180.0 / M_PI;
  out->leftDegrees = -(atanf(leftTan) * 180.0 / M_PI);
  out->rightDegrees = atanf(rightTan) * 180.0 / M_PI;
  return std::move(out);
}

VRVector3Ptr HmdVector3ToWebVR(const vr::HmdVector3_t& vec) {
  VRVector3Ptr out = VRVector3::New();
  out->x = vec.v[0];
  out->y = vec.v[1];
  out->z = vec.v[2];
  return std::move(out);
}

} // namespace

OpenVRDevice::OpenVRDevice(VRDeviceProvider* provider,
    vr::IVRSystem* vr_system,
    vr::TrackedDeviceIndex_t device_index)
    : VRDevice(provider)
    , vr_system_(vr_system)
    , device_index_(device_index)
    , frame_index_(0)
{
  LOG(INFO) << "New OpenVR device: " << device_index;
}

OpenVRDevice::~OpenVRDevice() {
}

std::string OpenVRDevice::getOpenVRString(vr::TrackedDeviceProperty prop) {
  std::string out;

  vr::TrackedPropertyError error = vr::TrackedProp_Success;
  char openvr_string[vr::k_unTrackingStringSize];
  vr_system_->GetStringTrackedDeviceProperty(
    device_index_,
    prop,
    openvr_string,
    vr::k_unTrackingStringSize,
    &error);

  if (error == vr::TrackedProp_Success)
    out = openvr_string;

  return out;
}

VRDeviceInfoPtr OpenVRDevice::GetVRDevice() {
  VRDeviceInfoPtr device = VRDeviceInfo::New();

  device->deviceName = getOpenVRString(vr::Prop_ManufacturerName_String) + " "
                     + getOpenVRString(vr::Prop_ModelNumber_String);

  vr::TrackedDeviceClass device_class = vr_system_->GetTrackedDeviceClass(
      device_index_);

  // Only populate the HMD info if this is actually an HMD
  if (device_class == vr::TrackedDeviceClass_HMD) {
    device->hmdInfo = VRHMDInfo::New();
    VRHMDInfoPtr& hmdInfo = device->hmdInfo;

    hmdInfo->leftEye = VREyeParameters::New();
    hmdInfo->rightEye = VREyeParameters::New();
    VREyeParametersPtr& leftEye = hmdInfo->leftEye;
    VREyeParametersPtr& rightEye = hmdInfo->rightEye;

    leftEye->recommendedFieldOfView = openVRFovToWebVR(vr_system_,
                                                       vr::Eye_Left);
    rightEye->recommendedFieldOfView = openVRFovToWebVR(vr_system_,
                                                        vr::Eye_Right);

    // Not supporting configurable FOV for now.
    leftEye->maximumFieldOfView = leftEye->recommendedFieldOfView.Clone();
    rightEye->maximumFieldOfView = rightEye->recommendedFieldOfView.Clone();
    leftEye->minimumFieldOfView = leftEye->recommendedFieldOfView.Clone();
    rightEye->minimumFieldOfView = rightEye->recommendedFieldOfView.Clone();

    vr::TrackedPropertyError error = vr::TrackedProp_Success;
    float ipd = vr_system_->GetFloatTrackedDeviceProperty(device_index_,
        vr::Prop_UserIpdMeters_Float, &error);
    if (error != vr::TrackedProp_Success)
      ipd = 0.06f; // Default average IPD

    leftEye->eyeTranslation = VRVector3::New();
    leftEye->eyeTranslation->x = -ipd * 0.5;
    leftEye->eyeTranslation->y = 0.0f;
    leftEye->eyeTranslation->z = 0.0f;

    rightEye->eyeTranslation = VRVector3::New();
    rightEye->eyeTranslation->x = ipd * 0.5;
    rightEye->eyeTranslation->y = 0.0;
    rightEye->eyeTranslation->z = 0.0;

    uint32_t width, height;
    // This is the render size per-eye. OpenVR docs don't make that clear.
    vr_system_->GetRecommendedRenderTargetSize(&width, &height);

    leftEye->renderRect = VRRect::New();
    leftEye->renderRect->x = 0;
    leftEye->renderRect->y = 0;
    leftEye->renderRect->width = width;
    leftEye->renderRect->height = height;

    rightEye->renderRect = VRRect::New();
    rightEye->renderRect->x = width;
    rightEye->renderRect->y = 0;
    rightEye->renderRect->width = width;
    rightEye->renderRect->height = height;
  }

  return std::move(device);
}

VRSensorStatePtr OpenVRDevice::GetSensorState() {
  VRSensorStatePtr state = VRSensorState::New();

  state->timestamp = frame_index_;
  state->frameIndex = frame_index_;
  frame_index_++;

  vr::TrackedDevicePose_t tracked_devices_poses[vr::k_unMaxTrackedDeviceCount];
  vr_system_->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseSeated,
      0.04f, tracked_devices_poses, vr::k_unMaxTrackedDeviceCount);

  const vr::TrackedDevicePose_t &pose = tracked_devices_poses[device_index_];
  if (pose.bPoseIsValid) {
    const vr::HmdMatrix34_t& mat = pose.mDeviceToAbsoluteTracking;
    gfx::Transform transform(
        mat.m[0][0], mat.m[0][1], mat.m[0][2], mat.m[0][3],
        mat.m[1][0], mat.m[1][1], mat.m[1][2], mat.m[1][3],
        mat.m[2][0], mat.m[2][1], mat.m[2][2], mat.m[2][3],
        0, 0, 0, 1);

    gfx::DecomposedTransform decomposed_transform;
    gfx::DecomposeTransform(&decomposed_transform, transform);

    state->orientation = VRVector4::New();
    state->orientation->x = decomposed_transform.quaternion[0];
    state->orientation->y = decomposed_transform.quaternion[1];
    state->orientation->z = decomposed_transform.quaternion[2];
    state->orientation->w = decomposed_transform.quaternion[3];

    state->position = VRVector3::New();
    state->position->x = decomposed_transform.translate[0];
    state->position->y = decomposed_transform.translate[1];
    state->position->z = decomposed_transform.translate[2];

    state->angularVelocity = HmdVector3ToWebVR(pose.vAngularVelocity);
    state->linearVelocity = HmdVector3ToWebVR(pose.vVelocity);
  }

  return std::move(state);
}

void OpenVRDevice::ResetSensor() {
  vr_system_->ResetSeatedZeroPose();
}

}  // namespace content
