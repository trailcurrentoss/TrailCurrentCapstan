/*
 * Forwarder to the EEZ Studio export one directory up. The staged UI code
 * lives in <export>/sim/ and includes "screens.h" by bare name, which the
 * simulator's include path does not resolve from a subdirectory.
 */
#pragma once
#include "../screens.h"
