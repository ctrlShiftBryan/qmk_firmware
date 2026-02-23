/* Copyright 2024 */
/* SPDX-License-Identifier: GPL-2.0-or-later */

#pragma once

// Mod-tap configuration matching ZMK behavior
#define TAPPING_TERM 250
#define QUICK_TAP_TERM 150

// Prefer tap over hold for mod-tap keys (matches ZMK "tap-preferred")
#define TAPPING_FORCE_HOLD_PER_KEY

// Enable permissive hold to improve mod-tap responsiveness
#define PERMISSIVE_HOLD

// One Shot Keys configuration (for sticky shift)
#define ONESHOT_TIMEOUT 3000

// Per-key tapping term support
#define TAPPING_TERM_PER_KEY
