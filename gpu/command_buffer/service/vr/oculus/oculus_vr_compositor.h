// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GPU_COMMAND_BUFFER_SERVICE_VR_OCULUS_VR_COMPOSITOR_H_
#define GPU_COMMAND_BUFFER_SERVICE_VR_OCULUS_VR_COMPOSITOR_H_

#include "gpu/command_buffer/service/vr/vr_compositor.h"
#include "third_party/libovr/LibOVR/Include/OVR_CAPI.h"

namespace gpu {

class OculusVRCompositor : public VRCompositor {
 public:
  OculusVRCompositor();
  ~OculusVRCompositor() override;

  void SubmitFrame(GLuint frame_texture,
      GLfloat x, GLfloat y, GLfloat z, GLfloat w) override;

 private:
  void Initialize();

  void CreateProgram();

  bool initialized_;
  ovrSession session_;
  ovrSwapTextureSet* swap_textures_;
  ovrRecti left_viewport_;
  ovrRecti right_viewport_;
  ovrFovPort left_fov_;
  ovrFovPort right_fov_;
  GLuint width_;
  GLuint height_;
  unsigned int fail_count_;

  GLuint buffer_;
  GLuint framebuffer_;
  GLuint program_;
  GLuint sampler_location_;
};

}  // namespace gpu

#endif  // GPU_COMMAND_BUFFER_SERVICE_VR_OCULUS_VR_COMPOSITOR_H_
