/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.watchsettings.nfc;

import android.content.Context;
import android.database.DataSetObserver;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ListAdapter;
import android.widget.TextView;
import com.qualcomm.qti.watchsettings.R;

public class NfcArrayAdapter extends ArrayAdapter<String> {

    private Context context;
    private String[] nfcOptions;
    private NfcStatus nfcStatus;
    public final static String DATA_STATE_TAG = "DATA_STATE";

    private class NFCListItemViewHolder {
        TextView text1;
        TextView text2;
    }

    public NfcArrayAdapter(Context context,
            String[] nfcOptions, NfcStatus nfcStatus) {
        super(context, R.layout.nfc_data_list_item, nfcOptions);
        this.context = context;
        this.nfcOptions = nfcOptions;
        this.nfcStatus = nfcStatus;
    }

    public int getCount() {
        return nfcOptions.length;
    }

    public View getView(int position, View convertView,
            ViewGroup parent) {
        NFCListItemViewHolder viewHolder;

        LayoutInflater li = (LayoutInflater) context.getSystemService(
                Context.LAYOUT_INFLATER_SERVICE);

        if (convertView == null) {
            convertView = li.inflate(R.layout.nfc_data_list_item, null);

            viewHolder = new NFCListItemViewHolder();
            viewHolder.text1 = (TextView) convertView.findViewById(R.id.text1);
            viewHolder.text2 = (TextView) convertView.findViewById(R.id.text2);
            viewHolder.text2.setTag(DATA_STATE_TAG);

            convertView.setTag(viewHolder);
        } else {
            viewHolder = (NFCListItemViewHolder)
                    convertView.getTag();
        }

        viewHolder.text1.setText(nfcOptions[position]);

        if(nfcOptions[position].equals(
                context.getString(R.string.nfc))) {

            nfcStatus.setNfcStatusText(viewHolder.text2,
                    nfcStatus.isNfcEnabled());

        }else if(nfcOptions[position].equals(
                context.getString(R.string.android_beam))) {

            nfcStatus.setAndroidBeamStatusText(viewHolder.text2,
                    nfcStatus.isAndroidBeamEnabled());

        }
        return convertView;
    }
}
