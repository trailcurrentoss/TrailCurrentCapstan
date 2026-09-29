# Capstan ↔ Headwaters MQTT contract

What the rotary display subscribes to, what it publishes, and which parts of
the prototype have no backing topic yet.

Everything here was read out of the shipped Headwaters code, not from the
platform documentation. Where the two disagree, this file follows the code and
says so.

> **`TrailCurrentDocumentation/10_Reference/MQTT_TOPICS.md` is partly stale.**
> Its entire `tc/…` hierarchy does not exist anywhere in Headwaters, and the
> `local/thermostat/*` rows it lists are aspirational — they appear in neither
> `containers/backend/src/mqtt.js` nor the `TOPIC_MAP` in
> `containers/backend/src/services/cloud-bridge.js`. Do not implement against
> it without checking the code first.

## Connecting

The in-rig broker is Mosquitto, run as its own container
(`containers/mosquitto/`, `docker-compose.yml`).

| | |
|---|---|
| Transport | **TLS only** |
| Port | **8883** |
| Plaintext 1883 | **does not exist** — no listener is configured |
| Anonymous | refused (`allow_anonymous false`) |
| Auth | username + password, required |
| Certificate | self-signed |

`config/mosquitto/mosquitto.conf` binds `listener 8883` and nothing else. The
prototype's settings screen captions the port field "Default 1883 · TLS 8883";
that is backwards for this platform, and Capstan defaults to 8883
(`CONFIG_CAPSTAN_MQTT_DEFAULT_PORT`).

### Broker address

