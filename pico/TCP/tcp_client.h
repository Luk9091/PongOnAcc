#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__

#include "pico/stdlib.h"

#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include <hardware/timer.h>
#include <pico/util/queue.h>
#include <hardware/gpio.h>

#define DEBUG_printf // printf
#define BLINK_LED 15

#define BUF_SIZE 200
#define TEST_ITERATIONS 10
#define POLL_TIME_S 5

typedef struct TCP_CLIENT_T_ {
    struct tcp_pcb *tcp_pcb;
    ip_addr_t remote_addr;
    uint8_t recv[BUF_SIZE];
    uint8_t send[BUF_SIZE];
    int recv_len;
    int send_len;
    bool complete;
    bool connected;
} TCP_CLIENT_T;

typedef struct{
    uint8_t data[BUF_SIZE];
    size_t len;
} tcp_data_t;

extern queue_t sender_queue;

void run_tcp_client(void);
void queue_push(tcp_data_t *data);

#endif