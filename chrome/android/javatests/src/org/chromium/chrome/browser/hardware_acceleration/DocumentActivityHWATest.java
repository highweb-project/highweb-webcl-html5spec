// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.chrome.browser.hardware_acceleration;

import static org.chromium.base.test.util.Restriction.RESTRICTION_TYPE_LOW_END_DEVICE;

import android.test.suitebuilder.annotation.SmallTest;

import org.chromium.base.ApplicationStatus;
import org.chromium.base.test.util.Restriction;
import org.chromium.chrome.browser.document.DocumentActivity;
import org.chromium.chrome.browser.document.DocumentModeTest;

/**
 * Tests that DocumentActivity is hardware accelerated only high-end devices.
 */
public class DocumentActivityHWATest extends DocumentModeTest {

    @SmallTest
    public void testHardwareAcceleration() throws Exception {
        Utils.assertHardwareAcceleration(startAndWaitActivity());
    }

    @Restriction(RESTRICTION_TYPE_LOW_END_DEVICE)
    @SmallTest
    public void testNoRenderThread() throws Exception {
        startAndWaitActivity();
        Utils.assertNoRenderThread();
    }

    private DocumentActivity startAndWaitActivity() throws Exception {
        launchViaViewIntent(false, URL_1, "Page 1");
        return (DocumentActivity) ApplicationStatus.getLastTrackedFocusedActivity();
    }
}
