/*
 *  Copyright (C) 2005-2021 Team Kodi <https://kodi.tv>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */
#pragma once

// Dependency-free Tracy profiler instrumentation shim.
//
// When TRACY_ENABLE is defined AND the tracy headers are available on the
// include path (e.g. via -DENABLE_TRACY=ON, which adds tracy/ to the include
// path), the Tracy macros are pulled in and zones are recorded. Otherwise the
// macros expand to nothing, so the source can be instrumented unconditionally
// without forcing a tracy dependency on regular builds.
//
// Usage:
//   #include "tracyprofiler.h"
//   void Render() {
//     TRACY_ZONE("Render");
//     ...
//   }
//
// Notes:
//   - Zones are scope-based via a local variable; do not place a TRACY_ZONE
//     macro on a line that also declares other variables.
//   - With TRACY_ON_DEMAND (set by the CMake option of the same name) zones
//     are only collected when a profiler connection is active, minimizing
//     runtime overhead in production builds.

#if defined(TRACY_ENABLE)
#  include <tracy/Tracy.hpp>
#  define TRACY_ZONE(name) ZoneScopedN(name)
#  define TRACY_FRAME() FrameMark
#  define TRACY_TEXT(str, len) ZoneText(str, len)
#else
#  define TRACY_ZONE(name) ((void)0)
#  define TRACY_FRAME() ((void)0)
#  define TRACY_TEXT(str, len) ((void)0)
#endif
