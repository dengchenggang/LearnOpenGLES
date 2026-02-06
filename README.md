# README

## 1 vscode 配置

### 1.1 c_cpp_properties.json

```json
{
    "configurations": [
        {
            "name": "Android-arm64",
            "includePath": [
                "${env:NDK_HOME}/toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/include",
                "${env:NDK_HOME}/toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/include/aarch64-linux-android",
                "${env:NDK_HOME}/toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/include/c++/v1",
                "${workspaceFolder}/app/src/main/cpp/**"
            ],
            "defines": [
                "__ANDROID__",
                "ANDROID",
                "ANDROID_ABI=arm64-v8a"
            ],
            "compilerPath": "${env:NDK_HOME}\\toolchains\\llvm\\prebuilt\\windows-x86_64\\bin\\aarch64-linux-android30-clang++.cmd",
            "cStandard": "c17",
            "cppStandard": "gnu++17",
            "intelliSenseMode": "windows-clang-arm64",
            "browse": {
                "path": ["${workspaceFolder}"],
                "limitSymbolsToIncludedHeaders": true,
                "databaseFilename": "${workspaceFolder}/.vscode/browse/browse.vc.db"
            }
        }
    ],
    "version": 4
}
```

```json
{
    "configurations": [
        {
            "name": "Android NDK",
            "includePath": [
                 "${workspaceFolder}/**",
                "C:/Users/11750/AppData/Local/Android/Sdk/ndk/27.0.12077973/toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/include/**"
            ],
            "defines": [
                "__ANDROID__",
                "ANDROID"
            ],
            "compilerPath": "C:/Users/11750/AppData/Local/Android/Sdk/ndk/27.0.12077973/toolchains/llvm/prebuilt/windows-x86_64/bin/clang++.exe",
            "cStandard": "c17",
            "cppStandard": "c++17",
            "intelliSenseMode": "linux-clang-arm64",
            "browse": {
                "path": ["${workspaceFolder}"],
                "limitSymbolsToIncludedHeaders": true,
                "databaseFilename": "${workspaceFolder}\\.vscode\\browse\\browse.vc.db"
            },
            "compilerArgs": [
                "--target=aarch64-none-linux-android30"
            ]
        }
    ],
    "version": 4
}
```

### 1.2 tasks.json

```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Gradle: assembleDebug",
            "type": "shell",
            "command": "./gradlew",
            "args": ["assembleDebug"],
            "group": "build",
            "problemMatcher": ["$gcc"]
        }
    ]
}
```

## 2 连接MUMU模拟器

```sh
# WIN11上设置环境变量
SystemPropertiesAdvanced

adb connect 127.0.0.1:16384
```

git remote add origin git@github.com:dengchenggang/LearnOpenGLES.git