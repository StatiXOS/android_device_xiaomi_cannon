/*
 * Copyright (C) 2022 StatiX
 * SPDX-License-Identifier: GPL-2.0-only
 */

package com.android.cannonextensions

import android.app.Service
import android.content.ContentResolver
import android.content.Context
import android.content.Intent
import android.database.ContentObserver
import android.net.Uri
import android.os.Handler
import android.os.IBinder
import android.os.Looper
import android.os.SystemProperties
import android.os.UserHandle
import android.provider.Settings.Secure

class DoubleTapService : Service() {
    private lateinit var mContext: Context
    private lateinit var mHandler: Handler
    private lateinit var mCustomSettingsObserver: CustomSettingsObserver
    
    override fun onBind(intent: Intent?): IBinder? {
        return null
    }

    override fun onDestroy() {
        super.onDestroy()
    }

    override fun onStartCommand(intent: Intent?, flags: Int, startid: Int): Int {
        mContext = this
        mHandler = Handler(Looper.getMainLooper())
        mCustomSettingsObserver = CustomSettingsObserver(mHandler)
        mCustomSettingsObserver!!.observe()
        mCustomSettingsObserver!!.update()
        return START_STICKY
    }

    private inner class CustomSettingsObserver internal constructor(handler: Handler?) : ContentObserver(handler) {
        fun observe() {
            val resolver: ContentResolver = mContext.getContentResolver()
            resolver.registerContentObserver(Secure.getUriFor(Secure.DOUBLE_TAP_TO_WAKE),
                    false, this, UserHandle.USER_CURRENT)
        }

        fun update() {
            val dt2wValue: Int = Secure.getInt(mContext.getContentResolver(), Secure.DOUBLE_TAP_TO_WAKE, 0)
            val dt2wEnabled = dt2wValue == 1
            SystemProperties.set("persist.sys.cannon.dt2w", if (dt2wEnabled) "1" else "0")
        }

        override fun onChange(selfChange: Boolean, uri: Uri) {
            if (uri.equals(Secure.getUriFor(Secure.DOUBLE_TAP_TO_WAKE))) {
                update()
            }
        }
    }
}
