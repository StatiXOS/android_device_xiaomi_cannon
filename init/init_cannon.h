/*
 * Copyright (c) 2021, StatiXOS
 *           (c) 2021, Vaisakh Murali
 * SPDX-License-Identifer: GPL-2.0-only
 */

#ifndef __INIT_CANNON__H__
#define __INIT_CANNON__H__

#include <sys/system_properties.h>

#define PROP_HW_REGION  "ro.boot.hwc"

void init_device_properties();

#endif /* __INIT_CANNON__H__ */