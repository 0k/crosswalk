// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Copyright (c) 2013 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.xwalk.runtime.client.embedded.test;

import android.test.suitebuilder.annotation.SmallTest;
import org.chromium.base.test.util.Feature;

/**
 * Test suite for loadAppFromManifest().
 */
public class LoadAppFromManifestTest extends XWalkRuntimeClientTestBase {

    @SmallTest
    @Feature({"LoadAppFromManifest"})
    public void testLoadAppFromManifest() throws Throwable {
        String expectedTitle = "Crosswalk Sample Application";

        getUtilInterface().loadManifestSync("file:///android_asset/manifest.json");

        String title = getUtilInterface().getRuntimeView().getTitleForTest();
        assertEquals(expectedTitle, title);
    }
}
