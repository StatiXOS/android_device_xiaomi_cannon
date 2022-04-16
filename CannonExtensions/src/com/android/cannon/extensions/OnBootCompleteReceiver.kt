package com.android.cannonextensions

import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent

class OnBootCompleteReceiver : BroadcastReceiver() {
    override fun onReceive(context: Context, intent: Intent?) {
        val sIntent = Intent(context, DT2WServiceCannon::class.java)
        context.startServiceAsUser(sIntent, UserHandle.CURRENT)
    }
}