# The GUI — EEZ Studio workflow

The screens are authored in [EEZ Studio](https://github.com/eez-open/studio)
and exported as C. This document covers how the three projects relate, where
output goes, and the rules that keep what you see in EEZ Studio matching what
appears on the board.

## Why three projects

An EEZ Studio project is **single-resolution**:
`settings.general.displayWidth` / `displayHeight` is one fixed pair per
`.eez-project`, and there is no breakpoint or multi-target concept.

LVGL 9 grid layout with `FR()` tracks and percentage sizing *does* reflow at
runtime, and it is used throughout — but it does not scale **contents**.
Fonts, arc widths, scale tick lengths and icon glyphs are absolute pixels. A
160 px numeral authored for 480×480 covers two thirds of a 240×240 panel.

Worse, one project would mean the canvas only ever shows one of the three
panels, so two of them would be shipping layouts nobody had previewed. That
is exactly the canvas-device divergence the rest of this document exists to
prevent.

So there are three projects:

| Project | Canvas | Export | Board that happens to use it today |
|---|---|---|---|
| `GUI/Capstan480.eez-project` | 480×480 | `main/ui/480/` | MaTouch 2.1" |
| `GUI/Capstan360.eez-project` | 360×360 | `main/ui/360/` | CrowPanel 1.46" |
| `GUI/Capstan240.eez-project` | 240×240 | `main/ui/240/` | CrowPanel 1.28" |

**The last column is incidental.** These projects are keyed by resolution,
not by board, and the one-to-one mapping today is only because the three
supported panels happen to be three different sizes. A project contains
nothing about a controller, a pin map or a vendor. Add a second 360×360
board and it reuses `Capstan360` as-is — there is no fourth project and
nothing to copy. Conversely, a change to how the 360 looks affects every
360×360 board, present and future, which is the intended behaviour.

The board-to-resolution decision lives in exactly one place,
`main/CMakeLists.txt`, which maps the selected board symbol to one variant
directory and compiles only that one.

**They are not maintained by hand.** One generator in `GUI/tmp/` emits all
three from a single description — shared colour tokens, styles, screen
structure and font families, differing only in canvas size, type scale, and
the 240×240 content simplifications. Editing one project by hand and not the
others is how they drift.

### What differs on the 240×240

Not a scaled-down 480. The content itself changes:

- **The app menu is NOT one of these.** It used to be — the 240 drew a
  scrolling list because nine icons on a ring need ~188 px of radius and a
  240 px panel has 120 px of half-width in total. The carousel that replaced
  the ring shows three items whatever the list length, so its geometry is
  identical on all three panels and the divergence is gone. See
  `page_menu()` in `GUI/tmp/screens_layout.py`.
- **Compact keyboard.** The prototype's 34×44 px keys give a 10-key row of
  340 px plus gaps. That does not fit in 240 px, and the round mask eats the
  corners on top of that.
- **Smaller type scale throughout**, and the hero numerals drop several
  steps.

## Where the output goes

`main/ui/<res>/` is **generated and disposable**. Delete the whole `main/ui`
tree at any time and re-export to recreate it.

Nothing hand-written lives there. `actions.c` and `vars.c` are in `main/`
precisely so that stays true — if hand-written code lived in the export
folder, deleting it would delete real source.

**One consequence worth stating:** EEZ Studio's Docker *full simulator*
copies only the export folder into its container and compiles what it finds
there, so it can only build a project whose hand-written C also lives in the
export folder. Keeping this tree purely generated means the full simulator is
not available for Capstan. EEZ Studio's canvas and its Run preview both still
work, and they are what validates layout anyway. On a three-variant project,
a disposable export tree is worth more than the simulator.

## The workflow

1. **Edit** `GUI/Capstan<res>.eez-project` — in EEZ Studio, or via a script
   in `GUI/tmp/`.
2. **If it was edited on disk, close and reopen the project in EEZ Studio.**
   EEZ Studio holds an in-memory copy from when the project was opened and
   does not reload a file changed underneath it. Skip this and Ctrl+B
   exports the *old* project — producing output that is genuinely newer than
   the file you edited, which defeats the obvious timestamp check and makes
   it look like your change did nothing.
3. **Ctrl+B** to export.
4. **`idf.py build`.**

Building before the export just produces `undefined reference to objects`
errors that have nothing to do with your C.

### A regenerated project invalidates EVERY existing export

This bit us once and will again. The sequence:

1. `main/ui/480/` held a valid export.
2. The generator switched the projects from Montserrat to the house Roboto
   and FontAwesome set.
3. The build failed with `'lv_font_montserrat_48' undeclared` — from
   `main/ui/480/styles.c`, a **generated** file nobody had touched.

The export was not stale relative to the *firmware*; it was stale relative
to the *project*. Generated C references fonts, styles and colour tokens by
name, so changing any of them in the `.eez-project` invalidates every
previously exported file that mentions them.

Two consequences worth internalising:

- **After regenerating, treat `main/ui/` as empty** whether or not it is.
  Delete the affected variant's folder and re-export. It is disposable by
  design, and deleting is cheaper than reasoning about which files went
  stale.
- **All three variants go stale together**, because the generator emits all
  three from one description. Exporting only the 480 leaves the 360 and 240
  broken in exactly this way, and nothing will tell you until you build for
  one of those boards.

The failure is at least loud — a compile error in generated code naming a
symbol nobody wrote. `main/CMakeLists.txt` also refuses to build when
`ui.h` exists but no generated sources do, which catches the stale-glob
version of the same problem.

### Scripted edits

Scripts in `GUI/tmp/` (gitignored — they are local working state, not source)
should: back up the `.eez-project` with a timestamped suffix first, mutate
idempotently, and write back with 2-space indent and a trailing newline to
match EEZ Studio's own format.

## Bring-up scaffolding, and where the line is

`main/main.c` contains hand-written LVGL screens. They are compiled **only
when no EEZ Studio export exists**:

```c
#if __has_include("ui.h")
    ui_init();              /* the real UI */
#else
    build_test_screen();    /* scaffolding */
#endif
```

That is deliberate and temporary: it brought the panel, the ring and touch
up before any `.eez-project` existed, and it vanishes from the binary the
moment one is exported. A bring-up jig is not product UI.

**The touch-calibration screen currently in there is the exception worth
watching.** Unlike the test screen it is a real product feature -- it has to
be re-runnable from Settings on a shipped unit. Hand-written in C it would
be invisible in EEZ Studio forever, which is the divergence this whole
document exists to prevent.

So when the projects are authored, it moves:

| Layer | Owns |
|---|---|
| EEZ Studio | the screen: five crosshair widgets at the five fixed target positions, title, progress label, result label |
| C | toggling `LV_OBJ_FLAG_HIDDEN` on the active crosshair, setting label text, running the fit |

That works because the targets are at **fixed, known** positions, so they
can be authored rather than placed at runtime. If a single crosshair had to
move around, C would have to call `lv_obj_set_pos()` on an EEZ widget --
the forbidden case.

The calibration **engine** is not UI and stays in C regardless:
`components/capstan_board/src/capstan_touch_cal.c` holds the least-squares
fit, the sanity gate and the NVS round-trip, and the correction is applied
between the touch driver and LVGL so no screen knows it exists.

The general test: **if end users will see it, developers must be able to see
it in EEZ Studio.** A shipped screen has to match the brand colours, the
type scale and the theme, and all three are defined in the project — a
hand-written screen duplicates them as hex literals that then drift, and
nobody can review a layout that exists only in C.

If it only exists to bring hardware up, it lives behind the `__has_include`
guard and is deleted at first export. "Behind a guard" justifies a jig; it
does not justify a screen with users.

## The rule: the canvas must match the device

When someone opens EEZ Studio, the canvas must show what is going to render
on the hardware. Two distinct things break that, and both are prohibited.

### C code must not set geometry

EEZ Studio's canvas renders from the JSON. It does not run your C. The device
runs both. So anything your C overrides is something the canvas cannot show.

**Never** call any of these on an `objects.<widget>` symbol:

```
lv_obj_set_pos / set_x / set_y          lv_obj_set_size / set_width / set_height
lv_obj_align / align_to / set_align     lv_obj_center
lv_obj_move_foreground / _background    lv_obj_set_style_align
lv_obj_set_style_min_* / max_*          lv_obj_set_style_text_font
lv_obj_set_style_*_color                lv_obj_set_style_translate_*
```

**Always fine**, because they change content or invisible runtime state:

```
lv_label_set_text, lv_textarea_set_text, lv_slider_set_value, lv_arc_set_value
lv_obj_add_state / clear_state      (LV_STATE_CHECKED, LV_STATE_DISABLED …)
lv_obj_add_flag / clear_flag        (LV_OBJ_FLAG_HIDDEN …)
lv_obj_add_event_cb
loadScreen
```

The discriminator: **EEZ Studio controls appearance, C controls state.** If a
state change does not look right, the fix belongs in EEZ Studio — not in a
`lv_obj_set_style_*` call.

### Percent positions are exported as whole numbers

Geometry here is in percent, so one description lays out at 480, 360 and 240.
There is one place that breaks down: **EEZ Studio's export rounds a percent
position to a whole number.** A widget authored at 30.417% ships as
`LV_PCT(30)`.

One percent is 2.4 px on a 240 panel, 4.8 px on a 480. That is invisible for a
title or a card, and destructive for anything small and repeated. The page-dot
rows were the case that found it: authored on a true circle 6 degrees apart,
they came out of the export with gaps alternating 9.6 px and 12 px, and the
arc's vertical sag — a fraction of a pixel between neighbours near the centre —
quantised into 2.4 px stair-steps. Nothing was wrong with the description, the
canvas, or the C. The unit was too coarse to carry the curve.

So a widget whose position needs sub-percent precision is authored in
**pixels**, which costs nothing here because the generator emits one project
per resolution and already knows the canvas size. `dot()` in
`GUI/tmp/layout.py` is the one current user; it takes a centre in percent and
converts once, because rounding a top-left that was itself derived from a
percent size rounds twice.

The generator's geometry checks read both units — see `as_pct()` in
`validate()`. A pixel-positioned widget that skipped the round-mask check
would be a bad trade, since the widgets most likely to be authored in pixels
are the small ones near the rim.

### Some JSON shapes are silently dropped

EEZ Studio's C generator is more forgiving than its own canvas renderer. A
widget with a missing required field or an unknown key can compile through to
the device and render there, while EEZ Studio silently drops it from the
canvas — no error, it simply is not drawn.

The two strictest cases:

- **`LVGLScreenWidget`** (a page root). Adding `useStyle` makes the page
  vanish from the Pages list entirely.
- **`LVGLUserWidgetWidget`** (a user-widget instance). Omitting
  `userPropertyValues` makes EEZ Studio refuse to load it.

Both fail with no diagnostic. When a change does not appear in the canvas,
check the JSON shape against a widget already rendering correctly rather than
trying another shape and hoping.

## Fonts

Built-in Montserrat sizes are referenced as `MONTSERRAT_<n>` (uppercase) and
must each have `CONFIG_LV_FONT_MONTSERRAT_<n>=y` in `sdkconfig.defaults`.
Custom families — the large numerals and the icon font — are lowercase,
listed in the project's `fonts[]`, embedded in the `.eez-project`, and
emitted as `ui_font_<name>.c` by the export.

Subset aggressively. Only the glyph ranges actually used (digits, `°`, `F`,
`C`, `.`, `-`, `:`) should be included, or a 160 px face costs far more flash
than it needs to. Every icon codepoint referenced in a label must be in that
font's range or it renders as an empty box — with no build warning.

LVGL's built-in Montserrat subset does **not** include `—`, `–`, `…`, curly
quotes or `·`. Use ASCII in label text.

## Version pinning

`settings.general.lvglVersion` in every project must match the
`lvgl/lvgl` pin in `main/idf_component.yml` (**9.2.2**) exactly. EEZ Studio
emits different API calls per version — the Spangroup API changes shape
between 9.2 and 9.3 — and a mismatch fails in generated code you are not
allowed to edit, pointing at a symbol nobody wrote. Bump both together, along
with `esp_lvgl_port`.

## See also

- [architecture.md](architecture.md) — where the UI sits, and the input rules
  every screen must obey
- [screens.md](screens.md) — what each screen does
- [icons.md](icons.md) — the icon font and its codepoints
