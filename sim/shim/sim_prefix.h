/*
 * Force-included at the top of every file scripts/sim_stage.sh copies into
 * the simulator build. See docs/simulator.md.
 *
 * The firmware gets CAPSTAN_HAVE_UI from main/CMakeLists.txt as a compiler
 * flag. The EEZ Studio simulator's CMakeLists is not ours and takes no extra
 * flags, so the define has to arrive in the source instead -- and it has to
 * arrive before the `#error` guards at the top of vars.c and friends, which is
 * why this is prepended rather than included from a shim header.
 */
#pragma once

#ifndef EEZ_LVGL_SIMULATOR
#  error "sim_prefix.h is for the EEZ Studio simulator build only"
#endif

/* The simulator only ever builds from an export, so the UI is always there. */
#define CAPSTAN_HAVE_UI 1

#include "sdkconfig.h"
