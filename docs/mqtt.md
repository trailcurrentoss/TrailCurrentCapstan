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
`local/playbill/…/status`, `os/timezone/current` and device LWTs are retained.

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
  "time_remaining_minutes": 430,
  "solar_watts": 640,
  "charge_type": "off|fault|bulk|absorption|float|equalize|unknown" }
```

`time_remaining_minutes` is omitted when it is 0 or 0xFFFF.

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

1. On boot, `mdns_init()`, hostname `esp32-XXXXXX`, instance name
   `TrailCurrent Capstan`.
2. Subscribe to `local/discovery/trigger`. Headwaters broadcasts the bare
   string `*` (not JSON) at QoS 0.
3. On trigger, advertise `_trailcurrent._tcp` on **port 80** with TXT records
   `type=capstan` and `fw=<app version>`, and serve `GET /discovery/confirm`.
4. The host daemon `local_code/discovery-mdns.py` browses for the service and
   publishes to `discovery/browse/found`:
   `{"hostname":"esp32-A1B2C3","type":"capstan","fw":"1.0.0","onboard":"confirm"}`
5. The backend publishes `discovery/confirm/request`; the daemon fetches
   `http://<hostname>.local/discovery/confirm`; the result goes to
   `discovery/confirm/response`.

> **Headwaters change required.** The `type` value must be registered in
> `containers/backend/src/routes/modules.js` (`MCU_MODULES`) with
> `wireless: true`, or the backend rejects the device. Capstan cannot complete
> discovery until that entry exists. This is tracked as a Headwaters task, not
> a Capstan one.

Also subscribe to `local/ota/trigger` — payload is a bare hostname string at
QoS 0, targeted at one device. On a match the device enters OTA mode.

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

**Status: implemented** against the bitmask. Labels and armed state live in
Mongo (`system_config.alarms.sensors`, keyed `"picket:0:5"`), so the display
needs its own labelling until a config topic exposes them.

### Alerts — WebSocket only

`containers/backend/src/services/alarms-service.js` consumes the raw input
topics and `local/energy/status` and emits `alarms_update` over **WebSocket**,
not MQTT. There is no MQTT alert topic.

**Status: derived on-device**, mirroring Fireside's `main/alarms.c` — Capstan
applies its own armed/label configuration to the raw Picket and Spoor bitmasks.
This keeps the alert overlay working when the Headwaters backend is down, at the
cost of duplicating the threshold logic.

A future `local/alarms/active` topic would remove that duplication and is worth
proposing, but it does not exist today.
