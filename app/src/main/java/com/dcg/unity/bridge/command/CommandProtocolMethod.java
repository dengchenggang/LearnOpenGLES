package com.dcg.unity.bridge.command;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

@Retention(RetentionPolicy.RUNTIME)    // 运行时可反射
@Target(ElementType.METHOD)            // 标注在方法上
public @interface CommandProtocolMethod {
    String name();                    // 暴露给 Unity 的逻辑名称
}
