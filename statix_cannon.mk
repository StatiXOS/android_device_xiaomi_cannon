#
# Copyright 2021 mvaisakh
#
# SPDX-License-Identifer: Apache-2.0

# Inherit from those products. Most specific first.
$(call inherit-product, $(SRC_TARGET_DIR)/product/core_64_bit.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base_telephony.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/aosp_base.mk)

# Disable GApps for bringup
TARGET_DOES_NOT_USE_GAPPS := true

# Inherit statix common configuration
$(call inherit-product, vendor/statix/config/common.mk)
$(call inherit-product, vendor/statix/config/gsm.mk)

# Inherit device configurations
$(call inherit-product, device/xiaomi/cannon/cannon.mk)

# Target specifications
PRODUCT_NAME := statix_cannon
PRODUCT_DEVICE := cannon
PRODUCT_BRAND := Xiaomi
PRODUCT_MODEL := Redmi Note 9T
PRODUCT_MANUFACTURER := xiaomi
