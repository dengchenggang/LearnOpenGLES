#include "OpenGLESRenderer.h"
#include "LogUtils.h"
#include <GLES3/gl3.h>

void OpenGLESRenderer::initialize(int32_t width, int32_t height) {
    mWidth = width;
    mHeight = height;
    glViewport(0, 0, width, height);
}

void OpenGLESRenderer::resize(int32_t width, int32_t height) {
    mWidth = width;
    mHeight = height;
    glViewport(0, 0, width, height);
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
}

void OpenGLESRenderer::update(int64_t deltaTime) {

}

void OpenGLESRenderer::render(int64_t deltaTime) {
    glViewport(0, 0, mWidth, mHeight);
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLESRenderer::destroy() {

}
