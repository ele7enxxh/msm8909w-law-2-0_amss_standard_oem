/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.watchsettings;

import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;

public class FactoryResetHandler {

    Context context;

    FactoryResetHandler(Context context) {

        this.context = context;
    }

    void performReset() {

        // show confirmation alert, perform reset only if user confirms
        AlertDialog.Builder builder = new AlertDialog.Builder(context);

        builder.setTitle(context.getString(R.string.factory_reset_confirmation_title));
        builder.setMessage(context.getString(R.string.factory_reset_confirmation_message));

        builder.setPositiveButton(context.getString(R.string.factory_reset_ok_button),
            new DialogInterface.OnClickListener() {

            @Override
            public void onClick(DialogInterface dialog, int which) {

                if (SettingsListClickListener.isShutdownEnabled(context)) {
                    // show a progress dialog in case reset takes long time
                    showWaitForResetAlert();

                    Intent intent = new Intent("android.intent.action.MASTER_CLEAR");
                    intent.addFlags(Intent.FLAG_RECEIVER_FOREGROUND); // run with high priority

                    context.sendBroadcast(intent);
                }
            }
        });

        builder.setNegativeButton(
            context.getString(R.string.factory_reset_cancel_button), null);

        builder.show();
    }

    private void showWaitForResetAlert() {

        // show indeterminate non cancellable progress dialog
        ProgressDialog.show(context, context.getString(R.string.factory_reset_wait_title),
            context.getString(R.string.factory_reset_wait_message), true, false);
    }
}
