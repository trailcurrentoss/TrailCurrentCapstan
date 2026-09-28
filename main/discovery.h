/*
 * Headwaters discovery: how this panel becomes visible to Overlook.
 *
 * THE HANDSHAKE
 *
 * Overlook's "scan for devices" does not probe the network. It asks the
 * Headwaters backend, which broadcasts `local/discovery/trigger` with the
 * payload `*` over MQTT and then browses mDNS for `_trailcurrent._tcp`. A
 * device only appears in the list if it answers that broadcast by putting
 * itself on mDNS -- a panel that is perfectly connected, publishing, and
 * visible in the broker's own logs still shows up nowhere, because nothing
 * asked the broker.
 *
 * So the sequence is:
 *
 *   1. Headwaters publishes `*` to `local/discovery/trigger`.
 *   2. We advertise `_trailcurrent._tcp` on port 80 with TXT `type` and `fw`,
 *      and start an HTTP server serving GET /discovery/confirm.
 *   3. Overlook lists us. The user picks us.
 *   4. Headwaters' host-side proxy GETs http://<hostname>.local/discovery/confirm.
 *   5. We stop advertising and go back to normal operation.
 *
 * A trigger addressed to one device carries that device's hostname instead of
 * `*`, and every other device must ignore it.
 *
 * WHY THE BROKER CONNECTION IS DROPPED FOR THE DURATION
 *
 * Port 80 has to bind cleanly, and on this chip the TLS session to the broker
 * is the largest single consumer of the heap the HTTP server needs. Both
 * Spotter and Fireside stop the MQTT client first for the same reason; this
 * follows them rather than inventing a third answer. It reconnects on the way
 * out whether the confirm arrived or the window timed out.
 *
 * WHAT THE USER SEES
 *
 * Nothing yet. Discovery is a network-side handshake with no screen of its
 * own; the panel keeps showing whatever it was showing. Adding a Setup-style
 * screen for it would be worth doing and is deliberately not bundled in here.
 */
#pragma once

#include <stdbool.h>

/*
 * How long to stay discoverable after a trigger.
 *
 * Three minutes, matching Spotter and Fireside. It has to outlast a human
 * reading a list on a phone and deciding, which is the actual bound -- the
 * browse itself takes a second or two.
 */
#define DISCOVERY_TIMEOUT_MS 180000

/**
 * Start the mDNS resolver and claim the hostname.
 *
 * Call once at boot, after Wi-Fi has been initialised. This does NOT make the
 * device discoverable -- it only makes `<hostname>.local` resolve, which is
 * what the confirm GET in step 4 above needs. The `_trailcurrent._tcp`
 * service is added and removed per discovery window.
 */
void discovery_init(void);

/**
 * A `local/discovery/trigger` arrived and it was for us.
 *
 * Spawns a worker task and returns immediately, so this is safe to call from
 * the MQTT parse path. Calls that arrive while a window is already open are
 * ignored -- the second one would tear down the server the first is using.
 */
void discovery_handle_trigger(void);

/** True while a discovery window is open. */
bool discovery_is_running(void);
