#
# Copyright (C) 2021 StatiXOS
# Copyright (C) 2021 Vaisakh Murali
#
# SPDX-License-Identifer: GPL-2.0-only

# Inherit from those products. Most specific first.
$(call inherit-product, $(SRC_TARGET_DIR)/product/core_64_bit.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/aosp_base_telephony.mk)

# Disable GApps for bringup
TARGET_DOES_NOT_USE_GAPPS := true

# Inherit statix common configuration
$(call inherit-product, vendor/statix/config/common.mk)
$(call inherit-product, vendor/statix/config/gsm.mk)

# Inherit device configurations
$(call inherit-product, device/xiaomi/cannon/device.mk)

# Target specifications
PRODUCT_NAME := statix_cannon
PRODUCT_DEVICE := cannon
PRODUCT_BRAND := xiaomi
PRODUCT_MODEL := Redmi Note 9T
PRODUCT_MANUFACTURER := xiaomi