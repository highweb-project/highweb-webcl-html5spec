// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MOJO_SHELL_PUBLIC_CPP_IDENTITY_H_
#define MOJO_SHELL_PUBLIC_CPP_IDENTITY_H_

#include <string>

#include "mojo/shell/public/interfaces/connector.mojom.h"

namespace mojo {

// Represents the identity of an application.
// |name| is the structured name of the application.
// |instance| is a string that allows to tie a specific instance to another. A
// typical use case of instance is to control process grouping for a given name.
class Identity {
 public:
  Identity();
  Identity(const std::string& name,
           const std::string& user_id);
  Identity(const std::string& name,
           const std::string& user_id,
           const std::string& instance);
  Identity(const Identity& other);
  ~Identity();

  bool operator<(const Identity& other) const;
  bool is_null() const { return name_.empty(); }
  bool operator==(const Identity& other) const;

  const std::string& name() const { return name_; }
  const std::string& user_id() const { return user_id_; }
  void set_user_id(const std::string& user_id) { user_id_ = user_id; }
  const std::string& instance() const { return instance_; }

 private:
  std::string name_;
  std::string user_id_;
  std::string instance_;
};

template <>
struct TypeConverter<shell::mojom::IdentityPtr, Identity> {
  static shell::mojom::IdentityPtr Convert(const Identity& input);
};
template <>
struct TypeConverter<Identity, shell::mojom::IdentityPtr> {
  static Identity Convert(const shell::mojom::IdentityPtr& input);
};

}  // namespace mojo

#endif  // MOJO_SHELL_PUBLIC_CPP_IDENTITY_H_
