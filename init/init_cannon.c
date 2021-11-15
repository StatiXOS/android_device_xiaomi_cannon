/*
 * Copyright (c) 2021, StatiXOS
 *           (c) 2021, Vaisakh Murali
 * SPDX-License-Identifer: GPL-2.0-only
 */

#include <stdlib.h>
#include "property_service.h"
#include "log.h"
#include "util.h"
#include "init_cannon.h"

#define LOG_TAG "libinit_cannon"

void init_device_properties()
{
    char platform, region, device, model;
    char device_props[] = {
        "ro.build.product", "ro.product.board", "ro.product.bootimage.device"
        "ro.product.device", "ro.product.odm.device", "ro.product.product.device",
        "ro.product.system.device", "ro.product.system_ext.device",
        "ro.product.vendor.device", "ro.product.vendor_dlkm.device"
    };
    char model_props[] {
        "ro.product.bootimage.model", "ro.product.model","ro.product.odm.model",
        "ro.product.product.model","ro.product.system.model",
        "ro.product.system_ext.model","ro.product.vendor.model",
        "ro.product.vendor_dlkm.model"
    };

    // Get region specification
    region = property_get(PROP_HW_REGION, platform);

    // Set device name according to region
    if (region == "CN") {
        ALOGI("This device is the chinese variant (Cannon)!");
        device = "cannon";
        model = "Redmi Note 9 5G";
    } else {
        ALOGI("This device is the global variant (Cannong)!");
        device = "cannong";
        model = "Redmi Note 9T";
    }

    // Override device props
    for (int i; i < sizeof(device_props; i++)) {
        property_set(device_props[i], device);
    }

    // Override model props
    for (int i; i < sizeof(model_props); i++) {
        property_set(model_props[i], model);
    }
}