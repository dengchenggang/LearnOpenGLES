package com.dcg.learnopengles.service;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.Service;
import android.content.Intent;
import android.os.Build;
import android.os.IBinder;
import android.provider.Settings;
import com.dcg.utils.Debug;

import androidx.core.app.NotificationCompat;

import com.dcg.learnopengles.view.FloatingWindowView;

public class RenderService extends Service {
    private static final String TAG = "RenderService";
    private static final int NOTIFICATION_ID = 1;
    private static final String CHANNEL_ID = "render_service_channel";

    private FloatingWindowView mFloatingWindowView;

    @Override
    public void onCreate() {
        super.onCreate();
        Debug.logI(TAG, "onCreate");
        mFloatingWindowView = new FloatingWindowView(this);
        mFloatingWindowView.setOnCloseListener(this::stopSelf);
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Debug.logI(TAG, "onStartCommand: startId=%d, intent=%s", startId, intent);
        startForeground(NOTIFICATION_ID, buildNotification());

        if (!Settings.canDrawOverlays(this)) {
            Debug.logE(TAG, "SYSTEM_ALERT_WINDOW permission is not granted");
            stopSelfResult(startId);
            return START_NOT_STICKY;
        }

        if (mFloatingWindowView != null) {
            mFloatingWindowView.attachToWindow();
        }
        return START_STICKY;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        Debug.logI(TAG, "onDestroy");
        if (mFloatingWindowView != null && mFloatingWindowView.isAttachedToWindow()) {
            mFloatingWindowView.detachFromWindow();
        }
        stopForeground(STOP_FOREGROUND_REMOVE);
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    private Notification buildNotification() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            NotificationChannel channel = new NotificationChannel(
                    CHANNEL_ID, "Render Service", NotificationManager.IMPORTANCE_LOW);
            channel.setDescription("OpenGL ES rendering service");
            NotificationManager manager = getSystemService(NotificationManager.class);
            if (manager != null) {
                manager.createNotificationChannel(channel);
            }
        }
        return new NotificationCompat.Builder(this, CHANNEL_ID)
                .setContentTitle("LearnOpenGLES")
                .setContentText("Rendering service is running")
                .setSmallIcon(android.R.drawable.ic_menu_compass)
                .setOngoing(true)
                .build();
    }
}
