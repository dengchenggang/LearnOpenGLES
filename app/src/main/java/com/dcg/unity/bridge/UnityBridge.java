package com.dcg.unity.bridge;
import androidx.annotation.NonNull;

import com.dcg.unity.bridge.command.Command;
import com.dcg.unity.bridge.command.ICommandProtocol;
import com.dcg.utils.Debug;
import com.unity3d.player.UnityPlayer;

public class UnityBridge {
    private static final String TAG = "UnityBridge::";

    public static void register(ICommandProtocol protocol) {
        BridgeProtocolManager.getInstance().addProtocol(protocol);
    }

    public static void unregister(ICommandProtocol protocol) {
        BridgeProtocolManager.getInstance().removeProtocol(protocol);
    }

    public static void register(Class<? extends ICommandProtocol> protocolClass) {
        BridgeProtocolManager.getInstance().addProtocol(protocolClass);
    }

    /// 调用TuanJie侧的统一入口
    public static void call(@NonNull final String protocol, @NonNull final String method, @NonNull final String args) {
        final Command command = new Command(protocol, method, args);
        final String str = command.toString();
        Debug.logI(TAG + "Invoke: command=" + str);
        UnityPlayer.UnitySendMessage("AndroidBridge", "OnAndroidCall", str);
    }

    /// TuanJie调用Android侧的统一入口
    public static void onUnityCall(String str) {
        Debug.logI(TAG + "onUnityCall: str=" + str);
         final Command cmd = Command.parse(str);
        if (cmd != null) {
            BridgeProtocolManager.getInstance().invoke(cmd.protocol, cmd.method, cmd.args);
        }
    }
}
