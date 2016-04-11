// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FullscreenDisplay_h
#define FullscreenDisplay_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "core/CoreExport.h"
#include "core/dom/Element.h"
#include "platform/heap/Handle.h"

namespace blink {

class CORE_EXPORT FullscreenDisplay : public GarbageCollected<FullscreenDisplay>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    virtual void didEnterFullScreenForElement(Element* element) {};
    virtual void exitFullscreen() {};

    DECLARE_VIRTUAL_TRACE();
};

} // namespace blink

#endif // FullscreenDisplay_h
