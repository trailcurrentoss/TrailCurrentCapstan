/*
 * EEZ Studio variable accessors.
 *
 * Expression-bound widget properties read through these on every ui_tick().
 * Hand-written, and deliberately in main/ rather than main/ui/, which is
 * disposable.
 *
 * Keep these cheap: ui_tick() calls every one of them at the UI frame rate.
 * They should read an already-updated value out of the data model, never
 * parse, allocate or block.
 */

/* From main/CMakeLists.txt. Never __has_include -- it records no
 * dependency on an absent file, so a unit compiled before the first EEZ
 * Studio export is never rebuilt when the export arrives. See main.c. */
#ifndef CAPSTAN_HAVE_UI
#  error "CAPSTAN_HAVE_UI is not defined -- main/CMakeLists.txt must set it"
#endif

#if CAPSTAN_HAVE_UI

#include "vars.h"

/* Accessors land here as the data model is built out. */

#else
typedef int capstan_vars_placeholder;
#endif
