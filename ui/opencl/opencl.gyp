# Copyright (c) 2015 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'chromium_code': 1,
  },

  'targets': [
    {
      'target_name': 'openlc_impl',
      'type': 'executable',
      'include_dirs': [
        '<(DEPTH)/third_party/opencl'
      ],
      'sources': [
		'opencl_test.h',
        'opencl_test.cc'
      ]
    },
  ]
}