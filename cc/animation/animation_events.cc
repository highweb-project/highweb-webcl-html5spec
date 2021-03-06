// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/animation/animation_events.h"

namespace cc {

AnimationEvent::AnimationEvent(AnimationEvent::Type type,
                               int layer_id,
                               int group_id,
                               TargetProperty::Type target_property,
                               base::TimeTicks monotonic_time)
    : type(type),
      layer_id(layer_id),
      group_id(group_id),
      target_property(target_property),
      monotonic_time(monotonic_time),
      is_impl_only(false),
      opacity(0.f) {}

AnimationEvent::AnimationEvent(const AnimationEvent& other) {
  type = other.type;
  layer_id = other.layer_id;
  group_id = other.group_id;
  target_property = other.target_property;
  monotonic_time = other.monotonic_time;
  is_impl_only = other.is_impl_only;
  opacity = other.opacity;
  transform = other.transform;
  filters = other.filters;
  animation_start_time = other.animation_start_time;
  if (other.curve)
    curve = other.curve->Clone();
}

AnimationEvent& AnimationEvent::operator=(const AnimationEvent& other) {
  type = other.type;
  layer_id = other.layer_id;
  group_id = other.group_id;
  target_property = other.target_property;
  monotonic_time = other.monotonic_time;
  is_impl_only = other.is_impl_only;
  opacity = other.opacity;
  transform = other.transform;
  filters = other.filters;
  animation_start_time = other.animation_start_time;
  if (other.curve)
    curve = other.curve->Clone();
  return *this;
}

AnimationEvent::~AnimationEvent() {}

AnimationEvents::AnimationEvents() {}

AnimationEvents::~AnimationEvents() {}

}  // namespace cc
