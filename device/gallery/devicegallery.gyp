# Copyright 2014 The Chromium Authors. All rights reserved.
# Copyright (C) 2016 INFRAWARE, Inc. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'chromium_code': 1,
  },
  'targets': [
    {
      # GN version: //device/gallery:mojo_bindings
      'target_name': 'device_gallery_mojo_bindings',
      'type': 'static_library',
      'includes': [
        '../../mojo/mojom_bindings_generator.gypi',
      ],
      'sources': [
        'devicegallery_manager.mojom',
        'devicegallery_ResultCode.mojom',
      ],
    },
    {
      # GN version: //device/gallery
      'target_name': 'device_gallery',
      'type': '<(component)',
      'dependencies': [
        '../../base/base.gyp:base',
        '../../base/third_party/dynamic_annotations/dynamic_annotations.gyp:dynamic_annotations',
        #'../../mojo/mojo_base.gyp:mojo_environment_chromium',
        '../../mojo/mojo_edk.gyp:mojo_system_impl',
        '../../mojo/mojo_public.gyp:mojo_cpp_bindings',
        'device_gallery_mojo_bindings',
      ],
      'defines': [
        'DEVICE_GALLERY_IMPLEMENTATION',
      ],
      'sources': [
        'devicegallery_manager_impl.h',
        'devicegallery_manager_impl_default.cc',
      ],
      'conditions': [
        ['OS == "android"', {
          'dependencies': [
            'device_gallery_java',
          ],
          'sources!': [
            'devicegallery_manager_impl_default.cc',
          ],
        }],
      ],
    },
  ],
  'conditions': [
    ['OS == "android"', {
      'targets': [
        {
          'target_name': 'device_gallery_java',
          'type': 'none',
          'dependencies': [
            '../../base/base.gyp:base',
            '../../mojo/mojo_public.gyp:mojo_bindings_java',
            'device_gallery_mojo_bindings',
          ],
          'variables': {
            'java_in_dir': '../../device/gallery/android/java',
          },
          'includes': [ '../../build/java.gypi' ],
        },
      ],
    }],
  ],
}
