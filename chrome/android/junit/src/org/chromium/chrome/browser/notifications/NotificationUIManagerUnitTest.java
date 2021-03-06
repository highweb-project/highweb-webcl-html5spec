// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.chrome.browser.notifications;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertTrue;

import android.app.Notification;

import org.chromium.base.test.util.Feature;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.BlockJUnit4ClassRunner;

import java.util.Arrays;

/**
 * Unit tests for NotificationUIManager.
 */
@RunWith(BlockJUnit4ClassRunner.class)
public class NotificationUIManagerUnitTest {
    /**
     * Verifies that the getOriginFromTag method returns the origin for valid input, and null for
     * invalid input.
     */
    @Test
    @Feature({"Browser", "Notifications"})
    public void testGetOriginFromTag() throws Exception {
        // The common case.
        assertEquals(
                "https://example.com", NotificationUIManager.getOriginFromTag(
                                               "NotificationUIManager;https://example.com;42"));

        // An tag that includes the separator. Probably a bit unusual, but valid.
        assertEquals("https://example.com", NotificationUIManager.getOriginFromTag(
                "NotificationUIManager;https://example.com;this;tag;contains;the;separator"));

        // Some invalid input.
        assertNull(NotificationUIManager.getOriginFromTag("SystemDownloadNotifier"));
        assertNull(NotificationUIManager.getOriginFromTag(null));
        assertNull(NotificationUIManager.getOriginFromTag(""));
        assertNull(NotificationUIManager.getOriginFromTag(";"));
        assertNull(NotificationUIManager.getOriginFromTag(";;;;;;;"));
        assertNull(NotificationUIManager.getOriginFromTag(
                "SystemDownloadNotifier;NotificationUIManager;42"));
        assertNull(NotificationUIManager.getOriginFromTag(
                "SystemDownloadNotifier;https://example.com;42"));
        assertNull(NotificationUIManager.getOriginFromTag(
                "NotificationUIManager;SystemDownloadNotifier;42"));
    }

    /**
     * Verifies that the makeDefaults method returns the generated notification defaults.
     */
    @Test
    @Feature({"Browser", "Notifications"})
    public void testMakeDefaults() throws Exception {
        // 0 should be returned if silent is true and vibration's length is 0.
        assertEquals(0, NotificationUIManager.makeDefaults(0, true));

        // Notification.DEFAULT_ALL should be returned if silent is false and
        // vibration's length is 0.
        assertEquals(Notification.DEFAULT_ALL, NotificationUIManager.makeDefaults(0, false));

        // Notification.DEFAULT_ALL & ~Notification.DEFAULT_VIBRATE should be returned
        // if silent is false and vibration's length is greater than 0.
        assertEquals(Notification.DEFAULT_ALL & ~Notification.DEFAULT_VIBRATE,
                NotificationUIManager.makeDefaults(10, false));
    }

    /**
     * Verifies that the makeVibrationPattern method returns vibration pattern used
     * in Android notification.
     */
    @Test
    @Feature({"Browser", "Notifications"})
    public void testMakeVibrationPattern() throws Exception {
        assertTrue(Arrays.equals(new long[] {0, 100, 200, 300},
                NotificationUIManager.makeVibrationPattern(new int[] {100, 200, 300})));
    }
}
