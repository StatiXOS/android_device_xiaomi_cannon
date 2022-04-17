/*
 * Copyright (C) 2022 StatiX
 * SPDX-License-Identifier: GPL-2.0-only
 */

package com.android.cannonextensions

import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.util.Log

class OnBootCompletedReceiver : BroadcastReceiver() {
    override fun onReceive(context: Context, intent: Intent) {
        Log.d(TAG, "Starting")
        DoubleTapUtils.startService(context)
    }

    companion object {
        private const val TAG = "CannonExtensions"
    }
}