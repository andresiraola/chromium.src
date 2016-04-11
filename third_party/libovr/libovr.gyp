# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'includes': [
    'libovr.gypi',
  ],
  'target_defaults': {
    'conditions': [
      ['OS=="mac" or OS=="ios"', {
        'xcode_settings': {
          'GCC_WARN_ABOUT_MISSING_NEWLINE': 'NO',
        },
      }],
    ],
  },
  'targets': [
    {
      'target_name': 'libovr',
      'type': 'static_library',
      'dependencies': [
        #'<(DEPTH)/third_party/mesa/mesa.gyp:mesa_headers',
      ],
      'include_dirs': [
        './LibOVR/Include',
      ],
      'sources': [
        '<@(libovr_sources)'
      ],
      'conditions': [
        ['OS=="win"', {
          'defines!': [
            #'WIN32_LEAN_AND_MEAN'
          ]
        }]
      ]
    },  # target libovr
  ]
}
