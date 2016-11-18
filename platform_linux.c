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
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "platform.h"
#include "node.h"
#include "transport.h"

#ifdef LWM2M_HTTP_CLIENT
typedef struct lwm2m_client_t {
	char *iface;
	int port;
	char *endpoint;
} lwm2m_client_t;
static lwm2m_client_t m_lwm2m_client;
#endif
static calvin_gpio_t *m_gpios[MAX_GPIOS];

void platform_init(void)
{
	int i = 0;

	srand(time(NULL));

	for (i = 0; i < MAX_GPIOS; i++)
		m_gpios[i] = NULL;
}

#ifdef LWM2M_HTTP_CLIENT
void platform_init_lwm2m(char *iface, int port, char *endpoint)
{
	m_lwm2m_client.iface = iface;
	m_lwm2m_client.port = port;
	m_lwm2m_client.endpoint = endpoint;
}
#endif

result_t platform_run(const char *ssdp_iface, const char *proxy_iface, const int proxy_port)
{
	uint32_t timeout = 60;

	if (node_start(ssdp_iface, proxy_iface, proxy_port) != SUCCESS) {
		log_error("Failed to start node");
		return FAIL;
	}

	while (1) {
		if (transport_select(timeout) != SUCCESS) {
			log_error("Failed to receive data");
			node_stop(true);
			return FAIL;
		}
	}
}

result_t platform_mem_init(void)
{
	// TODO: Create a memory pool
	return SUCCESS;
}

result_t platform_mem_alloc(void **buffer, uint32_t size)
{
	*buffer = malloc(size);
	if (*buffer == NULL) {
		log_error("Failed to allocate '%ld' memory", (unsigned long)size);
		return FAIL;
	}

	log_debug("Allocated '%ld'", (unsigned long)size);

	return SUCCESS;
}

void platform_mem_free(void *buffer)
{
	free(buffer);
}

calvin_gpio_t *platform_create_in_gpio(uint32_t pin, char pull, char edge)
{
	int i = 0;

	if (pull != 'u' && pull != 'd') {
		log_error("Unsupported pull direction");
		return NULL;
	}

	if (edge != 'r' && edge != 'f' && edge != 'b') {
		log_error("Unsupported edge");
		return NULL;
	}

	for (i = 0; i < MAX_GPIOS; i++) {
		if (m_gpios[i] == NULL) {
			if (platform_mem_alloc((void **)&m_gpios[i], sizeof(calvin_gpio_t)) != SUCCESS) {
				log_error("Failed to allocate memory");
				return NULL;
			}

			m_gpios[i]->pin = pin;
			m_gpios[i]->has_triggered = true;
			return m_gpios[i];
		}
	}

	return NULL;
}

calvin_gpio_t *platform_create_out_gpio(uint32_t pin)
{
	int i = 0;

	for (i = 0; i < MAX_GPIOS; i++) {
		if (m_gpios[i] == NULL) {
			if (platform_mem_alloc((void **)&m_gpios[i], sizeof(calvin_gpio_t)) != SUCCESS) {
				log_error("Failed to allocate memory");
				return NULL;
			}

			m_gpios[i]->pin = pin;
			return m_gpios[i];
		}
	}

	return NULL;
}

void platform_uninit_gpio(calvin_gpio_t *gpio)
{
	int i = 0;

	for (i = 0; i < MAX_GPIOS; i++) {
		if (m_gpios[i] != NULL && m_gpios[i]->pin == gpio->pin) {
			free(m_gpios[i]);
			m_gpios[i] = NULL;
			break;
		}
	}

	free(gpio);
}

void platform_set_gpio(calvin_gpio_t *gpio, uint32_t value)
{
	log("Setting gpio");
}

result_t platform_get_temperature(double *temp)
{
#ifdef LWM2M_HTTP_CLIENT
	char buffer[BUFFER_SIZE], *start = NULL, *end = NULL;
	float value;

	if (m_lwm2m_client.iface == NULL || m_lwm2m_client.port == 0 || m_lwm2m_client.endpoint == NULL) {
		log_error("Bad lwm2m arguments");
		return FAIL;
	}

	sprintf(buffer, "GET /api/clients/%s/3303/0/5700 HTTP/1.0\r\n\r\n", m_lwm2m_client.endpoint);
	if (transport_http_get(m_lwm2m_client.iface, m_lwm2m_client.port, buffer, BUFFER_SIZE) != SUCCESS) {
		log_error("Failed to send '%s' to '%s:%d'", buffer, m_lwm2m_client.iface, m_lwm2m_client.port);
		return FAIL;
	}

	start = strstr(buffer, "\"value\":");
	if (start == NULL) {
		log_error("Bad response '%s'", buffer);
		return FAIL;
	}

	start += 8;
	value = strtod(start, &end);
	*temp = (double)value;
#else
	*temp = 15.5;
#endif
	return SUCCESS;
}
