/*
 * Internal interface between the simulator's own files. Simulator only --
 * see docs/simulator.md.
 */
#pragma once

#ifdef EEZ_LVGL_SIMULATOR

/* sim_board.c: take over the simulator's mouse, wheel and keyboard so they
 * drive the ring callbacks the way board_encoder.c does on the device. */
void sim_board_attach_inputs(void);

/* sim_net.c: pretend Wi-Fi has an IP and the broker is up. */
void sim_net_init(void);

/* sim_feed.c: seed the rig's retained config, then publish readings. */
void sim_feed_seed_config(void);
void sim_feed_start(void);

#endif
