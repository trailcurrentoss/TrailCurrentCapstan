# Architecture

How Capstan is put together, and the rules that keep one source tree serving
three panels.

## Layers

```
        ┌──────────────────────────────────────────────┐
  GUI   │  GUI/Capstan{480,360,240}.eez-project        │
        │       ↓ EEZ Studio Build (Ctrl+B)            │
        │  main/ui/<res>/   generated, disposable      │
        └──────────────────────────────────────────────┘
                     ↕  actions.c / vars.c   (hand-written, in main/)
        ┌──────────────────────────────────────────────┐
  Model │  capstan_model   the values the UI observes  │
        └──────────────────────────────────────────────┘
             ↑                    ↑
        ┌─────────┐        ┌──────────────┐
  I/O   │ capstan │        │ capstan_wifi │   capstan_config (NVS)
        │  _mqtt  │        └──────────────┘
        └─────────┘
        ┌──────────────────────────────────────────────┐
  Board │  capstan_board   panel · touch · ring · LVGL │
        └──────────────────────────────────────────────┘
```

Data flows **up** — MQTT into the model, the model into the UI. Commands flow
**down** — an action publishes, and the UI changes only when the module
confirms. Nothing in the UI layer talks to MQTT directly.

## The rules

### 1. Board-specific code lives only in `capstan_board`

An `#ifdef CONFIG_CAPSTAN_BOARD_*` anywhere else means the abstraction is
missing something — add it to [`capstan_board.h`](../components/capstan_board/include/capstan_board.h)
instead. Everything above that header sees a display of a known size, a
touch indev, an encoder indev and a backlight, and does not know which panel
it is driving.

See [hardware.md](hardware.md) for what that component is hiding.

### 2. The ring reports DIRECTION, never position

This one shapes every screen, so it is a rule rather than a detail.

A rotary ring has no end stops. There is no "the encoder's value" — only what
the user just did with it. So the driver emits relative deltas
(`lv_indev_data_t::enc_diff`) and nothing anywhere keeps a running total.

The failure this avoids is dead travel. The vendor firmware on these boards
keeps an absolute `counter`: scroll past the end of a range, come back, and
nothing moves for as many detents as you overshot. The ring feels broken.

Concretely:

- The PCNT hardware counter is read and **immediately zeroed** on every read.
  It is a delta accumulator between reads, not a position.
- The sub-detent remainder is **discarded on direction reversal**, so a
  reversal takes effect on the very next detent instead of first cancelling
  out leftover travel.
- **Clamping is fine. Accumulated overshoot is not.** Turning past the end of
  a bounded range should simply stop doing anything — that is correct and
  expected. What must never happen is the overshoot being *stored*, so that
  reversing requires winding back through everything you overshot before the
  UI responds. Two full turns past the end of a list, then one detent back,
  must move the selection by one.
- The practical rule that guarantees it: **apply the delta to the displayed
  value and clamp the result**. Never keep a private counter and derive the
  displayed value from it — that is precisely how the overshoot gets stored.
- **Lists clamp. Carousels wrap.** A list of Wi-Fi networks or settings rows
  stops at its ends: reaching the last row and turning further does nothing,
  and the next detent the other way moves back immediately.

  The two carousels — the app menu and Devices — are the exception, and they
  follow the design prototype, which wraps. The reasoning that originally
  rejected wrapping — that silently jumping from the last item to the first is
  disorienting because nothing about the input tells you a boundary was
  crossed — holds for a list and does not hold here: a carousel shows the two
  neighbouring items and a row of dots, so the wrap is visible before and
  after it happens. On the menu it is also what puts Clock, the last item, one
  detent BACKWARDS from Climate, the first, which is the shortest path back to
  the clock face.

  **The test is the layout, not the screen.** A screen wraps when its
  neighbours are on the glass; if a third screen becomes a carousel it wraps
  too, and if Devices ever went back to being a list it would clamp again.

  Whichever applies, **the overshoot is never stored**. That is the invariant
  that matters, and it is independent of clamping versus wrapping.

Implemented in
[`board_encoder.c`](../components/capstan_board/src/board_encoder.c).

### 3. Touch is a per-screen policy, declared in one table

Touch and the ring press are the **same physical action** on this hardware —
the whole panel is the button. With both live, one press produces an encoder
`ENTER` *and* a touch event, and the press lands on whatever is under the
user's finger rather than on whatever the ring had focused.

So touch may need to be live on some screens (it was, for an on-device
keyboard that has since gone) and dead on the others. The wrong way to build that is to have
each screen enable touch on entry and remember to disable it on exit — one
missed exit path, one early return, one alert overlay stealing the
transition, and touch is silently live on a screen that fights it. That bug
is intermittent and miserable to find.

Instead:

- **The board layer owns the mechanism.** `capstan_board_touch_set_enabled()`
  lives there because that is where the LVGL indev handle is. It is not a
  policy decision and nothing calls it directly.
- **`ui_nav` owns the policy.** Every screen has one line in the table in
  [`ui_nav.c`](../main/ui_nav.c) declaring `RING_ONLY` or `RING_AND_TOUCH`,
  and `ui_nav_goto()` applies it on *every* transition — before the screen
  appears, so the first frame is already in the right mode.
- **A screen never decides.** It cannot forget to restore the setting,
  because it never sets it.

`ui_nav_goto()` is therefore the only supported way to change screens.
Calling `loadScreen()` or `lv_screen_load()` directly bypasses the policy and
leaves touch in whatever state the previous screen wanted.

