/*
 * Copyright (c) 2016 Ericsson AB
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef TRANSPORT_H
#define TRANSPORT_H

#include "common.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#ifdef USE_LWIP
#include "lwip/tcp.h"
#endif

#ifndef USE_LWIP
#define BUFFER_SIZE			512
#endif

typedef enum {
	TRANSPORT_DISCONNECTED,
	TRANSPORT_CONNECTED,
	TRANSPORT_JOINED
} transport_state_t;

typedef struct transport_buffer_t {
	char *buffer;
	uint32_t pos;
	uint32_t size;
} transport_buffer_t;

typedef struct transport_client_t {
#ifdef USE_LWIP
	struct tcp_pcb *tcp_port;
#else
	int fd;
#endif
	transport_state_t state;
	transport_buffer_t rx_buffer;
	transport_buffer_t tx_buffer;
} transport_client_t;

result_t transport_start(const char *ssdp_iface, const char *proxy_iface, const int proxy_port);
result_t transport_send(size_t len);
result_t transport_select(uint32_t timeout);
void transport_set_state(const transport_state_t state);
transport_state_t transport_get_state(void);
void transport_stop(void);
result_t transport_get_tx_buffer(char **buffer, uint32_t size);
bool transport_can_send(void);
#ifdef LWM2M_HTTP_CLIENT
result_t transport_http_get(char *iface, int port, char *url, char *buffer, int buffer_size);
#endif

#endif /* TRANSPORT_H */
