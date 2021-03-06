// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.chrome.browser.offlinepages;

import org.chromium.components.bookmarks.BookmarkId;

/**
 * Object to hold a client identifier for an offline page.
 */
public class ClientId {
    private String mNamespace;
    private String mId;

    public ClientId(String namespace, String id) {
        mNamespace = namespace;
        mId = id;
    }

    public String getNamespace() {
        return mNamespace;
    }

    public String getId() {
        return mId;
    }

    @Override
    public boolean equals(Object o) {
        if (o instanceof ClientId) {
            ClientId otherId = (ClientId) o;
            return otherId.getNamespace().equals(mNamespace) && otherId.getId().equals(mId);
        }
        return false;
    }

    @Override
    public int hashCode() {
        return (mNamespace + ":" + mId).hashCode();
    }

    /**
     * Create a client id for a bookmark
     * @param id The bookmark id to wrap.
     * @return A {@link ClientId} that represents this BookmarkId.
     */
    public static ClientId createClientIdForBookmarkId(BookmarkId id) {
        return new ClientId(OfflinePageBridge.BOOKMARK_NAMESPACE, id.toString());
    }
}
