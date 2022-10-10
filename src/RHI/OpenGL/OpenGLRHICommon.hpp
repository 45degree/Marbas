#pragma once

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