Default to `RING_ONLY`. Today **every** screen is: the on-device keyboard is
gone (Wi-Fi and MQTT are set from a phone), and the Back chip that followed it
was removed in favour of the long press. With nothing touchable, the touch
controller is not even started — `CONFIG_CAPSTAN_TOUCH_ENABLED` is off by
default, which also saves its I2C bus, its RAM and, on the CrowPanels, the
interrupt that woke the LVGL task on every press of the glass. Bringing a touch
target back means turning that on and making the screen's row
`RING_AND_TOUCH` — a one-word edit in the table, with no screen code to
touch.

### 4. Rotation is decoded in hardware

PCNT, not GPIO interrupts. Every vendor demo uses an ISR per edge, which is
fine when the CPU is idle and drops counts when it is not — and the CPU is
busiest exactly while the user is turning the ring, because LVGL is
redrawing. PCNT counts in hardware and cannot miss a step.

Full 4× quadrature decode also absorbs contact bounce for free: a bouncing
edge oscillates between two adjacent Gray-code states and nets to zero.

### 5. Long press is consumed by the driver

A short press becomes `LV_KEY_ENTER` and reaches LVGL normally. A **long**
press does not reach LVGL at all — it is consumed by the driver and delivered
to the navigation layer as "back", matching the prototype, where a long press
returns to the menu from anywhere. A release that already fired the long
press is swallowed so it is not also seen as a confirm.

### 6. The idle timeout is driven by real input, not LVGL's timer

`capstan_board_ms_since_input()` is fed by the touch and encoder drivers.
LVGL's own inactivity timer resets on any redraw — including the idle clock's
own second hand, which would keep the display awake forever.

### 7. EEZ Studio owns appearance; C owns state

C may set **content** (`lv_label_set_text`), **state**
(`lv_obj_add_state(LV_STATE_CHECKED)`), **flags** (`LV_OBJ_FLAG_HIDDEN`),
**events**, and **which screen is loaded**.

C must never set geometry, alignment, size, fonts or colours on an
EEZ-authored widget. Doing so makes the device disagree with what EEZ
Studio's canvas shows, and that divergence is invisible until someone flashes
a board. Details in [gui.md](gui.md).

### 8. `main/ui/` is disposable

Delete the whole tree and re-export at any time. Nothing hand-written lives
there, which is why `actions.c` and `vars.c` sit in `main/`.

## Components

| Component | Responsibility |
|---|---|
| `capstan_board` | Panel, touch, ring, backlight, LVGL port. The only board-aware code. |
| `capstan_config` | NVS-backed settings: Wi-Fi credentials, broker details, units, theme. Owns the factory-reset path. |
| `capstan_wifi` | Scan, join, reconnect, and reporting link state to the model. |
| `capstan_mqtt` | Broker connection, subscriptions, JSON parsing, publishing commands, per-module staleness watchdogs. |
| `capstan_model` | The values the UI observes, and the `--`/stale semantics. |

### capstan_board internals

| File | Role |
|---|---|
| `capstan_board.c` | Public API, LVGL port setup, backlight, boot order |
| `board_display.c` | Display bring-up, three variants |
| `board_touch.c` | CST816-family touch |
| `board_encoder.c` | PCNT quadrature, button, long press |
| `panel_jd9855.c` | The one hand-written panel driver |
| `board_pins.h` | Three pin maps, every value sourced |

## Threading

| Task | Priority | Notes |
|---|---|---|
| LVGL | 4, core 1 | Created by `esp_lvgl_port`. Owns all `lv_*` calls. |
| MQTT dispatch | 5 | Above LVGL so broker traffic is drained promptly; parses **outside** the LVGL lock. |
| Wi-Fi / LWIP | IDF default, core 0 | Kept off core 1 so it does not collide with redraws. |

Two locking rules:

- Any `lv_*` call from a task other than LVGL's own must be wrapped in
  `capstan_board_lock()` / `capstan_board_unlock()`.
- Callbacks **invoked by** LVGL — `lv_timer` callbacks, event handlers —
  already hold the lock. Taking it again deadlocks.

The MQTT layer parses JSON outside the lock and takes it only around the
setter calls. Local traffic runs around 200 msg/s and the naive ordering
produced roughly ten seconds of UI lag on Fireside.

## Boot order

```
nvs_flash_init()
  └─ capstan_board_init()        panel, touch, ring, LVGL — backlight OFF
       └─ ui_init()  (or the bring-up test screen)
            └─ lv_timer → ui_tick()      ← without this, nothing ever updates
                 └─ capstan_board_backlight_set(100)
```

The backlight stays dark until the first screen has been drawn, so the panel
never shows an unpainted frame.

**`ui_tick()` will not call itself.** It is what reads every expression-bound
property through its `get_var_*` accessor, and on the device driving it is the
application's job. Without the timer the screen draws perfectly once and then
never changes again — which reads like a data-layer bug and is not. If a
screen "looks right but does nothing", check this first.

## Multi-resolution

EEZ Studio projects are single-resolution: `displayWidth`/`displayHeight` is
one fixed pair per `.eez-project`, with no breakpoint concept. So there are
three projects, generated from **one** script in `GUI/tmp/` that shares the
colour tokens, styles, screen definitions and font families and differs only
in canvas size, type scale, and the 240×240 content simplifications.

LVGL 9 grid layout with `FR()` tracks and percentage sizing does reflow at
runtime, and is used throughout — but it does not scale *contents*. Fonts,
arc widths and tick lengths are absolute pixels, which is why a 480×480
layout cannot simply be stretched to 240×240 and why the smallest panel gets
a smaller type scale.

The app menu used to be on that list and no longer is. A layout that had to
differ per panel was the wrong shape, not an unavoidable cost of the smallest
panel: the carousel shows three items however many there are, so one
description serves 240, 360 and 480.

See [gui.md](gui.md).
