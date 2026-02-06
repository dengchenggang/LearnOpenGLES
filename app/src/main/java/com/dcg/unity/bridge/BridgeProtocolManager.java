package com.dcg.unity.bridge;

import android.text.TextUtils;

import androidx.annotation.NonNull;

import com.dcg.unity.bridge.command.CommandProtocolMethod;
import com.dcg.unity.bridge.command.CommandProtocol;
import com.dcg.unity.bridge.command.ICommandProtocol;
import com.dcg.utils.Debug;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.HashMap;
import java.util.Map;

class BridgeProtocolManager {

    private static final String TAG = "BridgeProtocolManager::";

    private final Map<String, Map<String, MethodHolder>> mMethodTable;
    private final Map<String, ICommandProtocol> mUnityProtocolTable;

    private BridgeProtocolManager() {
        mMethodTable = new HashMap<>();
        mUnityProtocolTable = new HashMap<>();
    }

    public void addProtocol(ICommandProtocol protocol) {
        final Class<? extends ICommandProtocol> protocolClass = protocol.getClass();
        final String name = this.checkUnityBridgeProtocol(protocolClass);
        if (TextUtils.isEmpty(name)) {
            Debug.logI(TAG + "addProtocol: " + protocolClass + ", UnityBridgeProtocol.name is isEmpty!");
            return;
        }
        this.addProtocol(name, protocol, protocolClass.getDeclaredMethods());
    }

    public void removeProtocol(ICommandProtocol protocol) {
        final Class<? extends ICommandProtocol> protocolClass = protocol.getClass();
        final String name = this.checkUnityBridgeProtocol(protocolClass);
        Debug.logI(TAG + "removeProtocol： protocol=" + name);
        mUnityProtocolTable.remove(name);
        mMethodTable.remove(name);
    }

    public void addProtocol(final Class<? extends ICommandProtocol> protocolClass)  {
        final String name = this.checkUnityBridgeProtocol(protocolClass);
        if (TextUtils.isEmpty(name)) {
            Debug.logI(TAG + "addProtocol: " + protocolClass + ", UnityBridgeProtocol.name is isEmpty!");
            return;
        }
        if (mUnityProtocolTable.containsKey(name)) {
            Debug.logE(TAG + "addProtocol: " + name + " has addProtocol.");
            return;
        }
        ICommandProtocol protocol = null;
        try {
            protocol = protocolClass.getDeclaredConstructor().newInstance();
        } catch (Exception e) {
            Debug.logI(TAG + "addProtocol: e=" + e);
        }
        if (protocol != null) {
            this.addProtocol(name, protocol, protocolClass.getDeclaredMethods());
        }
    }

    public void invoke(@NonNull final String protocol, @NonNull final String method, final String args) {
        final ICommandProtocol inst = mUnityProtocolTable.get(protocol);
        final Map<String, MethodHolder> methodHolderMap = mMethodTable.get(protocol);
        if ((inst == null) || (methodHolderMap == null)) {
            Debug.logE(TAG + "invoke: protocol=" + protocol + " not found!");
            return;
        }
        final MethodHolder methodHolder = methodHolderMap.get(method);
        if (methodHolder == null) {
            Debug.logE(TAG + "invoke: protocol=" + protocol + ", method=" + method + " not found!");
            return;
        }
        try {
            methodHolder.invoke(inst, args);
        } catch (Exception e) {
            Debug.logE(TAG + "invoke: e=" + e);
        }
    }

    private String checkUnityBridgeProtocol(final Class<? extends ICommandProtocol> protocolClass) {
        if (!protocolClass.isAnnotationPresent(CommandProtocol.class)) {
            Debug.logE(TAG + "checkUnityBridgeProtocol: " + protocolClass + " hasn't UnityBridgeProtocol.");
            return null;
        }
        final CommandProtocol ann = protocolClass.getAnnotation(CommandProtocol.class);
        if (ann == null) {
            Debug.logE(TAG + "checkUnityBridgeProtocol: " + protocolClass + " UnityBridgeProtocol instance is null" );
            return null;
        }

        return ann.name();
    }

    private void addProtocol(@NonNull final String name, @NonNull ICommandProtocol protocol, @NonNull final Method[] methods) {
        Debug.logI(TAG + "addProtocol: service=" + name);
        mUnityProtocolTable.putIfAbsent(name, protocol);
        final Map<String, MethodHolder> methodMap = new HashMap<>();
        for (Method m : methods) {
            CommandProtocolMethod ann = m.getAnnotation(CommandProtocolMethod.class);
            if (ann != null) {
                m.setAccessible(true);
                methodMap.put(ann.name(), new MethodHolder(m));
                Debug.logI(TAG + "addProtocol: service=" + name + ", method=" + ann.name());
            }
        }
        mMethodTable.putIfAbsent(name, methodMap);
    }

    private static class MethodHolder {
        private final Method mMethod;
        public MethodHolder(@NonNull final Method method) {
            mMethod = method;
        }

        void invoke(@NonNull final Object instance, @NonNull final String str) throws InvocationTargetException, IllegalAccessException {
            if (mMethod.getParameterTypes().length == 0) {
                mMethod.invoke(instance);
            } else {
                mMethod.invoke(instance, str);
            }
        }
    }

    private static class Holder {
        public static final BridgeProtocolManager INSTANCE = new BridgeProtocolManager();
    }

    public static BridgeProtocolManager getInstance() { return Holder.INSTANCE; }
}