The backend's own `MQTT_BROKER_URL` is `mqtts://mosquitto:8883`, where
`mosquitto` is a Docker-internal hostname. **A device on the LAN cannot resolve
it.** `containers/backend/src/routes/discovery.js` rewrites it for LAN clients,
preferring in order: `HEADWATERS_LAN_HOST`, then `TLS_CERT_HOSTNAME` (by
convention the rig's mDNS name), then `${os.hostname()}.local`.

So a display connects to `mqtts://<rig-hostname>.local:8883`, and the host is
entered by the user on the settings screen and stored in NVS. Nothing is
hardcoded.

### Certificate handling

No CA is provisioned on the device, so the certificate is not verified:
`CONFIG_ESP_TLS_INSECURE=y` and `CONFIG_ESP_TLS_SKIP_SERVER_CERT_VERIFY=y` in
`sdkconfig.defaults`, plus `.broker.verification.skip_cert_common_name_check =
true` on the client config.

Without those flags the handshake fails with mbedtls `0x8017` — *"No server
verification option set in esp_tls_cfg_t structure"* — which reads like a
broker or network fault and is neither. Fireside lost time to exactly this.

### Client identity

| | |
|---|---|
| Client ID | `tc-capstan-<12-hex-mac>` — the **full** 6-byte STA MAC |
| mDNS hostname | `esp32-XXXXXX` — last 3 MAC bytes, uppercase hex |
| LWT topic | `local/capstan/<12-hex-mac>/status` |
| LWT payload | `offline`, QoS 1, **retained** |
| On connect | publish `online` to the same topic, QoS 1, retained |

The full MAC in the client ID is not optional. A shortened 2-byte form caused
broker session eviction and connection flapping when two boards collided;
Fireside's `mqtt_client.c` carries the comment. With several dials in one rig
this is a live risk, not a theoretical one.

### Tuning that is already paid for

Copy these from Fireside rather than rediscovering them:

- `.session.keepalive = 60` and `.network.timeout_ms = 20000`. The IDF defaults
  produce spurious *"No PING_RESP, disconnected"* drops when Wi-Fi degrades.
- `.buffer.size = 1024`.
- Parse JSON **outside** the LVGL lock; take the lock only around the setter
  calls. Local traffic runs around 200 msg/s and the naive ordering produced
  roughly ten seconds of UI lag.
- Publishing takes a bounded 100 ms semaphore, never `portMAX_DELAY` — the
  unbounded form deadlocks against a concurrent reconnect.

## Namespaces

| Prefix | Broker | Use |
|---|---|---|
| `local/…` | the in-rig Mosquitto | **everything Capstan touches** |
| `rv/…` | the Farwatch **cloud** broker | remote mirror — not on the rig broker |
| `can/inbound`, `can/outbound` | rig broker | raw CAN frame envelopes |

Do not subscribe to `rv/*`. It is not present on the broker the display
connects to.

## Nothing is retained

`containers/backend/src/services/can-bridge.js` publishes **every** sensor and
status topic **without the retain flag**. Only `local/config/*`,
`local/mode/current`, `local/playbill/…/status`, `os/timezone/current` and
device LWTs are retained.

A freshly-booted display therefore shows nothing until the next periodic frame
arrives. The `--` empty-value convention is not decoration — it is the boot
state, and it is also what a stale module looks like.

Publish cadences and the staleness timeouts Fireside settled on:

| Module | Topic | Cadence | Stale after |
|---|---|---|---|
| Ampline | `local/energy/status` | 1 s | 10 s |
| Borealis | `local/airquality/*` | 2 s | 20 s |
| Milepost | `local/gps/*` | ~1 Hz | 15 s |
| Reservoir | `local/water/status` | 1 s | 10 s |
| Plateau | `local/level/*` | 2 Hz | 5 s |

On timeout, clear the affected values back to `--` rather than leaving the last
reading on screen. A stale number that looks live is worse than no number.

## Subscriptions

```
local/energy/status
local/airquality/temphumid
local/airquality/status
local/airquality/safety
local/water/status
local/level/tilt
local/level/corners
local/level/status
local/lights/+/status
local/relays/+/status
local/picket/+/inputs
local/spoor/+/inputs
local/config/pdm_channels        (retained — friendly names and icons)
local/config/relay_channels      (retained)
local/config/capstan/<esp32-XXXXXX>/controls
                                 (retained — this dial's device controls)
local/config/panel/<esp32-XXXXXX>/alarms
                                 (retained — this panel's alarms)
local/mode/current               (retained — camping | driving | storage)
local/gps/time                   (clock fallback when there is no SNTP)
os/timezone/current              (retained — IANA TZ string)
local/discovery/trigger
local/ota/trigger
```

## Payloads

Field names are exact, taken from `can-bridge.js`.

**`local/energy/status`** — accumulated from three CAN frames, so fields arrive
incrementally. **Treat every field as optional on every message.**

```json
{ "battery_voltage": 13.42,
  "battery_percent": 87.5,
  "consumption_watts": 220,
  "battery_watts": -220,
  "time_remaining_minutes": 430,
  "solar_watts": 640,
  "charge_type": "off|fault|bulk|absorption|float|equalize|unknown" }
```

`time_remaining_minutes` is omitted when it is 0 or 0xFFFF. It is never
cleared, so it goes stale while charging: the dial shows "Charging" instead
when `battery_watts` is positive, and the reported figure otherwise (an older
Headwaters without `battery_watts` cannot say it is charging).

**Watt signs.** `battery_watts` is the SmartShunt's `P`, relayed by Solstice on
CAN 0x024 as a sign byte (0xFF = negative) and magnitude. Victron's convention:
**positive = battery charging, negative = discharging.** `consumption_watts` is
the older, unsigned form -- the draw while discharging and 0 otherwise -- so it
cannot tell charging from idle. `solar_watts` is the MPPT's panel input (PPV),
always >= 0.

There is no per-device load measurement. The Energy screen's Loads page derives
total load as `solar_watts - battery_watts` (what the prototype's "Net" line
assumes), which is exact while solar is the only charge source; with shore
power or an alternator charging, that figure under-reads, and a negative result
is shown as `--`.

**`local/airquality/temphumid`** — the only ambient temperature on the bus.

```json
{ "tempInC": 21, "tempInF": 70, "humidity": 43.21 }
```

**`local/airquality/status`**

```json
{ "tvoc_ppb": 120, "eco2_ppm": 640 }
```

**`local/airquality/safety`** — thresholds are evaluated on-board Borealis.
Use the booleans; do not re-derive them from the ppm values.

`co2_warn` / `co2_alarm` / `voc_alarm` / `co_warn` / `co_alarm` are what drive
the Air quality screen's status word and ring colour — see
[screens.md](screens.md#air-quality). A payload that omits one reads as
`false`, i.e. "not flagged", which is the right answer for an older Borealis
that does not publish it.

```json
{ "co_ppm": 12, "lpg_rs_r0": 0.812, "alarm_flags": 0,
  "co_warn": false, "co_alarm": false,
  "lpg_warn": false, "lpg_alarm": false,
  "co2_warn": false, "co2_alarm": false, "voc_alarm": false }
```

**`local/water/status`** — percent, integers.

```json
{ "fresh": 72, "grey": 30, "black": 12 }
```

**`local/level/tilt`** — degrees, signed, 0.01° resolution.

```json
{ "front_back": -1.23, "side_to_side": 0.40,
  "front_back_diff_mm": -35, "left_right_diff_mm": 12 }
```

**`local/level/corners`** — lift required per wheel, mm, lowest corner
normalised to 0.

```json
{ "front_left_mm": 0, "front_right_mm": 12,
  "rear_left_mm": 40, "rear_right_mm": 28 }
```

**`local/level/status`**

```json
{ "imu_connected": true, "fully_calibrated": false,
  "cal_sys": 0, "cal_gyro": 0, "cal_accel": 0, "cal_mag": 0,
  "mounting": 0 }
```

**`local/lights/<1..24>/status`** — `state` is `brightness > 0`.

```json
{ "state": 1, "brightness": 255 }
```

**`local/relays/<1..24>/status`** — relay *n* appears as light id `100 + n` in
the PWA's unified view (`SWITCHBACK_ID_BASE = 100`).

```json
{ "state": 1 }
```

**Rate.** Headwaters broadcasts current state continuously, by design, so a
change made on Milepost or Overlook reaches every panel: every relay is
republished on every Switchback status frame, about 25 times a second — some
200 messages a second for 8 relays, nearly all unchanged. Capstan drops an
exact repeat of the last payload on the same `local/lights/*` or
`local/relays/*` topic in the MQTT event handler, before it is queued
(`is_repeat()` in `capstan_mqtt.c`). A different state always passes, at
once; the comparison is byte for byte, and the table is cleared on every
connect. Measured on the 1.28": ~204 of ~335 messages a second skipped, and
queue overflows fell from about 200 a minute to none — the overflow could
discard the one change a user was waiting for. Other topics are untouched:
their repeats are what keep a reading from going stale.

**`local/picket/<0..7>/inputs`** — reed switches. A set bit means open/active.

```json
{ "addr": 0, "inputs": 4095 }
```

**`local/spoor/<0..2>/inputs`** — Switchback digital inputs, 8-bit mask.

```json
{ "addr": 0, "inputs": 255 }
```

**`local/config/pdm_channels`** / **`local/config/relay_channels`** — retained.
This is how the display gets friendly names and icons without the REST API.
Publish anything to `local/config/request` to force a re-publish.

```json
{ "channels": [ /* … */ ] }
```

**`local/config/capstan/<esp32-XXXXXX>/controls`** — retained, per device.

Which Torrent (PDM) channels and Switchback relays *this* dial may switch.
Configured in the Headwaters PWA under **Settings → Network & Modules**, on the
Capstan's own Edit dialog, and published by
`containers/backend/src/services/capstan-control-sync.js`.

```json
{ "controls": [ { "id": 3,   "name": "Kitchen",    "icon": "lightbulb" },
                { "id": 104, "name": "Water Pump", "icon": "power-outlet" } ] }
```

`id` is the unified light id the display already commands with
`local/lights/<id>/command`: PDM channels are 1..N, Switchback relays start at
100. So the source never has to be special-cased — though it is still
recoverable, and `id >= 100` is exactly the toggle-only case described under
[Commands](#commands).

The topic is keyed by mDNS hostname, not MAC, because the hostname is what
Headwaters stores against the module. Two dials in one rig therefore carry
different lists, which is the point of the feature.

Three things about this payload are load-bearing:

- **The name and icon are resolved by Headwaters, not chosen for the dial.**
  The PWA stores only a reference — source, module hostname, channel — and the
  backend derives the id, label and icon at publish time from the channel's own
  configuration. Rename a PDM channel and the dial follows, with no second
  place to edit.
- **An empty array is a payload, not an absence.** It is what a Capstan that was
  deleted or disabled in the PWA is sent, and it clears the stored list.
- **It is stored in NVS.** The dial comes up with labelled controls before the
  broker is reachable, for the same reason the Wi-Fi and broker settings live
  there. The retained topic remains the authority and overwrites the copy;
  `capstan_config_set_controls()` skips the flash write when nothing changed,
  which matters because the retained message is redelivered on every connect.

At most `CAPSTAN_MAX_CONTROLS` (8) entries are kept. That ceiling is the
message buffer, not the screen — see the sizing note on `MSG_DATA_MAX` in
`components/capstan_mqtt/src/capstan_mqtt.c`. It is mirrored in the backend
(`MAX_CONTROLS`) and the PWA (`MAX_CAPSTAN_CONTROLS`); all three move together.

**`local/mode/current`** — retained.

What the rig is currently doing. Set from the PWA's segmented control and
persisted in `system_config.mode`.

```json
{ "mode": "camping" }
```

This used to live only in the PWA's `localStorage`, which made it a
per-browser view preference. It is rig state now because panels act on it —
see the alarms topic below. An unknown mode string leaves the panel on
whatever it had, rather than falling back to `camping`: a newer Headwaters
adding a fourth mode must not silently re-interpret every alarm on an older
panel.

**`local/config/panel/<esp32-XXXXXX>/alarms`** — retained, per device.

Which Picket reed switches and Switchback digital inputs this panel watches,
and **what each one means in each mode**.

```json
{ "alarms": [ { "key": "picket:0:3", "name": "Fridge", "icon": "snowflake",
                "modes": { "camping": "none",
                           "driving": "low",
                           "storage": "high" } } ] }
```

Keyed on `panel`, not `capstan`: Milepost and Fireside are the same kind of
consumer and should adopt this contract rather than each growing its own. (The
controls topic is still capstan-scoped and should migrate when they do.)

### Nothing on the bus is an alarm

Picket publishes a bitmask. Torrent publishes channel state. Whether a given
status event *constitutes* an alarm is an interpretation, and the
interpretation differs by panel and by mode — which is why this is a verdict
per mode rather than an armed flag plus a polarity.

The fridge above is the worked example:

| Mode | Verdict | Why |
|---|---|---|
| `storage` | `high` | Voltage present. Something switched it on. |
| `driving` | `low` | Voltage absent. It lost power on the road. |
| `camping` | `none` | Neither. It is supposed to cycle. |

And the kitchen panel can watch a sensor that the dial by the bed leaves off its
list entirely. That is the point of the config being per panel.

| Verdict | Meaning |
|---|---|
| `none` | This mode does not care. Not quite "disarmed" — the sensor is usually live in another mode. |

An alarm whose verdict is `none` in **every** mode is treated as `high` in every
mode — armed, alarming while the input is asserted — which is what arming a
sensor means on Headwaters and Milepost. Ignoring an alarm everywhere does
nothing useful (leave the sensor off the panel's list instead), and it is what
the PWA used to save by default.
| `high` | Alarm while the input is asserted. |
| `low` | Alarm while the input is not asserted. |

Every mode is spelled out, including the `none` ones. The panel should not
have to infer a missing key, and a payload whose size depends on how many
modes happen to be configured is a payload that fits in testing and overflows
in the field. A mode the payload omits, or a verdict this firmware does not
recognise, resolves to `none` — quiet. The alternative is an alarm that cannot
be switched off from the PWA because the PWA does not believe it exists.

`capstan_alarm_is_active()` in `capstan_config.h` is the only place the rule
is implemented; callers pass the raw input word and the current mode, and
never shift or mask it themselves.

### Why the source is always a digital input

Never a relay or PDM channel's reported state. **A relay can report ON while a
failed contact passes no voltage** — which is exactly the fault the alarm
exists to catch. A Picket DI wired to the load's supply sees what the load
sees, so it stays correct when the relay lies.

Do not "improve" this by adding `local/lights/<id>/status` as an alarm source.
That alarms on commanded state rather than on reality, and it would have
missed the failure the sense line was installed for.

### Keys and sizing

`key` is `<source>:<addr>:<sensor>` — byte-for-byte the identifier
`system_config.alarms.sensors` uses in Mongo and the PWA uses in **Settings →
Alarms**, so one string names one input across the whole platform.

**Sensor numbers are 1-based**, as everywhere else on the platform. The input
bitmasks are 0-based, so the bit to test is `sensor - 1`:

| `key` source | Input topic | Bits | Sensors |
|---|---|---|---|
| `picket` | `local/picket/<addr>/inputs` | 12 | 1..12 |
| `switchback` | `local/spoor/<addr>/inputs` | 8 | 1..8 |

Note a `switchback` alarm reads the **`spoor`** topic. The two names are not
interchangeable anywhere else, and this is the one place they meet.

Unlike a control, the **name and icon are carried in the payload rather than
derived**. A control points at a Torrent or Switchback channel that already
has both; a sensor has neither — `system_config.alarms.sensors` holds an armed
flag and a name and nothing else. The PWA seeds the name from the rig-wide one
when you pick a sensor, and the entry owns it from then on.

Stored in NVS, replaced whole on every message, and an empty array disarms the
panel. At most `CAPSTAN_MAX_ALARMS` — **six**, not eight like the controls.
A worst-case entry (longest key, 24-character name, 16-character icon, all
three modes spelled out) is 149 bytes, so six plus the wrapper is 906, inside
`MSG_DATA_MAX`. Eight would be 1204, and an oversized message is dropped
whole: the panel would have no alarms at all and one log line saying so. If
six is too few, the lever is `MSG_QUEUE_DEPTH` against `MSG_DATA_MAX` in
`capstan_mqtt.c`, which are deliberately traded against each other there.

**`local/gps/time`** — the rig's only clock source. Fields are **UTC**, from
Milepost's GNSS fix, republished at ~1 Hz. All six are required; a partial
date is not a date.

```json
{ "year": 2026, "month": 9, "day": 28,
  "hour": 17, "minute": 4, "second": 31 }
```

Before the receiver has a fix it still publishes, with a placeholder year, so
anything under 2020 is discarded rather than stepping the clock back decades.
Because `mktime()` reads its argument as *local* time, `TZ` is pinned to UTC
across the conversion — skipping that folds the local offset into the epoch,
which looks exactly like the timezone setting doing nothing.

**`os/timezone/current`** — retained, published by the Headwaters OS daemon
from `/etc/timezone`.

```json
{ "tz": "America/Denver" }
```

Newlib has no zoneinfo database, so the IANA name is translated to a POSIX
`TZ` string against a table of shipped zones in `capstan_model.c`. An unknown
zone is logged and ignored, keeping whatever was installed — silently
rendering UTC would be worse.

## Commands

The display publishes to:

```
local/lights/<id>/command     {"state": 0|1, "brightness": 0-255}
local/lights/all/command      {"state": true|false}
local/relays/all/command      {"state": true|false}
local/config/request          (any payload — re-publishes retained config)
```

`brightness` is optional. Switchback-sourced lights are toggle-only; PDM lights
accept explicit state and brightness. The backend resolves which is which from
Mongo, so the display does not need to know.

### Optimistic update, and why Capstan does not do it

The handoff specifies optimistic UI updates with rollback on timeout. Fireside
deliberately does the opposite — it flips the control only when the module
publishes its new state back on `local/lights/<id>/status` (see the comment at
`main/actions.c:257`). The round trip is fast on a healthy rig, and a light that
visibly flips back is a worse experience than one that takes 150 ms.

**Capstan follows Fireside: confirm on round-trip, no optimistic flip.** If the
status does not arrive within the module's stale timeout, the control returns to
the last confirmed state and the screen shows the value as stale. This is a
deliberate departure from the handoff.

### The raw CAN path

Fireside bypasses `local/lights/<id>/command` and publishes CAN envelopes to
`can/outbound` directly. That path exists and works, but it requires the display
to know instance and channel arithmetic (`instance = (id-1) / 8`,
`channel = (id-1) % 8`) and the per-module base identifiers. Capstan uses the
higher-level `local/…/command` topics instead, and should only drop to
`can/outbound` if a behaviour turns out to be unreachable from them.

## Discovery

Not Home Assistant discovery. TrailCurrent uses mDNS plus an MQTT-mediated
confirm handshake. Capstan follows the MCU "confirm" pattern that Fireside
implements in `main/discovery.c`:

1. Subscribe to `local/discovery/trigger`. Headwaters broadcasts the bare
   string `*` (not JSON) at QoS 0.
2. On trigger — and NOT before — `mdns_init()`, hostname `esp32-XXXXXX`,
   instance name `TrailCurrent Capstan esp32-XXXXXX`.
3. On trigger, advertise `_trailcurrent._tcp` on **port 80** with the DNS-SD
   service instance name `TrailCurrent Discovery esp32-XXXXXX` and TXT records
   `type=capstan` and `fw=<app version>`, and serve `GET /discovery/confirm`.

   The hostname suffix on both names is load-bearing. A DNS-SD service
   instance name must be unique on the link, and the browser indexes what it
   resolves by that name — two dials advertising the same literal collapse
   into one entry in Overlook's list. Nothing downstream parses it
   (`discovery-mdns.py` takes the hostname from SRV and the type from TXT),
   so it is free to make unique, and the mDNS component's rename-and-reprobe
   conflict resolution is a race that should not be relied on with several
   identical panels triggering off one broadcast.
4. The host daemon `local_code/discovery-mdns.py` browses for the service and
   publishes to `discovery/browse/found`:
   `{"hostname":"esp32-A1B2C3","type":"capstan","fw":"1.0.0","onboard":"confirm"}`
5. The backend publishes `discovery/confirm/request`; the daemon fetches
   `http://<hostname>.local/discovery/confirm`; the result goes to
   `discovery/confirm/response`.

Implemented in `main/discovery.c`. Two things about it are worth knowing before
debugging a device that does not appear in Overlook:

- **The broker connection is dropped for the duration of a window**, so port 80
  can bind and because the TLS session is the largest single heap consumer that
  would otherwise compete with the HTTP server. It reconnects on the way out
  whether the confirm arrived or the three-minute window timed out. The service
  loop in `main.c` skips its broker-reconnect poll while a window is open — 
  without that it would rebuild the client five seconds in and undo the
  teardown.
- **mDNS runs only inside the window, never at boot.** Precautionary: the
  broker is `headwaters.local`, resolved by lwIP through
  `CONFIG_LWIP_DNS_SUPPORT_MDNS_QUERIES`, and the espressif/mdns component
  binds the same UDP port with no getaddrinfo integration of its own. Capstan
  is the only TrailCurrent device whose broker is a `.local` name, which is why
  Fireside and Spotter start mDNS at boot without trouble. The window already
  stops the broker, so mDNS costs nothing there.

> **`getaddrinfo() returns 202` / `esp-tls 0x8001` is a NETWORK fault, not a
> firmware one.** It means nothing answered for `headwaters.local`. Check the
> Headwaters box is powered and on the same network as the panel — from a
> laptop, `avahi-resolve-host-name headwaters.local` and `avahi-browse -at`.
> All three panels once showed this at once and the cause was that the name
> was unresolvable from every machine on the network, not anything on the
> device.
- **A trigger addressed to another device is ignored.** Every device on the
  broker sees every trigger, so the payload is matched against `*` or this
  device's own `esp32-XXXXXX` hostname before anything happens. A device that
  skipped the check would drop its broker connection for three minutes every
  time any other device was targeted.

> **Headwaters side.** The `type` value has to be registered in
> `containers/backend/src/routes/modules.js` (`MCU_MODULES`) with
> `wireless: true`, or the backend rejects the device and it never reaches
> Overlook's list however correctly it advertises itself. `capstan` is now in
> that list. The host-side browser (`local_code/discovery-mdns.py`) needs no
> change — it is type-agnostic and routes anything that is not `playbill`
> through the `confirm` flow.

`local/ota/trigger` is subscribed to as well — payload is a bare hostname
string at QoS 0, targeted at one device — but **nothing implements OTA on this
device yet**. A matching trigger is logged and otherwise ignored. There is
deliberately no callback registered for it: a handler that quietly did nothing
would be worse than an unhandled topic, which at least says so in the log.

## Gaps

These are the prototype screens with no backing topic. Each is built in the UI
and stubbed behind the mock data source.

### Thermostat — no topic exists

There is no `local/thermostat/*` anywhere in Headwaters. No setpoint, no HVAC
command, no mode. `MQTT_TOPICS.md` documents one; the code does not implement
it. `TrailCurrentTherma` contains zero MQTT references and appears only as a
registry entry in `routes/modules.js`.

**Status: GUI built, backend stubbed.** The screen renders and the ring adjusts
a local setpoint, but nothing is published and nothing is subscribed. The topic
and payload will be designed alongside the CAN bus work and added here then.
Nothing has been invented in the meantime — there is deliberately no speculative
schema in this file to accidentally implement against.

The only ambient temperature available today is `tempInC` / `tempInF` from
`local/airquality/temphumid`.

### Heater — not a distinct entity

A heater is a PDM or Switchback channel with a flame icon
(`containers/frontend/public/js/components/fireside-icons.js:151` maps
`'heater'` → `'fire'`). It is controlled with `local/lights/<id>/command` like
any other channel, and identified at runtime from the retained
`local/config/pdm_channels`.

**Status: implemented as a channel**, not as its own domain.

### Doors — derived from Picket

No `local/doors/*` topic. Doors and hatches are reed switches on Picket,
arriving as a 12-bit mask in `local/picket/<addr>/inputs`.

**Status: implemented** against the bitmask, and labelled since
`local/config/capstan/<hostname>/alarms` landed — the dial no longer needs its
own labelling. Armed state is now implicit: a sensor a user added to this
Capstan's alarm list is armed on this Capstan, independently of the rig-wide
`system_config.alarms.sensors` arm flags.

### Alerts — WebSocket only

`containers/backend/src/services/alarms-service.js` consumes the raw input
topics and `local/energy/status` and emits `alarms_update` over **WebSocket**,
not MQTT. There is no MQTT alert topic.

**Status: derived on-device**, mirroring Fireside's `main/alarms.c` — Capstan
applies its own configuration to the raw Picket and Spoor bitmasks. That
configuration is no longer entered on the panel: it arrives on
`local/config/panel/<hostname>/alarms` and is stored in NVS, so the alert
overlay keeps working when the Headwaters backend is down, at the cost of
duplicating the evaluation logic.

Evaluation and the overlay live in `main/ui_alerts.c`: every UI refresh it
tests each configured alarm against its board's input word and the current
mode, folds in Borealis's `co_alarm` / `lpg_alarm`, and opens PageAlert on a
rising edge. A board that has never published is not evaluated, so a `low`
alarm does not fire at boot before the first input broadcast.

A future `local/alarms/active` topic would **not** replace this. The whole
point of the per-panel, per-mode config is that the rig has no single answer
to "is this an alarm" to publish — the nightstand dial and the kitchen panel
disagree, correctly, about the same input.

A future `local/alarms/active` topic would remove that duplication and is worth
proposing, but it does not exist today.
