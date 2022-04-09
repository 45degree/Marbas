#ifndef MARBAS_RHI_OPENGL_COMMON_H
#define MARBAS_RHI_OPENGL_COMMON_H

#include <GLFW/glfw3.h>

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif

#include <GL/glew.h>
#ifdef WIN32
#include <GL/GL.h>
#else
#include <GL/gl.h>
#endif

#endif
