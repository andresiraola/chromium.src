// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/browser/vr/oculus/oculus_vr_device.h"

#include <algorithm>
#include <math.h>

#include "base/strings/stringprintf.h"
#include "third_party/libovr/LibOVR/Include/OVR_CAPI.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace content {

OculusVRDevice::OculusVRDevice(VRDeviceProvider* provider, ovrSession session)
    : VRDevice(provider)
    , session_(session)
    , sensor_started_(false)
    , frame_index_(0)
{
  //CHECK(session_);
}

OculusVRDevice::~OculusVRDevice() {
  if (session_) {
    ovr_Destroy(session_);
    session_ = NULL;
  }
}

VRFieldOfViewPtr ovrFovPortToWebVR(const ovrFovPort& fov) {
  VRFieldOfViewPtr out = VRFieldOfView::New();
  out->upDegrees = atanf(fov.UpTan) * 180.0 / M_PI;
  out->downDegrees = atanf(fov.DownTan) * 180.0 / M_PI;
  out->leftDegrees = atanf(fov.LeftTan) * 180.0 / M_PI;
  out->rightDegrees = atanf(fov.RightTan) * 180.0 / M_PI;
  return std::move(out);
}

VRVector4Ptr ovrQuatfToWebVR(const ovrQuatf& quat) {
  VRVector4Ptr out = VRVector4::New();
  out->x = quat.x;
  out->y = quat.y;
  out->z = quat.z;
  out->w = quat.w;
  return std::move(out);
}

VRVector3Ptr ovrVector3fToWebVR(const ovrVector3f& vec) {
  VRVector3Ptr out = VRVector3::New();
  out->x = vec.x;
  out->y = vec.y;
  out->z = vec.z;
  return std::move(out);
}

VRDeviceInfoPtr OculusVRDevice::GetVRDevice() {
  VRDeviceInfoPtr device = VRDeviceInfo::New();

  ovrHmdDesc session_desc = ovr_GetHmdDesc(session_);

  device->deviceName = base::StringPrintf("%s, %s",
      session_desc.ProductName, session_desc.Manufacturer);

  device->hmdInfo = VRHMDInfo::New();
  VRHMDInfoPtr& hmdInfo = device->hmdInfo;

  hmdInfo->leftEye = VREyeParameters::New();
  hmdInfo->rightEye = VREyeParameters::New();
  VREyeParametersPtr& leftEye = hmdInfo->leftEye;
  VREyeParametersPtr& rightEye = hmdInfo->rightEye;

  leftEye->recommendedFieldOfView = ovrFovPortToWebVR(
      session_desc.DefaultEyeFov[ovrEye_Left]);
  rightEye->recommendedFieldOfView = ovrFovPortToWebVR(
      session_desc.DefaultEyeFov[ovrEye_Right]);

  // Not supporting configurable FOV for now.
  leftEye->maximumFieldOfView = leftEye->recommendedFieldOfView.Clone();
  rightEye->maximumFieldOfView = rightEye->recommendedFieldOfView.Clone();
  leftEye->minimumFieldOfView = leftEye->recommendedFieldOfView.Clone();
  rightEye->minimumFieldOfView = rightEye->recommendedFieldOfView.Clone();

  ovrEyeRenderDesc leftEyeDesc = ovr_GetRenderDesc(session_, ovrEye_Left,
      session_desc.DefaultEyeFov[ovrEye_Left]);
  ovrEyeRenderDesc rightEyeDesc = ovr_GetRenderDesc(session_, ovrEye_Right,
      session_desc.DefaultEyeFov[ovrEye_Right]);

  leftEye->eyeTranslation = VRVector3::New();
  leftEye->eyeTranslation->x = leftEyeDesc.HmdToEyeViewOffset.x;
  leftEye->eyeTranslation->y = leftEyeDesc.HmdToEyeViewOffset.y;
  leftEye->eyeTranslation->z = leftEyeDesc.HmdToEyeViewOffset.z;

  rightEye->eyeTranslation = VRVector3::New();
  rightEye->eyeTranslation->x = rightEyeDesc.HmdToEyeViewOffset.x;
  rightEye->eyeTranslation->y = rightEyeDesc.HmdToEyeViewOffset.y;
  rightEye->eyeTranslation->z = rightEyeDesc.HmdToEyeViewOffset.z;

  ovrSizei targetLeft = ovr_GetFovTextureSize(session_, ovrEye_Left,
      session_desc.DefaultEyeFov[ovrEye_Left], 1.0f);
  ovrSizei targetRight = ovr_GetFovTextureSize(session_, ovrEye_Right,
      session_desc.DefaultEyeFov[ovrEye_Right], 1.0f);

  leftEye->renderRect = VRRect::New();
  leftEye->renderRect->x = 0;
  leftEye->renderRect->y = 0;
  leftEye->renderRect->width = targetLeft.w;
  leftEye->renderRect->height = targetLeft.h;

  rightEye->renderRect = VRRect::New();
  rightEye->renderRect->x = targetLeft.w;
  rightEye->renderRect->y = 0;
  rightEye->renderRect->width = targetRight.w;
  rightEye->renderRect->height = targetRight.h;

  return std::move(device);
}

void OculusVRDevice::EnsureSensorStarted() {
  if (sensor_started_)
    return;

  ovr_ConfigureTracking(session_,
    ovrTrackingCap_Orientation |
    ovrTrackingCap_MagYawCorrection |
    ovrTrackingCap_Position,
    ovrTrackingCap_Orientation);
  sensor_started_ = true;
}

VRSensorStatePtr OculusVRDevice::GetSensorState() {
  VRSensorStatePtr state = VRSensorState::New();

  EnsureSensorStarted();

  ovrTrackingState ovr_state = ovr_GetTrackingState(session_,
      ovr_GetTimeInSeconds(), true);
  ovrPoseStatef pose_state = ovr_state.HeadPose;
  ovrPosef pose = pose_state.ThePose;

  state->timestamp = pose_state.TimeInSeconds;
  state->frameIndex = frame_index_;

  state->orientation = ovrQuatfToWebVR(pose.Orientation);
  state->position = ovrVector3fToWebVR(pose.Position);
  state->angularVelocity = ovrVector3fToWebVR(pose_state.AngularVelocity);
  state->linearVelocity = ovrVector3fToWebVR(pose_state.LinearVelocity);
  state->angularAcceleration = ovrVector3fToWebVR(
      pose_state.AngularAcceleration);
  state->linearAcceleration = ovrVector3fToWebVR(pose_state.LinearAcceleration);

  frame_index_++;

  return std::move(state);
}

void OculusVRDevice::ResetSensor() {
  ovr_RecenterPose(session_);
}

}  // namespace content
