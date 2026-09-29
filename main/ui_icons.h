/*
 * Icon glyphs the firmware writes into labels at runtime.
 *
 * WHY THIS FILE EXISTS
 *
 * Almost every icon on this device is authored in EEZ Studio, where the
 * codepoint is baked into the label's text and C never touches it. The app
 * carousel is the exception: it shows three items out of ten through three
 * fixed slots, so rotating the ring has to REWRITE the glyph in each slot.
 * That is content, not layout -- see docs/gui.md -- but it does mean the
 * codepoints have to exist in C as well as in the project file.
 *
 * TWO CONSTRAINTS ON ANYTHING ADDED HERE
 *
 * 1. The codepoint must match `FA` in GUI/tmp/layout.py. A mismatch shows
 *    the wrong icon rather than failing, so gen_eez_project.py parses this
 *    file and refuses to emit anything if the two disagree.
 *
 * 2. The codepoint must be in `FH_RANGE` in GUI/tmp/fonts.py, because the
 *    carousel's slots are styled with the reduced `fh` face. A glyph outside
 *    that subset renders as an empty box with no warning from anything.
 *
 * Written as explicit UTF-8 bytes rather than as \u escapes: LVGL takes a
 * const char *, and the byte sequence is what a hex dump of the export
 * shows, which is what you end up comparing when an icon comes out wrong.
 */
#pragma once

/* clang-format off */
#define UI_ICON_CLIMATE   "\xEF\x8B\x87"   /* 0xF2C7 thermometer-half */
#define UI_ICON_DEVICES   "\xEF\x83\xAB"   /* 0xF0EB lightbulb        */
#define UI_ICON_ENERGY    "\xEF\x83\xA7"   /* 0xF0E7 bolt             */
#define UI_ICON_WATER     "\xEF\x81\x83"   /* 0xF043 droplet          */
#define UI_ICON_AIR       "\xEF\x83\x82"   /* 0xF0C2 cloud            */
#define UI_ICON_LEVEL     "\xEF\x8F\xBD"   /* 0xF3FD gauge            */
#define UI_ICON_SETTINGS  "\xEF\x80\x93"   /* 0xF013 gear             */
#define UI_ICON_CLOCK     "\xEF\x80\x97"   /* 0xF017 clock            */
/* clang-format on */
