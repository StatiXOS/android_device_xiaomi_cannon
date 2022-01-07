package org.lineageos.dt2w.cannon;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.Build;
import android.os.UserHandle;

public class OnBootCompleteReceiver extends BroadcastReceiver {
    public void onReceive(Context context, Intent intent) {
        Intent sIntent = new Intent(context, DT2WServiceCannon.class);
        context.startServiceAsUser(sIntent, UserHandle.CURRENT);
    }
}
