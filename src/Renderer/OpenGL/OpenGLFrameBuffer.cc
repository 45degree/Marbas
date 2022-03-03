#include "Renderer/OpenGL/OpenGLFrameBuffer.h"

namespace Marbas {

OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferInfo& info) : FrameBuffer(info) {
    glGenFramebuffers(1, &frameBufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
}

OpenGLFrameBuffer::~OpenGLFrameBuffer() {
    glDeleteFramebuffers(1, &frameBufferID);
}

void OpenGLFrameBuffer::Bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
}

void OpenGLFrameBuffer::UnBind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLFrameBuffer::ChangeBufferSize(int width, int height) {
    Bind();
    glBindTexture(GL_TEXTURE_2D, colorAttachTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frameBufferInfo.width, frameBufferInfo.height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOG(ERROR) << "failed to create frame buffer";
    }
}

void OpenGLFrameBuffer::Create() {
    glGenTextures(1, &colorAttachTexture);
    glBindTexture(GL_TEXTURE_2D, colorAttachTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frameBufferInfo.width, frameBufferInfo.height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachTexture,
                           0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if(frameBufferInfo.depthAttach) {

    }

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOG(ERROR) << "failed to create frame buffer";
    }
}

}  // namespace Marbas
