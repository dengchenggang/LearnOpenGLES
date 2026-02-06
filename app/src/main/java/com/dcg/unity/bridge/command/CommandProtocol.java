package com.dcg.unity.bridge.command;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

@Retention(RetentionPolicy.RUNTIME)  // 运行时保留，便于反射
@Target(ElementType.TYPE)            // 标注在类/接口上
public @interface CommandProtocol {
    String name();                  // 服务对外的名字
}
