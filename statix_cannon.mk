#
# Copyright (C) 2021 StatiXOS
# Copyright (C) 2021 Vaisakh Murali
#
# SPDX-License-Identifer: GPL-2.0-only

# Inherit from those products. Most specific first.
$(call inherit-product, $(SRC_TARGET_DIR)/product/core_64_bit.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/aosp_base_telephony.mk)

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
PRODUCT_MANUFACTURER := Xiaomi

BUILD_FINGERPRINT := "Redmi/cannong_global/cannong:11/RP1A.200720.011/V12.5.6.0.RJEMIXM:user/release-keys"

PRODUCT_BUILD_PROP_OVERRIDES += \
    PRIVATE_BUILD_DESC="cannong-user 11 RP1A.200720.011 V12.5.6.0.RJEMIXM release-keys"

PRODUCT_GMS_CLIENTID_BASE := android-xiaomi
