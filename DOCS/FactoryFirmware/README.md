# Factory firmware backups

Full 16 MB flash images read off each board **before** any TrailCurrent
firmware was written to it. Keep these — the vendor does not publish
recoverable factory images for these boards, so if one of these files is lost
the board cannot be returned to its as-shipped state.

The `.bin` files are `.gitignore`d — 16 MB each, and they are images of
specific physical units rather than anything shareable. `SHA256SUMS.txt` is
ignored for the same reason: it lists those filenames, which carry each
board's MAC. Keep both alongside your own boards and archive them somewhere
off this machine.

## Contents

Images are named:

```
<board>_<resolution>_<MAC>_factory_full16MB.bin
```

| `<board>` | Board | Panel |
|---|---|---|
| `matouch21` | Makerfabs MaTouch ESP32-S3 Rotary 2.1" | 480×480 ST7701S |
| `crowpanel146` | Elecrow CrowPanel 1.46" Rotary | 360×360 JD9855 |
| `crowpanel128` | Elecrow CrowPanel 1.28" Rotary | 240×240 GC9A01 |

The MAC in the filename is what ties an image to the physical unit it came
off. It is per-unit, so the images in any given working copy are that
developer's own boards and nobody else's.

## Telling the two Elecrow boards apart

Both are ESP32-S3R8 with 16 MB flash and identical `esptool` output, both are
branded "CrowPanel … HMI ESP32 Rotary Display", and both enumerate as a bare
`Espressif USB JTAG/serial debug unit` with no model in the descriptor. There
is nothing on the USB bus that distinguishes them, which is how the first
backup here came to be filed under the wrong name.

Two reliable discriminators:

**MAC address** — a stable per-unit identifier, carried in the symlink:

```sh
ls -l /dev/serial/by-id/
# usb-Espressif_USB_JTAG_serial_debug_unit_<MAC>-if00 -> ../../ttyACMn
```

Note your own boards' MACs somewhere local and untracked. They differ for
every unit, so a table of them here would be wrong for everyone but whoever
wrote it.

**The factory image's own strings** — which is how to identify a board you
cannot otherwise place:

```sh
strings -n 4 <dump>.bin | grep -iE 'ELECROW|ESP32S3_1\.(28|46)_BLE'
#  -> ESP32S3_1.28_BLE_Server   CrowPanel 1.28"
#  -> ESP32S3_1.46_BLE_Server   CrowPanel 1.46"
#  -> (no match)                MaTouch 2.1"
```

**Verify, do not assume.** During this project's own bring-up both Elecrow
boards were initially filed under the wrong name, because the board present
was assumed to be the one just mentioned rather than identified. The cost of
guessing here is a backup that restores the wrong firmware to a board.

Each image is the entire flash, offset `0x0` to `0x1000000`, so it includes the
bootloader, partition table, NVS and any vendor calibration data — not just the
application.

## What was on them

Both boards shipped with Arduino-based firmware:

```
project = arduino-lib-builder
idf     = v4.4.6-dirty
built   = Oct  4 2023
```

Factory partition layouts differ from each other and from Capstan's:

**MaTouch 2.1"** — single app, no OTA, no filesystem.

```
nvs       data nvs      0x009000   24K
phy_init  data phy      0x00f000    4K
app0      app  factory  0x010000  16320K
```

**CrowPanel 1.46"** — OTA-capable with a SPIFFS partition (the demo's assets).

```
nvs       data nvs      0x009000   20K
otadata   data otadata  0x00e000    8K
app0      app  ota_0    0x010000  10240K
spiffs    data spiffs   0xa10000   896K
coredump  data coredump 0xaf0000    64K
```

Capstan repartitions (two 4 MB OTA slots — see `partitions.csv` at the repo
root), so restoring a factory image also restores the factory partition table.
Nothing to do by hand.

## Verifying a copy

```sh
sha256sum -c SHA256SUMS.txt
```

## Restoring a board to factory

Writing the whole image restores bootloader, partition table, app and NVS
together. Match the MAC in the symlink name to the MAC in the image
filename — plugging boards in a different order renumbers `ttyACM*`, so the
port alone is never a safe way to pick the target.

```sh
esptool --port <port> --baud 921600 write-flash 0x0 <image>.bin
```

The board's own MAC is in eFuse, not in flash, so an image is not strictly
board-specific — but restore the image taken from *that* board anyway, since
NVS may hold per-unit touch or display calibration.

## Taking a new backup

Before flashing any board for the first time:

```sh
# 1. Find the port and the board's MAC.
ls -l /dev/serial/by-id/

# 2. Confirm the flash size before assuming 16 MB.
esptool --port <port> --no-stub flash-id

# 3. Read the whole thing. Takes a few minutes; esptool buffers and writes
#    the file at the end, so a 0-byte file mid-run is normal, not a hang.
esptool --port <port> --baud 921600 \
        read-flash 0 0x1000000 <board>_<res>_<MAC>_factory_full16MB.bin

# 4. Verify the size is exactly 16777216 and record the hash.
stat -c%s <file> && sha256sum <file> >> SHA256SUMS.txt
```
