/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
   All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */


package com.qualcomm.qti.launcher;


import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.TextView;

/**
 * This class is an adapter for each element of the grid. Each element, has a image view
 * for the app icon and a label to show the name of the app
 */
public class GridAdapter extends BaseAdapter {
    private Context mContext;
    private MainActivity.PackageDetail[] packagesForAdapter;

    public GridAdapter(Context c, MainActivity.PackageDetail pacs[]) {
        mContext = c;
        packagesForAdapter = pacs;
    }

    @Override
    public int getCount() {
        return packagesForAdapter.length;
    }

    @Override
    public Object getItem(int position) {
        return null;
    }

    @Override
    public long getItemId(int position) {
        return 0;
    }

    static class ViewHolder {
        TextView text;
        ImageView icon;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {

        ViewHolder viewHolder;
        LayoutInflater li = (LayoutInflater) mContext.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        if (convertView == null) {
            convertView = li.inflate(R.layout.list_item, null);
            viewHolder = new ViewHolder();
            viewHolder.text = (TextView) convertView.findViewById(R.id.icon_text);
            viewHolder.icon = (ImageView) convertView.findViewById(R.id.icon_image);

            // to increase performance set this in the convertView so that can be reused
            convertView.setTag(viewHolder);
        } else {
            // it was already created, so re-use
            viewHolder = (ViewHolder) convertView.getTag();
        }

        viewHolder.icon.setImageDrawable(packagesForAdapter[position].icon);
        viewHolder.text.setText(packagesForAdapter[position].label);

        return convertView;
    }
}
