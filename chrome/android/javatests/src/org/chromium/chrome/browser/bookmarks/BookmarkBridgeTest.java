// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.chrome.browser.bookmarks;

import android.test.UiThreadTest;
import android.test.suitebuilder.annotation.SmallTest;

import org.chromium.base.ThreadUtils;
import org.chromium.base.test.util.Feature;
import org.chromium.chrome.browser.bookmarks.BookmarkBridge.BookmarkItem;
import org.chromium.chrome.browser.profiles.Profile;
import org.chromium.chrome.test.util.BookmarkTestUtil;
import org.chromium.components.bookmarks.BookmarkId;
import org.chromium.content.browser.test.NativeLibraryTestBase;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;

/**
 * Tests for bookmark bridge
 */
public class BookmarkBridgeTest extends NativeLibraryTestBase {

    private BookmarkBridge mBookmarkBridge;
    private BookmarkId mMobileNode;
    private BookmarkId mOtherNode;
    private BookmarkId mDesktopNode;

    @Override
    protected void setUp() throws Exception {
        super.setUp();
        loadNativeLibraryAndInitBrowserProcess();

        ThreadUtils.runOnUiThreadBlocking(new Runnable() {
            @Override
            public void run() {
                Profile profile = Profile.getLastUsedProfile();
                mBookmarkBridge = new BookmarkBridge(profile);
                mBookmarkBridge.loadEmptyPartnerBookmarkShimForTesting();
            }
        });

        BookmarkTestUtil.waitForBookmarkModelLoaded();
        ThreadUtils.runOnUiThreadBlocking(new Runnable() {
            @Override
            public void run() {
                mMobileNode = mBookmarkBridge.getMobileFolderId();
                mDesktopNode = mBookmarkBridge.getDesktopFolderId();
                mOtherNode = mBookmarkBridge.getOtherFolderId();
            }
        });
    }

    @UiThreadTest
    @SmallTest
    @Feature({"Bookmark"})
    public void testAddBookmarksAndFolders() {
        BookmarkId bookmarkA = mBookmarkBridge.addBookmark(mDesktopNode, 0, "a", "http://a.com");
        verifyBookmark(bookmarkA, "a", "http://a.com/", false, mDesktopNode);
        BookmarkId bookmarkB = mBookmarkBridge.addBookmark(mOtherNode, 0, "b", "http://b.com");
        verifyBookmark(bookmarkB, "b", "http://b.com/", false, mOtherNode);
        BookmarkId bookmarkC = mBookmarkBridge.addBookmark(mMobileNode, 0, "c", "http://c.com");
        verifyBookmark(bookmarkC, "c", "http://c.com/", false, mMobileNode);
        BookmarkId folderA = mBookmarkBridge.addFolder(mOtherNode, 0, "fa");
        verifyBookmark(folderA, "fa", null, true, mOtherNode);
        BookmarkId folderB = mBookmarkBridge.addFolder(mDesktopNode, 0, "fb");
        verifyBookmark(folderB, "fb", null, true, mDesktopNode);
        BookmarkId folderC = mBookmarkBridge.addFolder(mMobileNode, 0, "fc");
        verifyBookmark(folderC, "fc", null, true, mMobileNode);
        BookmarkId bookmarkAA = mBookmarkBridge.addBookmark(folderA, 0, "aa", "http://aa.com");
        verifyBookmark(bookmarkAA, "aa", "http://aa.com/", false, folderA);
        BookmarkId folderAA = mBookmarkBridge.addFolder(folderA, 0, "faa");
        verifyBookmark(folderAA, "faa", null, true, folderA);
    }

    private void verifyBookmark(BookmarkId idToVerify, String expectedTitle,
            String expectedUrl, boolean isFolder, BookmarkId expectedParent) {
        assertNotNull(idToVerify);
        BookmarkItem item = mBookmarkBridge.getBookmarkById(idToVerify);
        assertEquals(expectedTitle, item.getTitle());
        assertEquals(item.isFolder(), isFolder);
        if (!isFolder) assertEquals(expectedUrl, item.getUrl());
        assertEquals(item.getParentId(), expectedParent);
    }

    @UiThreadTest
    @SmallTest
    @Feature({"Bookmark"})
    public void testGetAllFoldersWithDepths() {
        BookmarkId folderA = mBookmarkBridge.addFolder(mMobileNode, 0, "a");
        BookmarkId folderB = mBookmarkBridge.addFolder(mDesktopNode, 0, "b");
        BookmarkId folderC = mBookmarkBridge.addFolder(mOtherNode, 0, "c");
        BookmarkId folderAA = mBookmarkBridge.addFolder(folderA, 0, "aa");
        BookmarkId folderBA = mBookmarkBridge.addFolder(folderB, 0, "ba");
        BookmarkId folderAAA = mBookmarkBridge.addFolder(folderAA, 0, "aaa");
        BookmarkId folderAAAA = mBookmarkBridge.addFolder(folderAAA, 0, "aaaa");

        mBookmarkBridge.addBookmark(mMobileNode, 0, "ua", "http://www.google.com");
        mBookmarkBridge.addBookmark(mDesktopNode, 0, "ua", "http://www.google.com");
        mBookmarkBridge.addBookmark(mOtherNode, 0, "ua", "http://www.google.com");
        mBookmarkBridge.addBookmark(folderA, 0, "ua", "http://www.medium.com");

        // Map folders to depths as expected results
        HashMap<BookmarkId, Integer> idToDepth = new HashMap<BookmarkId, Integer>();
        idToDepth.put(mMobileNode, 0);
        idToDepth.put(folderA, 1);
        idToDepth.put(folderAA, 2);
        idToDepth.put(folderAAA, 3);
        idToDepth.put(folderAAAA, 4);
        idToDepth.put(mDesktopNode, 0);
        idToDepth.put(folderB, 1);
        idToDepth.put(folderBA, 2);
        idToDepth.put(mOtherNode, 0);
        idToDepth.put(folderC, 1);

        List<BookmarkId> folderList = new ArrayList<BookmarkId>();
        List<Integer> depthList = new ArrayList<Integer>();
        mBookmarkBridge.getAllFoldersWithDepths(folderList, depthList);
        verifyFolderDepths(folderList, depthList, idToDepth);
    }

