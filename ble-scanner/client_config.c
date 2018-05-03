/*
 * ----------------------------------------------------------------------------
 * Copyright 2018 ARM Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ----------------------------------------------------------------------------
 */

#include "common/integer_length.h"
#include "pt-client/pt_api.h"
#include "pt-client/pt_device_object.h"
#include "pt-example/client_config.h"
#include "ipso_objects.h"
#include "mbed-trace/mbed_trace.h"
#include "bluetooth.h"

#define TRACE_GROUP "clnt-example"

#define LIFETIME           86400

pt_device_t *client_config_create_device(const char *device_id, const char *endpoint_postfix)
{
    pt_status_t status = PT_STATUS_SUCCESS;
    char *endpoint_id = malloc(strlen(device_id) + strlen(endpoint_postfix) + 1);
    sprintf(endpoint_id, "%s%s", device_id, endpoint_postfix);
    printf("endpoint_id = %s%s", device_id, endpoint_postfix);
    pt_device_t *device = pt_create_device(endpoint_id, LIFETIME, QUEUE, &status);
    if (status != PT_STATUS_SUCCESS) {
        tr_err("Could not allocate device structure.");
        return NULL;
    }
    return device;
}

pt_device_t *client_config_create_reappearing_device(const char *device_id, const char *endpoint_postfix)
{
    pt_device_t *device = client_config_create_device(device_id, endpoint_postfix);
    ipso_create_bluetooth(device, 0, 30);
    return device;
}

void example_reboot_callback(const pt_resource_opaque_t *resource, const uint8_t* value, const uint32_t value_length, void *userdata)
{
    tr_info("Example /3 device reboot resource executed.");
}

pt_device_list_t *client_config_create_device_list(const char *endpoint_postfix)
{
    pt_device_list_t *device_list = malloc(sizeof(pt_device_list_t));
    ns_list_init(device_list);
    struct ble_device_list *ble_list;

    ble_list = get_ble_devices();

    struct list_head *entry;
    list_for_each(entry, &ble_list->_list)
    {
        pt_device_t *device = NULL;
        char *name = list_entry(entry, struct ble_device, name);
        printf("name = %s\n", name);

        device = client_config_create_device(name, endpoint_postfix);
        if (NULL == device) {
            continue;
        }

        pt_device_entry_t *device_entry = malloc(sizeof(pt_device_entry_t));
        device_entry->device = device;
        ns_list_add_to_end(device_list, device_entry);
    }

    return device_list;
}

void client_config_add_device_to_config(pt_device_list_t *device_list, pt_device_t *device)
{
    pt_device_entry_t *device_entry = malloc(sizeof(pt_device_entry_t));
    device_entry->device = device;
    ns_list_add_to_end(device_list, device_entry);
}


void client_config_free()
{
}
