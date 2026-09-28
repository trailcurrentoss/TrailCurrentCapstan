#pragma once
#include <stdint.h>
#include "esp_err.h"

/** Answer every A query with this address (network byte order). */
esp_err_t portal_dns_start(uint32_t answer_ip_net_order);
void portal_dns_stop(void);
