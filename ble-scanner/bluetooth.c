#include "pt-client/pt_api.h"
#include "pt-client/client.h"
#include "mbed-trace/mbed_trace.h"
#include "pt-example/client_config.h"
#include "ble-scanner/client_example.h"
#include "ipso_objects.h"
#include "ble-scanner/byte_order.h"
#define TRACE_GROUP "ble-scanner"

#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <signal.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#include "bluetooth.h"

void ipso_create_bluetooth(pt_device_t *device, const uint16_t object_instance_id, char *name)
{
    pt_status_t status = PT_STATUS_SUCCESS;

    pt_object_t *object = pt_device_add_object(device, BLUETOOTH_SENSOR, &status);
    if (status != PT_STATUS_SUCCESS) {
        tr_err("Could not create an object with id (%d) to the device (%s).",
               BLUETOOTH_SENSOR, device->device_id);
    }

    pt_object_instance_t *instance =
        pt_object_add_object_instance(object, object_instance_id, &status);

    if (status != PT_STATUS_SUCCESS) {
        tr_err("Could not create an object instance with id (%d) to the object (%d).",
               object_instance_id, BLUETOOTH_SENSOR);
    }
}

static int check_report_filter(uint8_t procedure, le_advertising_info *info)
{
	uint8_t flags;

	/* If no discovery procedure is set, all reports are treat as valid */
	if (procedure == 0)
		return 1;

	/* Read flags AD type value from the advertising report if it exists */
	if (read_flags(&flags, info->data, info->length))
		return 0;

	switch (procedure) {
	case 'l': /* Limited Discovery Procedure */
		if (flags & FLAGS_LIMITED_MODE_BIT)
			return 1;
		break;
	case 'g': /* General Discovery Procedure */
		if (flags & (FLAGS_LIMITED_MODE_BIT | FLAGS_GENERAL_MODE_BIT))
			return 1;
		break;
	default:
		fprintf(stderr, "Unknown discovery procedure\n");
	}

	return 0;
}

static void eir_parse_name(uint8_t *eir, size_t eir_len,
						char *buf, size_t buf_len)
{
	size_t offset;

	offset = 0;
	while (offset < eir_len) {
		uint8_t field_len = eir[0];
		size_t name_len;

		/* Check for the end of EIR */
		if (field_len == 0)
			break;

		if (offset + field_len > eir_len)
			goto failed;

		switch (eir[1]) {
		case EIR_NAME_SHORT:
		case EIR_NAME_COMPLETE:
			name_len = field_len - 1;
			if (name_len > buf_len)
				goto failed;

			memcpy(buf, &eir[2], name_len);
			return;
		}

		offset += field_len + 1;
		eir += field_len + 1;
	}

failed:
	snprintf(buf, buf_len, "(unknown)");
}

static int get_advertising_devices(int dd, uint8_t filter_type,
    struct ble_device_list *ble_list)
{
	unsigned char buf[HCI_MAX_EVENT_SIZE], *ptr;
	struct hci_filter nf, of;
	socklen_t olen;
	int len;
    int no_devices = 10;

	olen = sizeof(of);
	if (getsockopt(dd, SOL_HCI, HCI_FILTER, &of, &olen) < 0) {
		printf("Could not get socket options\n");
		return -1;
	}

	hci_filter_clear(&nf);
	hci_filter_set_ptype(HCI_EVENT_PKT, &nf);
	hci_filter_set_event(EVT_LE_META_EVENT, &nf);

	if (setsockopt(dd, SOL_HCI, HCI_FILTER, &nf, sizeof(nf)) < 0) {
		printf("Could not set socket options\n");
		return -1;
	}

	while (no_devices-- > 0) {
		evt_le_meta_event *meta;
		le_advertising_info *info;
		char addr[BLE_MAX_ADDR_STRLEN];

		while ((len = read(dd, buf, sizeof(buf))) < 0) {
			if (errno == EINTR) {
				len = 0;
				goto done;
			}

			if (errno == EAGAIN || errno == EINTR)
				continue;
			goto done;
		}

		ptr = buf + (1 + HCI_EVENT_HDR_SIZE);
		len -= (1 + HCI_EVENT_HDR_SIZE);

		meta = (void *) ptr;

		if (meta->subevent != 0x02)
			goto done;

		/* Ignoring multiple reports */
		info = (le_advertising_info *) (meta->data + 1);
		if (check_report_filter(filter_type, info)) {
            struct ble_device *dev = malloc(sizeof(struct ble_device));
            if (NULL == dev) {
                perror("Could not allocate memory for BLE device!");
                continue;
            }
			memset(dev->name, 0, sizeof(dev->name));

			ba2str(&info->bdaddr, dev->addr);
			eir_parse_name(info->data, info->length,
							dev->name, sizeof(dev->name) - 1);

			printf("addr=%s name=%s\n", dev->addr, dev->name);

            INIT_LIST_HEAD(&dev->_list);

            list_add(&dev->_list, &ble_list->_list);
            ++ble_list->no_devices;
		}
	}

done:
	setsockopt(dd, SOL_HCI, HCI_FILTER, &of, sizeof(of));

	if (len < 0)
		return -1;

	return 0;
}


struct ble_device_list *get_ble_devices(void)
{
    int err, opt, dd, dev_id = -1;
    uint8_t own_type = 0x00;
    uint8_t scan_type = 0x01;
    uint8_t filter_type = 0;
    uint8_t filter_policy = 0x00;
    uint16_t interval = htobs(0x0010);
    uint16_t window = htobs(0x0010);
    uint8_t filter_dup = 1;
    struct ble_device_list *ble_list = NULL;

	if (dev_id < 0)
		dev_id = hci_get_route(NULL);

	dd = hci_open_dev(dev_id);
	if (dd < 0) {
		perror("Could not open device");
		goto cleanup;
	}

	err = hci_le_set_scan_parameters(dd, scan_type, interval, window,
						own_type, filter_policy, 1000);
	if (err < 0) {
		perror("Set scan parameters failed");
		goto cleanup;
	}

	err = hci_le_set_scan_enable(dd, 0x01, 0x1, 1000);
	if (err < 0) {
		perror("Enable scan failed");
		goto cleanup;
	}

	printf("LE Scan ...\n");

    ble_list = calloc(1, sizeof(struct ble_device_list));
    if (NULL == ble_list) {
        perror("Failed to allocate memory for BLE device list");
        goto cleanup;
    }
    INIT_LIST_HEAD(&ble_list->_list);

	err = get_advertising_devices(dd, filter_type, ble_list);
	if (err < 0) {
		perror("Could not receive advertising events");
		goto cleanup;
	}

	err = hci_le_set_scan_enable(dd, 0x00, filter_dup, 1000);
	if (err < 0) {
		perror("Disable scan failed");
		goto cleanup;
	}

cleanup:
	hci_close_dev(dd);

    return ble_list;
}
