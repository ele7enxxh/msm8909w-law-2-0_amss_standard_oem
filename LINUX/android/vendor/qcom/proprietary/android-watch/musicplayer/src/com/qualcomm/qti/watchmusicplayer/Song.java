/*
 * Copyright (c) 2015-16 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.watchmusicplayer;

import android.os.Parcel;
import android.os.Parcelable;

public class Song implements Parcelable{

    private long id;
    private String title;
    private String artist;
    private String album;
    private int duration;

    public Song(long id, String title, String artist, String album, int duration){
        this.id = id;
        this.title = title;
        this.artist = artist;
        this.album = album;
        this.duration = duration;
    }

    public Song(Parcel in) {
        this.id = in.readLong();
        this.title = in.readString();
        this.artist = in.readString();
        this.album = in.readString();
        this.duration = in.readInt();
    }

    public long getID(){return id;}
    public String getTitle(){return title;}
    public String getArtist(){return artist;}
    public String getAlbum() {return album;}
    public int getDuration() {return duration;}

    public static final Parcelable.Creator<Song> CREATOR = new Parcelable.Creator<Song>() {
        public Song createFromParcel(Parcel in) {
           return new Song(in);
        }

        public Song[] newArray(int size) {
           return new Song[size];
        }
     };

    @Override
    public int describeContents() {
        // TODO Auto-generated method stub
        return 0;
    }

    @Override
    public void writeToParcel(Parcel out, int flags) {
        out.writeLong(this.id);
        out.writeString(this.title);
        out.writeString(this.artist);
        out.writeString(this.album);
        out.writeInt(this.duration);

    }

}
