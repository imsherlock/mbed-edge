
#ifndef _CAT_BLUETOOTH_H
#define _CAT_BLUETOOTH_H

#include <map>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#include <linux/if_ether.h>

/* Unofficial value, might still change */
#define LE_LINK		0x03

#define FLAGS_AD_TYPE 0x01
#define FLAGS_LIMITED_MODE_BIT 0x01
#define FLAGS_GENERAL_MODE_BIT 0x02

#define EIR_FLAGS                   0x01  /* flags */
#define EIR_UUID16_SOME             0x02  /* 16-bit UUID, more available */
#define EIR_UUID16_ALL              0x03  /* 16-bit UUID, all listed */
#define EIR_UUID32_SOME             0x04  /* 32-bit UUID, more available */
#define EIR_UUID32_ALL              0x05  /* 32-bit UUID, all listed */
#define EIR_UUID128_SOME            0x06  /* 128-bit UUID, more available */
#define EIR_UUID128_ALL             0x07  /* 128-bit UUID, all listed */
#define EIR_NAME_SHORT              0x08  /* shortened local name */
#define EIR_NAME_COMPLETE           0x09  /* complete local name */
#define EIR_TX_POWER                0x0A  /* transmit power level */
#define EIR_DEVICE_ID               0x10  /* device ID */
#define EIR_MANUFACTURE_SPECIFIC    0xFF

#define BLE_MAX_NAME_LEN 30
#define BLE_MAX_ADDR_STRLEN 18

struct cmpBLE {
    bool operator()(const bdaddr_t a, const bdaddr_t b) const {
        return memcmp(&a, &b, 6) < 0;
    }
};

std::map<bdaddr_t, int, cmpBLE> get_ble_devices(void);

#endif
