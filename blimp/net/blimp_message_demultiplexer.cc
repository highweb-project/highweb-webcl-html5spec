// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "blimp/net/blimp_message_demultiplexer.h"

#include <string>

#include "base/strings/stringprintf.h"
#include "blimp/common/logging.h"
#include "blimp/net/common.h"
#include "net/base/net_errors.h"

namespace blimp {

BlimpMessageDemultiplexer::BlimpMessageDemultiplexer() {}

BlimpMessageDemultiplexer::~BlimpMessageDemultiplexer() {}

void BlimpMessageDemultiplexer::AddProcessor(BlimpMessage::Type type,
                                             BlimpMessageProcessor* receiver) {
  DCHECK(receiver);
  if (feature_receiver_map_.find(type) == feature_receiver_map_.end()) {
    feature_receiver_map_.insert(std::make_pair(type, receiver));
  } else {
    DLOG(FATAL) << "Handler already registered for type=" << type << ".";
  }
}

void BlimpMessageDemultiplexer::ProcessMessage(
    scoped_ptr<BlimpMessage> message,
    const net::CompletionCallback& callback) {
  DVLOG(2) << "ProcessMessage : " << *message;
  auto receiver_iter = feature_receiver_map_.find(message->type());
  if (receiver_iter == feature_receiver_map_.end()) {
    DLOG(ERROR) << "No registered receiver for " << *message << ".";
    if (!callback.is_null()) {
      callback.Run(net::ERR_NOT_IMPLEMENTED);
    }
    return;
  }

  DVLOG(2) << "Routed message " << *message << ".";
  receiver_iter->second->ProcessMessage(std::move(message), callback);
}

}  // namespace blimp
