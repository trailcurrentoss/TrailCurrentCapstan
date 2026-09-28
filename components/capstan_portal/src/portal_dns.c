/*
 * A DNS server that answers every question with our own address.
 *
 * WHY: that is what makes it a CAPTIVE portal rather than a web server
 * nobody finds. Phones probe a known URL after joining a network
 * (connectivitycheck.gstatic.com on Android, captive.apple.com on iOS).
 * If DNS resolves those to us and we answer with something other than
 * the expected 204/Success, the phone decides the network is "captive"
 * and pops the setup page by itself. Without this, the user has to be
 * told to type an IP address into a browser, which is exactly the
 * fiddliness the portal exists to remove.
 *
 * Only A queries are answered. AAAA and everything else are ignored
 * rather than refused: a phone that gets no AAAA falls back to IPv4,
 * whereas an error can make it give up on the name entirely.
 */

#include <string.h>
#include <sys/socket.h>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/sockets.h"

#include "portal_dns.h"

static const char *TAG = "portal.dns";

#define DNS_PORT 53
#define DNS_MAX  512

static TaskHandle_t s_task;
static int          s_sock = -1;
static uint32_t     s_answer_ip;          /* network byte order */

typedef struct __attribute__((packed)) {
    uint16_t id, flags, qdcount, ancount, nscount, arcount;
} dns_header_t;

static void dns_task(void *arg)
{
    (void)arg;
    uint8_t buf[DNS_MAX];

    while (s_sock >= 0) {
        struct sockaddr_in from;
        socklen_t flen = sizeof(from);
        const int n = recvfrom(s_sock, buf, sizeof(buf), 0,
                               (struct sockaddr *)&from, &flen);
        if (n < (int)sizeof(dns_header_t)) {
            continue;
        }

        dns_header_t *h = (dns_header_t *)buf;
        if (ntohs(h->qdcount) != 1) {
            continue;       /* not a simple single-name query */
        }

        /* Walk the QNAME labels to find the type/class that follow. */
        int p = sizeof(dns_header_t);
        while (p < n && buf[p] != 0) {
            p += buf[p] + 1;
        }
        p += 1;                      /* the root label */
        if (p + 4 > n) {
            continue;
        }
        const uint16_t qtype = (uint16_t)((buf[p] << 8) | buf[p + 1]);
        const int qend = p + 4;      /* past QTYPE and QCLASS */

        if (qtype != 1) {            /* A records only -- see the header */
            continue;
        }

        /* Reply: the original question, plus one answer pointing here.
         * 0x8180 = response, recursion available, no error. */
        h->flags   = htons(0x8180);
        h->ancount = htons(1);
        h->nscount = 0;
        h->arcount = 0;

        int o = qend;
        if (o + 16 > (int)sizeof(buf)) {
            continue;
        }
        buf[o++] = 0xC0; buf[o++] = 0x0C;          /* pointer to the name */
        buf[o++] = 0x00; buf[o++] = 0x01;          /* type A */
        buf[o++] = 0x00; buf[o++] = 0x01;          /* class IN */
        /* TTL 0: the moment setup finishes these answers are wrong, and a
         * cached one would send the phone here instead of the real site. */
        buf[o++] = 0; buf[o++] = 0; buf[o++] = 0; buf[o++] = 0;
        buf[o++] = 0x00; buf[o++] = 0x04;          /* RDLENGTH */
        memcpy(&buf[o], &s_answer_ip, 4); o += 4;

        sendto(s_sock, buf, o, 0, (struct sockaddr *)&from, flen);
    }

    ESP_LOGI(TAG, "dns responder stopped");
    s_task = NULL;
    vTaskDelete(NULL);
}

esp_err_t portal_dns_start(uint32_t answer_ip_net_order)
{
    if (s_task) {
        return ESP_OK;
    }
    s_answer_ip = answer_ip_net_order;

    s_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (s_sock < 0) {
        ESP_LOGE(TAG, "socket failed");
        return ESP_FAIL;
    }

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(DNS_PORT),
        .sin_addr.s_addr = htonl(INADDR_ANY),
    };
    if (bind(s_sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        ESP_LOGE(TAG, "bind :53 failed");
        close(s_sock);
        s_sock = -1;
        return ESP_FAIL;
    }

    /* 4 KB: the task holds a 512-byte packet buffer and does no parsing
     * beyond walking labels, but lwIP's socket calls are not frugal. */
    if (xTaskCreate(dns_task, "portal_dns", 4096, NULL, 4, &s_task) != pdPASS) {
        close(s_sock);
        s_sock = -1;
        return ESP_ERR_NO_MEM;
    }

    ESP_LOGI(TAG, "dns responder up on :53");
    return ESP_OK;
}

void portal_dns_stop(void)
{
    if (s_sock >= 0) {
        const int sock = s_sock;
        s_sock = -1;        /* the task's loop condition */
        shutdown(sock, SHUT_RDWR);
        close(sock);
    }
}
