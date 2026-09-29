/*
 * Forwarder to the EEZ Studio export one directory up. The staged UI code
 * lives in <export>/sim/ and includes "structs.h" by bare name, which the
 * simulator's include path does not resolve from a subdirectory.
 */
#pragma once
#include "../structs.h"
