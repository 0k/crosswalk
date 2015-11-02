// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.xwalk.core.internal;

import android.util.ArrayMap;

import org.chromium.base.CalledByNative;
import org.chromium.base.JNINamespace;

/**
 * Delegate for handling callbacks. All methods are called on the IO thread.
 */
@JNINamespace("xwalk")
public abstract class XWalkContentsIoThreadClient {
    @CalledByNative
    public abstract int getCacheMode();

    @CalledByNative
    public abstract boolean shouldBlockContentUrls();

    @CalledByNative
    public abstract boolean shouldBlockFileUrls();

    @CalledByNative
    public abstract boolean shouldBlockNetworkLoads();

    @CalledByNative
    public abstract void onDownloadStart(String url,
                                     String userAgent,
                                     String contentDisposition,
                                     String mimeType,
                                     long contentLength);

    @CalledByNative
    public abstract void newLoginRequest(String realm, String account, String args);

    public abstract XWalkWebResourceResponse shouldInterceptRequest(
        XWalkContentsClient.XWalkWebResourceRequest request);

    // Protected methods ---------------------------------------------------------------------------
    @CalledByNative
    protected XWalkWebResourceResponse shouldInterceptRequest(String url, boolean isMainFrame,
            boolean hasUserGesture, String method, String[] requestHeaderNames,
            String[] requestHeaderValues) {
        XWalkContentsClient.XWalkWebResourceRequest request =
            new XWalkContentsClient.XWalkWebResourceRequest();
        request.url = url;
        request.isMainFrame = isMainFrame;
        request.hasUserGesture = hasUserGesture;
        request.method = method;
        request.requestHeaders = new ArrayMap<String, String>(requestHeaderNames.length);
        for (int i = 0; i < requestHeaderNames.length; ++i) {
            request.requestHeaders.put(requestHeaderNames[i], requestHeaderValues[i]);
        }
        return shouldInterceptRequest(request);
    }
}
