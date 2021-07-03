#
# Copyright (C) 2021 StatiXOS
# Copyright (C) 2021 Vaisakh Murali
#
# SPDX-License-Identifer: GPL-2.0-only

#
# Top level guard makefile
#

LOCAL_PATH := $(call my-dir)

ifeq ($(TARGET_DEVICE),cannon)
include $(call all-makefiles-under,$(LOCAL_PATH))
endif 