    @UiThreadTest
    @SmallTest
    @Feature({"Bookmark"})
    public void testGetMoveDestinations() {
        BookmarkId folderA = mBookmarkBridge.addFolder(mMobileNode, 0, "a");
        BookmarkId folderB = mBookmarkBridge.addFolder(mDesktopNode, 0, "b");
        BookmarkId folderC = mBookmarkBridge.addFolder(mOtherNode, 0, "c");
        BookmarkId folderAA = mBookmarkBridge.addFolder(folderA, 0, "aa");
        BookmarkId folderBA = mBookmarkBridge.addFolder(folderB, 0, "ba");
        BookmarkId folderAAA = mBookmarkBridge.addFolder(folderAA, 0, "aaa");

        mBookmarkBridge.addBookmark(mMobileNode, 0, "ua", "http://www.google.com");
        mBookmarkBridge.addBookmark(mDesktopNode, 0, "ua", "http://www.google.com");
        mBookmarkBridge.addBookmark(mOtherNode, 0, "ua", "http://www.google.com");
        mBookmarkBridge.addBookmark(folderA, 0, "ua", "http://www.medium.com");

        // Map folders to depths as expected results
        HashMap<BookmarkId, Integer> idToDepth = new HashMap<BookmarkId, Integer>();

        List<BookmarkId> folderList = new ArrayList<BookmarkId>();
        List<Integer> depthList = new ArrayList<Integer>();

        mBookmarkBridge.getMoveDestinations(folderList, depthList, Arrays.asList(folderA));
        idToDepth.put(mMobileNode, 0);
        idToDepth.put(mDesktopNode, 0);
        idToDepth.put(folderB, 1);
        idToDepth.put(folderBA, 2);
        idToDepth.put(mOtherNode, 0);
        idToDepth.put(folderC, 1);
        verifyFolderDepths(folderList, depthList, idToDepth);

        mBookmarkBridge.getMoveDestinations(folderList, depthList, Arrays.asList(folderB));
        idToDepth.put(mMobileNode, 0);
        idToDepth.put(folderA, 1);
        idToDepth.put(folderAA, 2);
        idToDepth.put(folderAAA, 3);
        idToDepth.put(mDesktopNode, 0);
        idToDepth.put(mOtherNode, 0);
        idToDepth.put(folderC, 1);
        verifyFolderDepths(folderList, depthList, idToDepth);

        mBookmarkBridge.getMoveDestinations(folderList, depthList, Arrays.asList(folderC));
        idToDepth.put(mMobileNode, 0);
        idToDepth.put(folderA, 1);
        idToDepth.put(folderAA, 2);
        idToDepth.put(folderAAA, 3);
        idToDepth.put(mDesktopNode, 0);
        idToDepth.put(folderB, 1);
        idToDepth.put(folderBA, 2);
        idToDepth.put(mOtherNode, 0);
        verifyFolderDepths(folderList, depthList, idToDepth);

        mBookmarkBridge.getMoveDestinations(folderList, depthList, Arrays.asList(folderBA));
        idToDepth.put(mMobileNode, 0);
        idToDepth.put(folderA, 1);
        idToDepth.put(folderAA, 2);
        idToDepth.put(folderAAA, 3);
        idToDepth.put(mDesktopNode, 0);
        idToDepth.put(folderB, 1);
        idToDepth.put(mOtherNode, 0);
        idToDepth.put(folderC, 1);
        verifyFolderDepths(folderList, depthList, idToDepth);

        mBookmarkBridge.getMoveDestinations(folderList, depthList,
                Arrays.asList(folderAA, folderC));
        idToDepth.put(mMobileNode, 0);
        idToDepth.put(folderA, 1);
        idToDepth.put(mDesktopNode, 0);
        idToDepth.put(folderB, 1);
        idToDepth.put(folderBA, 2);
        idToDepth.put(mOtherNode, 0);
        verifyFolderDepths(folderList, depthList, idToDepth);
    }

    private void verifyFolderDepths(List<BookmarkId> folderList, List<Integer> depthList,
            HashMap<BookmarkId, Integer> idToDepth) {
        assertEquals(folderList.size(), depthList.size());
        assertEquals(folderList.size(), idToDepth.size());
        for (int i = 0; i < folderList.size(); i++) {
            BookmarkId folder = folderList.get(i);
            Integer depth = depthList.get(i);
            assertNotNull(folder);
            assertNotNull(depthList.get(i));
            assertTrue("Folder list contains non-folder elements: ",
                    mBookmarkBridge.getBookmarkById(folder).isFolder());
            assertTrue("Returned list contained unexpected key: ", idToDepth.containsKey(folder));
            assertEquals(idToDepth.get(folder), depth);
            idToDepth.remove(folder);
        }
        assertEquals(idToDepth.size(), 0);
        folderList.clear();
        depthList.clear();
    }
}