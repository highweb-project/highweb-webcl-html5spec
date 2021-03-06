// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_VIEWS_CONTROLS_BUTTON_MD_TEXT_BUTTON_H_
#define UI_VIEWS_CONTROLS_BUTTON_MD_TEXT_BUTTON_H_

#include "base/memory/scoped_ptr.h"
#include "ui/views/animation/button_ink_drop_delegate.h"
#include "ui/views/controls/button/label_button.h"

namespace views {

// A button class that implements the Material Design text button spec.
class VIEWS_EXPORT MdTextButton : public LabelButton {
 public:
  // Creates a normal STYLE_BUTTON LabelButton in pre-MD, or an MdTextButton
  // in MD mode.
  static LabelButton* CreateStandardButton(ButtonListener* listener,
                                           const base::string16& text);

  // LabelButton:
  SkColor GetInkDropBaseColor() const override;
  void SetText(const base::string16& text) override;

 private:
  MdTextButton(ButtonListener* listener);
  ~MdTextButton() override;

  ButtonInkDropDelegate ink_drop_delegate_;

  DISALLOW_COPY_AND_ASSIGN(MdTextButton);
};

}  // namespace views

#endif  // UI_VIEWS_CONTROLS_BUTTON_MD_TEXT_BUTTON_H_
