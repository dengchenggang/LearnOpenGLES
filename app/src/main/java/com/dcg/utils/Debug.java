package com.dcg.utils;

import android.util.Log;

import androidx.annotation.NonNull;

public class Debug {
    private static String TAG = "[LearnOpenGLES]";

    public void setTAG(@NonNull final String tag) {
        Debug.TAG = tag;
    }

    public static void logD(@NonNull final String tag, @NonNull final String format, Object... args) {
        Log.d(TAG, tag + String.format(format, args));
    }

    public static void logD(@NonNull final String str) {
        Log.d(TAG, str);
    }

    public static void logI(@NonNull final String tag, @NonNull final String format, Object... args) {
        Log.i(TAG, tag + String.format(format, args));
    }

    public static void logI(@NonNull final String str) {
        Log.i(TAG, str);
    }

    public static void logE(@NonNull final String tag, @NonNull final String format, Object... args) {
        Log.e(TAG, tag + String.format(format, args));
    }

    public static void logE(@NonNull final String str) {
        Log.e(TAG, str);
    }
}
