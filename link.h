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
#ifndef LINK_H
#define LINK_H

#include <stdint.h>
#include "common.h"

struct node_t;

typedef enum {
	LINK_DO_CONNECT,
	LINK_ENABLED,
	LINK_CONNECT_FAILED,
	LINK_PENDING
} link_state_t;

typedef struct link_t {
	char peer_id[UUID_BUFFER_SIZE];
	link_state_t state;
} link_t;

link_t *link_create(struct node_t *node, const char *peer_id, uint32_t peer_id_len, const link_state_t state);
void link_free(struct node_t *node, link_t *link);
link_t *link_get(struct node_t *node, const char *peer_id, uint32_t peer_id_len);
void link_remove(struct node_t *node, const char *peer_id);
result_t link_transmit(struct node_t *node, link_t *link);

#endif /* LINK_H */
