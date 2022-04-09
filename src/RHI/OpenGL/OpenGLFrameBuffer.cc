#include "RHI/OpenGL/OpenGLFrameBuffer.hpp"

#include <glog/logging.h>

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

    auto error = glGetError();
    LOG_IF(ERROR, error) << FORMAT("can't bind framebuffer: {}, error code is {}",
                                   frameBufferID, error);
}

void OpenGLFrameBuffer::UnBind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    auto error = glGetError();
    LOG_IF(ERROR, error) << FORMAT("can't unbind framebuffer: {}, error code is {}",
                                   frameBufferID, error);
}

void OpenGLFrameBuffer::Resize(int width, int height) {
    Bind();
    frameBufferInfo.height = height;
    frameBufferInfo.width = width;
    ReCreate();
}

void OpenGLFrameBuffer::ReCreate() {
    glDeleteFramebuffers(1, &frameBufferID);
    glDeleteTextures(1, &colorAttachTexture);
    glDeleteTextures(1, &depthAttachTexture);
    frameBufferID = 0;
    colorAttachTexture = 0;
    depthAttachTexture = 0;

    glGenFramebuffers(1, &frameBufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
    Create();
}

void OpenGLFrameBuffer::Create() {
    glGenTextures(1, &colorAttachTexture);
    glBindTexture(GL_TEXTURE_2D, colorAttachTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frameBufferInfo.width, frameBufferInfo.height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachTexture,
                           0);

    if(frameBufferInfo.depthAttach) {
        glGenRenderbuffers(1, &depthAttachTexture);
        glBindRenderbuffer(GL_RENDERBUFFER, depthAttachTexture);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, frameBufferInfo.width,
                              frameBufferInfo.height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
                                  depthAttachTexture);
    }

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOG(ERROR) << "failed to create frame buffer";
        throw std::runtime_error("failed to create frame buffer");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

}  // namespace Marbas
