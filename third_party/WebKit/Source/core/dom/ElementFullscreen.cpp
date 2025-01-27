// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/dom/ElementFullscreen.h"

#include "core/dom/Fullscreen.h"

namespace blink {

void ElementFullscreen::requestFullscreen(Element& element, const FullscreenOptions& options)
{
    Fullscreen::from(element.document()).requestFullscreen(element, Fullscreen::UnprefixedRequest, options);
}

void ElementFullscreen::webkitRequestFullscreen(Element& element, const FullscreenOptions& options)
{
    Fullscreen::from(element.document()).requestFullscreen(element, Fullscreen::PrefixedRequest, options);
}

} // namespace blink
