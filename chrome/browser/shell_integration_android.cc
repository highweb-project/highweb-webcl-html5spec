// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/logging.h"
#include "chrome/browser/shell_integration.h"

namespace shell_integration {

// TODO: crbug/115375 to track implementation for following methods.
bool SetAsDefaultBrowser() {
  NOTIMPLEMENTED();
  return false;
}

bool SetAsDefaultProtocolClient(const std::string& protocol) {
  NOTIMPLEMENTED();
  return false;
}

DefaultWebClientSetPermission CanSetAsDefaultBrowser() {
  NOTIMPLEMENTED();
  return SET_DEFAULT_NOT_ALLOWED;
}

DefaultWebClientState GetDefaultBrowser() {
  NOTIMPLEMENTED();
  return UNKNOWN_DEFAULT;
}

bool IsFirefoxDefaultBrowser() {
  return false;
}

DefaultWebClientState IsDefaultProtocolClient(const std::string& protocol) {
  NOTIMPLEMENTED();
  return UNKNOWN_DEFAULT;
}

}  // namespace shell_integration
