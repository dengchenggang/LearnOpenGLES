# document

在 Android、嵌入式 Linux、HarmonyOS 乃至大多数移动/嵌入式平台上，要使用 OpenGL ES 进行绘制，首先必须通过 EGL(Embedded-System Graphics Library) 把 “渲染 API(OpenGL ES)” 同 “本地窗口系统/显示设备” 联系起来。
最常见的几个核心对象/概念就是：
EGLDisplay（简称 Display）
EGLConfig（帧缓冲配置）
EGLSurface（绘制目标 Surface）
EGLContext（OpenGL ES 的状态机，本文题目没写，但在真正使用时必不可少）

## 1. EGLDisplay —— 跟底层窗口系统打招呼

- 含义：表示和本地窗口系统或物理显示设备的“连接句柄”。

## 2. EGLConfig —— 描述“想要什么样的帧缓冲”

- 含义：一组属性( attribute list )，规定了 Color Buffer、Depth/Stencil、Alpha、Multisample 等的“格式”和“能力”。
- 作用：
  - EGL 根据这些属性从驱动/硬件支持的所有配置里挑出符合条件的集合。
  - 选中的 EGLConfig 以后，同一份配置可以被用来创建任意数量的 Surface 和 Context。

## 3. EGLSurface —— 真正承载像素的“画布”

- 类型
  - Window Surface (eglCreateWindowSurface)
    - 后端通常关联一个 NativeWindow（Android 的 Surface / ANativeWindow）。
    - 最终会出现在屏幕上。
  - Pbuffer Surface (eglCreatePbufferSurface)
    - 离屏离线渲染，渲染结果可作为纹理或离屏缓冲使用。
    - Pixmap Surface（很少用）
- 作用
  - 为 OpenGL ES 的渲染命令提供 Framebuffer attachment；
  - 与 EGLContext 一起传给 eglMakeCurrent，告诉 driver“把我的 GL 命令画到哪儿”。
- 交换缓冲（前后缓冲交换）：eglSwapBuffers(display, surface);
  - 对 Window Surface 表示把后台缓冲里的像素“翻转”到前台，显示到屏幕。
  - 对 Pbuffer Surface 通常啥也不做（或实现为 no-op）。

## 4. EGLContext

- 保存并管理全部 OpenGL ES 状态
  - 顶点/片段着色器对象、Program 对象
  - 纹理、FBO、VBO、UBO 等对象
  - 纹理、FBO、VBO、UBO 等对象
- 提供对象命名空间
  - 每个 Context 拥有独立的“名字表”（GLuint id）。同一 id 在不同 Context 中互不冲突。
- 定义线程归属关系，EGLContext 与线程是一一对应的：
  - 一个线程在任何时刻最多有一个 “当前” Context
  - 一个 Context 任意时刻也只能被一个线程“当前”持有，通过 eglMakeCurrent(display, draw, read, ctx) 建立/切换。
- 可选的资源共享（share-context）
  - 创建时可指定与已有 Context 共享对象池，实现纹理、buffer 等 GPU 资源复用。