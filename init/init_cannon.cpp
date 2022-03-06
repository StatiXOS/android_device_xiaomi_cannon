/*
 * Copyright (c) 2021, StatiXOS
 *           (c) 2021, Vaisakh Murali
 * SPDX-License-Identifer: GPL-2.0-only
 */

#include <cstdlib>
#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>
#include <sys/sysinfo.h>

#include <android-base/properties.h>
#include "property_service.h"
#include "vendor_init.h"

using android::base::GetProperty;
using std::string;

void property_override(string prop, string value)
{
    auto pi = (prop_info*) __system_property_find(prop.c_str());

    if (pi != nullptr)
        __system_property_update(pi, value.c_str(), value.size());
    else
        __system_property_add(prop.c_str(), prop.size(), value.c_str(), value.size());
}

void dalvik_properties()
{
    struct sysinfo sys;

    sysinfo(&sys);

    if (sys.totalram > 4096ull * 1024 * 1024) {
        // Set for 6GB or more RAM
        property_override("dalvik.vm.heapstartsize", "16m");
        property_override("dalvik.vm.heapgrowthlimit", "256m");
        property_override("dalvik.vm.heapsize", "512m");
        property_override("dalvik.vm.heaptargetutilization", "0.5");
        property_override("dalvik.vm.heapmaxfree", "32m");
        property_override("dalvik.vm.heapminfree", "8m");
    } else if (sys.totalram > 3072ull * 1024 * 1024) {
        // Set for 4GB RAM
        property_override("dalvik.vm.heapstartsize", "8m");
        property_override("dalvik.vm.heapgrowthlimit", "256m");
        property_override("dalvik.vm.heapsize", "512m");
        property_override("dalvik.vm.heaptargetutilization", "0.6");
        property_override("dalvik.vm.heapmaxfree", "16m");
        property_override("dalvik.vm.heapminfree", "8m");
    }
}

void vendor_load_properties()
{
    string device, model;

    string region = GetProperty("ro.boot.hwc", "");
    string hwversion = GetProperty("ro.boot.hwversion", "");

    if (region == "GL") {
        device = "cannong";
        model = "Redmi Note 9T";
    } else if (region == "CN") {
        device = "cannon";
        model = "Redmi Note 9 5G";
    } else {
        // default to cannong
        device = "cannong";
        model = "Redmi Note 9T";
    }

    // Override all partitions' props
    string prop_partitions[] = { "", "odm.", "product.", "system.",
					"system_ext.", "bootimage.", "vendor." };

    for (const string &prop : prop_partitions) {
        property_override(string("ro.product.") + prop + string("name"), device);
        property_override(string("ro.product.") + prop + string("device"), device);
        property_override(string("ro.product.") + prop + string("model"), model);
        property_override(string("ro.") + prop + string("build.product"), device);
    }

    // Set hardware revision
    property_override("ro.boot.hardware.revision", hwversion);

    // Set hardware SKU prop
    property_override("ro.boot.product.hardware.sku", device);

    // Set camera model for EXIF data
    property_override("persist.vendor.camera.model", model);

    // Setup Dalvik values
    dalvik_properties();

}
