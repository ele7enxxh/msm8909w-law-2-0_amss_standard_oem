/**
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.settings.watchcellular;

import android.app.Activity;
import android.content.ContentResolver;
import android.os.AsyncResult;
import android.os.Bundle;
import android.widget.ListView;
import android.widget.TextView;
import android.util.Log;
import android.os.SystemProperties;

import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneFactory;

public class PreferredNetworkType extends Activity {
    private static final String APP_TAG = "PreferredNetworkType";
    private static final int preferredNetworkMode = Phone.PREFERRED_NT_MODE;
    private int radioButtonIndex;
    private Phone phone = null;
    private ListView listViewPrefNwType;
    PreferredNetworkTypeArrayAdapter preferredNetworkTypeArrayAdapter;
    PreferredNetworkTypeClickListener preferredNetworkTypeClickListener;
    private String PanelDisplay;

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_preferred_network_type);

        final String[] networkModes = {
                getString(R.string.lte),
                getString(R.string.lte_wcdma),
                getString(R.string.lte_gsm_wcdma),
                getString(R.string.lte_cdma_evdo),
                getString(R.string.wcdma),
                getString(R.string.gsm_wcdma),
                getString(R.string.gsm),
                getString(R.string.cdma)
        };

        phone =  PhoneFactory.getDefaultPhone();
        listViewPrefNwType = (ListView) findViewById(R.id.list_view);
        // layout for circuler display
        if(Utils.IsDisplayTypeCircular()) {
            TextView textView = (TextView)findViewById(R.id.textView);
            int paddingDp = getResources().getDimensionPixelOffset(R.dimen.circuler_padding);
            textView.setPadding(0, paddingDp, 0, 0);
            listViewPrefNwType.setPadding(0, 0, 0, paddingDp);
        }

        listViewPrefNwType.setChoiceMode(ListView.CHOICE_MODE_SINGLE);

        preferredNetworkTypeArrayAdapter = new PreferredNetworkTypeArrayAdapter(
            this, networkModes);
        listViewPrefNwType.setAdapter(preferredNetworkTypeArrayAdapter);

        preferredNetworkTypeClickListener =
            new PreferredNetworkTypeClickListener(this, listViewPrefNwType,
                phone);
        listViewPrefNwType.setOnItemClickListener(
            preferredNetworkTypeClickListener);

        // Based on network mode selected, set the respective radio button
        phone.getPreferredNetworkType(preferredNetworkTypeClickListener.handler.
            obtainMessage(preferredNetworkTypeClickListener.
                EVENT_QUERY_GET_PREF_NETWORK_DONE));
    }

    protected void onStart() {
        super.onStart();
        listViewPrefNwType.setClickable(true);
    }
}
