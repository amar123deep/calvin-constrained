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
#ifndef PORT_H
#define PORT_H

#include <stdbool.h>
#include "common.h"
#include "fifo.h"
#include "tunnel.h"

#define MAX_PORT_NAME_LENGTH		20
#define MAX_PENDING_TOKEN_RESPONSE	4

struct actor_t;
struct node_t;

typedef enum {
	PORT_DIRECTION_IN,
	PORT_DIRECTION_OUT
} port_direction_t;

typedef enum {
	PORT_DO_CONNECT,
	PORT_DO_DISCONNECT,
	PORT_DO_DELETE,
	PORT_DO_ENABLE,
	PORT_DO_PEER_LOOKUP,
	PORT_DISCONNECTED,
	PORT_ENABLED,
	PORT_PENDING
} port_state_t;

typedef enum {
	PORT_REPLY_TYPE_ACK,
	PORT_REPLY_TYPE_NACK,
	PORT_REPLY_TYPE_ABORT
} port_reply_type_t;

typedef struct pending_token_response_t {
	bool handled;
	uint32_t sequencenbr;
	bool ack;
} pending_token_response_t;

typedef struct port_t {
	char port_id[UUID_BUFFER_SIZE];
	char peer_id[UUID_BUFFER_SIZE];
	char peer_port_id[UUID_BUFFER_SIZE];
	char port_name[MAX_PORT_NAME_LENGTH];
	port_direction_t direction;
	tunnel_t *tunnel;
	port_state_t state;
	fifo_t fifo;
	pending_token_response_t pending_token_responses[MAX_PENDING_TOKEN_RESPONSE];
	struct actor_t *actor;
} port_t;

port_t *port_create(struct node_t *node, struct actor_t *actor, char *obj_port, char *obj_prev_connections, port_direction_t direction);
void port_free(port_t *port);
result_t add_pending_token_response(port_t *port, uint32_t sequencenbr, bool ack);
void port_disconnect(port_t *port);
port_t *port_get(struct node_t *node, const char *port_id, uint32_t port_id_len);
port_t *port_get_from_name(struct actor_t *actor, char *name, port_direction_t direction);
result_t port_handle_disconnect(struct node_t *node, const char *port_id, uint32_t port_id_len);
result_t port_handle_connect(struct node_t *node, const char *port_id, uint32_t port_id_len, const char *tunnel_id, uint32_t tunnel_id_len);
result_t port_iniate_connect(struct node_t *node, port_t *port);
void port_delete(port_t *port);
result_t port_transmit(struct node_t *node, port_t *port);

#endif /* PORT_H */
