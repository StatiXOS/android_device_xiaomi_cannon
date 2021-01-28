#
# Copyright 2021 mvaisakh
#
# SPDX-License-Identifer: Apache-2.0

LOCAL_PATH := $(call my-dir)

ifeq ($(TARGET_DEVICE),cannon)

include $(call all-makefiles-under,$(LOCAL_PATH))

endif