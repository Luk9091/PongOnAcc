/**
 * Copyright (c) 2022 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "tcp_client.h"

#include <string.h>
#include <time.h>

#include "pico/cyw43_arch.h"
#include "PASS.h"


queue_t sender_queue;

static err_t tcp_client_close(void *arg) {
    TCP_CLIENT_T *state = (TCP_CLIENT_T*)arg;
    err_t err = ERR_OK;
    if (state->tcp_pcb != NULL) {
        tcp_arg(state->tcp_pcb, NULL);
        tcp_poll(state->tcp_pcb, NULL, 0);
        tcp_sent(state->tcp_pcb, NULL);
        tcp_recv(state->tcp_pcb, NULL);
        tcp_err(state->tcp_pcb, NULL);
        err = tcp_close(state->tcp_pcb);
        if (err != ERR_OK) {
            DEBUG_printf("close failed %d, calling abort\n", err);
            tcp_abort(state->tcp_pcb);
            err = ERR_ABRT;
        }
        state->tcp_pcb = NULL;
    }
    return err;
}

// Called with results of operation
static err_t tcp_result(void *arg, int status) {
    TCP_CLIENT_T *state = (TCP_CLIENT_T*)arg;
    if (status == 0) {
        DEBUG_printf("test success\n");
    } else {
        DEBUG_printf("test failed %d\n", status);
    }
    state->complete = true;
    return tcp_client_close(arg);
}

static err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err) {
    TCP_CLIENT_T *state = (TCP_CLIENT_T*)arg;
    if (err != ERR_OK) {
        printf("connect failed %d\n", err);
        return tcp_result(arg, err);
    }
    state->connected = true;
    DEBUG_printf("Waiting for buffer from server\n");
    return ERR_OK;
}

static err_t tcp_client_poll(void *arg, struct tcp_pcb *tpcb) {
    DEBUG_printf("tcp_client_poll\n");
    return tcp_result(arg, -1); // no response is an error?
}

static void tcp_client_err(void *arg, err_t err) {
    if (err != ERR_ABRT) {
        DEBUG_printf("tcp_client_err %d\n", err);
        tcp_result(arg, err);
    }
}

err_t tcp_client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    // uint irq = save_and_disable_interrupts();
    TCP_CLIENT_T *state = (TCP_CLIENT_T*)arg;
    if (!p) {
        return tcp_result(arg, -1);
    }
    // this method is callback from lwIP, so cyw43_arch_lwip_begin is not required, however you
    // can use this method to cause an assertion in debug mode, if this method is called when
    // cyw43_arch_lwip_begin IS needed
    state->recv_len = 0;
    cyw43_arch_lwip_check();
    if (p->tot_len > 0) {
        DEBUG_printf("recv %d err %d\n", p->tot_len, err);
        const uint16_t buffer_left = BUF_SIZE - state->recv_len;
        state->recv_len += pbuf_copy_partial(p, state->recv + state->recv_len,
                                               p->tot_len > buffer_left ? buffer_left : p->tot_len, 0);
        tcp_recved(tpcb, p->tot_len);
    }
    pbuf_free(p);

    err = tcp_write(tpcb, state->send, state->send_len, TCP_WRITE_FLAG_COPY);

    if (err != ERR_OK) {
            DEBUG_printf("Failed to write data %d\n", err);
            return tcp_result(arg, -1);
    }
    // restore_interrupts(irq);
    return ERR_OK;
}

static bool tcp_client_open(void *arg) {
    TCP_CLIENT_T *state = (TCP_CLIENT_T*)arg;
    DEBUG_printf("Connecting to %s port %u\n", ip4addr_ntoa(&state->remote_addr), TCP_PORT);
    state->tcp_pcb = tcp_new_ip_type(IP_GET_TYPE(&state->remote_addr));
    if (!state->tcp_pcb) {
        DEBUG_printf("failed to create pcb\n");
        return false;
    }

    tcp_arg(state->tcp_pcb, state);
    tcp_poll(state->tcp_pcb, tcp_client_poll, POLL_TIME_S * 2);
    tcp_recv(state->tcp_pcb, tcp_client_recv);
    tcp_err(state->tcp_pcb, tcp_client_err);

    state->recv_len = 0;
    state->connected = true;

    // cyw43_arch_lwip_begin/end should be used around calls into lwIP to ensure correct locking.
    // You can omit them if you are in a callback from lwIP. Note that when using pico_cyw_arch_poll
    // these calls are a no-op and can be omitted, but it is a good practice to use them in
    // case you switch the cyw43_arch type later.
    cyw43_arch_lwip_begin();
    err_t err = tcp_connect(state->tcp_pcb, &state->remote_addr, TCP_PORT, tcp_client_connected);
    cyw43_arch_lwip_end();

    return err == ERR_OK;
}

// Perform initialisation
static TCP_CLIENT_T* tcp_client_init(void) {
    TCP_CLIENT_T *state = calloc(1, sizeof(TCP_CLIENT_T));
    if (!state) {
        DEBUG_printf("failed to allocate state\n");
        return NULL;
    }
    ip4addr_aton(TCP_SERVER_IP, &state->remote_addr);
    return state;
}

static err_t wifi_init(){
    if (cyw43_arch_init_with_country(CYW43_COUNTRY_POLAND)) {
        printf("failed to initialise\n");
        return ERR_CONN;
    }
    cyw43_arch_enable_sta_mode();

    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("failed to connect.\n");
        return ERR_CONN;
    } else {
        printf("Connected.\n");
    }
    return ERR_OK;
}

bool blinking(repeating_timer_t *t){
    // uint irq = save_and_disable_interrupts();
    gpio_put(BLINK_LED, !gpio_get(BLINK_LED));
    
    // restore_interrupts(irq);
    return true;
}

void run_tcp_client(void) {
    if(wifi_init() != ERR_OK){
        return;
    }

    gpio_init(BLINK_LED);
    gpio_set_dir(BLINK_LED, GPIO_OUT);
    gpio_put(BLINK_LED, 1);

    TCP_CLIENT_T *state = tcp_client_init();
    if (!state) {
        return;
    }
    state->complete = false;
    if (!tcp_client_open(state)) {
        tcp_result(state, -1);
        return;
    }

    tcp_data_t data;
    struct repeating_timer timer;
    add_repeating_timer_ms(500, blinking, NULL, &timer);

    while(!state->complete) {
    #if PICO_CYW43_ARCH_POLL
        cyw43_arch_poll();
        cyw43_arch_wait_for_work_until(make_timeout_time_ms(1000));
    #endif


        if(!queue_is_empty(&sender_queue)){
            queue_remove_blocking(&sender_queue, &data);
            printf("%s\n", data.data);
            
            memcpy(state->send, data.data, data.len);
            state->send_len = data.len;
        }

        if(state->connected && state->recv_len > 0){
            printf("Received: %s\n", state->recv);
            state->recv_len = 0;
        }
        // sleep_ms(1);
    }
    free(state);
    cancel_repeating_timer(&timer);
    gpio_put(BLINK_LED, 0);
    
    cyw43_arch_deinit();
}