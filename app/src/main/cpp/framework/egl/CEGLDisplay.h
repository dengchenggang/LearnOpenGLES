#ifndef C_EGL_DISPLAY_H
#define C_EGL_DISPLAY_H

class CEGLDisplay {
private:
    void* mDisplay;
    void* mConfig;
public:
    CEGLDisplay();
    ~CEGLDisplay();
    CEGLDisplay(const CEGLDisplay&) = delete;
    CEGLDisplay& operator= (const CEGLDisplay&) = delete;
    CEGLDisplay(CEGLDisplay&&) = delete;
    CEGLDisplay& operator = (CEGLDisplay&&) = delete;
public:
    explicit operator bool() const { return valid(); }
private:
    void initialize();
    bool valid() const;
};

#endif