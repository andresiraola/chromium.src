// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/command_buffer/service/vr/openvr/open_vr_compositor.h"

#include "base/logging.h"
#include "ui/gl/gl_bindings.h"

namespace gpu {

OpenVRCompositor::OpenVRCompositor()
    : VRCompositor()
    , initialized_(false)
    , vr_system_(nullptr)
    , compositor_(nullptr) {
  Initialize();
}

OpenVRCompositor::~OpenVRCompositor() {
  if (compositor_) {
    compositor_->ClearLastSubmittedFrame();
    //compositor_->HideMirrorWindow();
    vr::VR_Shutdown();
  }
}

void OpenVRCompositor::SubmitFrame(GLuint frame_texture,
    GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
  if (!initialized_)
    return;

  /*void *d3d_texture = nullptr;
  glGetTexturePointervANGLE(frame_texture, GL_TEXTURE_2D, &d3d_texture);

  compositor_->Submit(vr::Eye_Left, vr::API_DirectX,
      reinterpret_cast<void*>(frame_texture), &left_bounds_);
  compositor_->Submit(vr::Eye_Right, vr::API_DirectX,
      reinterpret_cast<void*>(frame_texture), &right_bounds_);*/

  compositor_->Submit(vr::Eye_Left, vr::API_OpenGL,
    reinterpret_cast<void*>(frame_texture), &left_bounds_);
  compositor_->Submit(vr::Eye_Right, vr::API_OpenGL,
      reinterpret_cast<void*>(frame_texture), &right_bounds_);

  vr::TrackedDevicePose_t poses[vr::k_unMaxTrackedDeviceCount];
  compositor_->WaitGetPoses(poses, vr::k_unMaxTrackedDeviceCount, NULL, 0);
}

void OpenVRCompositor::Initialize() {
  if (!initialized_) {
    vr::HmdError error = vr::HmdError_None;
    vr_system_ = vr::VR_Init(&error);

    if (error != vr::HmdError_None) {
      LOG(ERROR) << "Error creating OpenVR System: " << vr::VR_GetStringForHmdError(error);
      vr_system_ = nullptr;
      return;
    }

    compositor_ = static_cast<vr::IVRCompositor*>(
        vr::VR_GetGenericInterface(vr::IVRCompositor_Version, &error));

    if (error != vr::HmdError_None) {
      LOG(ERROR) << "Error creating OpenVR Compositor: " << vr::VR_GetStringForHmdError(error);
      compositor_ = nullptr;
      return;
    }

    left_bounds_.uMin = 0.0f;
    left_bounds_.vMin = 0.0f;
    left_bounds_.uMax = 0.5f;
    left_bounds_.vMax = 1.0f;

    right_bounds_.uMin = 0.5f;
    right_bounds_.vMin = 0.0f;
    right_bounds_.uMax = 1.0f;
    right_bounds_.vMax = 1.0f;

    initialized_ = true;
  }
}

}  // namespace gpu
